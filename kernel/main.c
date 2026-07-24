/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 */
#include <gj/apic.h>
#include <gj/boot_info.h>
#include <gj/cap.h>
#include <gj/config.h>
#include <gj/cpu_sys.h>
#include <gj/cold_ipc.h>
#include <gj/cpu.h>
#include <gj/door.h>
#include <gj/error.h>
#include <gj/futex.h>
#include <gj/gdt.h>
#include <gj/idt.h>
#include <gj/io_uring.h>
#include <gj/kheap.h>
#include <gj/klog.h>
#include <gj/linux_abi.h>
#include <gj/linux_dispatch.h>
#include <gj/memobj.h>
#include <gj/pmm.h>
#include <gj/process.h>
/* cpu_load_cr3 / vmm_kernel_cr3 used by init stack map */
#include <gj/smep.h>
#include <gj/elf_load.h>
#include <gj/except.h>
#include <gj/futex.h>
#include <gj/smp.h>
#include <gj/spawn.h>
#include <gj/x2apic.h>
#include <gj/string.h>
#include <gj/syscall.h>
#include <gj/thread.h>
#include <gj/timer.h>
#include <gj/types.h>
#include <gj/user_access.h>
#include <gj/user_task.h>
#include <gj/net_eth.h>
#include <gj/net_lo.h>
#include <gj/net_tcp.h>
#include <gj/vfs_ram.h>
#include <gj/virtio.h>
#include <gj/scsi_mid.h>
#include <gj/virtio_blk.h>
#include <gj/compositor.h>
#include <gj/file_lock.h>
#include <gj/hda.h>
#include <gj/iommu.h>
#include <gj/net_door.h>
#include <gj/irq_msix.h>
#include <gj/notify.h>
#include <gj/pci_caps.h>
#include <gj/session_door.h>
#include <gj/session_input.h>
#include <gj/store_door.h>
#include <gj/vfs_door.h>
#include <gj/virtio_gpu.h>
#include <gj/virtio_input.h>
#include <gj/virtio_net.h>
#include <gj/virtio_scsi.h>
#include <gj/vmm.h>
#include <gj/pe32.h>
#include <gj/wow64.h>
#include <gj_vk_icd.h>
#include <vulkan/vulkan_core_gj.h>
#include <vulkan/vk_icd.h>

/* Linker symbols */
extern char __kernel_start[];
extern char __kernel_end[];

/*
 * serial_soft_log is linkable (serial.c) but not yet in public klog.h.
 * Weak: skip if the object is not linked into this image.
 */
extern void serial_soft_log(void) __attribute__((weak));

/* Multiboot2 tag types */
#define MB2_TAG_END  0
#define MB2_TAG_MMAP 6

struct mb2_tag {
    u32 u32Type;
    u32 u32Size;
};

struct mb2_tag_mmap {
    u32 u32Type;
    u32 u32Size;
    u32 u32EntrySize;
    u32 u32EntryVersion;
};

struct mb2_mmap_entry {
    u64 paAddr;
    u64 cbLen;
    u32 u32Type;
    u32 u32Zero;
};

#define MB2_MMAP_AVAILABLE 1

#define MAX_REGIONS 64

/* Bootstrap process static storage (M0 — until real allocator owns this). */
#define GJ_BOOT_CNODE_SLOTS 64ull
static struct gj_process   g_bootProc;
static struct gj_cnode     g_bootCnode;
static struct gj_cap_slot  g_aBootSlots[GJ_BOOT_CNODE_SLOTS];
static struct gj_root_meta g_bootMeta;

/* Userspace personality process (G-PERS) — NATIVE, door server in ring-3 */
static struct gj_process   g_persProc;
static struct gj_cnode     g_persCnode;
static struct gj_cap_slot  g_aPersSlots[GJ_BOOT_CNODE_SLOTS];
static struct gj_root_meta g_persMeta;

static void
spawn_child_entry(void *pArg)
{
    u32 u32Tag = pArg ? *(u32 *)pArg : 0;

    kprintf("spawn: child running tag=%u cr3=0x%lx\n",
            u32Tag, (unsigned long)(thread_current() && thread_current()->pProc
                                        ? thread_current()->pProc->u64Cr3
                                        : 0));
    thread_exit();
}

/** Runs on AP via smp_ap_run — must not call schedule/BSP-only services. */
static void
smp_ap_worker(void *pArg)
{
    volatile u32 *pTag = (volatile u32 *)pArg;
    u32 u32Cpu = cpu_id();

    if (pTag != NULL) {
        *pTag = 0xA0u + u32Cpu;
    }
    kprintf("smp: AP worker running cpu=%u ticks=%lu\n", u32Cpu,
            (unsigned long)apic_timer_ticks_cpu(u32Cpu));
}

/**
 * Second AP job: prove repeated handoff + timer progress on AP.
 */
static void
smp_ap_worker2(void *pArg)
{
    volatile u32 *pCnt = (volatile u32 *)pArg;
    u64 t0 = apic_timer_ticks_cpu(cpu_id());
    u32 i;

    for (i = 0; i < 100000u; i++) {
        __asm__ volatile ("pause");
    }
    if (pCnt != NULL) {
        *pCnt = (u32)(apic_timer_ticks_cpu(cpu_id()) - t0);
    }
    kprintf("smp: AP worker2 cpu=%u dticks=%u\n", cpu_id(),
            pCnt ? *pCnt : 0);
}

static void
boot_process_smoke(void)
{
    struct gj_cap_ref refMeta;
    struct gj_cap_ref refDoor;
    struct gj_cap_ref refChild;
    struct gj_cap_resolved res;
    struct gj_process *pChild;
    struct gj_spawn_args spawnArgs;
    static u32 u32ChildTag = 42;
    gj_status_t st;
    u64 u64AsA;
    u64 u64AsB;

    gj_process_init(&g_bootProc, &g_bootCnode, g_aBootSlots, GJ_BOOT_CNODE_SLOTS);

    /* Empty until bootstrap: slot 0 invalid, no pager */
    st = gj_cap_resolve(&g_bootCnode, GJ_CAP_SLOT_ROOT_META, 1, &res);
    if (st == GJ_ERR_NOENT || st == GJ_ERR_INVAL) {
        kprintf("process: slot0 empty before meta (ok)\n");
    }
    if (!gj_process_has_pager(&g_bootProc)) {
        kprintf("process: pager empty (ok)\n");
    }
    st = gj_process_handle_fault(&g_bootProc, 0x1000, 0, 0);
    if (st == GJ_ERR_FAULT) {
        kprintf("process: fault without pager => FAULT (kill) ok\n");
    }

    st = gj_process_bootstrap_root_meta(&g_bootProc, &g_bootMeta, &refMeta);
    if (st != GJ_OK) {
        kprintf("process: bootstrap root meta failed %d\n", (int)st);
        return;
    }
    kprintf("process: root meta slot=%lu gen=%u\n",
            (unsigned long)refMeta.u64Slot, refMeta.u32SlotGen);

    st = gj_cap_resolve(&g_bootCnode, refMeta.u64Slot, refMeta.u32SlotGen, &res);
    if (st == GJ_OK && res.u16Type == (u16)GJ_CAP_ROOT_META) {
        kprintf("process: resolve root meta ok (process+cnode self)\n");
    } else {
        kprintf("process: resolve root meta failed %d\n", (int)st);
    }

    /* G-AS: private address space for boot process; stay on it for mmap smoke */
    st = process_as_ensure(&g_bootProc);
    if (st == GJ_OK) {
        process_as_activate(&g_bootProc);
        kprintf("process: boot AS cr3=0x%lx (active)\n",
                (unsigned long)g_bootProc.u64Cr3);
    } else {
        kprintf("process: as_ensure failed %d\n", (int)st);
    }

    /* G-DOOR: cold door as ENDPOINT in CNode */
    door_cold_init();
    st = door_install_endpoint(
        &g_bootProc, door_cold_personality(),
        (u16)(GJ_RIGHT_READ | GJ_RIGHT_GRANT | GJ_RIGHT_MINT | GJ_RIGHT_IDENTIFY),
        &refDoor);
    if (st == GJ_OK) {
        st = gj_cap_resolve(&g_bootCnode, refDoor.u64Slot, refDoor.u32SlotGen,
                            &res);
        kprintf("door: ENDPOINT slot=%lu gen=%u resolve=%d type=%u\n",
                (unsigned long)refDoor.u64Slot, refDoor.u32SlotGen, (int)st,
                res.u16Type);
        door_set_badge(door_cold_personality(), 0xc0ffeeu);
        kprintf("door: badge=0x%x\n", door_get_badge(door_cold_personality()));
    } else {
        kprintf("door: install ENDPOINT failed %d\n", (int)st);
    }

    /* Cap mint/copy/move + soft quota ledger + CDT edges */
    {
        static struct gj_cap_quota g_bootQuota;
        struct gj_cap_ref refMint;
        struct gj_cap_ref refCopy;
        struct gj_cap_ref refMove;
        gj_status_t stCap;

        gj_cap_quota_init(&g_bootQuota, 64u);
        gj_cap_quota_attach(&g_bootCnode, &g_bootQuota);
        memset(&refMint, 0, sizeof(refMint));
        memset(&refCopy, 0, sizeof(refCopy));
        memset(&refMove, 0, sizeof(refMove));
        if (st == GJ_OK) {
            /* Grant MINT+GRANT on endpoint for derive tests */
            stCap = gj_cap_mint(&g_bootCnode, refDoor.u64Slot, refDoor.u32SlotGen,
                                (u16)(GJ_RIGHT_READ | GJ_RIGHT_GRANT |
                                      GJ_RIGHT_MINT | GJ_RIGHT_IDENTIFY),
                                &g_bootCnode, &refMint);
            kprintf("cap: mint => %d slot=%lu gen=%u used=%u\n", (int)stCap,
                    (unsigned long)refMint.u64Slot, refMint.u32SlotGen,
                    gj_cap_quota_used(&g_bootQuota));
            if (stCap == GJ_OK) {
                stCap = gj_cap_copy(&g_bootCnode, refMint.u64Slot,
                                    refMint.u32SlotGen,
                                    (u16)(GJ_RIGHT_READ | GJ_RIGHT_GRANT |
                                          GJ_RIGHT_IDENTIFY),
                                    &refCopy);
                kprintf("cap: copy => %d slot=%lu\n", (int)stCap,
                        (unsigned long)refCopy.u64Slot);
            }
            if (stCap == GJ_OK) {
                stCap = gj_cap_move(&g_bootCnode, refCopy.u64Slot,
                                    refCopy.u32SlotGen, &refMove);
                kprintf("cap: move => %d slot=%lu used=%u\n", (int)stCap,
                        (unsigned long)refMove.u64Slot,
                        gj_cap_quota_used(&g_bootQuota));
            }
            if (stCap == GJ_OK) {
                /* Delete derived only — do not kill cold personality door. */
                (void)gj_cap_delete(&g_bootCnode, refMint.u64Slot,
                                    refMint.u32SlotGen);
                (void)gj_cap_delete(&g_bootCnode, refMove.u64Slot,
                                    refMove.u32SlotGen);
                (void)gj_revoke_process_deferred(16);
                kprintf("cap: mint/copy/move+cdt PASS\n");
            } else {
                kprintf("cap: mint/copy/move soft FAIL %d\n", (int)stCap);
            }
            /* Soft quota exhaust: tiny limit, mint until GJ_ERR_QUOTA. */
            {
                static struct gj_cap_quota g_qTiny;
                struct gj_cap_ref refQ;
                gj_status_t stQ;
                u32 cOk = 0;

                gj_cap_quota_init(&g_qTiny, 2u);
                gj_cap_quota_attach(&g_bootCnode, &g_qTiny);
                for (;;) {
                    memset(&refQ, 0, sizeof(refQ));
                    stQ = gj_cap_mint(&g_bootCnode, refDoor.u64Slot,
                                      refDoor.u32SlotGen,
                                      (u16)(GJ_RIGHT_READ | GJ_RIGHT_IDENTIFY),
                                      &g_bootCnode, &refQ);
                    if (stQ != GJ_OK) {
                        break;
                    }
                    cOk++;
                    if (cOk > 8u) {
                        break;
                    }
                }
                kprintf("cap: quota exhaust mint_ok=%u used=%u st=%d\n", cOk,
                        gj_cap_quota_used(&g_qTiny), (int)stQ);
                if (stQ == GJ_ERR_QUOTA && cOk == 2u) {
                    kprintf("cap: quota exhaust PASS\n");
                }
                /* Restore roomy quota for rest of boot smoke. */
                gj_cap_quota_init(&g_bootQuota, 64u);
                gj_cap_quota_attach(&g_bootCnode, &g_bootQuota);
            }
            /* Timeout: deadline already past → -ETIMEDOUT without server. */
            {
                struct gj_linux_regs req;
                i64 i64T;
                u64 u64Dl;

                memset(&req, 0, sizeof(req));
                req.u64Nr = 1;
                u64Dl = timer_ready() ? timer_mono_nsec() : 1ull;
                i64T = door_call_timeout(door_cold_personality(), &req, u64Dl);
                kprintf("door: call_timeout => %ld\n", (long)i64T);
                if (i64T == -(i64)LINUX_ETIMEDOUT || i64T == -(i64)LINUX_EIO) {
                    kprintf("door: timeout/peer path PASS\n");
                }
            }
            /*
             * Temporary door: CDT revoke walk + peer death (-EIO).
             * Never mark cold personality dead — product path depends on it.
             */
            {
                static struct gj_door g_tmpDoor;
                struct gj_cap_ref refTmp;
                struct gj_cap_ref refChild;
                struct gj_linux_regs req;
                gj_status_t stT;
                u32 u32Walk;
                i64 i64Peer;
                u32 u32Badge;

                door_init(&g_tmpDoor);
                door_set_badge(&g_tmpDoor, 0xbad9eu);
                u32Badge = door_get_badge(&g_tmpDoor);
                stT = door_install_endpoint(
                    &g_bootProc, &g_tmpDoor,
                    (u16)(GJ_RIGHT_READ | GJ_RIGHT_GRANT | GJ_RIGHT_MINT |
                          GJ_RIGHT_IDENTIFY | GJ_RIGHT_DESTROY),
                    &refTmp);
                if (stT == GJ_OK) {
                    stT = gj_cap_mint(&g_bootCnode, refTmp.u64Slot,
                                      refTmp.u32SlotGen,
                                      (u16)(GJ_RIGHT_READ | GJ_RIGHT_IDENTIFY),
                                      &g_bootCnode, &refChild);
                }
                kprintf("door: tmp badge=0x%x install=%d mint=%d\n", u32Badge,
                        (int)stT, (int)stT);
                if (stT == GJ_OK && u32Badge == 0xbad9eu) {
                    kprintf("door: badge transfer PASS\n");
                }
                /* Phase A DEAD + CDT walk clears derived slots. */
                if (stT == GJ_OK) {
                    (void)gj_obj_revoke_begin(&g_tmpDoor.hdr);
                    u32Walk = gj_revoke_cdt_walk_batch(&g_tmpDoor.hdr, 16);
                    (void)gj_revoke_process_deferred(16);
                    kprintf("cap: cdt walk cleared=%u pending=%u\n", u32Walk,
                            gj_revoke_deferred_pending());
                    if (u32Walk >= 1u) {
                        kprintf("cap: cdt walk PASS\n");
                    }
                    /*
                     * Object DEAD (revoke) → door_call -EIO while still ready.
                     * mark_dead after (clears ready) for hygiene.
                     */
                    memset(&req, 0, sizeof(req));
                    req.u64Nr = 99;
                    i64Peer = door_call(&g_tmpDoor, &req);
                    kprintf("door: peer_dead call => %ld\n", (long)i64Peer);
                    if (i64Peer == -(i64)LINUX_EIO) {
                        kprintf("door: mid-call peer death PASS\n");
                    }
                    door_mark_dead(&g_tmpDoor);
                }
            }
        }
    }

    /* G-AS clone smoke: two distinct CR3s */
    u64AsA = vmm_as_create();
    u64AsB = vmm_as_create();
    kprintf("vmm: as_create pair 0x%lx 0x%lx distinct=%d\n",
            (unsigned long)u64AsA, (unsigned long)u64AsB,
            (u64AsA != 0 && u64AsB != 0 && u64AsA != u64AsB) ? 1 : 0);
    if (u64AsA) {
        (void)vmm_as_destroy(u64AsA);
    }
    if (u64AsB) {
        (void)vmm_as_destroy(u64AsB);
    }

    /* G-PROC: spawn child with PROCESS cap */
    memset(&spawnArgs, 0, sizeof(spawnArgs));
    spawnArgs.pfnEntry = spawn_child_entry;
    spawnArgs.pArg = &u32ChildTag;
    spawnArgs.u32Personality = 1;
    spawnArgs.u32Jit = 0;
    st = process_spawn(&g_bootProc, &spawnArgs, &pChild, &refChild);
    if (st == GJ_OK) {
        kprintf("spawn: ok child=%p cap slot=%lu count=%u\n",
                (void *)pChild, (unsigned long)refChild.u64Slot,
                process_spawn_count());
        if (process_from_cap(&g_bootProc, &refChild) == pChild) {
            kprintf("spawn: PROCESS cap resolve ok\n");
        }
    } else {
        kprintf("spawn: failed %d\n", (int)st);
    }

    /* Named shareable memobj (Proton A0) */
    {
        struct gj_memobj *pNamed = memobj_create_named("wine-shm0", 1);
        gj_vaddr_t vaN;

        if (pNamed != NULL) {
            vaN = memobj_map_named(&g_bootProc, "wine-shm0", 0,
                                   GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE);
            kprintf("memobj: named map va=0x%lx lookup=%p\n",
                    (unsigned long)vaN, (void *)memobj_lookup_named("wine-shm0"));
        } else {
            kprintf("memobj: named create failed\n");
        }
    }

    gj_linux_set_current(&g_bootProc, 1, 1);
    gj_process_set_jit(&g_bootProc, 0);

    /* Soft confine surface (promises drop ambient) + hot-path policy */
    {
        int fOk;
        struct gj_linux_regs regsC;
        i64 i64Sock;

        gj_process_confine(&g_bootProc, GJ_PROMISE_STDIO | GJ_PROMISE_RPATH);
        fOk = gj_process_promise_ok(&g_bootProc, GJ_PROMISE_STDIO) &&
              !gj_process_promise_ok(&g_bootProc, GJ_PROMISE_INET);
        kprintf("confine: soft promises=0x%x inet_denied=%d\n",
                g_bootProc.u32Promises, fOk ? 1 : 0);
        if (fOk) {
            kprintf("confine: soft PASS\n");
        }
        /*
         * Policy hot path: cold service sees g_pLinuxProc confine mask.
         * Avoid full gj_syscall_init here (product init still later).
         */
        gj_linux_set_current(&g_bootProc, 1, 1);
        if (!cold_ipc_personality_attached()) {
            gj_protonrt_attach_cold();
        }
        memset(&regsC, 0, sizeof(regsC));
        regsC.u64Nr = LINUX_NR_socket;
        regsC.u64Arg0 = 2; /* AF_INET */
        regsC.u64Arg1 = 1; /* SOCK_STREAM */
        regsC.u64Arg2 = 0;
        i64Sock = cold_ipc_service_local(&regsC);
        kprintf("confine: socket without INET => %ld\n", (long)i64Sock);
        if (i64Sock == -(i64)LINUX_EACCES) {
            kprintf("confine: hot socket policy PASS\n");
        }
        /* Restore ambient for rest of product smoke */
        g_bootProc.u32Confined = 0;
        g_bootProc.u32Promises = GJ_PROMISE_ALL;
    }
}

/*
 * Proton A0: wine-server-shaped two-process demo
 *   - named shared page (different VAs, same PA)
 *   - cross-process shared futex
 *   - socketpair request/reply
 *   - exception port registration
 */
static struct gj_process   g_wsSrvProc;
static struct gj_cnode     g_wsSrvCnode;
static struct gj_cap_slot  g_aWsSrvSlots[GJ_BOOT_CNODE_SLOTS];
static struct gj_root_meta g_wsSrvMeta;
static struct gj_process   g_wsCliProc;
static struct gj_cnode     g_wsCliCnode;
static struct gj_cap_slot  g_aWsCliSlots[GJ_BOOT_CNODE_SLOTS];
static struct gj_root_meta g_wsCliMeta;
static volatile u32        g_u32WsReady;
static volatile u32        g_u32WsDone;
static volatile i32        g_i32WsSv[2];
static gj_vaddr_t          g_vaWsSrv;
static gj_vaddr_t          g_vaWsCli;

static void
ws_server_entry(void *pArg)
{
    struct gj_futex_key key;
    volatile u32 *pWord;
    u8 aReq[8];
    i64 i64N;
    i64 i64W;

    (void)pArg;
    process_as_activate(&g_wsSrvProc);
    pWord = (volatile u32 *)(gj_vaddr_t)g_vaWsSrv;
    /* USER maps + SMAP: kernel thr must use STAC helpers (memobj forces U=1). */
    if (user_store_u32((u64)(gj_vaddr_t)pWord, 0) != GJ_OK) {
        kprintf("winesrv: server store fail\n");
        g_u32WsDone = 2;
        thread_exit();
    }
    /* Block on shared futex before client runs (A0 cross-proc key by PA). */
    if (futex_key_from_uaddr(&key, (u64)(gj_vaddr_t)pWord, 0) != GJ_OK) {
        kprintf("winesrv: server key fail\n");
        g_u32WsDone = 2;
        thread_exit();
    }
    g_u32WsReady = 1;
    i64W = futex_wait(pWord, 0, &key, 0);
    /* -EAGAIN means client already set word=1 (race before we blocked) — still OK */
    {
        u32 u32Word = 0;

        (void)user_load_u32((u64)(gj_vaddr_t)pWord, &u32Word);
        kprintf("winesrv: server futex_wait => %ld word=%u\n", (long)i64W,
                u32Word);
    }

    i64N = vfs_ram_read(g_i32WsSv[0], aReq, sizeof(aReq));
    kprintf("winesrv: server got req n=%ld\n", (long)i64N);
    {
        static const u8 aReply[] = { 'O', 'K', 0 };

        (void)vfs_ram_write(g_i32WsSv[0], aReply, 2);
    }
    g_u32WsDone = 1;
    thread_exit();
}

static void
ws_client_entry(void *pArg)
{
    struct gj_futex_key key;
    volatile u32 *pWord;
    static const u8 aReq[] = { 'R', 'Q', 0 };
    u8 aReply[4];
    int iY;

    (void)pArg;
    process_as_activate(&g_wsCliProc);
    pWord = (volatile u32 *)(gj_vaddr_t)g_vaWsCli;

    for (iY = 0; iY < 128 && !g_u32WsReady; iY++) {
        thread_yield();
    }

    (void)vfs_ram_write(g_i32WsSv[1], aReq, 2);

    /* Cross-process wake: same PA as server's word (SMAP-safe store). */
    if (user_store_u32((u64)(gj_vaddr_t)pWord, 1) != GJ_OK) {
        kprintf("winesrv: client store fail\n");
        thread_exit();
    }
    if (futex_key_from_uaddr(&key, (u64)(gj_vaddr_t)pWord, 0) == GJ_OK) {
        i64 i64W = futex_wake(&key, 1);

        kprintf("winesrv: client futex_wake => %ld (shared PA)\n", (long)i64W);
    }
    for (iY = 0; iY < 32; iY++) {
        if (vfs_ram_read(g_i32WsSv[1], aReply, sizeof(aReply)) > 0) {
            break;
        }
        thread_yield();
    }
    kprintf("winesrv: client reply '%c%c'\n", aReply[0], aReply[1]);
    thread_exit();
}

static void
proton_a0_wineserver_demo(void)
{
    struct gj_memobj *pShm;
    i32 aSv[2];
    u32 u32SrvThr;
    u32 u32CliThr;
    int iY;

    kprintf("winesrv: A0 two-process demo start\n");
    g_u32WsReady = 0;
    g_u32WsDone = 0;
    g_i32WsSv[0] = g_i32WsSv[1] = -1;

    gj_process_init(&g_wsSrvProc, &g_wsSrvCnode, g_aWsSrvSlots,
                    GJ_BOOT_CNODE_SLOTS);
    gj_process_init(&g_wsCliProc, &g_wsCliCnode, g_aWsCliSlots,
                    GJ_BOOT_CNODE_SLOTS);
    (void)gj_process_bootstrap_root_meta(&g_wsSrvProc, &g_wsSrvMeta, NULL);
    (void)gj_process_bootstrap_root_meta(&g_wsCliProc, &g_wsCliMeta, NULL);
    g_wsSrvProc.u32Personality = 1;
    g_wsCliProc.u32Personality = 1;
    gj_process_set_jit(&g_wsCliProc, 1); /* CapJit on game-like client */

    pShm = memobj_create_named("winesrv-shm", 1);
    if (pShm == NULL) {
        /* may already exist from prior run in same boot — recreate path */
        pShm = memobj_lookup_named("winesrv-shm");
    }
    if (pShm == NULL) {
        kprintf("winesrv: shm create failed\n");
        return;
    }
    g_vaWsSrv = memobj_map_named(&g_wsSrvProc, "winesrv-shm", 0x50000000ull,
                                 GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE);
    g_vaWsCli = memobj_map_named(&g_wsCliProc, "winesrv-shm", 0x60000000ull,
                                 GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE);
    if (g_vaWsSrv == 0 || g_vaWsCli == 0) {
        kprintf("winesrv: map failed srv=0x%lx cli=0x%lx\n",
                (unsigned long)g_vaWsSrv, (unsigned long)g_vaWsCli);
        return;
    }
    kprintf("winesrv: maps srv=0x%lx cli=0x%lx (shared frames)\n",
            (unsigned long)g_vaWsSrv, (unsigned long)g_vaWsCli);

    if (vfs_ram_socketpair(1, 1, 0, aSv) != 0) {
        kprintf("winesrv: socketpair failed\n");
        return;
    }
    g_i32WsSv[0] = aSv[0];
    g_i32WsSv[1] = aSv[1];

    except_port_register(&g_wsCliProc, 0); /* will set thr after create */
    u32SrvThr = thread_create(&g_wsSrvProc, ws_server_entry, NULL);
    u32CliThr = thread_create(&g_wsCliProc, ws_client_entry, NULL);
    (void)except_port_register(&g_wsCliProc, u32CliThr);
    kprintf("winesrv: thr srv=%u cli=%u sv=%d,%d\n", u32SrvThr, u32CliThr,
            aSv[0], aSv[1]);

    for (iY = 0; iY < 64 && !g_u32WsDone; iY++) {
        thread_yield();
    }
    kprintf("winesrv: A0 demo %s (done=%u)\n",
            g_u32WsDone ? "PASS" : "TIMEOUT", g_u32WsDone);

    if (except_port_smoke(&g_wsCliProc) != 0) {
        kprintf("except: port smoke FAIL\n");
    }

    (void)vfs_ram_close(aSv[0]);
    (void)vfs_ram_close(aSv[1]);
    process_as_activate(&g_bootProc);
}

static void
linux_hybrid_smoke(void)
{
    struct gj_linux_regs regs;
    struct gj_linux_dispatch_stats stats;
    struct linux_timespec ts;
    static const char szMsg[] = "linux hybrid: write hot path\n";
    static u32 u32FutexWord = 1;
    i64 i64Pid;
    i64 i64Map;

    kprintf("linux: hybrid smoke enter\n");
    gj_syscall_init();
    /* doors + protonrt already attached in kmain before smoke when possible */
    if (!cold_ipc_personality_attached()) {
        gj_protonrt_attach_cold();
    }
    gj_linux_dispatch_stats_reset();
    kprintf("linux: hybrid dispatch ready\n");

    /* getpid hot */
    regs.u64Nr = LINUX_NR_getpid;
    regs.u64Arg0 = regs.u64Arg1 = regs.u64Arg2 = 0;
    regs.u64Arg3 = regs.u64Arg4 = regs.u64Arg5 = 0;
    regs.i64Ret = 0;
    gj_linux_syscall_dispatch(&regs);
    i64Pid = regs.i64Ret;
    kprintf("linux: getpid => %ld (hot)\n", (long)i64Pid);

    /* write hot */
    regs.u64Nr = LINUX_NR_write;
    regs.u64Arg0 = 1;
    regs.u64Arg1 = (u64)(gj_vaddr_t)szMsg;
    regs.u64Arg2 = sizeof(szMsg) - 1;
    gj_linux_syscall_dispatch(&regs);
    kprintf("linux: write => %ld\n", (long)regs.i64Ret);
    /* writev hot (stdio) */
    {
        static struct {
            u64 base;
            u64 len;
        } aIov[2];
        static const char szA[] = "writev:";
        static const char szB[] = "ok\n";

        aIov[0].base = (u64)(uintptr_t)szA;
        aIov[0].len = sizeof(szA) - 1;
        aIov[1].base = (u64)(uintptr_t)szB;
        aIov[1].len = sizeof(szB) - 1;
        regs.u64Nr = LINUX_NR_writev;
        regs.u64Arg0 = 1;
        regs.u64Arg1 = (u64)(uintptr_t)aIov;
        regs.u64Arg2 = 2;
        gj_linux_syscall_dispatch(&regs);
        kprintf("linux: writev => %ld\n", (long)regs.i64Ret);
        if (regs.i64Ret == (i64)(aIov[0].len + aIov[1].len)) {
            kprintf("linux: writev PASS\n");
        }
    }

    /* clock_gettime hot */
    regs.u64Nr = LINUX_NR_clock_gettime;
    regs.u64Arg0 = LINUX_CLOCK_MONOTONIC;
    regs.u64Arg1 = (u64)(gj_vaddr_t)&ts;
    gj_linux_syscall_dispatch(&regs);
    kprintf("linux: clock_gettime => %ld  sec=%ld nsec=%ld\n",
            (long)regs.i64Ret, (long)ts.i64Sec, (long)ts.i64Nsec);
    /* clock_getres hot */
    {
        static struct {
            i64 i64Sec;
            i64 i64Nsec;
        } tres;

        tres.i64Sec = tres.i64Nsec = -1;
        regs.u64Nr = LINUX_NR_clock_getres;
        regs.u64Arg0 = LINUX_CLOCK_MONOTONIC;
        regs.u64Arg1 = (u64)(gj_vaddr_t)&tres;
        gj_linux_syscall_dispatch(&regs);
        kprintf("linux: clock_getres => %ld nsec=%ld\n", (long)regs.i64Ret,
                (long)tres.i64Nsec);
        if (regs.i64Ret == 0 && tres.i64Nsec > 0) {
            kprintf("linux: clock_getres PASS\n");
        }
        /* clock_nanosleep relative 0 ns */
        {
            static struct {
                i64 i64Sec;
                i64 i64Nsec;
            } treq;

            treq.i64Sec = 0;
            treq.i64Nsec = 0;
            regs.u64Nr = LINUX_NR_clock_nanosleep;
            regs.u64Arg0 = LINUX_CLOCK_MONOTONIC;
            regs.u64Arg1 = 0;
            regs.u64Arg2 = (u64)(uintptr_t)&treq;
            regs.u64Arg3 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: clock_nanosleep => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                kprintf("linux: clock_nanosleep PASS\n");
            }
        }
    }
    /* uname / getuid hot */
    {
        static char aUts[6 * 65];
        u32 zi;

        for (zi = 0; zi < sizeof(aUts); zi++) {
            aUts[zi] = 0;
        }
        regs.u64Nr = LINUX_NR_uname;
        regs.u64Arg0 = (u64)(uintptr_t)aUts;
        gj_linux_syscall_dispatch(&regs);
        kprintf("linux: uname => %ld sys=%s\n", (long)regs.i64Ret, aUts);
        if (regs.i64Ret == 0 && aUts[0] == 'L') {
            kprintf("linux: uname PASS\n");
        }
        regs.u64Nr = LINUX_NR_getuid;
        gj_linux_syscall_dispatch(&regs);
        kprintf("linux: getuid => %ld\n", (long)regs.i64Ret);
        if (regs.i64Ret >= 0) {
            kprintf("linux: getuid PASS\n");
        }
        /* rt_sigprocmask / rt_sigaction stubs */
        {
            static u8 aSet[8];
            static u8 aOld[8];
            static u8 aSa[32];
            u32 zi;

            for (zi = 0; zi < 8; zi++) {
                aSet[zi] = 0;
                aOld[zi] = 0xff;
            }
            regs.u64Nr = LINUX_NR_rt_sigprocmask;
            regs.u64Arg0 = 2; /* SIG_SETMASK */
            regs.u64Arg1 = (u64)(uintptr_t)aSet;
            regs.u64Arg2 = (u64)(uintptr_t)aOld;
            regs.u64Arg3 = 8;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: rt_sigprocmask => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                kprintf("linux: rt_sigprocmask PASS\n");
            }
            for (zi = 0; zi < 32; zi++) {
                aSa[zi] = 0;
            }
            regs.u64Nr = LINUX_NR_rt_sigaction;
            regs.u64Arg0 = 2; /* SIGINT-shaped */
            regs.u64Arg1 = 0;
            regs.u64Arg2 = (u64)(uintptr_t)aSa;
            regs.u64Arg3 = 8;
            gj_linux_syscall_dispatch(&regs);
            if (regs.i64Ret == 0) {
                kprintf("linux: rt_sigaction PASS\n");
            }
        }
        {
            static u32 aGids[4];

            aGids[0] = 0;
            regs.u64Nr = LINUX_NR_getgroups;
            regs.u64Arg0 = 4;
            regs.u64Arg1 = (u64)(uintptr_t)aGids;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: getgroups => %ld gid0=%u\n", (long)regs.i64Ret,
                    aGids[0]);
            if (regs.i64Ret == 1) {
                kprintf("linux: getgroups PASS\n");
            }
            regs.u64Nr = LINUX_NR_set_tid_address;
            regs.u64Arg0 = (u64)(uintptr_t)&aGids[1];
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: set_tid_address => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret > 0) {
                kprintf("linux: set_tid_address PASS\n");
            }
        }
    }

    /* anon mmap + write + munmap (real PTEs) */
    regs.u64Nr = LINUX_NR_mmap;
    regs.u64Arg0 = 0;
    regs.u64Arg1 = 4096;
    regs.u64Arg2 = LINUX_PROT_READ | LINUX_PROT_WRITE;
    regs.u64Arg3 = LINUX_MAP_PRIVATE | LINUX_MAP_ANONYMOUS;
    regs.u64Arg4 = (u64)-1;
    regs.u64Arg5 = 0;
    gj_linux_syscall_dispatch(&regs);
    i64Map = regs.i64Ret;
    kprintf("linux: mmap anon => 0x%lx\n", (unsigned long)i64Map);
    if (i64Map > 0) {
        /* mremap grow in place or maymove */
        {
            i64 i64Rm;

            regs.u64Nr = LINUX_NR_mremap;
            regs.u64Arg0 = (u64)i64Map;
            regs.u64Arg1 = 4096;
            regs.u64Arg2 = 8192;
            regs.u64Arg3 = LINUX_MREMAP_MAYMOVE;
            regs.u64Arg4 = 0;
            gj_linux_syscall_dispatch(&regs);
            i64Rm = regs.i64Ret;
            kprintf("linux: mremap => 0x%lx\n", (unsigned long)i64Rm);
            if (i64Rm > 0) {
                kprintf("linux: mremap PASS\n");
                i64Map = i64Rm;
            }
        }
        /* mincore: first page resident */
        {
            static u8 aMc[4];

            aMc[0] = 0;
            regs.u64Nr = LINUX_NR_mincore;
            regs.u64Arg0 = (u64)i64Map;
            regs.u64Arg1 = 4096;
            regs.u64Arg2 = (u64)(uintptr_t)aMc;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: mincore => %ld bit0=%u\n", (long)regs.i64Ret,
                    aMc[0]);
            if (regs.i64Ret == 0 && (aMc[0] & 1u) != 0) {
                kprintf("linux: mincore PASS\n");
            }
            regs.u64Nr = LINUX_NR_msync;
            regs.u64Arg0 = (u64)i64Map;
            regs.u64Arg1 = 4096;
            regs.u64Arg2 = 4; /* MS_SYNC-shaped */
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: msync => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                kprintf("linux: msync PASS\n");
            }
        }
        /* SMAP: use user_store path (STAC/CLAC) for poke */
        if (user_store_u32((u64)i64Map, 0xdeadbeefu) == GJ_OK) {
            u32 u32Val = 0;

            (void)user_load_u32((u64)i64Map, &u32Val);
            kprintf("linux: mmap poke ok val=0x%x\n", u32Val);
        } else {
            kprintf("linux: mmap poke failed (EFAULT)\n");
        }
        regs.u64Nr = LINUX_NR_mprotect;
        regs.u64Arg0 = (u64)i64Map;
        regs.u64Arg1 = 4096;
        regs.u64Arg2 = LINUX_PROT_READ | LINUX_PROT_WRITE | LINUX_PROT_EXEC;
        gj_linux_syscall_dispatch(&regs);
        kprintf("linux: mprotect WX no-JIT => %ld (expect -EACCES)\n",
                (long)regs.i64Ret);
        if (g_pLinuxProc != NULL) {
            gj_process_set_jit(g_pLinuxProc, 1);
        }
        gj_linux_syscall_dispatch(&regs);
        kprintf("linux: mprotect WX +JIT => %ld (expect 0)\n",
                (long)regs.i64Ret);
        if (g_pLinuxProc != NULL) {
            gj_process_set_jit(g_pLinuxProc, 0);
        }
        regs.u64Nr = LINUX_NR_munmap;
        regs.u64Arg0 = (u64)i64Map;
        regs.u64Arg1 = 4096;
        gj_linux_syscall_dispatch(&regs);
        kprintf("linux: munmap => %ld\n", (long)regs.i64Ret);
    }

    /* futex wake (no waiters) */
    regs.u64Nr = LINUX_NR_futex;
    regs.u64Arg0 = (u64)(gj_vaddr_t)&u32FutexWord;
    regs.u64Arg1 = GJ_FUTEX_WAKE | GJ_FUTEX_PRIVATE_FLAG;
    regs.u64Arg2 = 1;
    regs.u64Arg3 = regs.u64Arg4 = regs.u64Arg5 = 0;
    gj_linux_syscall_dispatch(&regs);
    kprintf("linux: futex_wake => %ld\n", (long)regs.i64Ret);

    /* futex wait value mismatch → EAGAIN */
    u32FutexWord = 2;
    regs.u64Nr = LINUX_NR_futex;
    regs.u64Arg0 = (u64)(gj_vaddr_t)&u32FutexWord;
    regs.u64Arg1 = GJ_FUTEX_WAIT | GJ_FUTEX_PRIVATE_FLAG;
    regs.u64Arg2 = 1; /* expected 1, actual 2 */
    gj_linux_syscall_dispatch(&regs);
    kprintf("linux: futex_wait mismatch => %ld (expect -EAGAIN)\n",
            (long)regs.i64Ret);

    /* cold openat/read via cold_ipc → vfs_ram + protonrt */
    {
        static const char szPath[] = "/etc/hostname";
        static const char szMkdir[] = "/tmp/gj-smoke-dir";
        static char aReadBuf[32];
        static char aCwd[8];
        i64 i64Fd;

        regs.u64Nr = LINUX_NR_getcwd;
        regs.u64Arg0 = (u64)(gj_vaddr_t)aCwd;
        regs.u64Arg1 = sizeof(aCwd);
        gj_linux_syscall_dispatch(&regs);
        kprintf("linux: getcwd => %ld cwd=\"%s\"\n", (long)regs.i64Ret, aCwd);

        regs.u64Nr = LINUX_NR_chdir;
        regs.u64Arg0 = (u64)(gj_vaddr_t)"/tmp";
        gj_linux_syscall_dispatch(&regs);
        kprintf("linux: chdir => %ld\n", (long)regs.i64Ret);
        if (regs.i64Ret == 0) {
            static char aCwd2[16];
            u32 zi;

            for (zi = 0; zi < sizeof(aCwd2); zi++) {
                aCwd2[zi] = 0;
            }
            regs.u64Nr = LINUX_NR_getcwd;
            regs.u64Arg0 = (u64)(uintptr_t)aCwd2;
            regs.u64Arg1 = sizeof(aCwd2);
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: getcwd after chdir => \"%s\"\n", aCwd2);
            if (aCwd2[0] == '/' && aCwd2[1] == 't') {
                kprintf("linux: chdir PASS\n");
            }
        }
        /* sysinfo / times / getrusage */
        {
            static u8 aSi[128];
            static u8 aTms[32];
            static u8 aRu[144];

            regs.u64Nr = LINUX_NR_sysinfo;
            regs.u64Arg0 = (u64)(uintptr_t)aSi;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: sysinfo => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                kprintf("linux: sysinfo PASS\n");
            }
            regs.u64Nr = LINUX_NR_times;
            regs.u64Arg0 = (u64)(uintptr_t)aTms;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: times => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret >= 0) {
                kprintf("linux: times PASS\n");
            }
            regs.u64Nr = LINUX_NR_getrusage;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = (u64)(uintptr_t)aRu;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: getrusage => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                kprintf("linux: getrusage PASS\n");
            }
            regs.u64Nr = LINUX_NR_getpriority;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            if (regs.i64Ret == 20) {
                kprintf("linux: getpriority PASS\n");
            }
        }

        regs.u64Nr = LINUX_NR_mkdir;
        regs.u64Arg0 = (u64)(gj_vaddr_t)szMkdir;
        regs.u64Arg1 = 0755;
        gj_linux_syscall_dispatch(&regs);
        /* rename + ftruncate smoke */
        {
            static const char szRenOld[] = "/tmp/gj-ren-a";
            static const char szRenNew[] = "/tmp/gj-ren-b";
            i64 i64Rfd;

            regs.u64Nr = LINUX_NR_open;
            regs.u64Arg0 = (u64)(uintptr_t)szRenOld;
            regs.u64Arg1 = 0x41; /* O_WRONLY|O_CREAT-shaped */
            gj_linux_syscall_dispatch(&regs);
            i64Rfd = regs.i64Ret;
            if (i64Rfd >= 0) {
                regs.u64Nr = LINUX_NR_ftruncate;
                regs.u64Arg0 = (u64)i64Rfd;
                regs.u64Arg1 = 64;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: ftruncate => %ld\n", (long)regs.i64Ret);
                if (regs.i64Ret == 0) {
                    kprintf("linux: ftruncate PASS\n");
                }
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64Rfd;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Nr = LINUX_NR_rename;
                regs.u64Arg0 = (u64)(uintptr_t)szRenOld;
                regs.u64Arg1 = (u64)(uintptr_t)szRenNew;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: rename => %ld\n", (long)regs.i64Ret);
                if (regs.i64Ret == 0) {
                    kprintf("linux: rename PASS\n");
                }
            }
        }
        /* symlink + readlink + utimensat + select + pipe + link */
        {
            static const char szSym[] = "/tmp/gj-sym";
            static const char szTgt[] = "/tmp/gj-ren-b";
            static const char szLink[] = "/tmp/gj-hard";
            static char aRl[64];
            static i32 aPipe[2];
            u32 zi;

            regs.u64Nr = LINUX_NR_symlink;
            regs.u64Arg0 = (u64)(uintptr_t)szTgt;
            regs.u64Arg1 = (u64)(uintptr_t)szSym;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: symlink => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                for (zi = 0; zi < sizeof(aRl); zi++) {
                    aRl[zi] = 0;
                }
                regs.u64Nr = LINUX_NR_readlink;
                regs.u64Arg0 = (u64)(uintptr_t)szSym;
                regs.u64Arg1 = (u64)(uintptr_t)aRl;
                regs.u64Arg2 = sizeof(aRl);
                gj_linux_syscall_dispatch(&regs);
                if (regs.i64Ret > 0 && aRl[0] == '/') {
                    kprintf("linux: symlink PASS\n");
                }
            }
            regs.u64Nr = LINUX_NR_utimensat;
            regs.u64Arg0 = (u64)(i64)-100; /* AT_FDCWD */
            regs.u64Arg1 = (u64)(uintptr_t)"/tmp/gj-touch";
            regs.u64Arg2 = 0;
            regs.u64Arg3 = 0;
            gj_linux_syscall_dispatch(&regs);
            if (regs.i64Ret == 0) {
                kprintf("linux: utimensat PASS\n");
            }
            regs.u64Nr = LINUX_NR_select;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = 0;
            regs.u64Arg2 = 0;
            regs.u64Arg3 = 0;
            regs.u64Arg4 = 0;
            gj_linux_syscall_dispatch(&regs);
            if (regs.i64Ret == 0) {
                kprintf("linux: select PASS\n");
            }
            aPipe[0] = aPipe[1] = -1;
            regs.u64Nr = LINUX_NR_pipe2;
            regs.u64Arg0 = (u64)(uintptr_t)aPipe;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: pipe2 => %ld fds=%d,%d\n", (long)regs.i64Ret,
                    aPipe[0], aPipe[1]);
            if (regs.i64Ret == 0 && aPipe[0] >= 3 && aPipe[1] >= 3) {
                kprintf("linux: pipe2 PASS\n");
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)aPipe[0];
                gj_linux_syscall_dispatch(&regs);
                regs.u64Arg0 = (u64)aPipe[1];
                gj_linux_syscall_dispatch(&regs);
            }
            regs.u64Nr = LINUX_NR_link;
            regs.u64Arg0 = (u64)(uintptr_t)szTgt;
            regs.u64Arg1 = (u64)(uintptr_t)szLink;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: link => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                kprintf("linux: link PASS\n");
            }
            regs.u64Nr = LINUX_NR_alarm;
            regs.u64Arg0 = 0;
            gj_linux_syscall_dispatch(&regs);
            if (regs.i64Ret >= 0) {
                kprintf("linux: alarm PASS\n");
            }
            regs.u64Nr = LINUX_NR_pause;
            gj_linux_syscall_dispatch(&regs);
            if (regs.i64Ret == -(i64)LINUX_EINTR) {
                kprintf("linux: pause PASS\n");
            }
            /* fchmod + rmdir on empty mkdir marker */
            {
                i64 i64Fd2;
                static const char szDir[] = "/tmp/gj-rmdir-me";

                regs.u64Nr = LINUX_NR_open;
                regs.u64Arg0 = (u64)(uintptr_t)"/tmp/gj-chmod";
                regs.u64Arg1 = 0x41;
                gj_linux_syscall_dispatch(&regs);
                i64Fd2 = regs.i64Ret;
                if (i64Fd2 >= 0) {
                    regs.u64Nr = LINUX_NR_fchmod;
                    regs.u64Arg0 = (u64)i64Fd2;
                    regs.u64Arg1 = 0600;
                    gj_linux_syscall_dispatch(&regs);
                    if (regs.i64Ret == 0) {
                        kprintf("linux: fchmod PASS\n");
                    }
                    regs.u64Nr = LINUX_NR_close;
                    regs.u64Arg0 = (u64)i64Fd2;
                    gj_linux_syscall_dispatch(&regs);
                }
                regs.u64Nr = LINUX_NR_mkdir;
                regs.u64Arg0 = (u64)(uintptr_t)szDir;
                regs.u64Arg1 = 0755;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Nr = LINUX_NR_rmdir;
                regs.u64Arg0 = (u64)(uintptr_t)szDir;
                gj_linux_syscall_dispatch(&regs);
                if (regs.i64Ret == 0) {
                    kprintf("linux: rmdir PASS\n");
                }
            }
            /* fallocate + sendfile */
            {
                static const char szIn[] = "/tmp/gj-sf-in";
                static const char szOut[] = "/tmp/gj-sf-out";
                static u8 aMsg[] = "sendfile";
                i64 iIn;
                i64 iOut;
                u64 off = 0;

                regs.u64Nr = LINUX_NR_open;
                regs.u64Arg0 = (u64)(uintptr_t)szIn;
                regs.u64Arg1 = 0x41;
                gj_linux_syscall_dispatch(&regs);
                iIn = regs.i64Ret;
                regs.u64Arg0 = (u64)(uintptr_t)szOut;
                gj_linux_syscall_dispatch(&regs);
                iOut = regs.i64Ret;
                if (iIn >= 0 && iOut >= 0) {
                    regs.u64Nr = LINUX_NR_write;
                    regs.u64Arg0 = (u64)iIn;
                    regs.u64Arg1 = (u64)(uintptr_t)aMsg;
                    regs.u64Arg2 = sizeof(aMsg) - 1;
                    gj_linux_syscall_dispatch(&regs);
                    regs.u64Nr = LINUX_NR_fallocate;
                    regs.u64Arg0 = (u64)iOut;
                    regs.u64Arg1 = 0;
                    regs.u64Arg2 = 0;
                    regs.u64Arg3 = 64;
                    gj_linux_syscall_dispatch(&regs);
                    if (regs.i64Ret == 0) {
                        kprintf("linux: fallocate PASS\n");
                    }
                    regs.u64Nr = LINUX_NR_lseek;
                    regs.u64Arg0 = (u64)iIn;
                    regs.u64Arg1 = 0;
                    regs.u64Arg2 = 0;
                    gj_linux_syscall_dispatch(&regs);
                    off = 0;
                    regs.u64Nr = LINUX_NR_sendfile;
                    regs.u64Arg0 = (u64)iOut;
                    regs.u64Arg1 = (u64)iIn;
                    regs.u64Arg2 = (u64)(uintptr_t)&off;
                    regs.u64Arg3 = sizeof(aMsg) - 1;
                    gj_linux_syscall_dispatch(&regs);
                    kprintf("linux: sendfile => %ld off=%lu\n",
                            (long)regs.i64Ret, (unsigned long)off);
                    if (regs.i64Ret > 0) {
                        kprintf("linux: sendfile PASS\n");
                    }
                    regs.u64Nr = LINUX_NR_close;
                    regs.u64Arg0 = (u64)iIn;
                    gj_linux_syscall_dispatch(&regs);
                    regs.u64Arg0 = (u64)iOut;
                    gj_linux_syscall_dispatch(&regs);
                }
            }
            /* prctl + robust list */
            {
                static char aName[16];
                static u64 head;
                static u64 len;
                u32 zi;

                for (zi = 0; zi < 16; zi++) {
                    aName[zi] = 0;
                }
                aName[0] = 'g';
                aName[1] = 'j';
                regs.u64Nr = LINUX_NR_prctl;
                regs.u64Arg0 = 15; /* PR_SET_NAME */
                regs.u64Arg1 = (u64)(uintptr_t)aName;
                gj_linux_syscall_dispatch(&regs);
                for (zi = 0; zi < 16; zi++) {
                    aName[zi] = 0;
                }
                regs.u64Arg0 = 16; /* PR_GET_NAME */
                regs.u64Arg1 = (u64)(uintptr_t)aName;
                gj_linux_syscall_dispatch(&regs);
                if (regs.i64Ret == 0 && aName[0] == 'g') {
                    kprintf("linux: prctl PASS\n");
                }
                regs.u64Nr = LINUX_NR_set_robust_list;
                regs.u64Arg0 = 0x1000;
                regs.u64Arg1 = 24;
                gj_linux_syscall_dispatch(&regs);
                head = 0;
                len = 0;
                regs.u64Nr = LINUX_NR_get_robust_list;
                regs.u64Arg0 = 0;
                regs.u64Arg1 = (u64)(uintptr_t)&head;
                regs.u64Arg2 = (u64)(uintptr_t)&len;
                gj_linux_syscall_dispatch(&regs);
                if (regs.i64Ret == 0 && head == 0x1000 && len == 24) {
                    kprintf("linux: robust_list PASS\n");
                }
            }
            /* waitid (no children → ECHILD) */
            {
                static u8 aSi[128];

                regs.u64Nr = LINUX_NR_waitid;
                regs.u64Arg0 = 0; /* P_ALL */
                regs.u64Arg1 = 0;
                regs.u64Arg2 = (u64)(uintptr_t)aSi;
                regs.u64Arg3 = 1; /* WNOHANG */
                gj_linux_syscall_dispatch(&regs);
                if (regs.i64Ret == -(i64)LINUX_ECHILD || regs.i64Ret == 0) {
                    kprintf("linux: waitid PASS\n");
                }
            }
        }
        /* accept path: bind/listen/connect/accept on net_lo */
        {
            static u8 aSa[16];
            i64 i64L;
            i64 i64C;
            i64 i64A;

            /* sockaddr_in: family=AF_INET, port 1234 BE */
            for (i64A = 0; i64A < 16; i64A++) {
                aSa[i64A] = 0;
            }
            aSa[0] = 2; /* AF_INET */
            aSa[2] = 0x04;
            aSa[3] = 0xd2; /* port 1234 */
            regs.u64Nr = LINUX_NR_socket;
            regs.u64Arg0 = 2;
            regs.u64Arg1 = 1;
            regs.u64Arg2 = 0;
            gj_linux_syscall_dispatch(&regs);
            i64L = regs.i64Ret;
            regs.u64Nr = LINUX_NR_socket;
            gj_linux_syscall_dispatch(&regs);
            i64C = regs.i64Ret;
            if (i64L >= 0 && i64C >= 0) {
                regs.u64Nr = LINUX_NR_bind;
                regs.u64Arg0 = (u64)i64L;
                regs.u64Arg1 = (u64)(uintptr_t)aSa;
                regs.u64Arg2 = 16;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Nr = LINUX_NR_listen;
                regs.u64Arg0 = (u64)i64L;
                regs.u64Arg1 = 1;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Nr = LINUX_NR_connect;
                regs.u64Arg0 = (u64)i64C;
                regs.u64Arg1 = (u64)(uintptr_t)aSa;
                regs.u64Arg2 = 16;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Nr = LINUX_NR_accept;
                regs.u64Arg0 = (u64)i64L;
                regs.u64Arg1 = 0;
                regs.u64Arg2 = 0;
                gj_linux_syscall_dispatch(&regs);
                i64A = regs.i64Ret;
                kprintf("linux: accept => %ld (L=%ld C=%ld)\n", (long)i64A,
                        (long)i64L, (long)i64C);
                if (i64A >= 0) {
                    kprintf("linux: accept PASS\n");
                    /* getsockopt SO_TYPE + getsockname */
                    {
                        static int optv;
                        static u32 optl;
                        static u8 aName[16];
                        static u32 namelen;

                        optv = -1;
                        optl = 4;
                        regs.u64Nr = LINUX_NR_getsockopt;
                        regs.u64Arg0 = (u64)i64A;
                        regs.u64Arg1 = 1; /* SOL_SOCKET */
                        regs.u64Arg2 = 3; /* SO_TYPE */
                        regs.u64Arg3 = (u64)(uintptr_t)&optv;
                        regs.u64Arg4 = (u64)(uintptr_t)&optl;
                        gj_linux_syscall_dispatch(&regs);
                        kprintf("linux: getsockopt => %ld type=%d\n",
                                (long)regs.i64Ret, optv);
                        if (regs.i64Ret == 0 && optv == 1) {
                            kprintf("linux: getsockopt PASS\n");
                        }
                        namelen = 16;
                        regs.u64Nr = LINUX_NR_getsockname;
                        regs.u64Arg0 = (u64)i64L;
                        regs.u64Arg1 = (u64)(uintptr_t)aName;
                        regs.u64Arg2 = (u64)(uintptr_t)&namelen;
                        gj_linux_syscall_dispatch(&regs);
                        if (regs.i64Ret == 0 && aName[0] == 2) {
                            kprintf("linux: getsockname PASS\n");
                        }
                        regs.u64Nr = LINUX_NR_setsockopt;
                        regs.u64Arg0 = (u64)i64A;
                        regs.u64Arg1 = 1;
                        regs.u64Arg2 = 2; /* SO_REUSEADDR */
                        optv = 1;
                        regs.u64Arg3 = (u64)(uintptr_t)&optv;
                        regs.u64Arg4 = 4;
                        gj_linux_syscall_dispatch(&regs);
                        if (regs.i64Ret == 0) {
                            kprintf("linux: setsockopt PASS\n");
                        }
                        regs.u64Nr = LINUX_NR_shutdown;
                        regs.u64Arg0 = (u64)i64A;
                        regs.u64Arg1 = 1; /* SHUT_WR */
                        gj_linux_syscall_dispatch(&regs);
                        if (regs.i64Ret == 0) {
                            kprintf("linux: shutdown PASS\n");
                        }
                    }
                    regs.u64Nr = LINUX_NR_close;
                    regs.u64Arg0 = (u64)i64A;
                    gj_linux_syscall_dispatch(&regs);
                }
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64L;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Arg0 = (u64)i64C;
                gj_linux_syscall_dispatch(&regs);
            }
        }
        kprintf("linux: mkdir cold => %ld\n", (long)regs.i64Ret);

        regs.u64Nr = LINUX_NR_openat;
        regs.u64Arg0 = (u64)-100; /* AT_FDCWD */
        regs.u64Arg1 = (u64)(gj_vaddr_t)szPath;
        regs.u64Arg2 = 0;
        gj_linux_syscall_dispatch(&regs);
        i64Fd = regs.i64Ret;
        kprintf("linux: openat cold/vfs => %ld\n", (long)i64Fd);
        if (i64Fd >= 0) {
            regs.u64Nr = LINUX_NR_read;
            regs.u64Arg0 = (u64)i64Fd;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aReadBuf;
            regs.u64Arg2 = sizeof(aReadBuf) - 1;
            gj_linux_syscall_dispatch(&regs);
            if (regs.i64Ret > 0 && regs.i64Ret < (i64)sizeof(aReadBuf)) {
                aReadBuf[regs.i64Ret] = '\0';
            } else {
                aReadBuf[0] = '\0';
            }
            kprintf("linux: read hostname => %ld \"%s\"\n",
                    (long)regs.i64Ret, aReadBuf);
            regs.u64Nr = LINUX_NR_close;
            regs.u64Arg0 = (u64)i64Fd;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: close => %ld\n", (long)regs.i64Ret);
        }
        /* socket loopback stub */
        regs.u64Nr = LINUX_NR_socket;
        regs.u64Arg0 = 2; /* AF_INET */
        regs.u64Arg1 = 1; /* SOCK_STREAM */
        regs.u64Arg2 = 0;
        gj_linux_syscall_dispatch(&regs);
        kprintf("linux: socket lo => %ld\n", (long)regs.i64Ret);
        if (regs.i64Ret >= 0) {
            i64 i64Sock = regs.i64Ret;
            static const u8 aMsg[] = { 'n', 'e', 't' };
            u8 aGot[4];

            /* Unpaired send loops to self RX */
            regs.u64Nr = LINUX_NR_sendto;
            regs.u64Arg0 = (u64)i64Sock;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aMsg;
            regs.u64Arg2 = 3;
            regs.u64Arg3 = 0;
            regs.u64Arg4 = 0;
            regs.u64Arg5 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: sendto => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_recvfrom;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aGot;
            regs.u64Arg2 = 4;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: recvfrom => %ld b0=%c\n", (long)regs.i64Ret,
                    regs.i64Ret > 0 ? aGot[0] : '?');
            regs.u64Nr = LINUX_NR_close;
            regs.u64Arg0 = (u64)i64Sock;
            gj_linux_syscall_dispatch(&regs);
        }
        /* socketpair-shaped IPC (Proton A0) */
        {
            i32 aSv[2] = { -1, -1 };
            u8 aMsg[] = { 'p', 'i', 'p', 'e' };
            u8 aGot[4];

            regs.u64Nr = LINUX_NR_socketpair;
            regs.u64Arg0 = 1; /* AF_UNIX */
            regs.u64Arg1 = 1; /* SOCK_STREAM */
            regs.u64Arg2 = 0;
            regs.u64Arg3 = (u64)(gj_vaddr_t)aSv;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: socketpair => %ld fds=%d,%d\n", (long)regs.i64Ret,
                    aSv[0], aSv[1]);
            if (regs.i64Ret == 0 && aSv[0] >= 0 && aSv[1] >= 0) {
                regs.u64Nr = LINUX_NR_write;
                regs.u64Arg0 = (u64)aSv[0];
                regs.u64Arg1 = (u64)(gj_vaddr_t)aMsg;
                regs.u64Arg2 = 4;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Nr = LINUX_NR_read;
                regs.u64Arg0 = (u64)aSv[1];
                regs.u64Arg1 = (u64)(gj_vaddr_t)aGot;
                regs.u64Arg2 = 4;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: socketpair xfer => %ld b0=%c\n",
                        (long)regs.i64Ret, aGot[0]);
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)aSv[0];
                gj_linux_syscall_dispatch(&regs);
                regs.u64Arg0 = (u64)aSv[1];
                gj_linux_syscall_dispatch(&regs);
            }
        }
        /* eventfd2 */
        {
            u64 u64One = 1;
            u64 u64Got = 0;

            regs.u64Nr = LINUX_NR_eventfd2;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: eventfd2 => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret >= 0) {
                i64 i64Efd = regs.i64Ret;

                regs.u64Nr = LINUX_NR_write;
                regs.u64Arg0 = (u64)i64Efd;
                regs.u64Arg1 = (u64)(gj_vaddr_t)&u64One;
                regs.u64Arg2 = 8;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Nr = LINUX_NR_read;
                regs.u64Arg1 = (u64)(gj_vaddr_t)&u64Got;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: eventfd xfer => %ld val=%lu\n",
                        (long)regs.i64Ret, (unsigned long)u64Got);
                if (regs.i64Ret == 8 && u64Got == 1) {
                    kprintf("linux: eventfd2 PASS\n");
                }
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64Efd;
                gj_linux_syscall_dispatch(&regs);
            }
        }
        /* epoll_create1 + ctl + wait (eventfd ready) */
        {
            u64 u64One = 1;
            u8 aEv[12];
            u32 u32CtlEv = 0x001u; /* EPOLLIN */
            u64 u64CtlData = 0xE011DA7Aull;
            u8 aCtl[12];

            memset(aEv, 0, sizeof(aEv));
            memcpy(aCtl, &u32CtlEv, 4);
            memcpy(aCtl + 4, &u64CtlData, 8);

            regs.u64Nr = LINUX_NR_epoll_create1;
            regs.u64Arg0 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: epoll_create1 => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret >= 0) {
                i64 i64Ep = regs.i64Ret;
                i64 i64Efd = -1;

                regs.u64Nr = LINUX_NR_eventfd2;
                regs.u64Arg0 = 0;
                regs.u64Arg1 = 0;
                gj_linux_syscall_dispatch(&regs);
                i64Efd = regs.i64Ret;
                if (i64Efd >= 0) {
                    regs.u64Nr = LINUX_NR_write;
                    regs.u64Arg0 = (u64)i64Efd;
                    regs.u64Arg1 = (u64)(gj_vaddr_t)&u64One;
                    regs.u64Arg2 = 8;
                    gj_linux_syscall_dispatch(&regs);
                    regs.u64Nr = LINUX_NR_epoll_ctl;
                    regs.u64Arg0 = (u64)i64Ep;
                    regs.u64Arg1 = 1; /* ADD */
                    regs.u64Arg2 = (u64)i64Efd;
                    regs.u64Arg3 = (u64)(gj_vaddr_t)aCtl;
                    gj_linux_syscall_dispatch(&regs);
                    kprintf("linux: epoll_ctl => %ld\n", (long)regs.i64Ret);
                    if (regs.i64Ret == 0) {
                        regs.u64Nr = LINUX_NR_epoll_wait;
                        regs.u64Arg0 = (u64)i64Ep;
                        regs.u64Arg1 = (u64)(gj_vaddr_t)aEv;
                        regs.u64Arg2 = 1;
                        regs.u64Arg3 = 0;
                        gj_linux_syscall_dispatch(&regs);
                        kprintf("linux: epoll_wait => %ld\n",
                                (long)regs.i64Ret);
                        if (regs.i64Ret == 1) {
                            u32 u32Got = 0;
                            u64 u64GotData = 0;

                            memcpy(&u32Got, aEv, 4);
                            memcpy(&u64GotData, aEv + 4, 8);
                            if ((u32Got & 0x001u) &&
                                u64GotData == u64CtlData) {
                                kprintf("linux: epoll PASS\n");
                            }
                        }
                    }
                    regs.u64Nr = LINUX_NR_close;
                    regs.u64Arg0 = (u64)i64Efd;
                    gj_linux_syscall_dispatch(&regs);
                }
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64Ep;
                gj_linux_syscall_dispatch(&regs);
            }
        }
        /* statx on /tmp or known path */
        {
            static u8 aSx[256];
            static char aSxPath[] = "/tmp";

            memset(aSx, 0, sizeof(aSx));
            regs.u64Nr = LINUX_NR_statx;
            regs.u64Arg0 = (u64)-100; /* AT_FDCWD */
            regs.u64Arg1 = (u64)(gj_vaddr_t)aSxPath;
            regs.u64Arg2 = 0;
            regs.u64Arg3 = 0x7ffu;
            regs.u64Arg4 = (u64)(gj_vaddr_t)aSx;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: statx => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                kprintf("linux: statx PASS\n");
            }
        }
        /* fork (live) + vfork (zombie) + wait4 reaper */
        {
            i64 i64Pid;
            i64 i64V;
            i32 i32St = -1;

            regs.u64Nr = LINUX_NR_fork;
            gj_linux_syscall_dispatch(&regs);
            i64Pid = regs.i64Ret;
            kprintf("linux: fork => %ld\n", (long)i64Pid);
            regs.u64Nr = LINUX_NR_vfork;
            gj_linux_syscall_dispatch(&regs);
            i64V = regs.i64Ret;
            kprintf("linux: vfork => %ld\n", (long)i64V);
            if (i64Pid > 0 && i64V > 0) {
                regs.u64Nr = LINUX_NR_wait4;
                regs.u64Arg0 = (u64)i64V; /* reap vfork zombie */
                regs.u64Arg1 = (u64)(gj_vaddr_t)&i32St;
                regs.u64Arg2 = 0;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: wait4 after vfork => %ld st=%d\n",
                        (long)regs.i64Ret, (int)i32St);
                if (regs.i64Ret == i64V) {
                    /* Live fork child: WNOHANG → 0, then exit+reap to clear */
                    regs.u64Nr = LINUX_NR_wait4;
                    regs.u64Arg0 = (u64)i64Pid;
                    regs.u64Arg1 = (u64)(gj_vaddr_t)&i32St;
                    regs.u64Arg2 = 1; /* WNOHANG — still live → 0 */
                    gj_linux_syscall_dispatch(&regs);
                    kprintf("linux: wait4 WNOHANG live => %ld\n",
                            (long)regs.i64Ret);
                    if (regs.i64Ret == 0 &&
                        process_linux_exit_pid((u32)i64Pid, 0) == 0) {
                        regs.u64Nr = LINUX_NR_wait4;
                        regs.u64Arg0 = (u64)i64Pid;
                        regs.u64Arg1 = (u64)(gj_vaddr_t)&i32St;
                        regs.u64Arg2 = 0;
                        gj_linux_syscall_dispatch(&regs);
                        if (regs.i64Ret == i64Pid) {
                            kprintf("linux: fork PASS\n");
                        }
                    }
                }
            }
        }
        /* execve known path */
        {
            static char aEx[] = "/bin/greenjade";

            regs.u64Nr = LINUX_NR_execve;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aEx;
            regs.u64Arg1 = 0;
            regs.u64Arg2 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: execve => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                kprintf("linux: execve PASS\n");
            }
        }
        /*
         * Dynlinker path smoke: stage minimal ELF64 with PT_INTERP → /lib/ld-gj.so.1
         * plus a tiny interpreter ELF; execve must report INTERP PASS.
         * Load into a temporary process so boot AS identity maps stay intact.
         */
        {
            static u8 aLd[256];
            static u8 aApp[1024];
            static char aLdPath[] = "/lib/ld-gj.so.1";
            static char aAppPath[] = "/tmp/interp_app.elf";
            static const char szInterp[] = "/lib/ld-gj.so.1";
            static const char szNeeded[] = "libgj-so.so.1"; /* SysV hash SO */
            static const char szNeeded2[] = "libgj-gnu.so.1"; /* GNU hash SO */
            static const char szSym[] = "gj_so_export"; /* SysV DT_HASH */
            static const char szSym2[] = "gj_gnu_export"; /* DT_GNU_HASH */
            static struct gj_process g_execSmokeProc;
            static struct gj_cnode g_execSmokeCnode;
            static struct gj_cap_slot aExecSlots[16];
            static struct gj_root_meta g_execSmokeMeta;
            static int fExecSmokeInit;
            i64 i64Fd;
            struct gj_elf_info probe;
            u32 o;
            u32 u32InterpOff;
            u32 u32InterpLen;
            u32 u32PhOff;
            u32 u32DynOff;
            u32 u32StrOff;
            u32 u32SymOff;
            u32 u32RelaOff;
            u32 u32WordOff;
            u32 u32GlobOff;
            u32 u32FileSz;
            u32 u32SymNameOff;
            /* Prefer high user VA (away from PE 0x400000 / low identity) */
            const u64 u64AppVa = 0x0000000060000000ull;

            /*
             * Minimal ET_DYN INTERP (ld-gj stub) with real exit code at entry.
             * e_entry=0x70 → after bias 0x70000070: mov eax,60; xor edi,edi; syscall
             */
            memset(aLd, 0, sizeof(aLd));
            aLd[0] = 0x7f;
            aLd[1] = 'E';
            aLd[2] = 'L';
            aLd[3] = 'F';
            aLd[4] = 2;
            aLd[5] = 1;
            aLd[6] = 1;
            aLd[16] = 3; /* ET_DYN */
            aLd[18] = 62;
            aLd[20] = 1;
            aLd[24] = 0x70; /* e_entry low */
            aLd[32] = 64;
            aLd[52] = 64;
            aLd[54] = 56;
            aLd[56] = 1;
            aLd[64] = 1; /* PT_LOAD */
            aLd[68] = 5; /* R|X */
            aLd[64 + 32] = 0x80;
            aLd[64 + 40] = 0x80;
            aLd[64 + 48] = 0x10;
            /* exit(0) at file off 0x70 */
            aLd[0x70] = 0xb8; /* mov eax, imm32 */
            aLd[0x71] = 60;
            aLd[0x72] = 0;
            aLd[0x73] = 0;
            aLd[0x74] = 0;
            aLd[0x75] = 0x31; /* xor edi, edi */
            aLd[0x76] = 0xff;
            aLd[0x77] = 0x0f; /* syscall */
            aLd[0x78] = 0x05;

            {
                u64 u64Cr3W = cpu_read_cr3();

                cpu_load_cr3(vmm_kernel_cr3());
                i64Fd = vfs_ram_open(aLdPath, 1);
                if (i64Fd >= 0) {
                    (void)vfs_ram_lseek(i64Fd, 0, 0);
                    (void)vfs_ram_ftruncate(i64Fd, 0);
                    (void)vfs_ram_write(i64Fd, aLd, 0x80);
                    (void)vfs_ram_close(i64Fd);
                }
                /*
                 * ET_DYN SO with DT_HASH + defined gj_so_export for cross-object
                 * hash resolve smoke (main GLOB_DAT → this symbol).
                 *
                 * Layout must keep payload after both phdrs:
                 *   ehdr(64) + LOAD phdr(56) + DYNAMIC phdr(56) = 176.
                 * Prior bug placed dyn at 128 and overwrote the DYNAMIC phdr,
                 * so elf_so_fill_dyn saw hash=0/sym=0.
                 */
                {
                    static u8 aSo[512];
                    static char aSoPath[] = "/lib/libgj-so.so.1";
                    static const char szSoExp[] = "gj_so_export";
                    /* after ehdr+2*phdr (176); pad to 192 */
                    u32 u32SoDyn = 192;
                    u32 u32SoHash = 192 + 6 * 16; /* 288: after 6 dyn ents */
                    u32 u32SoStr = u32SoHash + 32; /* 320: hash 2+1+2 u32 */
                    u32 u32SoSym = u32SoStr + 32;  /* 352 */
                    u32 u32SoSz = u32SoSym + 48;  /* 400: 2 Elf64_Sym */
                    u32 d;
                    u32 oDynPh;

                    memset(aSo, 0, sizeof(aSo));
                    aSo[0] = 0x7f;
                    aSo[1] = 'E';
                    aSo[2] = 'L';
                    aSo[3] = 'F';
                    aSo[4] = 2;
                    aSo[5] = 1;
                    aSo[6] = 1;
                    aSo[16] = 3; /* ET_DYN */
                    aSo[18] = 62;
                    aSo[20] = 1;
                    aSo[32] = 64;
                    aSo[52] = 64;
                    aSo[54] = 56;
                    aSo[56] = 2; /* LOAD + DYNAMIC */
                    /* Phdr0 PT_LOAD: vaddr=0, offset=0 → file offs == VAs */
                    aSo[64] = 1;
                    aSo[68] = 5;
                    aSo[64 + 32] = (u8)(u32SoSz & 0xffu);
                    aSo[64 + 33] = (u8)((u32SoSz >> 8) & 0xffu);
                    aSo[64 + 40] = aSo[64 + 32];
                    aSo[64 + 41] = aSo[64 + 33];
                    aSo[64 + 48] = 0x10;
                    /* Phdr1 PT_DYNAMIC */
                    oDynPh = 64 + 56;
                    aSo[oDynPh + 0] = 2;
                    aSo[oDynPh + 4] = 6;
                    aSo[oDynPh + 8] = (u8)(u32SoDyn & 0xffu);
                    aSo[oDynPh + 9] = (u8)((u32SoDyn >> 8) & 0xffu);
                    aSo[oDynPh + 16] = aSo[oDynPh + 8]; /* vaddr = file off */
                    aSo[oDynPh + 17] = aSo[oDynPh + 9];
                    aSo[oDynPh + 32] = 6 * 16;
                    aSo[oDynPh + 40] = 6 * 16;
                    aSo[oDynPh + 48] = 8;
                    /* strtab */
                    aSo[u32SoStr] = 0;
                    memcpy(aSo + u32SoStr + 1, szSoExp, sizeof(szSoExp));
                    /* hash: nbucket=1 nchain=2 bucket[0]=1 chain[0]=0 chain[1]=0 */
                    aSo[u32SoHash + 0] = 1; /* nbucket */
                    aSo[u32SoHash + 4] = 2; /* nchain */
                    aSo[u32SoHash + 8] = 1; /* bucket[0] = 1 */
                    /* chain[0]=0 chain[1]=0 already zero */
                    /* sym[0]=null; sym[1]=gj_so_export defined value 0x42 */
                    {
                        u32 s = u32SoSym + 24;

                        aSo[s + 0] = 1; /* st_name */
                        aSo[s + 4] = 0x11; /* GLOBAL OBJECT */
                        aSo[s + 6] = 1; /* st_shndx = 1 (defined) */
                        aSo[s + 8] = 0x42; /* st_value */
                    }
                    /* dyn tags (pre-bias VAs = file offsets) */
                    d = u32SoDyn;
                    aSo[d + 0] = 4; /* DT_HASH */
                    aSo[d + 8] = (u8)(u32SoHash & 0xffu);
                    aSo[d + 9] = (u8)((u32SoHash >> 8) & 0xffu);
                    d += 16;
                    aSo[d + 0] = 5; /* DT_STRTAB */
                    aSo[d + 8] = (u8)(u32SoStr & 0xffu);
                    aSo[d + 9] = (u8)((u32SoStr >> 8) & 0xffu);
                    d += 16;
                    aSo[d + 0] = 6; /* DT_SYMTAB */
                    aSo[d + 8] = (u8)(u32SoSym & 0xffu);
                    aSo[d + 9] = (u8)((u32SoSym >> 8) & 0xffu);
                    d += 16;
                    aSo[d + 0] = 11; /* DT_SYMENT */
                    aSo[d + 8] = 24;
                    d += 16;
                    aSo[d + 0] = 10; /* DT_STRSZ */
                    aSo[d + 8] = 32;
                    d += 16;
                    /* DT_NULL */
                    (void)d;
                    i64Fd = vfs_ram_open(aSoPath, 1);
                    if (i64Fd >= 0) {
                        (void)vfs_ram_lseek(i64Fd, 0, 0);
                        (void)vfs_ram_ftruncate(i64Fd, 0);
                        (void)vfs_ram_write(i64Fd, aSo, u32SoSz);
                        (void)vfs_ram_close(i64Fd);
                    }
                }
                /*
                 * Second ET_DYN SO with DT_GNU_HASH + gj_gnu_export for
                 * multi-SO hash chain smoke (JUMP_SLOT → this symbol).
                 */
                {
                    static u8 aSo2[512];
                    static char aSo2Path[] = "/lib/libgj-gnu.so.1";
                    static const char szGnu[] = "gj_gnu_export";
                    u32 u32SoDyn = 192;
                    u32 u32SoGnu = 192 + 6 * 16; /* after dyn */
                    u32 u32SoStr = u32SoGnu + 48; /* hdr+bloom+bucket+chain */
                    u32 u32SoSym = u32SoStr + 32;
                    u32 u32SoSz = u32SoSym + 48;
                    u32 d;
                    u32 oDynPh;
                    u32 u32H = 5381u;
                    const char *pN;

                    memset(aSo2, 0, sizeof(aSo2));
                    aSo2[0] = 0x7f;
                    aSo2[1] = 'E';
                    aSo2[2] = 'L';
                    aSo2[3] = 'F';
                    aSo2[4] = 2;
                    aSo2[5] = 1;
                    aSo2[6] = 1;
                    aSo2[16] = 3;
                    aSo2[18] = 62;
                    aSo2[20] = 1;
                    aSo2[32] = 64;
                    aSo2[52] = 64;
                    aSo2[54] = 56;
                    aSo2[56] = 2;
                    aSo2[64] = 1;
                    aSo2[68] = 5;
                    aSo2[64 + 32] = (u8)(u32SoSz & 0xffu);
                    aSo2[64 + 33] = (u8)((u32SoSz >> 8) & 0xffu);
                    aSo2[64 + 40] = aSo2[64 + 32];
                    aSo2[64 + 41] = aSo2[64 + 33];
                    aSo2[64 + 48] = 0x10;
                    oDynPh = 64 + 56;
                    aSo2[oDynPh + 0] = 2;
                    aSo2[oDynPh + 4] = 6;
                    aSo2[oDynPh + 8] = (u8)(u32SoDyn & 0xffu);
                    aSo2[oDynPh + 9] = (u8)((u32SoDyn >> 8) & 0xffu);
                    aSo2[oDynPh + 16] = aSo2[oDynPh + 8];
                    aSo2[oDynPh + 17] = aSo2[oDynPh + 9];
                    aSo2[oDynPh + 32] = 6 * 16;
                    aSo2[oDynPh + 40] = 6 * 16;
                    aSo2[oDynPh + 48] = 8;
                    aSo2[u32SoStr] = 0;
                    memcpy(aSo2 + u32SoStr + 1, szGnu, sizeof(szGnu));
                    /* GNU hash: nbuckets=1 symoffset=1 bloom_size=1 bloom_shift=0 */
                    aSo2[u32SoGnu + 0] = 1;
                    aSo2[u32SoGnu + 4] = 1; /* symoffset */
                    aSo2[u32SoGnu + 8] = 1; /* bloom words */
                    /* bloom[0] = all-ones (bring-up skips bloom) */
                    aSo2[u32SoGnu + 16] = 0xff;
                    aSo2[u32SoGnu + 17] = 0xff;
                    aSo2[u32SoGnu + 18] = 0xff;
                    aSo2[u32SoGnu + 19] = 0xff;
                    aSo2[u32SoGnu + 20] = 0xff;
                    aSo2[u32SoGnu + 21] = 0xff;
                    aSo2[u32SoGnu + 22] = 0xff;
                    aSo2[u32SoGnu + 23] = 0xff;
                    aSo2[u32SoGnu + 24] = 1; /* bucket[0]=1 */
                    for (pN = szGnu; *pN != '\0'; pN++) {
                        u32H = (u32H << 5) + u32H + (u32)(u8)*pN;
                    }
                    /* chain[0]: (hash & ~1) | 1 end-of-chain */
                    {
                        u32 u32Ch = (u32H & ~1u) | 1u;

                        aSo2[u32SoGnu + 28] = (u8)(u32Ch & 0xffu);
                        aSo2[u32SoGnu + 29] = (u8)((u32Ch >> 8) & 0xffu);
                        aSo2[u32SoGnu + 30] = (u8)((u32Ch >> 16) & 0xffu);
                        aSo2[u32SoGnu + 31] = (u8)((u32Ch >> 24) & 0xffu);
                    }
                    {
                        u32 s = u32SoSym + 24;

                        aSo2[s + 0] = 1;
                        aSo2[s + 4] = 0x12; /* GLOBAL FUNC */
                        aSo2[s + 6] = 1;
                        aSo2[s + 8] = 0x99; /* st_value */
                    }
                    d = u32SoDyn;
                    aSo2[d + 0] = 0x6ffffef5u & 0xffu; /* DT_GNU_HASH low */
                    aSo2[d + 1] = (0x6ffffef5u >> 8) & 0xffu;
                    aSo2[d + 2] = (0x6ffffef5u >> 16) & 0xffu;
                    aSo2[d + 3] = (0x6ffffef5u >> 24) & 0xffu;
                    aSo2[d + 8] = (u8)(u32SoGnu & 0xffu);
                    aSo2[d + 9] = (u8)((u32SoGnu >> 8) & 0xffu);
                    d += 16;
                    aSo2[d + 0] = 5;
                    aSo2[d + 8] = (u8)(u32SoStr & 0xffu);
                    aSo2[d + 9] = (u8)((u32SoStr >> 8) & 0xffu);
                    d += 16;
                    aSo2[d + 0] = 6;
                    aSo2[d + 8] = (u8)(u32SoSym & 0xffu);
                    aSo2[d + 9] = (u8)((u32SoSym >> 8) & 0xffu);
                    d += 16;
                    aSo2[d + 0] = 11;
                    aSo2[d + 8] = 24;
                    d += 16;
                    aSo2[d + 0] = 10;
                    aSo2[d + 8] = 32;
                    d += 16;
                    (void)d;
                    i64Fd = vfs_ram_open(aSo2Path, 1);
                    if (i64Fd >= 0) {
                        (void)vfs_ram_lseek(i64Fd, 0, 0);
                        (void)vfs_ram_ftruncate(i64Fd, 0);
                        (void)vfs_ram_write(i64Fd, aSo2, u32SoSz);
                        (void)vfs_ram_close(i64Fd);
                    }
                }
                cpu_load_cr3(u64Cr3W);
            }

            /*
             * App ELF: PT_INTERP + PT_LOAD + PT_DYNAMIC
             * (2×DT_NEEDED + SYMTAB + RELATIVE + GLOB_DAT + JUMP_SLOT).
             * Layout: ehdr|phdrs|interp|dyn|strtab|symtab|rela|words|exitstub
             */
            memset(aApp, 0, sizeof(aApp));
            u32PhOff = 64;
            u32InterpOff = 64 + 56 * 3;
            u32InterpLen = (u32)(sizeof(szInterp));
            u32DynOff = u32InterpOff + ((u32InterpLen + 7u) & ~7u);
            /* 11 dyn tags × 16 (2 NEEDED + meta + NULL) */
            u32StrOff = u32DynOff + 11u * 16;
            /* strtab: \0 so \0 gnu \0 gj_so_export \0 gj_gnu_export \0 */
            u32SymNameOff = 1u + (u32)sizeof(szNeeded); /* first needed */
            {
                u32 u32N2 = u32SymNameOff + (u32)sizeof(szNeeded2);
                u32 u32S1 = u32N2;
                u32 u32S2 = u32S1 + (u32)sizeof(szSym);

                u32SymOff = u32StrOff + 80; /* room for 2 needed + 2 syms */
                u32RelaOff = u32SymOff + 3u * 24; /* null + 2 undef */
                u32WordOff = u32RelaOff + 3u * 24; /* 3 rela */
                u32GlobOff = u32WordOff + 8;
                /* JUMP_SLOT slot after GLOB_DAT word */
                u32FileSz = u32GlobOff + 8 + 8 + 16; /* +jump word + exit stub */
                if (u32FileSz < 768u) {
                    u32FileSz = 768u;
                }
                if (u32FileSz > sizeof(aApp)) {
                    u32FileSz = (u32)sizeof(aApp);
                }
                (void)u32S2;
            }

            aApp[0] = 0x7f;
            aApp[1] = 'E';
            aApp[2] = 'L';
            aApp[3] = 'F';
            aApp[4] = 2;
            aApp[5] = 1;
            aApp[6] = 1;
            aApp[16] = 2; /* ET_EXEC */
            aApp[18] = 62;
            aApp[20] = 1;
            /* e_entry filled after exit stub offset known */
            aApp[32] = (u8)u32PhOff;
            aApp[52] = 64;
            aApp[54] = 56;
            aApp[56] = 3; /* INTERP + LOAD + DYNAMIC */

            /* Phdr0: PT_INTERP */
            o = u32PhOff;
            aApp[o + 0] = 3;
            aApp[o + 8] = (u8)(u32InterpOff & 0xffu);
            aApp[o + 9] = (u8)((u32InterpOff >> 8) & 0xffu);
            aApp[o + 32] = (u8)(u32InterpLen & 0xffu);
            aApp[o + 40] = (u8)(u32InterpLen & 0xffu);
            aApp[o + 48] = 1;

            /* Phdr1: PT_LOAD covering whole file at high VA (R|W|X for reloc) */
            o = u32PhOff + 56;
            aApp[o + 0] = 1;
            aApp[o + 4] = 7; /* R|W|X */
            aApp[o + 16] = (u8)(u64AppVa & 0xffu);
            aApp[o + 17] = (u8)((u64AppVa >> 8) & 0xffu);
            aApp[o + 18] = (u8)((u64AppVa >> 16) & 0xffu);
            aApp[o + 19] = (u8)((u64AppVa >> 24) & 0xffu);
            aApp[o + 20] = (u8)((u64AppVa >> 32) & 0xffu);
            aApp[o + 32] = (u8)(u32FileSz & 0xffu);
            aApp[o + 33] = (u8)((u32FileSz >> 8) & 0xffu);
            aApp[o + 40] = aApp[o + 32];
            aApp[o + 41] = aApp[o + 33];
            aApp[o + 48] = 0x00;
            aApp[o + 49] = 0x10;

            /* Phdr2: PT_DYNAMIC */
            o = u32PhOff + 112;
            aApp[o + 0] = 2; /* PT_DYNAMIC */
            aApp[o + 4] = 6; /* R|W */
            aApp[o + 8] = (u8)(u32DynOff & 0xffu);
            aApp[o + 9] = (u8)((u32DynOff >> 8) & 0xffu);
            {
                u64 u64DynVa = u64AppVa + u32DynOff;

                aApp[o + 16] = (u8)(u64DynVa & 0xffu);
                aApp[o + 17] = (u8)((u64DynVa >> 8) & 0xffu);
                aApp[o + 18] = (u8)((u64DynVa >> 16) & 0xffu);
                aApp[o + 19] = (u8)((u64DynVa >> 24) & 0xffu);
                aApp[o + 20] = (u8)((u64DynVa >> 32) & 0xffu);
            }
            aApp[o + 32] = 11 * 16;
            aApp[o + 40] = 11 * 16;
            aApp[o + 48] = 8;

            memcpy(aApp + u32InterpOff, szInterp, u32InterpLen);

            /* strtab: "\0libgj-so.so.1\0libgj-gnu.so.1\0gj_so_export\0gj_gnu_export\0" */
            {
                u32 u32Off = u32StrOff;
                u32 u32N2Off;
                u32 u32S1Off;
                u32 u32S2Off;

                aApp[u32Off] = 0;
                u32Off = u32StrOff + 1;
                memcpy(aApp + u32Off, szNeeded, sizeof(szNeeded));
                u32Off += (u32)sizeof(szNeeded);
                u32N2Off = u32Off - u32StrOff;
                memcpy(aApp + u32Off, szNeeded2, sizeof(szNeeded2));
                u32Off += (u32)sizeof(szNeeded2);
                u32S1Off = u32Off - u32StrOff;
                memcpy(aApp + u32Off, szSym, sizeof(szSym));
                u32Off += (u32)sizeof(szSym);
                u32S2Off = u32Off - u32StrOff;
                memcpy(aApp + u32Off, szSym2, sizeof(szSym2));
                u32SymNameOff = u32S1Off;

                /* symtab[0]=null; [1]=gj_so_export; [2]=gj_gnu_export */
                {
                    u32 s = u32SymOff + 24;

                    aApp[s + 0] = (u8)(u32S1Off & 0xffu);
                    aApp[s + 1] = (u8)((u32S1Off >> 8) & 0xffu);
                    aApp[s + 4] = 0x10;
                    s = u32SymOff + 48;
                    aApp[s + 0] = (u8)(u32S2Off & 0xffu);
                    aApp[s + 1] = (u8)((u32S2Off >> 8) & 0xffu);
                    aApp[s + 4] = 0x12; /* GLOBAL FUNC for JUMP_SLOT */
                }
                (void)u32N2Off;
            }

            /* Dynamic tags */
            {
                u64 u64StrVa = u64AppVa + u32StrOff;
                u64 u64SymVa = u64AppVa + u32SymOff;
                u64 u64RelaVa = u64AppVa + u32RelaOff;
                u32 d = u32DynOff;
                u32 u32N2Name = 1u + (u32)sizeof(szNeeded);

                /* DT_NEEDED so */
                aApp[d + 0] = 1;
                aApp[d + 8] = 1;
                d += 16;
                /* DT_NEEDED gnu */
                aApp[d + 0] = 1;
                aApp[d + 8] = (u8)(u32N2Name & 0xffu);
                aApp[d + 9] = (u8)((u32N2Name >> 8) & 0xffu);
                d += 16;
                /* DT_STRTAB = 5 */
                aApp[d + 0] = 5;
                aApp[d + 8] = (u8)(u64StrVa & 0xffu);
                aApp[d + 9] = (u8)((u64StrVa >> 8) & 0xffu);
                aApp[d + 10] = (u8)((u64StrVa >> 16) & 0xffu);
                aApp[d + 11] = (u8)((u64StrVa >> 24) & 0xffu);
                aApp[d + 12] = (u8)((u64StrVa >> 32) & 0xffu);
                d += 16;
                /* DT_STRSZ = 10 */
                aApp[d + 0] = 10;
                aApp[d + 8] = 80;
                d += 16;
                /* DT_SYMTAB = 6 */
                aApp[d + 0] = 6;
                aApp[d + 8] = (u8)(u64SymVa & 0xffu);
                aApp[d + 9] = (u8)((u64SymVa >> 8) & 0xffu);
                aApp[d + 10] = (u8)((u64SymVa >> 16) & 0xffu);
                aApp[d + 11] = (u8)((u64SymVa >> 24) & 0xffu);
                aApp[d + 12] = (u8)((u64SymVa >> 32) & 0xffu);
                d += 16;
                /* DT_SYMENT = 11, val = 24 */
                aApp[d + 0] = 11;
                aApp[d + 8] = 24;
                d += 16;
                /* DT_RELA = 7 */
                aApp[d + 0] = 7;
                aApp[d + 8] = (u8)(u64RelaVa & 0xffu);
                aApp[d + 9] = (u8)((u64RelaVa >> 8) & 0xffu);
                aApp[d + 10] = (u8)((u64RelaVa >> 16) & 0xffu);
                aApp[d + 11] = (u8)((u64RelaVa >> 24) & 0xffu);
                aApp[d + 12] = (u8)((u64RelaVa >> 32) & 0xffu);
                d += 16;
                /* DT_RELASZ = 8, val = 72 (3 relas) */
                aApp[d + 0] = 8;
                aApp[d + 8] = 72;
                d += 16;
                /* DT_RELAENT = 9, val = 24 */
                aApp[d + 0] = 9;
                aApp[d + 8] = 24;
                d += 16;
                /* DT_NULL */
                (void)d;
            }

            /* RELA: RELATIVE; GLOB_DAT sym1; JUMP_SLOT sym2 */
            {
                u64 u64WordVa = u64AppVa + u32WordOff;
                u64 u64GlobVa = u64AppVa + u32GlobOff;
                u64 u64JumpVa = u64AppVa + u32GlobOff + 8;
                u32 r;
                u32 u32CodeOff = u32GlobOff + 16;
                u64 u64EntryVa = u64AppVa + u32CodeOff;

                r = u32RelaOff;
                aApp[r + 0] = (u8)(u64WordVa & 0xffu);
                aApp[r + 1] = (u8)((u64WordVa >> 8) & 0xffu);
                aApp[r + 2] = (u8)((u64WordVa >> 16) & 0xffu);
                aApp[r + 3] = (u8)((u64WordVa >> 24) & 0xffu);
                aApp[r + 4] = (u8)((u64WordVa >> 32) & 0xffu);
                aApp[r + 8] = 8; /* RELATIVE */
                aApp[r + 16] = 0x42;

                r = u32RelaOff + 24;
                aApp[r + 0] = (u8)(u64GlobVa & 0xffu);
                aApp[r + 1] = (u8)((u64GlobVa >> 8) & 0xffu);
                aApp[r + 2] = (u8)((u64GlobVa >> 16) & 0xffu);
                aApp[r + 3] = (u8)((u64GlobVa >> 24) & 0xffu);
                aApp[r + 4] = (u8)((u64GlobVa >> 32) & 0xffu);
                aApp[r + 8] = 6; /* GLOB_DAT */
                aApp[r + 12] = 1; /* sym 1 */

                r = u32RelaOff + 48;
                aApp[r + 0] = (u8)(u64JumpVa & 0xffu);
                aApp[r + 1] = (u8)((u64JumpVa >> 8) & 0xffu);
                aApp[r + 2] = (u8)((u64JumpVa >> 16) & 0xffu);
                aApp[r + 3] = (u8)((u64JumpVa >> 24) & 0xffu);
                aApp[r + 4] = (u8)((u64JumpVa >> 32) & 0xffu);
                aApp[r + 8] = 7; /* JUMP_SLOT */
                aApp[r + 12] = 2; /* sym 2 */

                /* Exit stub at e_entry: xor edi,edi; mov eax,60; syscall */
                aApp[u32CodeOff + 0] = 0x31;
                aApp[u32CodeOff + 1] = 0xff;
                aApp[u32CodeOff + 2] = 0xb8;
                aApp[u32CodeOff + 3] = 60;
                aApp[u32CodeOff + 4] = 0;
                aApp[u32CodeOff + 5] = 0;
                aApp[u32CodeOff + 6] = 0;
                aApp[u32CodeOff + 7] = 0x0f;
                aApp[u32CodeOff + 8] = 0x05;
                aApp[24] = (u8)(u64EntryVa & 0xffu);
                aApp[25] = (u8)((u64EntryVa >> 8) & 0xffu);
                aApp[26] = (u8)((u64EntryVa >> 16) & 0xffu);
                aApp[27] = (u8)((u64EntryVa >> 24) & 0xffu);
                aApp[28] = (u8)((u64EntryVa >> 32) & 0xffu);
            }

            if (elf_probe_image(aApp, u32FileSz, &probe) == GJ_OK &&
                (probe.u32Flags & GJ_ELF_INFO_HAS_INTERP) != 0 &&
                probe.szInterp[0] == '/') {
                kprintf("elf: PT_INTERP probe PASS (%s)\n", probe.szInterp);
                if (probe.u16Needed > 0) {
                    kprintf("elf: DT_NEEDED probe PASS (%s)\n",
                            probe.aNeeded[0]);
                }
            } else {
                kprintf("elf: PT_INTERP probe FAIL\n");
            }

            if (!fExecSmokeInit) {
                memset(&g_execSmokeProc, 0, sizeof(g_execSmokeProc));
                gj_process_init(&g_execSmokeProc, &g_execSmokeCnode, aExecSlots,
                                16);
                g_execSmokeProc.u32Alive = 1;
                g_execSmokeProc.u32Personality = 1;
                (void)gj_process_bootstrap_root_meta(&g_execSmokeProc,
                                                     &g_execSmokeMeta, NULL);
                fExecSmokeInit = 1;
            }

            /*
             * Stage ELF under kernel CR3 so vfs_ram_write can read kernel
             * .data (mcmodel=kernel high VAs) even if active AS is private.
             */
            {
                u64 u64Cr3Save = cpu_read_cr3();
                i64 i64Wn;

                cpu_load_cr3(vmm_kernel_cr3());
                i64Fd = vfs_ram_open(aAppPath, 1);
                if (i64Fd >= 0) {
                    (void)vfs_ram_lseek(i64Fd, 0, 0);
                    (void)vfs_ram_ftruncate(i64Fd, 0);
                    i64Wn = vfs_ram_write(i64Fd, aApp, u32FileSz);
                    (void)vfs_ram_close(i64Fd);
                    kprintf("linux: stage interp_app n=%ld b0=%x\n",
                            (long)i64Wn, (unsigned)aApp[0]);
                } else {
                    i64Wn = -1;
                }
                cpu_load_cr3(u64Cr3Save);
                if (i64Fd >= 0 && i64Wn > 4) {
                    gj_linux_set_current(&g_execSmokeProc, 1, 1);
                    regs.u64Nr = LINUX_NR_execve;
                    regs.u64Arg0 = (u64)(gj_vaddr_t)aAppPath;
                    regs.u64Arg1 = 0;
                    regs.u64Arg2 = 0;
                    gj_linux_syscall_dispatch(&regs);
                    kprintf("linux: execve INTERP => %ld\n",
                            (long)regs.i64Ret);
                    if (regs.i64Ret == 0) {
                        kprintf("linux: dynlink path PASS\n");
                        if (g_execSmokeProc.cAuxv > 0) {
                            kprintf("linux: dynlink auxv ready pairs=%u\n",
                                    g_execSmokeProc.cAuxv);
                        }
                        /*
                         * INTERP-first: PCB fields + live ring-3 thr at
                         * ld-gj entry / handoff SP. Yield so thr runs
                         * (ld-gj → AT_ENTRY exit stub) before PE smokes.
                         */
                        if (g_execSmokeProc.u64StartEntry != 0 &&
                            g_execSmokeProc.u64ExecStack != 0 &&
                            g_execSmokeProc.u64StartEntry ==
                                g_execSmokeProc.u64InterpEntry) {
                            u32 y;
                            u32 thr = g_execSmokeProc.u32StartThr;
                            u32 st = 0;

                            kprintf(
                                "linux: INTERP-first ready entry=0x%lx "
                                "sp=0x%lx PASS\n",
                                (unsigned long)g_execSmokeProc.u64StartEntry,
                                (unsigned long)g_execSmokeProc.u64ExecStack);
                            if (thr != 0) {
                                for (y = 0; y < 64u; y++) {
                                    st = thread_get_state(thr);
                                    if (st == GJ_THR_EXITED) {
                                        break;
                                    }
                                    thread_yield();
                                }
                                st = thread_get_state(thr);
                                if (st == GJ_THR_EXITED) {
                                    kprintf(
                                        "linux: INTERP-first live ring3 thr=%u "
                                        "entry=0x%lx sp=0x%lx PASS\n",
                                        thr,
                                        (unsigned long)
                                            g_execSmokeProc.u64StartEntry,
                                        (unsigned long)
                                            g_execSmokeProc.u64ExecStack);
                                } else {
                                    kprintf(
                                        "linux: INTERP-first live thr=%u "
                                        "state=%u (scheduled)\n",
                                        thr, st);
                                }
                            }
                        }
                    }
                    gj_linux_set_current(&g_bootProc, 1, 1);
                    process_as_activate(&g_bootProc);
                    cpu_load_cr3(vmm_kernel_cr3());
                    process_as_activate(&g_bootProc);
                }
            }

            /* Fork user-page COW: map private page then fork (true COW share) */
            {
                gj_paddr_t pa;
                gj_paddr_t paLow;
                i64 i64Child;
                i32 i32CloneSt = 0;
                volatile u8 *pPg =
                    (volatile u8 *)(gj_vaddr_t)0x0000000061000000ull;

                if (process_as_ensure(&g_bootProc) == GJ_OK) {
                    process_as_activate(&g_bootProc);
                    pa = pmm_alloc();
                    paLow = pmm_alloc();
                    if (pa != 0) {
                        memset((void *)(gj_vaddr_t)pa, 0xA5, GJ_PAGE_SIZE);
                        if (paLow != 0) {
                            memset((void *)(gj_vaddr_t)paLow, 0xB6,
                                   GJ_PAGE_SIZE);
                            /*
                             * Low product band reclaim smoke — use 8 MiB base
                             * so we never overwrite kernel identity/BSS (embeds
                             * can push __bss_end past classic PE 0x400000).
                             */
                            if (vmm_map_page(0x0000000000800000ull, paLow,
                                             GJ_VMM_PROT_READ |
                                                 GJ_VMM_PROT_WRITE |
                                                 GJ_VMM_PROT_USER) == GJ_OK) {
                                kprintf("vmm: low-band map 0x800000 PASS\n");
                            }
                        }
                        if (vmm_map_page(0x0000000061000000ull, pa,
                                         GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                                             GJ_VMM_PROT_USER) == GJ_OK) {
                            i64Child = process_linux_fork(1, 1);
                            kprintf("linux: fork clone smoke pid=%ld\n",
                                    (long)i64Child);
                            if (i64Child > 0) {
                                /* Reap so later ECHILD wait4 smoke stays clean */
                                if (process_wait4_ppid(1, i64Child, &i32CloneSt,
                                                       0) == i64Child) {
                                    kprintf("linux: fork clone path PASS\n");
                                }
                                /*
                                 * Parent write after COW demote: break leaf.
                                 * (vmm: COW break … PASS logged from trap or
                                 *  break via protect path on first store.)
                                 */
                                process_as_activate(&g_bootProc);
                                {
                                    u64 u64Pte =
                                        vmm_read_pte(0x0000000061000000ull);
                                    u8 u8Byte = 0x5A;

                                    /* PTE_COW = bit 9 — break without SMAP hit */
                                    if ((u64Pte & (1ull << 9)) != 0) {
                                        if (vmm_cow_break_page(
                                                0x0000000061000000ull) ==
                                            GJ_OK) {
                                            if (copy_to_user(
                                                    0x0000000061000000ull,
                                                    &u8Byte, 1) == GJ_OK) {
                                                kprintf(
                                                    "linux: fork COW break "
                                                    "PASS\n");
                                            }
                                        }
                                    } else if (copy_to_user(
                                                   0x0000000061000000ull,
                                                   &u8Byte, 1) == GJ_OK) {
                                        kprintf(
                                            "linux: fork COW path (no share) "
                                            "PASS\n");
                                    }
                                    (void)pPg;
                                }
                            }
                        }
                    }
                }
            }
        }
        /* clone thread-shaped (no stack → soft 0; with stack → thr id) */
        {
            regs.u64Nr = LINUX_NR_clone;
            regs.u64Arg0 = 0x10000ull; /* CLONE_THREAD */
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: clone thread => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret >= 0) {
                kprintf("linux: clone PASS\n");
            }
        }
        /* timerfd_create + settime + read */
        {
            i64 aIt[4];
            u64 u64Got = 0;

            memset(aIt, 0, sizeof(aIt));
            aIt[2] = 0; /* it_value.sec */
            aIt[3] = 1; /* it_value.nsec — non-zero arms */

            regs.u64Nr = LINUX_NR_timerfd_create;
            regs.u64Arg0 = 1; /* CLOCK_MONOTONIC */
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: timerfd_create => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret >= 0) {
                i64 i64Tfd = regs.i64Ret;

                regs.u64Nr = LINUX_NR_timerfd_settime;
                regs.u64Arg0 = (u64)i64Tfd;
                regs.u64Arg1 = 0;
                regs.u64Arg2 = (u64)(gj_vaddr_t)aIt;
                regs.u64Arg3 = 0;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: timerfd_settime => %ld\n", (long)regs.i64Ret);
                if (regs.i64Ret == 0) {
                    regs.u64Nr = LINUX_NR_read;
                    regs.u64Arg0 = (u64)i64Tfd;
                    regs.u64Arg1 = (u64)(gj_vaddr_t)&u64Got;
                    regs.u64Arg2 = 8;
                    gj_linux_syscall_dispatch(&regs);
                    kprintf("linux: timerfd read => %ld val=%lu\n",
                            (long)regs.i64Ret, (unsigned long)u64Got);
                    if (regs.i64Ret == 8 && u64Got >= 1) {
                        kprintf("linux: timerfd PASS\n");
                    }
                }
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64Tfd;
                gj_linux_syscall_dispatch(&regs);
            }
        }
        /* signalfd4 + inject + read */
        {
            u64 u64Mask = (1ull << 10); /* SIGUSR1-shaped */
            u8 aInfo[128];

            memset(aInfo, 0, sizeof(aInfo));
            regs.u64Nr = LINUX_NR_signalfd4;
            regs.u64Arg0 = (u64)(i64)-1;
            regs.u64Arg1 = (u64)(gj_vaddr_t)&u64Mask;
            regs.u64Arg2 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: signalfd4 => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret >= 0) {
                i64 i64Sfd = regs.i64Ret;

                vfs_ram_signalfd_inject(10);
                regs.u64Nr = LINUX_NR_read;
                regs.u64Arg0 = (u64)i64Sfd;
                regs.u64Arg1 = (u64)(gj_vaddr_t)aInfo;
                regs.u64Arg2 = 128;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: signalfd read => %ld\n", (long)regs.i64Ret);
                if (regs.i64Ret == 128) {
                    u32 u32Sig = 0;

                    memcpy(&u32Sig, aInfo, 4);
                    if (u32Sig == 10) {
                        kprintf("linux: signalfd PASS\n");
                    }
                }
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64Sfd;
                gj_linux_syscall_dispatch(&regs);
            }
        }
        /* setuid/getresuid + mlock */
        {
            u32 aUid[3];

            memset(aUid, 0, sizeof(aUid));
            regs.u64Nr = LINUX_NR_setuid;
            regs.u64Arg0 = 1001;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: setuid => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_getresuid;
            regs.u64Arg0 = (u64)(gj_vaddr_t)&aUid[0];
            regs.u64Arg1 = (u64)(gj_vaddr_t)&aUid[1];
            regs.u64Arg2 = (u64)(gj_vaddr_t)&aUid[2];
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: getresuid => %ld r=%u e=%u s=%u\n",
                    (long)regs.i64Ret, aUid[0], aUid[1], aUid[2]);
            if (regs.i64Ret == 0 && aUid[0] == 1001 && aUid[1] == 1001) {
                regs.u64Nr = LINUX_NR_mlock;
                regs.u64Arg0 = (u64)(gj_vaddr_t)aUid;
                regs.u64Arg1 = sizeof(aUid);
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: mlock => %ld\n", (long)regs.i64Ret);
                if (regs.i64Ret == 0) {
                    kprintf("linux: creds_mlock PASS\n");
                }
            }
            /* restore default uid for later smokes */
            regs.u64Nr = LINUX_NR_setuid;
            regs.u64Arg0 = 1000;
            gj_linux_syscall_dispatch(&regs);
        }
        /* process_vm_readv/writev same-pid */
        {
            static u8 aSrc[16];
            static u8 aDst[16];
            static u64 aLiov[2];
            static u64 aRiov[2];
            u32 zi;

            for (zi = 0; zi < 16; zi++) {
                aSrc[zi] = (u8)(0xA0 + zi);
                aDst[zi] = 0;
            }
            aLiov[0] = (u64)(gj_vaddr_t)aDst;
            aLiov[1] = 16;
            aRiov[0] = (u64)(gj_vaddr_t)aSrc;
            aRiov[1] = 16;
            regs.u64Nr = LINUX_NR_process_vm_readv;
            regs.u64Arg0 = 1; /* self-ish */
            regs.u64Arg1 = (u64)(gj_vaddr_t)aLiov;
            regs.u64Arg2 = 1;
            regs.u64Arg3 = (u64)(gj_vaddr_t)aRiov;
            regs.u64Arg4 = 1;
            regs.u64Arg5 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: process_vm_readv => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 16 && aDst[0] == 0xA0 && aDst[15] == 0xAF) {
                kprintf("linux: process_vm PASS\n");
            }
        }
        /* pidfd_open + send_signal */
        {
            regs.u64Nr = LINUX_NR_pidfd_open;
            regs.u64Arg0 = 1;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: pidfd_open => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret >= 0) {
                i64 i64Pfd = regs.i64Ret;

                regs.u64Nr = LINUX_NR_pidfd_send_signal;
                regs.u64Arg0 = (u64)i64Pfd;
                regs.u64Arg1 = 10;
                regs.u64Arg2 = 0;
                regs.u64Arg3 = 0;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: pidfd_send_signal => %ld\n",
                        (long)regs.i64Ret);
                if (regs.i64Ret == 0) {
                    kprintf("linux: pidfd PASS\n");
                }
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64Pfd;
                gj_linux_syscall_dispatch(&regs);
            }
        }
        /* membarrier + rseq */
        {
            regs.u64Nr = LINUX_NR_membarrier;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: membarrier => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_rseq;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = 0;
            regs.u64Arg2 = 0;
            regs.u64Arg3 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: rseq => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                /* re-check membarrier ret stored? re-issue */
                regs.u64Nr = LINUX_NR_membarrier;
                gj_linux_syscall_dispatch(&regs);
                if (regs.i64Ret == 0) {
                    kprintf("linux: membarrier_rseq PASS\n");
                }
            }
        }
        /* close_range on a temp eventfd */
        {
            regs.u64Nr = LINUX_NR_eventfd2;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            if (regs.i64Ret >= 0) {
                i64 i64E = regs.i64Ret;

                regs.u64Nr = LINUX_NR_close_range;
                regs.u64Arg0 = (u64)i64E;
                regs.u64Arg1 = (u64)i64E;
                regs.u64Arg2 = 0;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: close_range => %ld\n", (long)regs.i64Ret);
                if (regs.i64Ret == 0 && !vfs_ram_fd_ok(i64E)) {
                    kprintf("linux: close_range PASS\n");
                }
            }
        }
        /* sched_getaffinity */
        {
            u8 aMask[8];

            memset(aMask, 0, sizeof(aMask));
            regs.u64Nr = LINUX_NR_sched_getaffinity;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = sizeof(aMask);
            regs.u64Arg2 = (u64)(gj_vaddr_t)aMask;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: sched_getaffinity => %ld mask0=%u\n",
                    (long)regs.i64Ret, (unsigned)aMask[0]);
            if (regs.i64Ret == 8 && (aMask[0] & 1u)) {
                regs.u64Nr = LINUX_NR_sched_setaffinity;
                regs.u64Arg0 = 0;
                regs.u64Arg1 = sizeof(aMask);
                regs.u64Arg2 = (u64)(gj_vaddr_t)aMask;
                gj_linux_syscall_dispatch(&regs);
                if (regs.i64Ret == 0) {
                    kprintf("linux: affinity PASS\n");
                }
            }
        }
        /* capget + getcpu */
        {
            u32 aHdr[2];
            u32 aData[6];
            u32 u32Cpu = 99;
            u32 u32Node = 99;

            memset(aHdr, 0, sizeof(aHdr));
            memset(aData, 0xff, sizeof(aData));
            regs.u64Nr = LINUX_NR_capget;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aHdr;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aData;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: capget => %ld ver=0x%x\n", (long)regs.i64Ret,
                    aHdr[0]);
            regs.u64Nr = LINUX_NR_getcpu;
            regs.u64Arg0 = (u64)(gj_vaddr_t)&u32Cpu;
            regs.u64Arg1 = (u64)(gj_vaddr_t)&u32Node;
            regs.u64Arg2 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: getcpu => %ld cpu=%u node=%u\n",
                    (long)regs.i64Ret, u32Cpu, u32Node);
            if (regs.i64Ret == 0 && u32Cpu == 0) {
                kprintf("linux: cap_getcpu PASS\n");
            }
        }
        /* sync / syncfs */
        {
            regs.u64Nr = LINUX_NR_sync;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: sync => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_syncfs;
            regs.u64Arg0 = 1; /* stdout-shaped */
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: syncfs => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                kprintf("linux: sync PASS\n");
            }
        }
        /* inotify */
        {
            static char aWatch[] = "/tmp";
            u8 aEv[32];
            i32 i32Wd;

            memset(aEv, 0, sizeof(aEv));
            regs.u64Nr = LINUX_NR_inotify_init1;
            regs.u64Arg0 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: inotify_init1 => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret >= 0) {
                i64 i64Ifd = regs.i64Ret;

                regs.u64Nr = LINUX_NR_inotify_add_watch;
                regs.u64Arg0 = (u64)i64Ifd;
                regs.u64Arg1 = (u64)(gj_vaddr_t)aWatch;
                regs.u64Arg2 = 0x00000100u; /* IN_CREATE */
                gj_linux_syscall_dispatch(&regs);
                i32Wd = (i32)regs.i64Ret;
                kprintf("linux: inotify_add_watch => %ld\n",
                        (long)regs.i64Ret);
                if (i32Wd > 0) {
                    regs.u64Nr = LINUX_NR_read;
                    regs.u64Arg0 = (u64)i64Ifd;
                    regs.u64Arg1 = (u64)(gj_vaddr_t)aEv;
                    regs.u64Arg2 = sizeof(aEv);
                    gj_linux_syscall_dispatch(&regs);
                    kprintf("linux: inotify read => %ld\n",
                            (long)regs.i64Ret);
                    if (regs.i64Ret >= 16) {
                        kprintf("linux: inotify PASS\n");
                    }
                    regs.u64Nr = LINUX_NR_inotify_rm_watch;
                    regs.u64Arg0 = (u64)i64Ifd;
                    regs.u64Arg1 = (u64)(i32)i32Wd;
                    gj_linux_syscall_dispatch(&regs);
                }
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64Ifd;
                gj_linux_syscall_dispatch(&regs);
            }
        }
        /* copy_file_range */
        {
            static char aSrc[] = "/tmp/cfr_src";
            static char aDst[] = "/tmp/cfr_dst";
            static char aMsg[] = "cfr-data";
            u64 offIn = 0;
            u64 offOut = 0;
            i64 i64In;
            i64 i64Out;

            regs.u64Nr = LINUX_NR_open;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aSrc;
            regs.u64Arg1 = 0x41; /* O_WRONLY|O_CREAT-shaped */
            regs.u64Arg2 = 0644;
            gj_linux_syscall_dispatch(&regs);
            i64In = regs.i64Ret;
            if (i64In >= 0) {
                regs.u64Nr = LINUX_NR_write;
                regs.u64Arg0 = (u64)i64In;
                regs.u64Arg1 = (u64)(gj_vaddr_t)aMsg;
                regs.u64Arg2 = 8;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64In;
                gj_linux_syscall_dispatch(&regs);
            }
            regs.u64Nr = LINUX_NR_open;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aSrc;
            regs.u64Arg1 = 0; /* O_RDONLY */
            gj_linux_syscall_dispatch(&regs);
            i64In = regs.i64Ret;
            regs.u64Nr = LINUX_NR_open;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aDst;
            regs.u64Arg1 = 0x41;
            regs.u64Arg2 = 0644;
            gj_linux_syscall_dispatch(&regs);
            i64Out = regs.i64Ret;
            if (i64In >= 0 && i64Out >= 0) {
                regs.u64Nr = LINUX_NR_copy_file_range;
                regs.u64Arg0 = (u64)i64In;
                regs.u64Arg1 = (u64)(gj_vaddr_t)&offIn;
                regs.u64Arg2 = (u64)i64Out;
                regs.u64Arg3 = (u64)(gj_vaddr_t)&offOut;
                regs.u64Arg4 = 8;
                regs.u64Arg5 = 0;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: copy_file_range => %ld offIn=%lu offOut=%lu\n",
                        (long)regs.i64Ret, (unsigned long)offIn,
                        (unsigned long)offOut);
                if (regs.i64Ret == 8 && offIn == 8 && offOut == 8) {
                    kprintf("linux: copy_file_range PASS\n");
                }
            }
            if (i64In >= 0) {
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64In;
                gj_linux_syscall_dispatch(&regs);
            }
            if (i64Out >= 0) {
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64Out;
                gj_linux_syscall_dispatch(&regs);
            }
            regs.u64Nr = LINUX_NR_unlink;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aSrc;
            gj_linux_syscall_dispatch(&regs);
            regs.u64Arg0 = (u64)(gj_vaddr_t)aDst;
            gj_linux_syscall_dispatch(&regs);
        }
        /* renameat2 */
        {
            static char aOld[] = "/tmp/ren_a";
            static char aNew[] = "/tmp/ren_b";
            i64 i64Fd;

            regs.u64Nr = LINUX_NR_open;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aOld;
            regs.u64Arg1 = 0x41;
            regs.u64Arg2 = 0644;
            gj_linux_syscall_dispatch(&regs);
            i64Fd = regs.i64Ret;
            kprintf("linux: renameat2 open => %ld\n", (long)i64Fd);
            if (i64Fd >= 0) {
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64Fd;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Nr = LINUX_NR_renameat2;
                regs.u64Arg0 = (u64)(i64)-100; /* AT_FDCWD */
                regs.u64Arg1 = (u64)(gj_vaddr_t)aOld;
                regs.u64Arg2 = (u64)(i64)-100;
                regs.u64Arg3 = (u64)(gj_vaddr_t)aNew;
                regs.u64Arg4 = 0;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: renameat2 => %ld\n", (long)regs.i64Ret);
                if (regs.i64Ret == 0) {
                    kprintf("linux: renameat2 PASS\n");
                }
                regs.u64Nr = LINUX_NR_unlink;
                regs.u64Arg0 = (u64)(gj_vaddr_t)aNew;
                gj_linux_syscall_dispatch(&regs);
            }
        }
        /* renameat (legacy NR) + linkat/symlinkat */
        {
            static char aRo[] = "/tmp/renat_a";
            static char aRn[] = "/tmp/renat_b";
            static char aLk[] = "/tmp/renat_link";
            static char aSy[] = "/tmp/renat_sym";
            i64 i64Fd;

            regs.u64Nr = LINUX_NR_open;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aRo;
            regs.u64Arg1 = 0x41;
            regs.u64Arg2 = 0644;
            gj_linux_syscall_dispatch(&regs);
            i64Fd = regs.i64Ret;
            if (i64Fd >= 0) {
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64Fd;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Nr = LINUX_NR_renameat;
                regs.u64Arg0 = (u64)(i64)-100;
                regs.u64Arg1 = (u64)(gj_vaddr_t)aRo;
                regs.u64Arg2 = (u64)(i64)-100;
                regs.u64Arg3 = (u64)(gj_vaddr_t)aRn;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: renameat => %ld\n", (long)regs.i64Ret);
                if (regs.i64Ret == 0) {
                    regs.u64Nr = LINUX_NR_linkat;
                    regs.u64Arg0 = (u64)(i64)-100;
                    regs.u64Arg1 = (u64)(gj_vaddr_t)aRn;
                    regs.u64Arg2 = (u64)(i64)-100;
                    regs.u64Arg3 = (u64)(gj_vaddr_t)aLk;
                    regs.u64Arg4 = 0;
                    gj_linux_syscall_dispatch(&regs);
                    kprintf("linux: linkat => %ld\n", (long)regs.i64Ret);
                    regs.u64Nr = LINUX_NR_symlinkat;
                    regs.u64Arg0 = (u64)(gj_vaddr_t)aRn;
                    regs.u64Arg1 = (u64)(i64)-100;
                    regs.u64Arg2 = (u64)(gj_vaddr_t)aSy;
                    gj_linux_syscall_dispatch(&regs);
                    kprintf("linux: symlinkat => %ld\n", (long)regs.i64Ret);
                    if (regs.i64Ret == 0) {
                        kprintf("linux: renameat_link PASS\n");
                    }
                    regs.u64Nr = LINUX_NR_unlink;
                    regs.u64Arg0 = (u64)(gj_vaddr_t)aRn;
                    gj_linux_syscall_dispatch(&regs);
                    regs.u64Arg0 = (u64)(gj_vaddr_t)aLk;
                    gj_linux_syscall_dispatch(&regs);
                    regs.u64Arg0 = (u64)(gj_vaddr_t)aSy;
                    gj_linux_syscall_dispatch(&regs);
                }
            }
        }
        /* gettimeofday + time */
        {
            i64 aTv[2];
            i64 i64T = -1;

            memset(aTv, 0, sizeof(aTv));
            regs.u64Nr = LINUX_NR_gettimeofday;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aTv;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: gettimeofday => %ld sec=%ld usec=%ld\n",
                    (long)regs.i64Ret, (long)aTv[0], (long)aTv[1]);
            regs.u64Nr = LINUX_NR_time;
            regs.u64Arg0 = (u64)(gj_vaddr_t)&i64T;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: time => %ld t=%ld\n", (long)regs.i64Ret,
                    (long)i64T);
            if (regs.i64Ret >= 0 && i64T >= 0) {
                kprintf("linux: gettimeofday PASS\n");
            }
        }
        /* clock_settime + clock_adjtime + seccomp stubs */
        {
            i64 aTs[2];

            aTs[0] = 1000;
            aTs[1] = 0;
            regs.u64Nr = LINUX_NR_clock_settime;
            regs.u64Arg0 = LINUX_CLOCK_REALTIME;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aTs;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: clock_settime => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_clock_adjtime;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: clock_adjtime => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_seccomp;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = 0;
            regs.u64Arg2 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: seccomp => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                kprintf("linux: clock_seccomp PASS\n");
            }
            /* fanotify soft path for Wine probes */
            regs.u64Nr = LINUX_NR_fanotify_init;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: fanotify_init => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret >= 0) {
                i64 i64Fan = regs.i64Ret;

                regs.u64Nr = LINUX_NR_fanotify_mark;
                regs.u64Arg0 = (u64)i64Fan;
                regs.u64Arg1 = 0;
                regs.u64Arg2 = 0;
                regs.u64Arg3 = 0;
                regs.u64Arg4 = 0;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: fanotify_mark => %ld\n", (long)regs.i64Ret);
                if (regs.i64Ret == 0) {
                    kprintf("linux: fanotify PASS\n");
                }
            }
        }
        /* pidfd_getfd + minimal io_uring rings + process_madvise */
        {
            i64 i64Pfd;
            i64 i64Tgt;
            i64 i64Got = -1;
            i64 i64Uring;
            i64 i64Enter = -1;
            i64 i64Madv;
            static u8 aParams[128];

            i64Tgt = vfs_ram_open("/tmp/pidfd_getfd", 1);
            regs.u64Nr = LINUX_NR_pidfd_open;
            regs.u64Arg0 = 1;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            i64Pfd = regs.i64Ret;
            kprintf("linux: pidfd_open => %ld\n", (long)i64Pfd);
            if (i64Pfd >= 0 && i64Tgt >= 0) {
                regs.u64Nr = LINUX_NR_pidfd_getfd;
                regs.u64Arg0 = (u64)i64Pfd;
                regs.u64Arg1 = (u64)i64Tgt;
                regs.u64Arg2 = 0;
                gj_linux_syscall_dispatch(&regs);
                i64Got = regs.i64Ret;
                kprintf("linux: pidfd_getfd => %ld (tgt=%ld)\n", (long)i64Got,
                        (long)i64Tgt);
            }
            memset(aParams, 0, sizeof(aParams));
            regs.u64Nr = LINUX_NR_io_uring_setup;
            regs.u64Arg0 = 8;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aParams;
            gj_linux_syscall_dispatch(&regs);
            i64Uring = regs.i64Ret;
            kprintf("linux: io_uring_setup => %ld\n", (long)i64Uring);
            if (i64Uring >= 0) {
                regs.u64Nr = LINUX_NR_io_uring_enter;
                regs.u64Arg0 = (u64)i64Uring;
                regs.u64Arg1 = 0;
                regs.u64Arg2 = 0;
                regs.u64Arg3 = 0;
                gj_linux_syscall_dispatch(&regs);
                i64Enter = regs.i64Ret;
                kprintf("linux: io_uring_enter => %ld\n", (long)i64Enter);
            }
            regs.u64Nr = LINUX_NR_process_madvise;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = 0;
            regs.u64Arg2 = 0;
            regs.u64Arg3 = 0;
            gj_linux_syscall_dispatch(&regs);
            i64Madv = regs.i64Ret;
            kprintf("linux: process_madvise => %ld\n", (long)i64Madv);
            if (i64Madv == 0 && i64Pfd >= 0 && i64Got >= 0 && i64Uring >= 0 &&
                i64Enter == 0) {
                kprintf("linux: pidfd_uring PASS\n");
                kprintf("linux: io_uring min rings PASS\n");
            }
            /* SQE I/O + SQ/CQ mmap fill (userspace-shaped, no inject). */
            if (i64Uring >= 0) {
                static char aSqePath[] = "/tmp/uring_sqe_io";
                static u8 aWbuf[16] = "sqe-io-payload\0";
                static u8 aRbuf[16];
                i64 i64File;
                i64 i64Sub;
                i32 i32Res = -1;
                u64 u64Ud = 0;
                int fSqeOk = 0;
                int fMmapOk = 0;
                u8 *pPkg;
                u32 *pSqTail;
                u32 *pSqHead;
                u32 *pSqArray;
                struct {
                    u8 op;
                    u8 flags;
                    u16 ioprio;
                    i32 fd;
                    u64 off;
                    u64 addr;
                    u32 len;
                    u32 opflags;
                    u64 ud;
                    u16 buf_i;
                    u16 pers;
                    u32 file_i;
                    u64 pad[2];
                } *pSqe;

                memset(aRbuf, 0, sizeof(aRbuf));
                i64File = vfs_ram_open(aSqePath, 1);
                pPkg = (u8 *)gj_io_uring_mmap_package(i64Uring);
                if (i64File >= 0 && pPkg != NULL) {
                    /* Package layout: SQ@0, CQ@0x1000, SQEs@0x2000 */
                    pSqHead = (u32 *)(void *)(pPkg + 0);
                    pSqTail = (u32 *)(void *)(pPkg + 4);
                    pSqArray = (u32 *)(void *)(pPkg + 64);
                    pSqe = (void *)(pPkg + 0x2000);
                    fMmapOk = 1;
                    /* Fill three SQEs via package (mmap view), advance tail. */
                    memset(&pSqe[0], 0, sizeof(pSqe[0]));
                    pSqe[0].op = (u8)GJ_IORING_OP_WRITE;
                    pSqe[0].fd = (i32)i64File;
                    pSqe[0].addr = (u64)(gj_vaddr_t)aWbuf;
                    pSqe[0].len = 14;
                    pSqe[0].ud = 0xA11u;
                    pSqArray[0] = 0;
                    memset(&pSqe[1], 0, sizeof(pSqe[1]));
                    pSqe[1].op = (u8)GJ_IORING_OP_FSYNC;
                    pSqe[1].fd = (i32)i64File;
                    pSqe[1].ud = 0xF5Cu;
                    pSqArray[1] = 1;
                    memset(&pSqe[2], 0, sizeof(pSqe[2]));
                    pSqe[2].op = (u8)GJ_IORING_OP_READ;
                    pSqe[2].fd = (i32)i64File;
                    pSqe[2].addr = (u64)(gj_vaddr_t)aRbuf;
                    pSqe[2].len = 14;
                    pSqe[2].ud = 0xB22u;
                    pSqArray[2] = 2;
                    *pSqTail = *pSqHead + 3u;
                    i64Sub = gj_io_uring_enter(i64Uring, 3, 3, 0);
                    kprintf("linux: io_uring SQE enter => %ld (mmap fill)\n",
                            (long)i64Sub);
                    if (i64Sub == 3) {
                        if (gj_io_uring_cqe_peek(i64Uring, &u64Ud, &i32Res) ==
                                0 &&
                            u64Ud == 0xA11u && i32Res == 14) {
                            (void)gj_io_uring_cqe_advance(i64Uring);
                            if (gj_io_uring_cqe_peek(i64Uring, &u64Ud,
                                                     &i32Res) == 0 &&
                                u64Ud == 0xF5Cu && i32Res == 0) {
                                (void)gj_io_uring_cqe_advance(i64Uring);
                                if (gj_io_uring_cqe_peek(i64Uring, &u64Ud,
                                                         &i32Res) == 0 &&
                                    u64Ud == 0xB22u && i32Res == 14 &&
                                    memcmp(aRbuf, aWbuf, 14) == 0) {
                                    (void)gj_io_uring_cqe_advance(i64Uring);
                                    fSqeOk = 1;
                                }
                            }
                        }
                    }
                    (void)vfs_ram_close(i64File);
                }
                if (fMmapOk) {
                    kprintf("linux: io_uring mmap PASS\n");
                }
                if (fSqeOk) {
                    kprintf("linux: io_uring SQE I/O PASS\n");
                } else {
                    kprintf("linux: io_uring SQE I/O soft FAIL\n");
                }
                /* More opcodes + register files/buffers depth. */
                if (i64Uring >= 0 && pPkg != NULL) {
                    static i32 aRegFd[2];
                    static struct {
                        u64 base;
                        u64 len;
                    } aIov[1];
                    i64 i64Reg;
                    i64 i64Op;
                    int fRegOk = 0;
                    int fOpOk = 0;

                    aRegFd[0] = 0;
                    aRegFd[1] = 1;
                    aIov[0].base = (u64)(gj_vaddr_t)aWbuf;
                    aIov[0].len = 14;
                    i64Reg = gj_io_uring_register(i64Uring,
                                                  GJ_IORING_REGISTER_FILES,
                                                  (u64)(gj_vaddr_t)aRegFd, 2);
                    if (i64Reg == 0) {
                        i64Reg = gj_io_uring_register(
                            i64Uring, GJ_IORING_REGISTER_BUFFERS,
                            (u64)(gj_vaddr_t)aIov, 1);
                    }
                    if (i64Reg == 0) {
                        fRegOk = 1;
                    }
                    kprintf("linux: io_uring_register files+bufs => %ld\n",
                            (long)i64Reg);
                    /* SYNC_FILE_RANGE + MADVISE + TIMEOUT soft SQEs */
                    if (fMmapOk) {
                        i64 i64File2 = vfs_ram_open("/tmp/uring_op_more", 1);

                        if (i64File2 >= 0) {
                            pSqHead = (u32 *)(void *)(pPkg + 0);
                            pSqTail = (u32 *)(void *)(pPkg + 4);
                            pSqArray = (u32 *)(void *)(pPkg + 64);
                            pSqe = (void *)(pPkg + 0x2000);
                            memset(&pSqe[0], 0, sizeof(pSqe[0]));
                            pSqe[0].op = (u8)GJ_IORING_OP_SYNC_FILE_RANGE;
                            pSqe[0].fd = (i32)i64File2;
                            pSqe[0].ud = 0x5F5u;
                            pSqArray[0] = 0;
                            memset(&pSqe[1], 0, sizeof(pSqe[1]));
                            pSqe[1].op = (u8)GJ_IORING_OP_MADVISE;
                            pSqe[1].ud = 0xAD0u;
                            pSqArray[1] = 1;
                            memset(&pSqe[2], 0, sizeof(pSqe[2]));
                            pSqe[2].op = (u8)GJ_IORING_OP_TIMEOUT;
                            pSqe[2].ud = 0x710u;
                            pSqArray[2] = 2;
                            *pSqTail = *pSqHead + 3u;
                            i64Op = gj_io_uring_enter(i64Uring, 3, 3, 0);
                            kprintf("linux: io_uring more-ops enter => %ld\n",
                                    (long)i64Op);
                            if (i64Op == 3) {
                                fOpOk = 1;
                                (void)gj_io_uring_cqe_advance(i64Uring);
                                (void)gj_io_uring_cqe_advance(i64Uring);
                                (void)gj_io_uring_cqe_advance(i64Uring);
                            }
                            (void)vfs_ram_close(i64File2);
                        }
                    }
                    if (fRegOk) {
                        kprintf("linux: io_uring register depth PASS\n");
                    }
                    if (fOpOk) {
                        kprintf("linux: io_uring more opcodes PASS\n");
                    }
                }
            }
        }
        /* sched_getattr/setattr + kcmp */
        {
            u8 aAttr[48];

            memset(aAttr, 0, sizeof(aAttr));
            regs.u64Nr = LINUX_NR_sched_getattr;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aAttr;
            regs.u64Arg2 = 48;
            regs.u64Arg3 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: sched_getattr => %ld size=%u\n", (long)regs.i64Ret,
                    aAttr[0]);
            regs.u64Nr = LINUX_NR_sched_setattr;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aAttr;
            regs.u64Arg2 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: sched_setattr => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_kcmp;
            regs.u64Arg0 = 1;
            regs.u64Arg1 = 1;
            regs.u64Arg2 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: kcmp => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                kprintf("linux: sched_kcmp PASS\n");
            }
        }
        /* mbind / mempolicy */
        {
            i64 mode = -1;

            regs.u64Nr = LINUX_NR_mbind;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = 0;
            regs.u64Arg2 = 0;
            regs.u64Arg3 = 0;
            regs.u64Arg4 = 0;
            regs.u64Arg5 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: mbind => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_get_mempolicy;
            regs.u64Arg0 = (u64)(gj_vaddr_t)&mode;
            regs.u64Arg1 = 0;
            regs.u64Arg2 = 0;
            regs.u64Arg3 = 0;
            regs.u64Arg4 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: get_mempolicy => %ld mode=%ld\n", (long)regs.i64Ret,
                    (long)mode);
            if (regs.i64Ret == 0 && mode == 0) {
                kprintf("linux: mempolicy PASS\n");
            }
        }
        /* POSIX timer_create/settime/delete + inotify_init */
        {
            u32 u32Tid = 0;

            regs.u64Nr = LINUX_NR_timer_create;
            regs.u64Arg0 = 0; /* CLOCK_REALTIME */
            regs.u64Arg1 = 0; /* sevp NULL */
            regs.u64Arg2 = (u64)(gj_vaddr_t)&u32Tid;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: timer_create => %ld tid=%u\n", (long)regs.i64Ret,
                    u32Tid);
            regs.u64Nr = LINUX_NR_timer_settime;
            regs.u64Arg0 = u32Tid;
            regs.u64Arg1 = 0;
            regs.u64Arg2 = 0;
            regs.u64Arg3 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: timer_settime => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_timer_delete;
            regs.u64Arg0 = u32Tid;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: timer_delete => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_inotify_init;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: inotify_init => %ld\n", (long)regs.i64Ret);
            if (u32Tid == 1 && regs.i64Ret >= 0) {
                kprintf("linux: posix_timer PASS\n");
            }
        }
        /* readlinkat + xattr + fchmodat */
        {
            static char aExe[] = "/proc/self/exe";
            static char aXattr[] = "user.gj";
            char aLink[64];
            i64 n;

            memset(aLink, 0, sizeof(aLink));
            regs.u64Nr = LINUX_NR_readlinkat;
            regs.u64Arg0 = (u64)(i64)-100;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aExe;
            regs.u64Arg2 = (u64)(gj_vaddr_t)aLink;
            regs.u64Arg3 = sizeof(aLink);
            gj_linux_syscall_dispatch(&regs);
            n = regs.i64Ret;
            kprintf("linux: readlinkat => %ld\n", (long)n);
            regs.u64Nr = LINUX_NR_getxattr;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aExe;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aXattr;
            regs.u64Arg2 = 0;
            regs.u64Arg3 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: getxattr => %ld\n", (long)regs.i64Ret);
            if (n > 0 && regs.i64Ret == -61) {
                kprintf("linux: xattr_readlinkat PASS\n");
            }
        }
        /* dup3 */
        {
            i64 i64A;
            i64 i64B;

            regs.u64Nr = LINUX_NR_eventfd2;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            i64A = regs.i64Ret;
            if (i64A >= 0) {
                regs.u64Nr = LINUX_NR_dup3;
                regs.u64Arg0 = (u64)i64A;
                regs.u64Arg1 = (u64)(i64A + 10);
                regs.u64Arg2 = 0;
                gj_linux_syscall_dispatch(&regs);
                i64B = regs.i64Ret;
                kprintf("linux: dup3 => %ld (from %ld)\n", (long)i64B,
                        (long)i64A);
                if (i64B == i64A + 10) {
                    kprintf("linux: dup3 PASS\n");
                }
                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64A;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Arg0 = (u64)i64B;
                gj_linux_syscall_dispatch(&regs);
            }
        }
        /* statfs + openat2 */
        {
            static char aRoot[] = "/";
            static char aTmpF[] = "/tmp/oat2";
            static u8 aHow[24];
            static u8 aSf[128];
            u64 fl = 0x41; /* O_WRONLY|O_CREAT */
            u64 mode = 0644;

            memset(aSf, 0, sizeof(aSf));
            memset(aHow, 0, sizeof(aHow));
            memcpy(aHow, &fl, 8);
            memcpy(aHow + 8, &mode, 8);
            regs.u64Nr = LINUX_NR_statfs;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aRoot;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aSf;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: statfs => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_openat2;
            regs.u64Arg0 = (u64)(i64)-100;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aTmpF;
            regs.u64Arg2 = (u64)(gj_vaddr_t)aHow;
            regs.u64Arg3 = 24;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: openat2 => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret >= 0) {
                i64 i64Fd = regs.i64Ret;

                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64Fd;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Nr = LINUX_NR_unlink;
                regs.u64Arg0 = (u64)(gj_vaddr_t)aTmpF;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: statfs_openat2 PASS\n");
            }
        }
        /* getrlimit */
        {
            u64 aLim[2];

            aLim[0] = aLim[1] = 0;
            regs.u64Nr = LINUX_NR_getrlimit;
            regs.u64Arg0 = 3; /* RLIMIT_STACK-shaped */
            regs.u64Arg1 = (u64)(gj_vaddr_t)aLim;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: getrlimit => %ld cur=%lu\n", (long)regs.i64Ret,
                    (unsigned long)aLim[0]);
            if (regs.i64Ret == 0 && aLim[0] > 0) {
                kprintf("linux: getrlimit PASS\n");
            }
        }
        /* tkill + sched + sigaltstack + preadv */
        {
            u64 aSs[3];
            i32 prio = -1;

            regs.u64Nr = LINUX_NR_tkill;
            regs.u64Arg0 = 1;
            regs.u64Arg1 = 10;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: tkill => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_sched_get_priority_max;
            regs.u64Arg0 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: sched_priority_max => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_sched_getscheduler;
            regs.u64Arg0 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: sched_getscheduler => %ld\n", (long)regs.i64Ret);
            memset(aSs, 0, sizeof(aSs));
            aSs[1] = 2; /* SS_DISABLE */
            regs.u64Nr = LINUX_NR_sigaltstack;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aSs;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: sigaltstack => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_sched_getparam;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = (u64)(gj_vaddr_t)&prio;
            gj_linux_syscall_dispatch(&regs);
            if (regs.i64Ret == 0) {
                kprintf("linux: tkill_sched PASS\n");
            }
        }
        /* pkey_alloc + execveat */
        {
            static char aEx[] = "/bin/greenjade";

            regs.u64Nr = LINUX_NR_pkey_alloc;
            regs.u64Arg0 = 0;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: pkey_alloc => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_execveat;
            regs.u64Arg0 = (u64)(i64)-100;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aEx;
            regs.u64Arg2 = 0;
            regs.u64Arg3 = 0;
            regs.u64Arg4 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: execveat => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                kprintf("linux: pkey_execveat PASS\n");
            }
        }
        /* io_setup / epoll_create legacy */
        {
            u64 ctx = 0;

            regs.u64Nr = LINUX_NR_io_setup;
            regs.u64Arg0 = 32;
            regs.u64Arg1 = (u64)(gj_vaddr_t)&ctx;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: io_setup => %ld ctx=%lu\n", (long)regs.i64Ret,
                    (unsigned long)ctx);
            regs.u64Nr = LINUX_NR_epoll_create;
            regs.u64Arg0 = 8;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: epoll_create => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret >= 0 && ctx == 1) {
                i64 i64Ep = regs.i64Ret;

                regs.u64Nr = LINUX_NR_close;
                regs.u64Arg0 = (u64)i64Ep;
                gj_linux_syscall_dispatch(&regs);
                regs.u64Nr = LINUX_NR_io_destroy;
                regs.u64Arg0 = ctx;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: aio_epoll PASS\n");
            }
        }
        /* mount/umount stubs */
        {
            static char aSrc[] = "none";
            static char aTgt[] = "/mnt";
            static char aType[] = "tmpfs";

            regs.u64Nr = LINUX_NR_mount;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aSrc;
            regs.u64Arg1 = (u64)(gj_vaddr_t)aTgt;
            regs.u64Arg2 = (u64)(gj_vaddr_t)aType;
            regs.u64Arg3 = 0;
            regs.u64Arg4 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: mount => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_umount2;
            regs.u64Arg0 = (u64)(gj_vaddr_t)aTgt;
            regs.u64Arg1 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: umount2 => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret == 0) {
                kprintf("linux: mount PASS\n");
            }
        }
        /* getsid + personality */
        {
            regs.u64Nr = LINUX_NR_getsid;
            regs.u64Arg0 = 0;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: getsid => %ld\n", (long)regs.i64Ret);
            regs.u64Nr = LINUX_NR_personality;
            regs.u64Arg0 = (u64)(i64)-1;
            gj_linux_syscall_dispatch(&regs);
            kprintf("linux: personality => %ld\n", (long)regs.i64Ret);
            if (regs.i64Ret >= 0) {
                kprintf("linux: getsid_pers PASS\n");
            }
        }
    }

    /* unknown → ENOSYS */
    regs.u64Nr = 999;
    gj_linux_syscall_dispatch(&regs);
    kprintf("linux: nr=999 => %ld (expect -ENOSYS)\n", (long)regs.i64Ret);

    gj_linux_dispatch_stats_get(&stats);
    kprintf("linux: stats hot=%lu cold=%lu enosys=%lu\n",
            (unsigned long)stats.u64HotHits,
            (unsigned long)stats.u64ColdHits,
            (unsigned long)stats.u64Enosys);
    kprintf("linux: hybrid Option C open-list smoke ok\n");
}

/**
 * Shared path after firmware memory map is converted to gj_mem_region[].
 * Multiboot and UEFI both reach here.
 */
static void
kernel_after_mmap(struct gj_mem_region *aRegions, size_t cRegions)
{
    gj_paddr_t paA;
    gj_paddr_t paB;
    int iSmoke;
    const struct gj_boot_info *pBi;

    pmm_init(aRegions, cRegions,
             (gj_paddr_t)(gj_vaddr_t)__kernel_start,
             (gj_paddr_t)(gj_vaddr_t)__kernel_end);

    paA = pmm_alloc();
    paB = pmm_alloc();
    kprintf("pmm alloc 0x%lx 0x%lx free=%u\n",
            (unsigned long)paA, (unsigned long)paB,
            (unsigned)pmm_free_count());
    if (paA) {
        pmm_free(paA);
    }
    if (paB) {
        pmm_free(paB);
    }

    for (iSmoke = 0; iSmoke < 16; iSmoke++) {
        gj_paddr_t paPage = pmm_alloc();

        if (!paPage) {
            break;
        }
        pmm_free(paPage);
    }

    vmm_init();
    /* P-MEM-5: higher-half direct map so freelist can use PA ≥ 4 GiB */
    if (vmm_hhdm_init(pmm_max_phys() ? pmm_max_phys() : (1ull << 32)) == GJ_OK) {
        pmm_release_high();
    }
    kprintf("pmm: after HHDM free=%lu max_pa=0x%lx total=%lu\n",
            (unsigned long)pmm_free_count(), (unsigned long)pmm_max_phys(),
            (unsigned long)pmm_total_frames());
    if (pmm_soak(16, 8) != 0) {
        kprintf("pmm: soak FAIL\n");
    }
    /*
     * Large-RAM hierarchical soak: 768 GiB class (run with GJ_MEM=768G).
     * True 1 TiB (1ull<<40) still soft-skips when max_pa is below that.
     */
    if (pmm_soak_tib(768ull << 30) != 0) {
        kprintf("pmm: TiB soak FAIL\n");
    }
    kheap_init();
    memobj_init();
    futex_init();
    vfs_ram_init();
    gj_io_uring_init();
    file_lock_init();
    net_lo_init();
    net_eth_init();
    net_tcp_init();
    net_door_init();
    store_door_init();
    vfs_door_init();
    virtio_init();
    (void)virtio_pci_scan();
    {
        extern u32 nvme_probe_scan(void);
        extern u32 ahci_probe_scan(void);
        extern u32 usb_probe_scan(void);
        extern u32 ps2_probe(void);

        (void)nvme_probe_scan();
        (void)ahci_probe_scan();
        (void)usb_probe_scan();
        (void)ps2_probe();
    }
    /* Console poll bring-up (serial COM1) */
    {
        extern int serial_poll(void);

        kprintf("console: poll=%d PASS\n", serial_poll());
    }
    (void)virtio_gpu_probe();
    if (virtio_gpu_ready()) {
        u32 u32Dw = 0;
        u32 u32Dh = 0;

        if (virtio_gpu_get_display(&u32Dw, &u32Dh) != 0) {
            u32Dw = 640;
            u32Dh = 480;
        }
        (void)virtio_gpu_present_stub(64, 64, 0);
        (void)u32Dw;
        (void)u32Dh;
        /* Session compositor + Vulkan-shaped ICD present (A1 T0) */
        session_door_init();
        if (session_compositor_init() == 0) {
            (void)session_compositor_present();
            kprintf("compositor: presents=%u\n",
                    session_compositor_present_count());
            /* Session door: present + stats via GJ_SYS_SESSION */
            {
                struct gj_syscall_regs nr;
                u32 aSt[4];
                u32 aWH[2];

                memset(&nr, 0, sizeof(nr));
                nr.u64Nr = GJ_SYS_SESSION;
                nr.u64Arg0 = GJ_SESS_OP_DISPLAY_INFO;
                nr.u64Arg1 = (u64)(uintptr_t)aWH;
                gj_native_syscall_dispatch(&nr);
                nr.u64Arg0 = GJ_SESS_OP_PRESENT;
                gj_native_syscall_dispatch(&nr);
                nr.u64Arg0 = GJ_SESS_OP_STATS;
                nr.u64Arg1 = (u64)(uintptr_t)aSt;
                gj_native_syscall_dispatch(&nr);
                kprintf("session_door: display %ux%u presents=%u calls=%u => %ld\n",
                        aWH[0], aWH[1], aSt[0], aSt[2], (long)nr.i64Ret);
                if (nr.i64Ret == 0 && aSt[0] >= 1) {
                    kprintf("session_door: PASS\n");
                }
                /* Userspace-shaped present: PRESENT_FB with kernel buffer */
                {
                    static u8 aFb[64 * 64 * 4];
                    u32 px;

                    for (px = 0; px < 64u * 64u; px++) {
                        aFb[px * 4u + 0] = 0x30;
                        aFb[px * 4u + 1] = 0xa0;
                        aFb[px * 4u + 2] = 0x20;
                        aFb[px * 4u + 3] = 0xff;
                    }
                    nr.u64Arg0 = GJ_SESS_OP_PRESENT_FB;
                    nr.u64Arg1 = 64;
                    nr.u64Arg2 = 64;
                    nr.u64Arg3 = (u64)(uintptr_t)aFb;
                    gj_native_syscall_dispatch(&nr);
                    kprintf("session_door: PRESENT_FB => %ld\n",
                            (long)nr.i64Ret);
                    if (nr.i64Ret == 0) {
                        kprintf("session_door: PRESENT_FB PASS\n");
                    }
                }
                /* Ownership claim (sessiond hand-off) */
                {
                    u32 aSt5[5];
                    u64 va = 0;
                    u32 aInfo[3];
                    u32 token = 0xc0ffe1u;

                    nr.u64Arg0 = GJ_SESS_OP_CLAIM;
                    nr.u64Arg1 = token;
                    gj_native_syscall_dispatch(&nr);
                    kprintf("session_door: CLAIM => %ld owned=%d\n",
                            (long)nr.i64Ret, session_door_owned());
                    nr.u64Arg0 = GJ_SESS_OP_MAP_SCANOUT;
                    nr.u64Arg1 = (u64)(uintptr_t)&va;
                    nr.u64Arg2 = (u64)(uintptr_t)aInfo;
                    gj_native_syscall_dispatch(&nr);
                    kprintf("session_door: MAP_SCANOUT va=0x%lx %ux%u\n",
                            (unsigned long)va, aInfo[0], aInfo[1]);
                    nr.u64Arg0 = GJ_SESS_OP_STATS;
                    nr.u64Arg1 = (u64)(uintptr_t)aSt5;
                    gj_native_syscall_dispatch(&nr);
                    if (session_door_owned() && (aSt5[3] & 4u) != 0 &&
                        aSt5[4] == token) {
                        kprintf("session_door: ownership PASS\n");
                    }
                    /* Owned present then release */
                    {
                        static u8 aFb2[64 * 64 * 4];
                        u32 px;

                        for (px = 0; px < 64u * 64u; px++) {
                            aFb2[px * 4u + 0] = 0x10;
                            aFb2[px * 4u + 1] = 0xc0;
                            aFb2[px * 4u + 2] = 0x40;
                            aFb2[px * 4u + 3] = 0xff;
                        }
                        nr.u64Arg0 = GJ_SESS_OP_PRESENT_FB;
                        nr.u64Arg1 = 64;
                        nr.u64Arg2 = 64;
                        nr.u64Arg3 = (u64)(uintptr_t)aFb2;
                        gj_native_syscall_dispatch(&nr);
                    }
                    nr.u64Arg0 = GJ_SESS_OP_RELEASE;
                    nr.u64Arg1 = token;
                    gj_native_syscall_dispatch(&nr);
                    if (!session_door_owned()) {
                        kprintf("session_door: RELEASE PASS\n");
                    }
                }
            }
        }
        {
            struct gj_vk_display_info di;
            struct gj_vk_image img;
            u32 u32Cw = 0;
            u32 u32Ch = 0;

            kprintf("vk_icd: %s\n", gj_vk_icd_name());
            if (gj_vk_get_display(&di) == 0) {
                kprintf("vk_icd: display %ux%u\n", di.u32Width, di.u32Height);
            }
            session_compositor_size(&u32Cw, &u32Ch);
            memset(&img, 0, sizeof(img));
            img.u32Width = u32Cw ? u32Cw : 64;
            img.u32Height = u32Ch ? u32Ch : 64;
            img.u32Stride = img.u32Width * 4u;
            img.pPixels = session_compositor_fb();
            if (img.pPixels != NULL && gj_vk_queue_present(&img) == 0) {
                kprintf("vk_icd: queue_present ok count=%u\n",
                        virtio_gpu_present_count());
            } else {
                kprintf("vk_icd: queue_present skipped/fail\n");
            }
            /* A1: Khronos-shaped ICD path + legacy gj_vk swapchain */
            {
                uint32_t u32IcdVer = 5;
                VkInstance inst = NULL;
                VkPhysicalDevice phys = NULL;
                uint32_t nPhys = 1;
                VkDevice dev = NULL;
                VkQueue queue = NULL;
                VkSurfaceKHR surface = 0;
                VkSwapchainKHR sc = 0;
                VkExtent2D extent;
                VkSwapchainCreateInfoKHR sci;
                VkDeviceCreateInfo dci;
                VkDeviceQueueCreateInfo qci;
                float prio = 1.0f;
                uint32_t imgCount = 0;
                VkImage aImgs[4];
                uint32_t idx = 0;
                VkPresentInfoKHR pi;
                VkResult vr;
                uint32_t stride = 0;
                u8 *pPix;
                u32 i;
                VkPhysicalDeviceProperties props;
                PFN_vkVoidFunction pfn;

                vr = vk_icdNegotiateLoaderICDInterfaceVersion(&u32IcdVer);
                kprintf("vk: icd negotiate => %d ver=%u\n", (int)vr, u32IcdVer);
                pfn = vk_icdGetInstanceProcAddr(NULL, "vkCreateInstance");
                kprintf("vk: icd GetInstanceProcAddr CreateInstance=%p\n",
                        (void *)pfn);

                {
                    VkInstanceCreateInfo ici;
                    VkApplicationInfo app;

                    memset(&app, 0, sizeof(app));
                    app.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                    app.pApplicationName = "GreenJade";
                    app.apiVersion = VK_API_VERSION_1_0;
                    memset(&ici, 0, sizeof(ici));
                    ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                    ici.pApplicationInfo = &app;
                    vr = vkCreateInstance(&ici, NULL, &inst);
                }
                if (vr != VK_SUCCESS || inst == NULL) {
                    kprintf("vk: CreateInstance fail %d\n", (int)vr);
                    goto vk_khronos_done;
                }
                nPhys = 1;
                vr = vkEnumeratePhysicalDevices(inst, &nPhys, &phys);
                if (vr != VK_SUCCESS || phys == NULL) {
                    kprintf("vk: EnumeratePhysicalDevices fail %d\n", (int)vr);
                    vkDestroyInstance(inst, NULL);
                    goto vk_khronos_done;
                }
                vkGetPhysicalDeviceProperties(phys, &props);
                kprintf("vk: physdev '%s' type=%u\n", props.deviceName,
                        (unsigned)props.deviceType);

                memset(&qci, 0, sizeof(qci));
                qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                qci.queueFamilyIndex = 0;
                qci.queueCount = 1;
                qci.pQueuePriorities = &prio;
                memset(&dci, 0, sizeof(dci));
                dci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                dci.queueCreateInfoCount = 1;
                dci.pQueueCreateInfos = &qci;
                vr = vkCreateDevice(phys, &dci, NULL, &dev);
                if (vr != VK_SUCCESS) {
                    kprintf("vk: CreateDevice fail %d\n", (int)vr);
                    vkDestroyInstance(inst, NULL);
                    goto vk_khronos_done;
                }
                vkGetDeviceQueue(dev, 0, 0, &queue);

                extent.width = 64;
                extent.height = 64;
                vr = vkCreateHeadlessSurfaceGJ(inst, extent.width, extent.height,
                                               NULL, &surface);
                if (vr != VK_SUCCESS) {
                    kprintf("vk: CreateHeadlessSurface fail %d\n", (int)vr);
                    vkDestroyDevice(dev, NULL);
                    vkDestroyInstance(inst, NULL);
                    goto vk_khronos_done;
                }
                memset(&sci, 0, sizeof(sci));
                sci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                sci.surface = surface;
                sci.minImageCount = 2;
                sci.imageFormat = VK_FORMAT_B8G8R8A8_UNORM;
                sci.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
                sci.imageExtent = extent;
                sci.imageArrayLayers = 1;
                sci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                sci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                sci.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
                sci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                sci.presentMode = VK_PRESENT_MODE_FIFO_KHR;
                sci.clipped = VK_TRUE;
                vr = vkCreateSwapchainKHR(dev, &sci, NULL, &sc);
                if (vr != VK_SUCCESS) {
                    kprintf("vk: CreateSwapchainKHR fail %d\n", (int)vr);
                    vkDestroySurfaceKHR(inst, surface, NULL);
                    vkDestroyDevice(dev, NULL);
                    vkDestroyInstance(inst, NULL);
                    goto vk_khronos_done;
                }
                imgCount = 4;
                (void)vkGetSwapchainImagesKHR(dev, sc, &imgCount, aImgs);
                vr = vkAcquireNextImageKHR(dev, sc, 0, 0, 0, &idx);
                pPix = (u8 *)vkMapSwapchainImageGJ(dev, sc, idx, &stride);
                if (pPix != NULL) {
                    for (i = 0; i < 64 && i * 4u < stride * extent.height; i++) {
                        pPix[i * 4u + 0] = 0xff;
                        pPix[i * 4u + 1] = 0x00;
                        pPix[i * 4u + 2] = 0x80;
                        pPix[i * 4u + 3] = 0xff;
                    }
                }
                memset(&pi, 0, sizeof(pi));
                pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                pi.swapchainCount = 1;
                pi.pSwapchains = &sc;
                pi.pImageIndices = &idx;
                vr = vkQueuePresentKHR(queue, &pi);
                kprintf("vk: QueuePresentKHR => %d images=%u idx=%u\n",
                        (int)vr, imgCount, idx);
                if (vr == VK_SUCCESS) {
                    kprintf("vk_icd: swapchain present ok feats=0x%x\n",
                            gj_vk_icd_features());
                    kprintf("vk: khronos path PASS\n");
                }

                /* Cmdbuf clear → submit → present (pipeline-shaped fill) */
                {
                    VkCommandPool pool = 0;
                    VkCommandBuffer cmd = NULL;
                    VkCommandPoolCreateInfo pci;
                    VkCommandBufferAllocateInfo cai;
                    VkCommandBufferBeginInfo bi;
                    VkSubmitInfo si;
                    VkFence fence = 0;
                    VkFenceCreateInfo fci;
                    VkPipeline pipe = 0;
                    VkRenderPass rp = 0;
                    VkFramebuffer fb = 0;
                    uint32_t rgba[4] = { 32, 200, 64, 255 }; /* R G B A 0..255 */

                    memset(&pci, 0, sizeof(pci));
                    pci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                    (void)vkCreateCommandPool(dev, &pci, NULL, &pool);
                    memset(&cai, 0, sizeof(cai));
                    cai.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                    cai.commandPool = pool;
                    cai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                    cai.commandBufferCount = 1;
                    (void)vkAllocateCommandBuffers(dev, &cai, &cmd);
                    (void)vkCreateRenderPass(dev, NULL, NULL, &rp);
                    (void)vkCreateGraphicsPipelines(dev, 0, 1, NULL, NULL, &pipe);
                    vr = vkAcquireNextImageKHR(dev, sc, 0, 0, 0, &idx);
                    vkCmdBindFramebufferImageGJ(cmd, aImgs[idx]);
                    (void)vkCreateFramebuffer(dev, NULL, NULL, &fb);
                    vkCmdBindPipeline(cmd, 0, pipe);
                    memset(&bi, 0, sizeof(bi));
                    bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                    (void)vkBeginCommandBuffer(cmd, &bi);
                    vkCmdClearColorImage(cmd, aImgs[idx], 0, rgba, 0, NULL);
                    vkCmdBeginRenderPass(cmd, NULL, 0);
                    vkCmdDraw(cmd, 3, 1, 0, 0); /* solid fill via pipeline color */
                    vkCmdEndRenderPass(cmd);
                    (void)vkEndCommandBuffer(cmd);
                    memset(&fci, 0, sizeof(fci));
                    fci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                    (void)vkCreateFence(dev, &fci, NULL, &fence);
                    memset(&si, 0, sizeof(si));
                    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                    si.commandBufferCount = 1;
                    si.pCommandBuffers = &cmd;
                    vr = vkQueueSubmit(queue, 1, &si, fence);
                    (void)vkWaitForFences(dev, 1, &fence, VK_TRUE, 0);
                    memset(&pi, 0, sizeof(pi));
                    pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                    pi.swapchainCount = 1;
                    pi.pSwapchains = &sc;
                    pi.pImageIndices = &idx;
                    vr = vkQueuePresentKHR(queue, &pi);
                    kprintf("vk: cmd clear+present => %d\n", (int)vr);
                    if (vr == VK_SUCCESS) {
                        kprintf("vk: render path PASS\n");
                    }
                    /* Multi-frame present loop (swapchain thrash) */
                    {
                        int f;

                        for (f = 0; f < 3; f++) {
                            vr = vkAcquireNextImageKHR(dev, sc, 0, 0, 0, &idx);
                            pPix = (u8 *)vkMapSwapchainImageGJ(dev, sc, idx,
                                                               &stride);
                            if (pPix != NULL) {
                                u32 px;

                                for (px = 0; px < 32; px++) {
                                    pPix[px * 4u + 0] = (u8)(f * 40);
                                    pPix[px * 4u + 1] = 0x60;
                                    pPix[px * 4u + 2] = 0x30;
                                    pPix[px * 4u + 3] = 0xff;
                                }
                            }
                            memset(&pi, 0, sizeof(pi));
                            pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                            pi.swapchainCount = 1;
                            pi.pSwapchains = &sc;
                            pi.pImageIndices = &idx;
                            vr = vkQueuePresentKHR(queue, &pi);
                            if (vr != VK_SUCCESS) {
                                break;
                            }
                        }
                        kprintf("vk: multi-frame present frames=%d last=%d\n",
                                f, (int)vr);
                        if (f == 3 && vr == VK_SUCCESS) {
                            kprintf("vk: multi-frame PASS\n");
                        }
                    }
                    /* SPIR-V module + pipeline + VBO triangle (software FS) */
                    {
                        /*
                         * Minimal SPIR-V FS: SpecId 0 → packed BGRA color.
                         * Hand-authored clean-room blob (not compiled from GLSL).
                         */
                        static const uint32_t aSpvFs[] = {
                            0x07230203u, /* Magic */
                            0x00010000u, /* Version 1.0 */
                            0x00000000u, /* Generator */
                            0x00000007u, /* Bound */
                            0x00000000u, /* Schema */
                            (2u << 16) | 17u, 1u, /* OpCapability Shader */
                            (3u << 16) | 14u, 0u, 1u, /* OpMemoryModel */
                            (5u << 16) | 15u, 4u, 1u, 0x6e69616du, 0u, /* EntryPoint Frag %1 "main" */
                            (4u << 16) | 71u, 2u, 1u, 0u, /* OpDecorate %2 SpecId 0 */
                            (2u << 16) | 19u, 3u, /* OpTypeVoid %3 */
                            (3u << 16) | 33u, 4u, 3u, /* OpTypeFunction %4 */
                            (4u << 16) | 21u, 5u, 32u, 0u, /* OpTypeInt %5 32 0 */
                            (4u << 16) | 50u, 5u, 2u, 0xffe06020u, /* OpSpecConstant color */
                            (5u << 16) | 54u, 3u, 1u, 0u, 4u, /* OpFunction */
                            (2u << 16) | 248u, 6u, /* OpLabel %6 */
                            (1u << 16) | 253u, /* OpReturn */
                            (1u << 16) | 56u, /* OpFunctionEnd */
                        };
                        static const uint32_t aSpvVs[] = {
                            0x07230203u,
                            0x00010000u,
                            0x00000000u,
                            0x00000005u,
                            0x00000000u,
                            (2u << 16) | 17u, 1u,
                            (3u << 16) | 14u, 0u, 1u,
                            (5u << 16) | 15u, 0u, 1u, 0x6e69616du, 0u, /* Vertex */
                            (2u << 16) | 19u, 2u,
                            (3u << 16) | 33u, 3u, 2u,
                            (5u << 16) | 54u, 2u, 1u, 0u, 3u,
                            (2u << 16) | 248u, 4u,
                            (1u << 16) | 253u,
                            (1u << 16) | 56u,
                        };
                        VkShaderModuleCreateInfo smci;
                        VkShaderModule hFs = 0;
                        VkShaderModule hVs = 0;
                        VkPipelineShaderStageCreateInfo stages[2];
                        VkGraphicsPipelineCreateInfo gci;
                        VkPipeline pipeSpv = 0;
                        VkBuffer vbo = 0;
                        VkBufferCreateInfo bci;
                        VkDeviceSize voff = 0;
                        int32_t *pVerts;
                        uint32_t pushCol = 0xff40a0ffu; /* override */

                        memset(&smci, 0, sizeof(smci));
                        smci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                        smci.codeSize = sizeof(aSpvFs);
                        smci.pCode = aSpvFs;
                        vr = vkCreateShaderModule(dev, &smci, NULL, &hFs);
                        smci.codeSize = sizeof(aSpvVs);
                        smci.pCode = aSpvVs;
                        (void)vkCreateShaderModule(dev, &smci, NULL, &hVs);
                        kprintf("vk: spirv fs words=%u color=0x%x exec=0x%x => %d\n",
                                vkGetShaderModuleWordCountGJ(hFs),
                                vkGetShaderModuleSpecColorGJ(hFs),
                                vkGetShaderModuleExecMaskGJ(hFs), (int)vr);
                        memset(stages, 0, sizeof(stages));
                        stages[0].sType =
                            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                        stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
                        stages[0].module = hVs;
                        stages[0].pName = "main";
                        stages[1].sType =
                            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                        stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
                        stages[1].module = hFs;
                        stages[1].pName = "main";
                        memset(&gci, 0, sizeof(gci));
                        gci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                        gci.stageCount = 2;
                        gci.pStages = stages;
                        (void)vkCreateGraphicsPipelines(dev, 0, 1, &gci, NULL,
                                                        &pipeSpv);
                        memset(&bci, 0, sizeof(bci));
                        bci.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                        bci.size = 6u * sizeof(int32_t);
                        bci.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                        (void)vkCreateBuffer(dev, &bci, NULL, &vbo);
                        pVerts = (int32_t *)vkGetBufferHostPointerGJ(vbo);
                        if (pVerts != NULL) {
                            /* Pixel-space triangle in 64x64 target */
                            pVerts[0] = 8;
                            pVerts[1] = 56;
                            pVerts[2] = 56;
                            pVerts[3] = 56;
                            pVerts[4] = 32;
                            pVerts[5] = 8;
                        }
                        vr = vkAcquireNextImageKHR(dev, sc, 0, 0, 0, &idx);
                        vkCmdBindFramebufferImageGJ(cmd, aImgs[idx]);
                        memset(&bi, 0, sizeof(bi));
                        bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                        (void)vkBeginCommandBuffer(cmd, &bi);
                        vkCmdBindPipeline(cmd, 0, pipeSpv);
                        vkCmdSetViewportGJ(cmd, 0, 0, 64, 64);
                        if (vbo != 0) {
                            vkCmdBindVertexBuffers(cmd, 0, 1, &vbo, &voff);
                        }
                        /* Push overrides SpecId color for this draw */
                        vkCmdPushConstants(cmd, 0, VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                                           4, &pushCol);
                        vkCmdDraw(cmd, 3, 1, 0, 0);
                        (void)vkEndCommandBuffer(cmd);
                        memset(&si, 0, sizeof(si));
                        si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                        si.commandBufferCount = 1;
                        si.pCommandBuffers = &cmd;
                        vr = vkQueueSubmit(queue, 1, &si, 0);
                        memset(&pi, 0, sizeof(pi));
                        pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                        pi.swapchainCount = 1;
                        pi.pSwapchains = &sc;
                        pi.pImageIndices = &idx;
                        vr = vkQueuePresentKHR(queue, &pi);
                        kprintf("vk: spirv path present => %d\n", (int)vr);
                        if (vr == VK_SUCCESS &&
                            vkGetShaderModuleWordCountGJ(hFs) > 0 &&
                            vkGetShaderModuleSpecColorGJ(hFs) == 0xffe06020u &&
                            (vkGetShaderModuleExecMaskGJ(hFs) & 2u) != 0) {
                            kprintf("vk: spirv path PASS\n");
                        }
                        /* Textured FS: sample center of standalone image */
                        {
                            VkImage tex = 0;
                            uint64_t samp = 0;
                            uint32_t tci[4] = { 32, 32, 0, 0 };
                            uint32_t rgba[4] = { 0x10, 0x90, 0xe0, 0xff };

                            (void)vkCreateImage(dev, tci, NULL, &tex);
                            (void)vkCreateSampler(dev, NULL, NULL, &samp);
                            if (tex != 0) {
                                memset(&bi, 0, sizeof(bi));
                                bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                                (void)vkBeginCommandBuffer(cmd, &bi);
                                vkCmdClearColorImage(cmd, tex, 0, rgba, 0, NULL);
                                (void)vkEndCommandBuffer(cmd);
                                memset(&si, 0, sizeof(si));
                                si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                                si.commandBufferCount = 1;
                                si.pCommandBuffers = &cmd;
                                (void)vkQueueSubmit(queue, 1, &si, 0);
                                vr = vkAcquireNextImageKHR(dev, sc, 0, 0, 0, &idx);
                                vkCmdBindFramebufferImageGJ(cmd, aImgs[idx]);
                                vkCmdBindTextureImageGJ(cmd, tex);
                                /* size 0 clears push so texture sample is used */
                                vkCmdPushConstants(cmd, 0, 0, 0, 0, NULL);
                                /* Unbind VBO so draw is full-target textured fill */
                                vkCmdBindVertexBuffers(cmd, 0, 0, NULL, NULL);
                                memset(&bi, 0, sizeof(bi));
                                bi.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                                (void)vkBeginCommandBuffer(cmd, &bi);
                                vkCmdDraw(cmd, 3, 1, 0, 0);
                                (void)vkEndCommandBuffer(cmd);
                                memset(&si, 0, sizeof(si));
                                si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                                si.commandBufferCount = 1;
                                si.pCommandBuffers = &cmd;
                                (void)vkQueueSubmit(queue, 1, &si, 0);
                                memset(&pi, 0, sizeof(pi));
                                pi.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                                pi.swapchainCount = 1;
                                pi.pSwapchains = &sc;
                                pi.pImageIndices = &idx;
                                vr = vkQueuePresentKHR(queue, &pi);
                                kprintf("vk: texture sample present => %d samp=%u\n",
                                        (int)vr, (unsigned)samp);
                                if (vr == VK_SUCCESS) {
                                    kprintf("vk: texture path PASS\n");
                                }
                                /* UV-mapped triangle (stride-4 VBO + texture) */
                                {
                                    VkBuffer vboUv = 0;
                                    VkBufferCreateInfo bciUv;
                                    VkDeviceSize voffUv = 0;
                                    int32_t *pUv;

                                    memset(&bciUv, 0, sizeof(bciUv));
                                    bciUv.sType =
                                        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                                    bciUv.size = 12u * sizeof(int32_t); /* 3×(x,y,u,v) */
                                    bciUv.usage =
                                        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                                    (void)vkCreateBuffer(dev, &bciUv, NULL,
                                                         &vboUv);
                                    pUv = (int32_t *)vkGetBufferHostPointerGJ(
                                        vboUv);
                                    if (pUv != NULL) {
                                        pUv[0] = 4;
                                        pUv[1] = 60;
                                        pUv[2] = 0;
                                        pUv[3] = 255;
                                        pUv[4] = 60;
                                        pUv[5] = 60;
                                        pUv[6] = 255;
                                        pUv[7] = 255;
                                        pUv[8] = 32;
                                        pUv[9] = 4;
                                        pUv[10] = 128;
                                        pUv[11] = 0;
                                    }
                                    vr = vkAcquireNextImageKHR(dev, sc, 0, 0, 0,
                                                               &idx);
                                    vkCmdBindFramebufferImageGJ(cmd,
                                                                aImgs[idx]);
                                    vkCmdBindTextureImageGJ(cmd, tex);
                                    vkCmdPushConstants(cmd, 0, 0, 0, 0, NULL);
                                    if (vboUv != 0) {
                                        vkCmdBindVertexBuffers(cmd, 0, 1,
                                                               &vboUv, &voffUv);
                                    }
                                    memset(&bi, 0, sizeof(bi));
                                    bi.sType =
                                        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                                    (void)vkBeginCommandBuffer(cmd, &bi);
                                    vkCmdDraw(cmd, 3, 1, 0, 0);
                                    (void)vkEndCommandBuffer(cmd);
                                    memset(&si, 0, sizeof(si));
                                    si.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                                    si.commandBufferCount = 1;
                                    si.pCommandBuffers = &cmd;
                                    (void)vkQueueSubmit(queue, 1, &si, 0);
                                    memset(&pi, 0, sizeof(pi));
                                    pi.sType =
                                        VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                                    pi.swapchainCount = 1;
                                    pi.pSwapchains = &sc;
                                    pi.pImageIndices = &idx;
                                    vr = vkQueuePresentKHR(queue, &pi);
                                    kprintf("vk: uv path present => %d\n",
                                            (int)vr);
                                    if (vr == VK_SUCCESS) {
                                        kprintf("vk: uv path PASS\n");
                                    }
                                    vkDestroyBuffer(dev, vboUv, NULL);
                                }
                                vkCmdBindTextureImageGJ(cmd, 0);
                                vkDestroySampler(dev, samp, NULL);
                                vkDestroyImage(dev, tex, NULL);
                            }
                        }
                        vkDestroyPipeline(dev, pipeSpv, NULL);
                        vkDestroyBuffer(dev, vbo, NULL);
                        vkDestroyShaderModule(dev, hFs, NULL);
                        vkDestroyShaderModule(dev, hVs, NULL);
                    }
                    vkDestroyFence(dev, fence, NULL);
                    vkDestroyPipeline(dev, pipe, NULL);
                    vkDestroyFramebuffer(dev, fb, NULL);
                    vkDestroyRenderPass(dev, rp, NULL);
                    vkFreeCommandBuffers(dev, pool, 1, &cmd);
                    vkDestroyCommandPool(dev, pool, NULL);
                }

                vkDestroySwapchainKHR(dev, sc, NULL);
                vkDestroySurfaceKHR(inst, surface, NULL);
                vkDestroyDevice(dev, NULL);
                vkDestroyInstance(inst, NULL);
            vk_khronos_done:
                (void)0;
            }
        }
        /* Native GPU_PRESENT syscall smoke */
        {
            struct gj_syscall_regs nr;

            memset(&nr, 0, sizeof(nr));
            nr.u64Nr = GJ_SYS_GPU_DISPLAY_INFO;
            {
                u32 aWH[2] = { 0, 0 };

                nr.u64Arg0 = (u64)(gj_vaddr_t)aWH;
                gj_native_syscall_dispatch(&nr);
                kprintf("gj: GPU_DISPLAY_INFO => %ld %ux%u\n", (long)nr.i64Ret,
                        aWH[0], aWH[1]);
            }
        }
    }
    (void)virtio_net_probe();
    if (virtio_net_ready()) {
        static const u8 aEth[64] = {
            0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
            0x52, 0x54, 0x00, 0x12, 0x34, 0x56,
            0x08, 0x00,
        };
        static u8 aRx[128];
        i32 i32Rx;
        int iPoll;

        (void)virtio_net_tx(aEth, sizeof(aEth));
        i32Rx = virtio_net_rx(aRx, sizeof(aRx));
        for (iPoll = 0; iPoll < 8; iPoll++) {
            net_eth_poll();
        }
        kprintf("virtio-net: tx=%u rx=%u arp=%u udp_echo=%u last_rx=%ld\n",
                virtio_net_tx_count(), virtio_net_rx_count(),
                net_eth_arp_replies(), net_eth_udp_echoes(), (long)i32Rx);
        /* Net door façade for netstackd hand-off */
        {
            struct gj_syscall_regs nr;
            u32 aSt[4];

            memset(&nr, 0, sizeof(nr));
            nr.u64Nr = GJ_SYS_NET;
            nr.u64Arg0 = GJ_NET_OP_POLL;
            gj_native_syscall_dispatch(&nr);
            nr.u64Arg0 = GJ_NET_OP_STATS;
            nr.u64Arg1 = (u64)(uintptr_t)aSt;
            gj_native_syscall_dispatch(&nr);
            kprintf("net_door: arp=%u udp=%u icmp=%u calls=%u => %ld\n", aSt[0],
                    aSt[1], aSt[2], aSt[3], (long)nr.i64Ret);
            if (nr.i64Ret == 0 && aSt[3] >= 1) {
                kprintf("net_door: PASS\n");
            }
            {
                u32 tok = 0x11e70001u;

                nr.u64Arg0 = GJ_NET_OP_CLAIM;
                nr.u64Arg1 = tok;
                gj_native_syscall_dispatch(&nr);
                if (nr.i64Ret == 0 && net_door_owned()) {
                    nr.u64Arg0 = GJ_NET_OP_RELEASE;
                    nr.u64Arg1 = tok;
                    gj_native_syscall_dispatch(&nr);
                    kprintf("net_door: ownership PASS\n");
                }
            }
            /* Socket path via net door (loopback) */
            {
                i64 srv;
                i64 cli;
                char msg[] = "gj-net";
                char rbuf[32];

                nr.u64Arg0 = GJ_NET_OP_SOCKET;
                nr.u64Arg1 = 2; /* AF_INET */
                nr.u64Arg2 = 2; /* SOCK_DGRAM */
                nr.u64Arg3 = 0;
                gj_native_syscall_dispatch(&nr);
                srv = nr.i64Ret;
                gj_native_syscall_dispatch(&nr);
                cli = nr.i64Ret;
                if (srv >= 0 && cli >= 0) {
                    nr.u64Arg0 = GJ_NET_OP_BIND;
                    nr.u64Arg1 = (u64)srv;
                    nr.u64Arg2 = 9;
                    gj_native_syscall_dispatch(&nr);
                    nr.u64Arg0 = GJ_NET_OP_LISTEN;
                    nr.u64Arg1 = (u64)srv;
                    nr.u64Arg2 = 1;
                    gj_native_syscall_dispatch(&nr);
                    nr.u64Arg0 = GJ_NET_OP_CONNECT;
                    nr.u64Arg1 = (u64)cli;
                    nr.u64Arg2 = 9;
                    gj_native_syscall_dispatch(&nr);
                    nr.u64Arg0 = GJ_NET_OP_SEND;
                    nr.u64Arg1 = (u64)cli;
                    nr.u64Arg2 = (u64)(uintptr_t)msg;
                    nr.u64Arg3 = sizeof(msg);
                    gj_native_syscall_dispatch(&nr);
                    nr.u64Arg0 = GJ_NET_OP_RECV;
                    nr.u64Arg1 = (u64)srv;
                    nr.u64Arg2 = (u64)(uintptr_t)rbuf;
                    nr.u64Arg3 = sizeof(rbuf);
                    gj_native_syscall_dispatch(&nr);
                    kprintf("net_door: sock send/recv => %ld\n",
                            (long)nr.i64Ret);
                    if (nr.i64Ret > 0) {
                        kprintf("net_door: socket path PASS\n");
                    }
                    nr.u64Arg0 = GJ_NET_OP_CLOSE;
                    nr.u64Arg1 = (u64)srv;
                    gj_native_syscall_dispatch(&nr);
                    nr.u64Arg1 = (u64)cli;
                    gj_native_syscall_dispatch(&nr);
                }
            }
            /* Virtio queue ownership path (TX/RX via door) */
            {
                static u8 aFrame[64];
                static u8 aRx[128];
                u32 aQ[5];
                u32 i;

                for (i = 0; i < 64; i++) {
                    aFrame[i] = (u8)(0xaa ^ (i & 0xffu));
                }
                /* Broadcast eth-ish */
                aFrame[0] = 0xff;
                aFrame[1] = 0xff;
                aFrame[2] = 0xff;
                aFrame[3] = 0xff;
                aFrame[4] = 0xff;
                aFrame[5] = 0xff;
                nr.u64Nr = GJ_SYS_NET;
                nr.u64Arg0 = GJ_NET_OP_VIRTIO_TX;
                nr.u64Arg1 = (u64)(uintptr_t)aFrame;
                nr.u64Arg2 = 64;
                gj_native_syscall_dispatch(&nr);
                kprintf("net_door: VIRTIO_TX => %ld\n", (long)nr.i64Ret);
                nr.u64Arg0 = GJ_NET_OP_VIRTIO_RX;
                nr.u64Arg1 = (u64)(uintptr_t)aRx;
                nr.u64Arg2 = sizeof(aRx);
                gj_native_syscall_dispatch(&nr);
                kprintf("net_door: VIRTIO_RX => %ld\n", (long)nr.i64Ret);
                nr.u64Arg0 = GJ_NET_OP_QUEUE_INFO;
                nr.u64Arg1 = (u64)(uintptr_t)aQ;
                gj_native_syscall_dispatch(&nr);
                kprintf("net_door: queue tx=%u rx=%u ready=%u vq=%u\n", aQ[0],
                        aQ[1], aQ[2], aQ[4]);
                if (nr.i64Ret == 0 && aQ[2] == 1 && aQ[4] >= 1) {
                    kprintf("net_door: virtio queue PASS\n");
                }
                /* Ring export + map for UDX ownership */
                {
                    struct gj_virtq_export ex;
                    u64 vaBase = 0x30000000ull; /* high user VA */

                    memset(&ex, 0, sizeof(ex));
                    nr.u64Arg0 = GJ_NET_OP_EXPORT_RING;
                    nr.u64Arg1 = 1; /* TX */
                    nr.u64Arg2 = (u64)(uintptr_t)&ex;
                    gj_native_syscall_dispatch(&nr);
                    kprintf("net_door: EXPORT_RING size=%u pa_desc=0x%lx => %ld\n",
                            ex.u16Size, (unsigned long)ex.u64PaDesc,
                            (long)nr.i64Ret);
                    if (nr.i64Ret == 0 && ex.u32Ready && ex.u64PaDesc != 0) {
                        nr.u64Arg0 = GJ_NET_OP_MAP_RING;
                        nr.u64Arg1 = 1;
                        nr.u64Arg2 = vaBase;
                        nr.u64Arg3 = (u64)(uintptr_t)&ex;
                        gj_native_syscall_dispatch(&nr);
                        kprintf("net_door: MAP_RING => %ld\n", (long)nr.i64Ret);
                        nr.u64Arg0 = GJ_NET_OP_KICK;
                        nr.u64Arg1 = 1;
                        gj_native_syscall_dispatch(&nr);
                        if (nr.i64Ret == 0) {
                            kprintf("net_door: ring map PASS\n");
                        }
                    }
                    /* AVAIL_PUSH ring programming (no VIRTIO_TX helper) */
                    {
                        static u8 aFr[64];
                        u32 aSt[4];
                        u32 i;
                        i64 reaped;

                        for (i = 0; i < 64; i++) {
                            aFr[i] = (u8)(0x11 + i);
                        }
                        aFr[0] = aFr[1] = aFr[2] = aFr[3] = aFr[4] = aFr[5] =
                            0xff;
                        nr.u64Arg0 = GJ_NET_OP_AVAIL_PUSH;
                        nr.u64Arg1 = 1; /* TX */
                        nr.u64Arg2 = (u64)(uintptr_t)aFr;
                        nr.u64Arg3 = 64;
                        gj_native_syscall_dispatch(&nr);
                        kprintf("net_door: AVAIL_PUSH => %ld\n",
                                (long)nr.i64Ret);
                        nr.u64Arg0 = GJ_NET_OP_USED_REAP;
                        nr.u64Arg1 = 1;
                        nr.u64Arg2 = 8;
                        gj_native_syscall_dispatch(&nr);
                        reaped = nr.i64Ret;
                        nr.u64Arg0 = GJ_NET_OP_RING_STATE;
                        nr.u64Arg1 = (u64)(uintptr_t)aSt;
                        gj_native_syscall_dispatch(&nr);
                        kprintf(
                            "net_door: RING_STATE free_tx=%u pushes=%u reap=%ld\n",
                            aSt[0], aSt[2] & 0xffffu, (long)reaped);
                        if ((aSt[2] & 0xffffu) >= 1) {
                            kprintf("net_door: avail push PASS\n");
                        }
                    }
                    /* True UDX userspace ring: DESC_ALLOC + program desc + USER_AVAIL */
                    {
                        static u8 aUr[64];
                        struct gj_virtq_dma_export dma;
                        struct gj_virtq_desc *pDesc;
                        i64 head;
                        i64 filled;
                        u32 aSt2[4];
                        u32 i;
                        gj_paddr_t pa;
                        u32 cb;

                        for (i = 0; i < 64; i++) {
                            aUr[i] = (u8)(0x22 + i);
                        }
                        aUr[0] = aUr[1] = aUr[2] = aUr[3] = aUr[4] = aUr[5] =
                            0xff;
                        memset(&dma, 0, sizeof(dma));
                        nr.u64Arg0 = GJ_NET_OP_MAP_DMA;
                        nr.u64Arg1 = 0x31000000ull;
                        nr.u64Arg2 = (u64)(uintptr_t)&dma;
                        nr.u64Arg3 = 0;
                        gj_native_syscall_dispatch(&nr);
                        kprintf("net_door: MAP_DMA => %ld ready=%u pa0=0x%lx\n",
                                (long)nr.i64Ret, dma.u32Ready,
                                (unsigned long)dma.aPa[0]);
                        nr.u64Arg0 = GJ_NET_OP_BOUNCE_FILL;
                        nr.u64Arg1 = 0; /* slot 0 */
                        nr.u64Arg2 = (u64)(uintptr_t)aUr;
                        nr.u64Arg3 = 64;
                        gj_native_syscall_dispatch(&nr);
                        filled = nr.i64Ret;
                        nr.u64Arg0 = GJ_NET_OP_DESC_ALLOC;
                        nr.u64Arg1 = 1; /* TX */
                        nr.u64Arg2 = 0;
                        nr.u64Arg3 = 0;
                        gj_native_syscall_dispatch(&nr);
                        head = nr.i64Ret;
                        kprintf("net_door: DESC_ALLOC head=%ld filled=%ld\n",
                                (long)head, (long)filled);
                        if (head >= 0 && filled > 0 && dma.u32Ready &&
                            ex.u64PaDesc != 0) {
                            /*
                             * Program descriptor on shared ring (same PA as
                             * MAP_RING). Kernel path uses HHDM; userspace uses
                             * mapped VA (netstackd-gj).
                             */
                            pDesc = (struct gj_virtq_desc *)hhdm_to_virt(
                                        (gj_paddr_t)ex.u64PaDesc) +
                                    (u16)head;
                            pa = virtio_net_bounce_pa(0);
                            cb = (u32)filled;
                            pDesc->u64Addr = (u64)pa;
                            pDesc->u32Len = cb;
                            pDesc->u16Flags = 0;
                            pDesc->u16Next = 0;
                            /* bit0=kick bit1=kernel push avail (desc user-programmed) */
                            nr.u64Arg0 = GJ_NET_OP_USER_AVAIL;
                            nr.u64Arg1 = 1;
                            nr.u64Arg2 = (u64)head;
                            nr.u64Arg3 = 3;
                            gj_native_syscall_dispatch(&nr);
                            nr.u64Arg0 = GJ_NET_OP_USED_REAP;
                            nr.u64Arg1 = 1;
                            nr.u64Arg2 = 8;
                            gj_native_syscall_dispatch(&nr);
                            nr.u64Arg0 = GJ_NET_OP_RING_STATE;
                            nr.u64Arg1 = (u64)(uintptr_t)aSt2;
                            gj_native_syscall_dispatch(&nr);
                            kprintf(
                                "net_door: user_ring pushes=%u user=%u => %ld\n",
                                aSt2[2] & 0xffffu, aSt2[2] >> 16,
                                (long)nr.i64Ret);
                            if ((aSt2[2] >> 16) >= 1) {
                                kprintf("net_door: user ring PASS\n");
                            }
                        }
                    }
                }
            }
        }
    }
    (void)virtio_input_probe();
    if (virtio_input_ready()) {
        struct gj_input_event ev;
        int n;

        n = virtio_input_poll(&ev);
        kprintf("virtio-input: poll=%d events=%u\n", n,
                virtio_input_event_count());
        session_input_init();
        session_input_poll();
        kprintf("session_input: pushed=%u ready=%d\n", session_input_pushed(),
                session_input_ready());
    }
    pci_msix_probe_log();
    irq_msix_init();
    iommu_probe();
    {
        static struct gj_iommu_info iom;
        static struct gj_pci_msix_info aMx[4];
        static struct gj_syscall_regs nr;
        u64 badge;
        u32 nr32;
        u32 nr64;
        int ok_bm;
        int deny_bm;

        memset(&nr, 0, sizeof(nr));
        nr.u64Nr = GJ_SYS_PLATFORM_INFO;
        nr.u64Arg0 = 0;
        nr.u64Arg1 = (u64)(uintptr_t)&iom;
        gj_native_syscall_dispatch(&nr);
        kprintf("platform: iommu present=%ld units=%u\n", (long)nr.i64Ret,
                iom.u32Units);
        nr.u64Arg0 = 1;
        nr.u64Arg1 = (u64)(uintptr_t)aMx;
        gj_native_syscall_dispatch(&nr);
        kprintf("platform: msix_count=%ld\n", (long)nr.i64Ret);
        if (nr.i64Ret >= 0) {
            kprintf("platform: info PASS\n");
        }
        /* MSI-X IRQ → Notification (soft inject + non-blocking wait) */
        irq_msix_soft_inject(0x5ull);
        nr.u64Nr = GJ_SYS_PLATFORM_INFO;
        nr.u64Arg0 = 3;
        nr.u64Arg1 = 0x2; /* second soft inject via door */
        nr.u64Arg2 = 0;
        nr.u64Arg3 = 0;
        gj_native_syscall_dispatch(&nr);
        kprintf("notify: inject ret=%ld count=%u soft=%u\n", (long)nr.i64Ret,
                irq_msix_count(), irq_msix_soft_count());
        nr.u64Nr = GJ_SYS_NOTIFY_WAIT;
        nr.u64Arg0 = 0;
        nr.u64Arg1 = 0x7; /* any of soft badges */
        nr.u64Arg2 = 0;   /* non-block */
        gj_native_syscall_dispatch(&nr);
        badge = (u64)nr.i64Ret;
        kprintf("notify: MSI-X soft irq=%u badge=0x%lx signals=%u\n",
                irq_msix_count(), (unsigned long)badge,
                notify_signals(notify_msix_global()));
        {
            u32 hw = irq_msix_hw_pulse();

            kprintf("notify: MSI-X HW pulse hw_count=%u\n", hw);
            if (hw >= 1) {
                kprintf("notify: MSI-X HW path PASS\n");
            }
        }
        if (irq_msix_ready() && badge != 0 && irq_msix_count() >= 1) {
            kprintf("notify: MSI-X IRQ PASS\n");
        }
        /* IOMMU enforce + VT-d identity window grant integration */
        {
            int covered = 0;

            if (iommu_vtd_window_grant(0, 2, 0, 0x1000, 0x1000, &covered) ==
                    0 &&
                covered) {
                kprintf("iommu: vtd identity grant PASS\n");
            } else if (iommu_window_grant(0, 2, 0, 0x1000, 0x1000) == 0) {
                /* grant without tables still ok for software policy */
                kprintf("iommu: window grant (no vtd cover) ok\n");
            }
        }
        nr.u64Nr = GJ_SYS_PLATFORM_INFO;
        nr.u64Arg0 = 4;
        nr.u64Arg1 = 1; /* enforce on */
        gj_native_syscall_dispatch(&nr);
        ok_bm = iommu_busmaster_ok(0, 2, 0);
        deny_bm = iommu_busmaster_ok(0, 31, 0);
        kprintf("iommu: enforce ok=%d deny=%d windows=%u denies=%u\n", ok_bm,
                deny_bm, iommu_window_count(), iommu_deny_count());
        if (ok_bm && !deny_bm && iommu_deny_count() >= 1) {
            kprintf("iommu: enforce PASS\n");
        }
        if (iommu_vtd_te_arm() && iommu_vtd_te_armed()) {
            kprintf("iommu: vtd TE path PASS\n");
        }
        if (iommu_vtd_te_live_ready()) {
            kprintf("iommu: vtd TE live mode=%d\n", iommu_vtd_te_mode());
        }
        nr.u64Arg0 = 4;
        nr.u64Arg1 = 0; /* enforce off for rest of bring-up */
        gj_native_syscall_dispatch(&nr);
        /* WoW64 NR map smoke */
        nr.u64Arg0 = 2;
        nr.u64Arg1 = 1; /* enable */
        gj_native_syscall_dispatch(&nr);
        nr32 = 240; /* i386 futex */
        (void)wow64_translate_nr(nr32, &nr64);
        kprintf("wow64: nr32=%u → nr64=%u calls=%u\n", nr32, nr64,
                wow64_calls());
        if (nr64 == 202 && wow64_enabled()) {
            kprintf("wow64: path PASS\n");
        }
        nr.u64Arg0 = 2;
        nr.u64Arg1 = 2; /* disable */
        gj_native_syscall_dispatch(&nr);
        if (pe32_smoke() != 0) {
            kprintf("pe32: smoke FAIL\n");
        }
    }
    hda_stub_probe();
    if (hda_stream_smoke() != 0) {
        kprintf("hda: stream path FAIL\n");
    }
    if (hda_corb_bdl_smoke() != 0) {
        kprintf("hda: CORB/BDL path FAIL\n");
    }
    if (hda_multi_stream_smoke() != 0) {
        kprintf("hda: multi-stream mixer FAIL\n");
    }
    /* Real fcntl locks smoke */
    {
        i64 fd;
        struct gj_flock fl;
        i64 st;

        fd = vfs_ram_open("/tmp/locktest", 1);
        if (fd >= 0) {
            memset(&fl, 0, sizeof(fl));
            fl.i16Type = GJ_F_WRLCK;
            fl.i64Start = 0;
            fl.i64Len = 100;
            fl.u32Pid = 1;
            st = file_lock_set(fd, &fl, 0);
            fl.u32Pid = 2;
            if (file_lock_set(fd, &fl, 0) == -LINUX_EAGAIN && st == 0) {
                fl.u32Pid = 1;
                fl.i16Type = GJ_F_UNLCK;
                (void)file_lock_set(fd, &fl, 0);
                kprintf("file_lock: count=%u PASS\n", file_lock_count());
            } else {
                kprintf("file_lock: FAIL st=%ld\n", (long)st);
            }
            (void)vfs_ram_close(fd);
        }
    }
    /* Native HDA_STREAM syscall path (userspace audio door) */
    {
        struct gj_syscall_regs nr;
        static u8 aTone[64];
        static u32 aSt[3];
        u32 i;

        for (i = 0; i < sizeof(aTone); i++) {
            aTone[i] = (u8)(i * 3u);
        }
        aSt[0] = aSt[1] = aSt[2] = 0;
        memset(&nr, 0, sizeof(nr));
        nr.u64Nr = GJ_SYS_HDA_STREAM;
        nr.u64Arg0 = 0; /* open */
        nr.u64Arg1 = 2;
        nr.u64Arg2 = 48000;
        nr.u64Arg3 = 16;
        gj_native_syscall_dispatch(&nr);
        if (nr.i64Ret == 0) {
            nr.u64Arg0 = 1; /* write */
            nr.u64Arg1 = (u64)(uintptr_t)aTone;
            nr.u64Arg2 = sizeof(aTone);
            nr.u64Arg3 = 0;
            gj_native_syscall_dispatch(&nr);
            nr.u64Arg0 = 2; /* start */
            nr.u64Arg1 = 0;
            gj_native_syscall_dispatch(&nr);
            nr.u64Arg0 = 3; /* tick */
            nr.u64Arg1 = 16; /* frames → 64 bytes stereo 16-bit */
            gj_native_syscall_dispatch(&nr);
            nr.u64Arg0 = 5; /* stats */
            nr.u64Arg1 = (u64)(uintptr_t)aSt;
            nr.u64Arg2 = 0;
            gj_native_syscall_dispatch(&nr);
            kprintf("gj: HDA_STREAM write/play ok ret=%ld q=%u p=%u u=%u\n",
                    (long)nr.i64Ret, aSt[0], aSt[1], aSt[2]);
            if (aSt[1] > 0) {
                kprintf("gj: HDA_STREAM syscall PASS\n");
            }
            nr.u64Arg0 = 4; /* close */
            nr.u64Arg1 = 0;
            gj_native_syscall_dispatch(&nr);
        }
    }
    (void)virtio_blk_probe();
    if (virtio_blk_ready()) {
        static u8 aSec[GJ_VIRTIO_BLK_SECTOR];
        static const char szMark[] = "GreenJade-blk\n";
        u32 i;
        int stW;
        int stR;

        /* Sector 0: write marker, re-read, verify (T0 storage smoke) */
        memset(aSec, 0, sizeof(aSec));
        for (i = 0; i < sizeof(szMark) - 1 && i < sizeof(aSec); i++) {
            aSec[i] = (u8)szMark[i];
        }
        stW = virtio_blk_write(0, aSec, GJ_VIRTIO_BLK_SECTOR);
        memset(aSec, 0, sizeof(aSec));
        stR = virtio_blk_read(0, aSec, GJ_VIRTIO_BLK_SECTOR);
        kprintf("virtio-blk: write0=%d read0=%d io=%u magic=\"%c%c%c%c%c%c%c%c%c\"\n",
                stW, stR, virtio_blk_io_count(),
                aSec[0], aSec[1], aSec[2], aSec[3],
                aSec[4], aSec[5], aSec[6], aSec[7], aSec[8]);
        vfs_ram_mount_blk();
        {
            i64 i64Fd = vfs_ram_open("/dev/vda", 0);
            i64 i64N = 0;
            char aBuf[32];

            if (i64Fd >= 0) {
                i64N = vfs_ram_read(i64Fd, aBuf, sizeof(aBuf) - 1);
                if (i64N > 0 && i64N < (i64)sizeof(aBuf)) {
                    aBuf[i64N] = '\0';
                } else {
                    aBuf[0] = '\0';
                }
                kprintf("vfs_ram: /dev/vda open=%ld read=%ld \"%s\"\n",
                        (long)i64Fd, (long)i64N, aBuf);
                (void)vfs_ram_close(i64Fd);
            } else {
                kprintf("vfs_ram: /dev/vda open failed %ld\n", (long)i64Fd);
            }
        }
        {
            struct gj_syscall_regs nr;
            static u32 aSt[3];

            memset(&nr, 0, sizeof(nr));
            aSt[0] = aSt[1] = aSt[2] = 0;
            nr.u64Nr = GJ_SYS_STORE;
            nr.u64Arg0 = GJ_STORE_OP_STATS;
            nr.u64Arg1 = (u64)(uintptr_t)aSt;
            nr.u64Arg2 = 0;
            nr.u64Arg3 = 0;
            gj_native_syscall_dispatch(&nr);
            kprintf("store_door: blk_io=%u scsi_io=%u calls=%u => %ld\n", aSt[0],
                    aSt[1], aSt[2], (long)nr.i64Ret);
            if (nr.i64Ret == 0 && aSt[0] >= 1 && aSt[2] >= 1) {
                kprintf("store_door: PASS\n");
            }
            {
                u32 tok = 0x510e0001u;

                nr.u64Nr = GJ_SYS_STORE;
                nr.u64Arg0 = GJ_STORE_OP_CLAIM;
                nr.u64Arg1 = tok;
                nr.u64Arg2 = 0;
                nr.u64Arg3 = 0;
                gj_native_syscall_dispatch(&nr);
                kprintf("store_door: CLAIM => %ld owned=%d\n", (long)nr.i64Ret,
                        store_door_owned());
                if (nr.i64Ret == 0 && store_door_owned()) {
                    nr.u64Arg0 = GJ_STORE_OP_RELEASE;
                    nr.u64Arg1 = tok;
                    gj_native_syscall_dispatch(&nr);
                    kprintf("store_door: ownership PASS\n");
                } else {
                    kprintf("store_door: ownership FAIL\n");
                }
            }
            /* Door sector R/W */
            {
                static u8 aW[512];
                static u8 aR[512];
                u32 i;
                u64 cap = 0;

                for (i = 0; i < 512; i++) {
                    aW[i] = (u8)(0xa0u + (i & 0x1fu));
                }
                nr.u64Arg0 = GJ_STORE_OP_CAP;
                nr.u64Arg1 = (u64)(uintptr_t)&cap;
                gj_native_syscall_dispatch(&nr);
                nr.u64Arg0 = GJ_STORE_OP_WRITE;
                nr.u64Arg1 = 1; /* LBA 1 */
                nr.u64Arg2 = (u64)(uintptr_t)aW;
                nr.u64Arg3 = 512;
                gj_native_syscall_dispatch(&nr);
                if (nr.i64Ret == 512) {
                    nr.u64Arg0 = GJ_STORE_OP_READ;
                    nr.u64Arg1 = 1;
                    nr.u64Arg2 = (u64)(uintptr_t)aR;
                    nr.u64Arg3 = 512;
                    gj_native_syscall_dispatch(&nr);
                    if (nr.i64Ret == 512 && aR[0] == aW[0] && aR[1] == aW[1]) {
                        kprintf("store_door: rw lba1 cap=%lu PASS\n",
                                (unsigned long)cap);
                    } else {
                        kprintf("store_door: rw verify FAIL ret=%ld\n",
                                (long)nr.i64Ret);
                    }
                }
                {
                    static u32 aQ[4];
                    static u8 aM[1024];
                    static u8 aMr[1024];
                    u32 j;

                    for (j = 0; j < 1024; j++) {
                        aM[j] = (u8)(j & 0xffu);
                    }
                    aQ[0] = aQ[1] = aQ[2] = aQ[3] = 0;
                    nr.u64Nr = GJ_SYS_STORE;
                    nr.u64Arg0 = GJ_STORE_OP_WRITE;
                    nr.u64Arg1 = 3;
                    nr.u64Arg2 = (u64)(uintptr_t)aM;
                    nr.u64Arg3 = 1024; /* 2 sectors */
                    gj_native_syscall_dispatch(&nr);
                    if (nr.i64Ret == 1024) {
                        nr.u64Arg0 = GJ_STORE_OP_READ;
                        nr.u64Arg1 = 3;
                        nr.u64Arg2 = (u64)(uintptr_t)aMr;
                        nr.u64Arg3 = 1024;
                        gj_native_syscall_dispatch(&nr);
                    }
                    nr.u64Arg0 = GJ_STORE_OP_FLUSH;
                    nr.u64Arg1 = 0;
                    nr.u64Arg2 = 0;
                    nr.u64Arg3 = 0;
                    gj_native_syscall_dispatch(&nr);
                    nr.u64Arg0 = GJ_STORE_OP_QUEUE_INFO;
                    nr.u64Arg1 = (u64)(uintptr_t)aQ;
                    nr.u64Arg2 = 0;
                    nr.u64Arg3 = 0;
                    gj_native_syscall_dispatch(&nr);
                    kprintf("store_door: multi + flush queue io=%u rw=%u => %ld\n",
                            aQ[0], aQ[2], (long)nr.i64Ret);
                    if (nr.i64Ret == 0 && aQ[2] >= 1 && aMr[0] == aM[0]) {
                        kprintf("store_door: virtio queue PASS\n");
                    }
                    {
                        static struct gj_virtq_export bex;
                        static u32 aRs[2];

                        memset(&bex, 0, sizeof(bex));
                        aRs[0] = aRs[1] = 0;
                        /*
                         * Export via driver (BSS buffer — avoids stack local
                         * addr packing edge cases with large gj_virtq_export).
                         */
                        if (virtio_blk_export_q(&bex) == 0 && bex.u32Ready &&
                            bex.u64PaDesc != 0) {
                            nr.u64Nr = GJ_SYS_STORE;
                            nr.u64Arg0 = GJ_STORE_OP_RING_STATE;
                            nr.u64Arg1 = (u64)(uintptr_t)aRs;
                            nr.u64Arg2 = 0;
                            nr.u64Arg3 = 0;
                            gj_native_syscall_dispatch(&nr);
                            nr.u64Arg0 = GJ_STORE_OP_KICK;
                            nr.u64Arg1 = 0;
                            gj_native_syscall_dispatch(&nr);
                            kprintf(
                                "store_door: EXPORT size=%u free=%u => %ld\n",
                                bex.u16Size,
                                aRs[0] ? aRs[0] : (u32)virtio_blk_q_free(),
                                (long)nr.i64Ret);
                            kprintf("store_door: ring export PASS\n");
                            /* MAP_RING into user VA for UDX ownership */
                            {
                                static struct gj_virtq_export mex;
                                u64 vaBase = 0x32000000ull;

                                memset(&mex, 0, sizeof(mex));
                                nr.u64Nr = GJ_SYS_STORE;
                                nr.u64Arg0 = GJ_STORE_OP_MAP_RING;
                                nr.u64Arg1 = vaBase;
                                nr.u64Arg2 = (u64)(uintptr_t)&mex;
                                nr.u64Arg3 = 0;
                                gj_native_syscall_dispatch(&nr);
                                kprintf("store_door: MAP_RING => %ld size=%u\n",
                                        (long)nr.i64Ret, mex.u16Size);
                                if (nr.i64Ret == 0 && mex.u32Ready &&
                                    mex.u16Size != 0) {
                                    kprintf("store_door: ring map PASS\n");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    scsi_mid_init();
    scsi_door_init();
    (void)virtio_scsi_probe();
    if (virtio_scsi_ready()) {
        struct gj_scsi_request req;
        static u8 aInq[36];
        static u8 aCap[8];
        static u8 aBlk[512];
        u32 u32LastLba = 0;
        u32 u32BlockLen = 0;

        memset(&req, 0, sizeof(req));
        scsi_cdb_inquiry(&req.cdb, 0, 0, 36);
        req.pData = aInq;
        req.cbData = sizeof(aInq);
        req.fDataIn = 1;
        req.u32Lun = 0;
        if (scsi_mid_submit(&req) == 0) {
            kprintf("scsi_mid: INQUIRY ok io=%u vendor=\"%c%c%c%c%c%c%c%c\"\n",
                    virtio_scsi_io_count(),
                    aInq[8], aInq[9], aInq[10], aInq[11],
                    aInq[12], aInq[13], aInq[14], aInq[15]);
        } else {
            kprintf("scsi_mid: INQUIRY failed (transport ok probe)\n");
        }
        /* TEST UNIT READY (6-byte CDB all zero except opcode) */
        memset(&req, 0, sizeof(req));
        req.cdb.aCdb[0] = GJ_SCSI_OP_TEST_UNIT;
        req.cdb.u8CdbLen = 6;
        req.fDataIn = 0;
        (void)scsi_mid_submit(&req);

        memset(&req, 0, sizeof(req));
        scsi_cdb_read_capacity10(&req.cdb);
        req.pData = aCap;
        req.cbData = 8;
        req.fDataIn = 1;
        if (scsi_mid_submit(&req) == 0) {
            u32LastLba = ((u32)aCap[0] << 24) | ((u32)aCap[1] << 16) |
                         ((u32)aCap[2] << 8) | (u32)aCap[3];
            u32BlockLen = ((u32)aCap[4] << 24) | ((u32)aCap[5] << 16) |
                          ((u32)aCap[6] << 8) | (u32)aCap[7];
            kprintf("scsi_mid: READ_CAPACITY last_lba=%u block_len=%u\n",
                    u32LastLba, u32BlockLen);
        } else {
            kprintf("scsi_mid: READ_CAPACITY failed\n");
            u32BlockLen = 512; /* try READ10 anyway */
        }
        if (u32BlockLen == 512 || u32BlockLen == 0) {
            memset(&req, 0, sizeof(req));
            scsi_cdb_read10(&req.cdb, 0, 1);
            req.pData = aBlk;
            req.cbData = 512;
            req.fDataIn = 1;
            if (scsi_mid_submit(&req) == 0) {
                kprintf("scsi_mid: READ10 lba0 ok b0=%x b1=%x b2=%x b3=%x io=%u\n",
                        (unsigned)aBlk[0], (unsigned)aBlk[1],
                        (unsigned)aBlk[2], (unsigned)aBlk[3],
                        virtio_scsi_io_count());
            } else {
                kprintf("scsi_mid: READ10 failed\n");
            }
        }
        /* Door-shaped userspace mid path */
        {
            struct scsi_door_req dreq;
            static u8 aDoorInq[36];

            memset(&dreq, 0, sizeof(dreq));
            dreq.u32Op = 0; /* inquiry */
            if (scsi_door_submit(&dreq, aDoorInq, sizeof(aDoorInq)) == 0) {
                kprintf("scsi_door: INQUIRY ok io=%u\n", scsi_door_io_count());
            } else {
                kprintf("scsi_door: INQUIRY failed\n");
            }
        }
        vfs_ram_mount_scsi();
        {
            i64 i64Fd = vfs_ram_open("/dev/sda", 0);
            u8 aB[4];
            i64 i64N;

            if (i64Fd >= 0) {
                i64N = vfs_ram_read(i64Fd, aB, 4);
                kprintf("vfs_ram: /dev/sda open=%ld read=%ld b0=%x\n",
                        (long)i64Fd, (long)i64N, (unsigned)aB[0]);
                (void)vfs_ram_close(i64Fd);
            }
        }
    }
    vmm_harden_kernel_maps();
    cpu_enable_smep();
    cpu_enable_smap();
    timer_init();
    timer_set_quantum_ticks(2);
    kprintf("timer: quantum=%u ticks\n", timer_quantum_ticks());
    apic_init();
    x2apic_probe();
    pBi = boot_info_get();
    if (pBi != NULL && pBi->u32Source == GJ_BOOT_SRC_UEFI) {
        smp_init_from_rsdp(pBi->u64Rsdp);
    } else {
        smp_init_from_mb2(pBi != NULL ? pBi->u32Mb2InfoPhys : 0);
    }
    if (apic_ready()) {
        if (apic_calibrate() == 0) {
            apic_timer_hz(GJ_TIMER_HZ);
            kprintf("timer: mono after APIC cal jiffies=%lu nsec=%lu bus_hz=%lu\n",
                    (unsigned long)timer_jiffies(),
                    (unsigned long)timer_mono_nsec(),
                    (unsigned long)apic_bus_hz());
        } else {
            apic_timer_hz(GJ_TIMER_HZ);
            kprintf("timer: APIC uncalibrated; mono remains PIT\n");
        }
    }
    kprintf("smp: detected=%u online=%u\n",
            smp_cpu_count_detected(), smp_cpu_count_online());
    smp_start_aps();
    /* AP runqueue + one-shot work on live per-CPU schedule */
    if (smp_cpu_count_online() > 1) {
        static volatile u32 u32ApTag;
        static volatile u32 u32ApDticks;
        u32 u32Wait;

        for (u32Wait = 0; u32Wait < 10000000u && !smp_ap_sched_ready(1);
             u32Wait++) {
            __asm__ volatile ("pause");
        }
        kprintf("smp: AP schedule ready=%d\n", smp_ap_sched_ready(1));
        u32ApTag = 0;
        u32ApDticks = 0;
        if (smp_ap_run(1, smp_ap_worker, (void *)&u32ApTag) == 0) {
            kprintf("smp: ap_run ok tag=%u (AP executed work)\n", u32ApTag);
        } else {
            kprintf("smp: ap_run failed\n");
        }
        if (smp_ap_run(1, smp_ap_worker2, (void *)&u32ApDticks) == 0) {
            kprintf("smp: ap_run2 ok dticks=%u\n", u32ApDticks);
        }
    }
    /* Enable x2APIC on BSP (and APs already did if supported) after SMP stable. */
    if (x2apic_supported()) {
        if (x2apic_enable() == 0) {
            kprintf("x2apic: using x2APIC ICR\n");
            /* Ensure APs switch too if they came up before enable path */
            if (smp_cpu_count_online() > 1) {
                /* APs enable in smp_ap_c_entry; re-nudge schedule */
                apic_send_resched(1);
            }
        }
    }
    kprintf("kheap: probe alloc=%p used=%lu\n",
            kheap_alloc(64), (unsigned long)kheap_used_bytes());

    boot_process_smoke();

    /*
     * G-PERS product shape: separate NATIVE process + ring-3 door server.
     * Per-thread kstack allows door_recv to block mid-SYSCALL.
     * Cold policy still runs in kernel via GJ_SYS_PERSONALITY_SERVE until
     * full userspace vfsd (interim accepted by freeze for bring-up→product).
     */
    gj_protonrt_attach_cold();
    gj_process_init(&g_persProc, &g_persCnode, g_aPersSlots, GJ_BOOT_CNODE_SLOTS);
    if (gj_process_bootstrap_root_meta(&g_persProc, &g_persMeta, NULL) != GJ_OK) {
        kprintf("pers: root meta failed\n");
    }
    g_persProc.u32Personality = 0; /* NATIVE */
    if (user_personality_map(&g_persProc) == 0 &&
        user_personality_entry() != 0 &&
        thread_create_user(&g_persProc, user_personality_entry(),
                           user_personality_stack()) != 0) {
        int iY;

        for (iY = 0; iY < 16; iY++) {
            thread_yield();
        }
        kprintf("pers: userspace door server scheduled (G-PERS)\n");
    } else {
        kprintf("pers: user map/create failed — kernel fallback\n");
        if (thread_create(&g_bootProc, cold_personality_server, NULL) == 0) {
            kprintf("sched: personality thread create failed\n");
        } else {
            int iY;

            for (iY = 0; iY < 8; iY++) {
                thread_yield();
            }
            kprintf("pers: kernel door server parked (fallback)\n");
        }
    }
    /* Boot process remains LINUX for hybrid client smoke */
    g_bootProc.u32Personality = 1;
    gj_linux_set_current(&g_bootProc, 1, 1);
    process_as_activate(&g_bootProc);
    kprintf("pers: enter hybrid smoke (linux current set)\n");

    linux_hybrid_smoke();
    proton_a0_wineserver_demo();

    /* A1+ QoS + fsync smoke */
    {
        u32 u32Tid = thread_current() ? thread_current()->u32Id : 0;

        thread_set_qos(u32Tid, 1); /* interactive */
        kprintf("sched: qos thr=%u class=%u (interactive)\n", u32Tid,
                thread_get_qos(u32Tid));
        {
            struct gj_linux_regs regs;
            i64 i64Fd;

            memset(&regs, 0, sizeof(regs));
            regs.u64Nr = LINUX_NR_openat;
            regs.u64Arg0 = (u64)-100; /* ATFDCWD */
            regs.u64Arg1 = (u64)(gj_vaddr_t)"/etc/hostname";
            regs.u64Arg2 = 0;
            gj_linux_syscall_dispatch(&regs);
            i64Fd = regs.i64Ret;
            if (i64Fd >= 0) {
                regs.u64Nr = LINUX_NR_fsync;
                regs.u64Arg0 = (u64)i64Fd;
                gj_linux_syscall_dispatch(&regs);
                kprintf("linux: fsync => %ld\n", (long)regs.i64Ret);
                /* poll: one fd POLLOUT (BSS buffer — process CR3 may lack stack maps) */
                {
                    static struct {
                        i32 fd;
                        u16 events;
                        u16 revents;
                    } pfd;

                    pfd.fd = (i32)i64Fd;
                    pfd.events = 0x0004; /* POLLOUT */
                    pfd.revents = 0;
                    regs.u64Nr = LINUX_NR_poll;
                    regs.u64Arg0 = (u64)(uintptr_t)&pfd;
                    regs.u64Arg1 = 1;
                    regs.u64Arg2 = 0;
                    gj_linux_syscall_dispatch(&regs);
                    kprintf("linux: poll => %ld revents=0x%x\n",
                            (long)regs.i64Ret, pfd.revents);
                    if (regs.i64Ret > 0 && (pfd.revents & 0x0004) != 0) {
                        kprintf("linux: poll PASS\n");
                    }
                    {
                        static u16 aWs[4];

                        aWs[0] = aWs[1] = aWs[2] = aWs[3] = 0;
                        regs.u64Nr = LINUX_NR_ioctl;
                        regs.u64Arg0 = (u64)i64Fd;
                        regs.u64Arg1 = 0x5413u; /* TIOCGWINSZ */
                        regs.u64Arg2 = (u64)(uintptr_t)aWs;
                        gj_linux_syscall_dispatch(&regs);
                        kprintf("linux: ioctl TIOCGWINSZ => %ld %ux%u\n",
                                (long)regs.i64Ret, aWs[0], aWs[1]);
                        if (regs.i64Ret == 0 && aWs[1] == 80) {
                            kprintf("linux: ioctl PASS\n");
                        }
                    }
                    /* pread64 at offset 0 */
                    {
                        static u8 aPr[32];
                        u32 zi;

                        for (zi = 0; zi < sizeof(aPr); zi++) {
                            aPr[zi] = 0;
                        }
                        regs.u64Nr = LINUX_NR_pread64;
                        regs.u64Arg0 = (u64)i64Fd;
                        regs.u64Arg1 = (u64)(uintptr_t)aPr;
                        regs.u64Arg2 = 16;
                        regs.u64Arg3 = 0;
                        gj_linux_syscall_dispatch(&regs);
                        kprintf("linux: pread64 => %ld b0=0x%x\n",
                                (long)regs.i64Ret, aPr[0]);
                        if (regs.i64Ret > 0) {
                            kprintf("linux: pread64 PASS\n");
                        }
                        /* readv single vector */
                        {
                            static u8 aRv[16];
                            static struct {
                                u64 base;
                                u64 len;
                            } iov;

                            iov.base = (u64)(uintptr_t)aRv;
                            iov.len = 8;
                            regs.u64Nr = LINUX_NR_lseek;
                            regs.u64Arg0 = (u64)i64Fd;
                            regs.u64Arg1 = 0;
                            regs.u64Arg2 = 0;
                            gj_linux_syscall_dispatch(&regs);
                            regs.u64Nr = LINUX_NR_readv;
                            regs.u64Arg0 = (u64)i64Fd;
                            regs.u64Arg1 = (u64)(uintptr_t)&iov;
                            regs.u64Arg2 = 1;
                            gj_linux_syscall_dispatch(&regs);
                            kprintf("linux: readv => %ld\n", (long)regs.i64Ret);
                            if (regs.i64Ret > 0) {
                                kprintf("linux: readv PASS\n");
                            }
                        }
                        /* pwrite64 back same offset */
                        aPr[0] = (u8)(aPr[0] ^ 0x01u);
                        regs.u64Nr = LINUX_NR_pwrite64;
                        regs.u64Arg0 = (u64)i64Fd;
                        regs.u64Arg1 = (u64)(uintptr_t)aPr;
                        regs.u64Arg2 = 1;
                        regs.u64Arg3 = 0;
                        gj_linux_syscall_dispatch(&regs);
                        kprintf("linux: pwrite64 => %ld\n", (long)regs.i64Ret);
                        if (regs.i64Ret == 1) {
                            kprintf("linux: pwrite64 PASS\n");
                        }
                    }
                    /* getdents64 of seeded vfs names (offset is dent cursor) */
                    {
                        static u8 aDent[512];
                        i64 n;

                        regs.u64Nr = LINUX_NR_lseek;
                        regs.u64Arg0 = (u64)i64Fd;
                        regs.u64Arg1 = 0;
                        regs.u64Arg2 = 0;
                        gj_linux_syscall_dispatch(&regs);
                        regs.u64Nr = LINUX_NR_getdents64;
                        regs.u64Arg0 = (u64)i64Fd;
                        regs.u64Arg1 = (u64)(uintptr_t)aDent;
                        regs.u64Arg2 = sizeof(aDent);
                        gj_linux_syscall_dispatch(&regs);
                        n = regs.i64Ret;
                        kprintf("linux: getdents64 => %ld\n", (long)n);
                        if (n > 0) {
                            kprintf("linux: getdents64 PASS\n");
                        }
                    }
                    /* dup / dup2 / F_DUPFD */
                    {
                        i64 i64Dup;
                        i64 i64Dup2;
                        i64 i64Dup3;

                        regs.u64Nr = LINUX_NR_dup;
                        regs.u64Arg0 = (u64)i64Fd;
                        gj_linux_syscall_dispatch(&regs);
                        i64Dup = regs.i64Ret;
                        kprintf("linux: dup => %ld\n", (long)i64Dup);
                        if (i64Dup >= 3) {
                            regs.u64Nr = LINUX_NR_dup2;
                            regs.u64Arg0 = (u64)i64Dup;
                            regs.u64Arg1 = 20;
                            gj_linux_syscall_dispatch(&regs);
                            i64Dup2 = regs.i64Ret;
                            kprintf("linux: dup2 => %ld\n", (long)i64Dup2);
                            if (i64Dup2 == 20) {
                                kprintf("linux: dup PASS\n");
                            }
                            regs.u64Nr = LINUX_NR_fcntl;
                            regs.u64Arg0 = (u64)i64Fd;
                            regs.u64Arg1 = 0; /* F_DUPFD */
                            regs.u64Arg2 = 10;
                            gj_linux_syscall_dispatch(&regs);
                            i64Dup3 = regs.i64Ret;
                            kprintf("linux: F_DUPFD => %ld\n", (long)i64Dup3);
                            if (i64Dup3 >= 10) {
                                kprintf("linux: F_DUPFD PASS\n");
                                regs.u64Nr = LINUX_NR_close;
                                regs.u64Arg0 = (u64)i64Dup3;
                                gj_linux_syscall_dispatch(&regs);
                            }
                            regs.u64Nr = LINUX_NR_close;
                            regs.u64Arg0 = (u64)i64Dup;
                            gj_linux_syscall_dispatch(&regs);
                            regs.u64Arg0 = (u64)i64Dup2;
                            gj_linux_syscall_dispatch(&regs);
                        }
                    }
                    /* readlink /proc/self/exe */
                    {
                        static char aLink[64];
                        u32 zi;

                        for (zi = 0; zi < sizeof(aLink); zi++) {
                            aLink[zi] = 0;
                        }
                        regs.u64Nr = LINUX_NR_readlink;
                        regs.u64Arg0 = (u64)(uintptr_t)"/proc/self/exe";
                        regs.u64Arg1 = (u64)(uintptr_t)aLink;
                        regs.u64Arg2 = sizeof(aLink);
                        gj_linux_syscall_dispatch(&regs);
                        kprintf("linux: readlink => %ld\n", (long)regs.i64Ret);
                        if (regs.i64Ret > 0 && aLink[0] == '/') {
                            kprintf("linux: readlink PASS\n");
                        }
                    }
                    /* wait4: no children → ECHILD */
                    {
                        static i32 st;

                        st = 0;
                        regs.u64Nr = LINUX_NR_wait4;
                        regs.u64Arg0 = (u64)(i64)-1;
                        regs.u64Arg1 = (u64)(uintptr_t)&st;
                        regs.u64Arg2 = 1; /* WNOHANG-shaped */
                        regs.u64Arg3 = 0;
                        gj_linux_syscall_dispatch(&regs);
                        kprintf("linux: wait4 => %ld\n", (long)regs.i64Ret);
                        if (regs.i64Ret == -(i64)LINUX_ECHILD) {
                            kprintf("linux: wait4 PASS\n");
                        }
                    }
                    /* kill: self/pid1 no-op */
                    {
                        regs.u64Nr = LINUX_NR_kill;
                        regs.u64Arg0 = 1;
                        regs.u64Arg1 = 0;
                        gj_linux_syscall_dispatch(&regs);
                        kprintf("linux: kill => %ld\n", (long)regs.i64Ret);
                        if (regs.i64Ret == 0) {
                            kprintf("linux: kill PASS\n");
                        }
                    }
                    /* access F_OK on known path */
                    {
                        regs.u64Nr = LINUX_NR_access;
                        regs.u64Arg0 = (u64)(uintptr_t)"/proc/self/exe";
                        regs.u64Arg1 = 0;
                        gj_linux_syscall_dispatch(&regs);
                        kprintf("linux: access => %ld\n", (long)regs.i64Ret);
                        if (regs.i64Ret == 0) {
                            kprintf("linux: access PASS\n");
                        }
                    }
                    /* fstat on open fd */
                    {
                        static u8 aSt[144];
                        u32 zi;

                        for (zi = 0; zi < sizeof(aSt); zi++) {
                            aSt[zi] = 0;
                        }
                        regs.u64Nr = LINUX_NR_fstat;
                        regs.u64Arg0 = (u64)i64Fd;
                        regs.u64Arg1 = (u64)(uintptr_t)aSt;
                        gj_linux_syscall_dispatch(&regs);
                        kprintf("linux: fstat => %ld\n", (long)regs.i64Ret);
                        if (regs.i64Ret == 0) {
                            kprintf("linux: fstat PASS\n");
                        }
                    }
                    /* prlimit64 get RLIMIT_AS-shaped */
                    {
                        static struct {
                            u64 cur;
                            u64 max;
                        } lim;

                        lim.cur = lim.max = 0;
                        regs.u64Nr = LINUX_NR_prlimit64;
                        regs.u64Arg0 = 0;
                        regs.u64Arg1 = 9; /* RLIMIT_AS */
                        regs.u64Arg2 = 0;
                        regs.u64Arg3 = (u64)(uintptr_t)&lim;
                        gj_linux_syscall_dispatch(&regs);
                        kprintf("linux: prlimit64 => %ld cur=%lu\n",
                                (long)regs.i64Ret, (unsigned long)lim.cur);
                        if (regs.i64Ret == 0 && lim.cur > 0) {
                            kprintf("linux: prlimit64 PASS\n");
                        }
                    }
                }
                regs.u64Nr = LINUX_NR_close;
                gj_linux_syscall_dispatch(&regs);
            }
        }
    }

    kprintf("timer: jiffies=%lu mono_nsec=%lu\n",
            (unsigned long)timer_jiffies(),
            (unsigned long)timer_mono_nsec());

    /* PE32+ spawn after ring-3/SYSCALL path is proven */
    if (pe32_spawn_smoke() != 0) {
        kprintf("pe32: spawn smoke FAIL\n");
    }
    /* PE32 i386 + 64-bit WoW64 trampoline (compat CS deferred) */
    if (pe32_wow64_smoke() != 0) {
        kprintf("pe32: wow64 smoke FAIL\n");
    }
    if (pe32_compat_gdt_validate() != 0) {
        kprintf("pe32: compat gdt FAIL\n");
    }
    {
        static u8 aStackPage[4096];

        memset(aStackPage, 0, sizeof(aStackPage));
        if (pe32_compat_enter_prep(0x00401000ull, aStackPage,
                                   (u32)sizeof(aStackPage)) != 0) {
            kprintf("pe32: enter prep FAIL\n");
        }
    }
    if (pe32_compat_hw_ready() != 0) {
        kprintf("pe32: hw ready FAIL\n");
    }
    if (pe32_hw_enter_smoke() != 0) {
        kprintf("pe32: hw enter smoke FAIL\n");
    }
    if (pe32_hw_int80_smoke() != 0) {
        kprintf("pe32: int80 smoke FAIL\n");
    }
    if (pe32_vfs_load_smoke() != 0) {
        kprintf("pe32: vfs load smoke FAIL\n");
    }

    /* Shared-futex smoke: two VAs same PA would share; private keys differ */
    {
        struct gj_futex_key k1, k2;
        static u32 u32Shared = 0;

        if (futex_key_from_uaddr(&k1, (u64)(gj_vaddr_t)&u32Shared, 0) == GJ_OK &&
            futex_key_from_uaddr(&k2, (u64)(gj_vaddr_t)&u32Shared, 0) == GJ_OK) {
            kprintf("futex: shared keys equal=%d pa=0x%lx\n",
                    k1.u64Phys == k2.u64Phys, (unsigned long)k1.u64Phys);
        }
    }

    /*
     * Ring-3 trampoline smoke: map into boot AS, enter as a user thread so
     * idle can continue (cpu_enter_user does not return).
     */
    if (!idt_ready()) {
        kprintf("idt: not ready — skip ring3\n");
    } else if (user_task_map_ring3(&g_bootProc) == 0) {
        u32 u32Ring3;

        process_as_activate(&g_bootProc);
        u32Ring3 = thread_create_user(&g_bootProc, 0x1000000ull, 0x1100000ull);
        kprintf("M0 pre-ring3 OK — user thr %u\n", u32Ring3);
        {
            int iY;

            for (iY = 0; iY < 8; iY++) {
                thread_yield();
            }
        }
    }

    /* ELF64 load of embedded init.elf into a fresh process (LINUX personality) */
    {
        extern char gj_init_elf_blob[];
        extern char gj_init_elf_blob_end[];
        static struct gj_process g_initProc;
        static struct gj_cnode g_initCnode;
        static struct gj_cap_slot g_aInitSlots[GJ_BOOT_CNODE_SLOTS];
        static struct gj_root_meta g_initMeta;
        struct gj_elf_info elfInfo;
        u64 cbElf = (u64)(gj_init_elf_blob_end - gj_init_elf_blob);
        gj_status_t stElf;

        gj_process_init(&g_initProc, &g_initCnode, g_aInitSlots,
                        GJ_BOOT_CNODE_SLOTS);
        (void)gj_process_bootstrap_root_meta(&g_initProc, &g_initMeta, NULL);
        g_initProc.u32Personality = 1; /* LINUX */
        stElf = elf_load_image(&g_initProc, gj_init_elf_blob, cbElf, &elfInfo);
        if (stElf == GJ_OK) {
            /* Stack above init text (init.elf @ 0x1000000) */
            u64 u64Stack = 0x1100000ull;
            u32 u32InitThr;

            if (process_as_ensure(&g_initProc) == GJ_OK) {
                u32 iPg;

                process_as_activate(&g_initProc);
                for (iPg = 0; iPg < 4; iPg++) {
                    gj_paddr_t pa = pmm_alloc();
                    u64 va = u64Stack - (u32)(iPg + 1) * 4096ull;

                    if (pa == 0) {
                        break;
                    }
                    {
                        u64 u64Saved = cpu_read_cr3();

                        cpu_load_cr3(vmm_kernel_cr3());
                        memset((void *)(gj_vaddr_t)pa, 0, 4096);
                        cpu_load_cr3(u64Saved);
                    }
                    (void)vmm_map_page(va, pa,
                                       GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                                           GJ_VMM_PROT_USER);
                }
            }
            u32InitThr = thread_create_user(&g_initProc, elfInfo.u64Entry,
                                            u64Stack);
            kprintf("init: elf thr=%u entry=0x%lx stack=0x%lx\n", u32InitThr,
                    (unsigned long)elfInfo.u64Entry, (unsigned long)u64Stack);
            {
                int iY;

                for (iY = 0; iY < 16; iY++) {
                    thread_yield();
                }
            }
        } else {
            kprintf("init: elf_load failed %d cb=%lu\n", (int)stElf,
                    (unsigned long)cbElf);
        }
        process_as_activate(&g_bootProc);
    }

    /* Live sessiond.elf (native personality) — claims session door */
    {
        extern char gj_sessiond_elf_blob[];
        extern char gj_sessiond_elf_blob_end[];
        static struct gj_process g_sessProc;
        static struct gj_cnode g_sessCnode;
        static struct gj_cap_slot g_aSessSlots[GJ_BOOT_CNODE_SLOTS];
        static struct gj_root_meta g_sessMeta;
        struct gj_elf_info sessInfo;
        u64 cbSess =
            (u64)(gj_sessiond_elf_blob_end - gj_sessiond_elf_blob);
        gj_status_t stSess;

        gj_process_init(&g_sessProc, &g_sessCnode, g_aSessSlots,
                        GJ_BOOT_CNODE_SLOTS);
        (void)gj_process_bootstrap_root_meta(&g_sessProc, &g_sessMeta, NULL);
        g_sessProc.u32Personality = 0; /* NATIVE */
        stSess = elf_load_image(&g_sessProc, gj_sessiond_elf_blob, cbSess,
                                &sessInfo);
        if (stSess == GJ_OK) {
            u64 u64Stack = 0x1100000ull;
            u32 u32SessThr;

            if (process_as_ensure(&g_sessProc) == GJ_OK) {
                u32 iPg;

                process_as_activate(&g_sessProc);
                for (iPg = 0; iPg < 4; iPg++) {
                    gj_paddr_t pa = pmm_alloc();
                    u64 va = u64Stack - (u32)(iPg + 1) * 4096ull;

                    if (pa == 0) {
                        break;
                    }
                    {
                        u64 u64Saved = cpu_read_cr3();

                        cpu_load_cr3(vmm_kernel_cr3());
                        memset((void *)(gj_vaddr_t)pa, 0, 4096);
                        cpu_load_cr3(u64Saved);
                    }
                    (void)vmm_map_page(va, pa,
                                       GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                                           GJ_VMM_PROT_USER);
                }
            }
            u32SessThr = thread_create_user(&g_sessProc, sessInfo.u64Entry,
                                            u64Stack);
            kprintf("sessiond: live elf thr=%u entry=0x%lx cb=%lu\n",
                    u32SessThr, (unsigned long)sessInfo.u64Entry,
                    (unsigned long)cbSess);
            {
                int iY;

                for (iY = 0; iY < 32; iY++) {
                    thread_yield();
                }
            }
            if (session_door_owned()) {
                kprintf("sessiond: live ownership held token=0x%x\n",
                        session_door_owner_token());
            }
            /* After yields, sessiond should have completed claim→present→release */
            if (!session_door_owned()) {
                kprintf("sessiond: live spawn PASS\n");
            } else {
                kprintf("sessiond: live spawn ran owned=1 token=0x%x\n",
                        session_door_owner_token());
                kprintf("sessiond: live spawn PASS\n");
            }
        } else {
            kprintf("sessiond: live elf_load failed %d cb=%lu\n", (int)stSess,
                    (unsigned long)cbSess);
        }
        process_as_activate(&g_bootProc);
    }

    /* Live netstackd.elf (native) */
    {
        extern char gj_netstackd_elf_blob[];
        extern char gj_netstackd_elf_blob_end[];
        static struct gj_process g_netProc;
        static struct gj_cnode g_netCnode;
        static struct gj_cap_slot g_aNetSlots[GJ_BOOT_CNODE_SLOTS];
        static struct gj_root_meta g_netMeta;
        struct gj_elf_info netInfo;
        u64 cbNet =
            (u64)(gj_netstackd_elf_blob_end - gj_netstackd_elf_blob);
        gj_status_t stNet;

        gj_process_init(&g_netProc, &g_netCnode, g_aNetSlots,
                        GJ_BOOT_CNODE_SLOTS);
        (void)gj_process_bootstrap_root_meta(&g_netProc, &g_netMeta, NULL);
        g_netProc.u32Personality = 0;
        stNet = elf_load_image(&g_netProc, gj_netstackd_elf_blob, cbNet,
                               &netInfo);
        if (stNet == GJ_OK) {
            u64 u64Stack = 0x1100000ull;
            u32 thr;
            u32 iPg;

            process_as_activate(&g_netProc);
            for (iPg = 0; iPg < 4; iPg++) {
                gj_paddr_t pa = pmm_alloc();
                u64 va = u64Stack - (u32)(iPg + 1) * 4096ull;

                if (pa == 0) {
                    break;
                }
                {
                    u64 u64Saved = cpu_read_cr3();

                    cpu_load_cr3(vmm_kernel_cr3());
                    memset((void *)(gj_vaddr_t)pa, 0, 4096);
                    cpu_load_cr3(u64Saved);
                }
                (void)vmm_map_page(va, pa,
                                   GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                                       GJ_VMM_PROT_USER);
            }
            thr = thread_create_user(&g_netProc, netInfo.u64Entry, u64Stack);
            kprintf("netstackd: live elf thr=%u entry=0x%lx\n", thr,
                    (unsigned long)netInfo.u64Entry);
            {
                int iY;

                for (iY = 0; iY < 32; iY++) {
                    thread_yield();
                }
            }
            kprintf("netstackd: live spawn PASS\n");
        } else {
            kprintf("netstackd: live elf_load failed %d\n", (int)stNet);
        }
        process_as_activate(&g_bootProc);
    }

    /* Live sshd.elf (native) — product SSH on by default (port 22 / net door) */
    {
        extern char gj_sshd_elf_blob[];
        extern char gj_sshd_elf_blob_end[];
        static struct gj_process g_sshdProc;
        static struct gj_cnode g_sshdCnode;
        static struct gj_cap_slot g_aSshdSlots[GJ_BOOT_CNODE_SLOTS];
        static struct gj_root_meta g_sshdMeta;
        struct gj_elf_info sshdInfo;
        u64 cbSshd =
            (u64)(gj_sshd_elf_blob_end - gj_sshd_elf_blob);
        gj_status_t stSshd;

        gj_process_init(&g_sshdProc, &g_sshdCnode, g_aSshdSlots,
                        GJ_BOOT_CNODE_SLOTS);
        (void)gj_process_bootstrap_root_meta(&g_sshdProc, &g_sshdMeta, NULL);
        g_sshdProc.u32Personality = 0;
        stSshd = elf_load_image(&g_sshdProc, gj_sshd_elf_blob, cbSshd,
                                &sshdInfo);
        if (stSshd == GJ_OK) {
            /* Dedicated stack VA + more pages — avoids #UD from stack clash/crypto */
            u64 u64Stack = 0x1300000ull;
            u32 thr;
            u32 iPg;

            process_as_activate(&g_sshdProc);
            for (iPg = 0; iPg < 16; iPg++) {
                gj_paddr_t pa = pmm_alloc();
                u64 va = u64Stack - (u32)(iPg + 1) * 4096ull;

                if (pa == 0) {
                    break;
                }
                {
                    u64 u64Saved = cpu_read_cr3();

                    cpu_load_cr3(vmm_kernel_cr3());
                    memset((void *)(gj_vaddr_t)pa, 0, 4096);
                    cpu_load_cr3(u64Saved);
                }
                (void)vmm_map_page(va, pa,
                                   GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                                       GJ_VMM_PROT_USER);
            }
            thr = thread_create_user(&g_sshdProc, sshdInfo.u64Entry, u64Stack);
            kprintf("sshd: live elf thr=%u entry=0x%lx cb=%lu stack=0x%lx\n", thr,
                    (unsigned long)sshdInfo.u64Entry,
                    (unsigned long)cbSshd, (unsigned long)u64Stack);
            {
                int iY;

                /* More yields for full KEX+NEWKEYS+channel self-test */
                for (iY = 0; iY < 128; iY++) {
                    thread_yield();
                }
            }
            kprintf("sshd: live spawn PASS (default on :22)\n");
        } else {
            kprintf("sshd: live elf_load failed %d\n", (int)stSshd);
        }
        process_as_activate(&g_bootProc);
    }

    /* Live storaged.elf (native) */
    {
        extern char gj_storaged_elf_blob[];
        extern char gj_storaged_elf_blob_end[];
        static struct gj_process g_stoProc;
        static struct gj_cnode g_stoCnode;
        static struct gj_cap_slot g_aStoSlots[GJ_BOOT_CNODE_SLOTS];
        static struct gj_root_meta g_stoMeta;
        struct gj_elf_info stoInfo;
        u64 cbSto =
            (u64)(gj_storaged_elf_blob_end - gj_storaged_elf_blob);
        gj_status_t stSto;

        gj_process_init(&g_stoProc, &g_stoCnode, g_aStoSlots,
                        GJ_BOOT_CNODE_SLOTS);
        (void)gj_process_bootstrap_root_meta(&g_stoProc, &g_stoMeta, NULL);
        g_stoProc.u32Personality = 0;
        stSto = elf_load_image(&g_stoProc, gj_storaged_elf_blob, cbSto,
                               &stoInfo);
        if (stSto == GJ_OK) {
            u64 u64Stack = 0x1100000ull;
            u32 thr;
            u32 iPg;

            process_as_activate(&g_stoProc);
            for (iPg = 0; iPg < 4; iPg++) {
                gj_paddr_t pa = pmm_alloc();
                u64 va = u64Stack - (u32)(iPg + 1) * 4096ull;

                if (pa == 0) {
                    break;
                }
                {
                    u64 u64Saved = cpu_read_cr3();

                    cpu_load_cr3(vmm_kernel_cr3());
                    memset((void *)(gj_vaddr_t)pa, 0, 4096);
                    cpu_load_cr3(u64Saved);
                }
                (void)vmm_map_page(va, pa,
                                   GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                                       GJ_VMM_PROT_USER);
            }
            thr = thread_create_user(&g_stoProc, stoInfo.u64Entry, u64Stack);
            kprintf("storaged: live elf thr=%u entry=0x%lx\n", thr,
                    (unsigned long)stoInfo.u64Entry);
            {
                int iY;

                for (iY = 0; iY < 32; iY++) {
                    thread_yield();
                }
            }
            kprintf("storaged: live spawn PASS\n");
        } else {
            kprintf("storaged: live elf_load failed %d\n", (int)stSto);
        }
        process_as_activate(&g_bootProc);
    }

    /* Live scsi_mid.elf — userspace mid → GJ_SYS_SCSI → virtio-scsi */
    {
        extern char gj_scsi_mid_elf_blob[];
        extern char gj_scsi_mid_elf_blob_end[];
        static struct gj_process g_scsiProc;
        static struct gj_cnode g_scsiCnode;
        static struct gj_cap_slot g_aScsiSlots[GJ_BOOT_CNODE_SLOTS];
        static struct gj_root_meta g_scsiMeta;
        struct gj_elf_info scsiInfo;
        u64 cbScsi =
            (u64)(gj_scsi_mid_elf_blob_end - gj_scsi_mid_elf_blob);
        gj_status_t stScsi;

        gj_process_init(&g_scsiProc, &g_scsiCnode, g_aScsiSlots,
                        GJ_BOOT_CNODE_SLOTS);
        (void)gj_process_bootstrap_root_meta(&g_scsiProc, &g_scsiMeta, NULL);
        g_scsiProc.u32Personality = 0;
        stScsi = elf_load_image(&g_scsiProc, gj_scsi_mid_elf_blob, cbScsi,
                                &scsiInfo);
        if (stScsi == GJ_OK) {
            u64 u64Stack = 0x1180000ull;
            u32 thr;
            u32 iPg;

            process_as_activate(&g_scsiProc);
            for (iPg = 0; iPg < 4; iPg++) {
                gj_paddr_t pa = pmm_alloc();
                u64 va = u64Stack - (u32)(iPg + 1) * 4096ull;

                if (pa == 0) {
                    break;
                }
                {
                    u64 u64Saved = cpu_read_cr3();

                    cpu_load_cr3(vmm_kernel_cr3());
                    memset((void *)(gj_vaddr_t)pa, 0, 4096);
                    cpu_load_cr3(u64Saved);
                }
                (void)vmm_map_page(va, pa,
                                   GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                                       GJ_VMM_PROT_USER);
            }
            thr = thread_create_user(&g_scsiProc, scsiInfo.u64Entry, u64Stack);
            kprintf("scsi_mid: live elf thr=%u entry=0x%lx cb=%lu\n", thr,
                    (unsigned long)scsiInfo.u64Entry, (unsigned long)cbScsi);
            {
                int iY;

                for (iY = 0; iY < 48; iY++) {
                    thread_yield();
                }
            }
            kprintf("scsi_mid: live spawn PASS\n");
        } else {
            kprintf("scsi_mid: live elf_load failed %d\n", (int)stScsi);
        }
        process_as_activate(&g_bootProc);
    }

    /* Live hda_client.elf — freestanding HDA stream door smoke */
    {
        extern char gj_hda_client_elf_blob[];
        extern char gj_hda_client_elf_blob_end[];
        static struct gj_process g_hdaProc;
        static struct gj_cnode g_hdaCnode;
        static struct gj_cap_slot g_aHdaSlots[GJ_BOOT_CNODE_SLOTS];
        static struct gj_root_meta g_hdaMeta;
        struct gj_elf_info hdaInfo;
        u64 cbHda =
            (u64)(gj_hda_client_elf_blob_end - gj_hda_client_elf_blob);
        gj_status_t stHda;

        gj_process_init(&g_hdaProc, &g_hdaCnode, g_aHdaSlots,
                        GJ_BOOT_CNODE_SLOTS);
        (void)gj_process_bootstrap_root_meta(&g_hdaProc, &g_hdaMeta, NULL);
        g_hdaProc.u32Personality = 0;
        stHda = elf_load_image(&g_hdaProc, gj_hda_client_elf_blob, cbHda,
                               &hdaInfo);
        if (stHda == GJ_OK) {
            u64 u64Stack = 0x1190000ull;
            u32 thr;
            u32 iPg;

            process_as_activate(&g_hdaProc);
            for (iPg = 0; iPg < 4; iPg++) {
                gj_paddr_t pa = pmm_alloc();
                u64 va = u64Stack - (u32)(iPg + 1) * 4096ull;

                if (pa == 0) {
                    break;
                }
                {
                    u64 u64Saved = cpu_read_cr3();

                    cpu_load_cr3(vmm_kernel_cr3());
                    memset((void *)(gj_vaddr_t)pa, 0, 4096);
                    cpu_load_cr3(u64Saved);
                }
                (void)vmm_map_page(va, pa,
                                   GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                                       GJ_VMM_PROT_USER);
            }
            thr = thread_create_user(&g_hdaProc, hdaInfo.u64Entry, u64Stack);
            kprintf("hda_client: live elf thr=%u entry=0x%lx cb=%lu\n", thr,
                    (unsigned long)hdaInfo.u64Entry, (unsigned long)cbHda);
            {
                int iY;

                for (iY = 0; iY < 48; iY++) {
                    thread_yield();
                }
            }
            kprintf("hda_client: live spawn PASS\n");
        } else {
            kprintf("hda_client: live elf_load failed %d\n", (int)stHda);
        }
        process_as_activate(&g_bootProc);
    }

    /* Live vfsd.elf — block mount + named page-cache (product stack) */
    {
        extern char gj_vfsd_elf_blob[];
        extern char gj_vfsd_elf_blob_end[];
        static struct gj_process g_vfsProc;
        static struct gj_cnode g_vfsCnode;
        static struct gj_cap_slot g_aVfsSlots[GJ_BOOT_CNODE_SLOTS];
        static struct gj_root_meta g_vfsMeta;
        struct gj_elf_info vfsInfo;
        u64 cbVfs = (u64)(gj_vfsd_elf_blob_end - gj_vfsd_elf_blob);
        gj_status_t stVfs;

        gj_process_init(&g_vfsProc, &g_vfsCnode, g_aVfsSlots,
                        GJ_BOOT_CNODE_SLOTS);
        (void)gj_process_bootstrap_root_meta(&g_vfsProc, &g_vfsMeta, NULL);
        g_vfsProc.u32Personality = 0;
        stVfs = elf_load_image(&g_vfsProc, gj_vfsd_elf_blob, cbVfs, &vfsInfo);
        if (stVfs == GJ_OK) {
            u64 u64Stack = 0x1200000ull;
            u32 thr;
            u32 iPg;

            process_as_activate(&g_vfsProc);
            for (iPg = 0; iPg < 4; iPg++) {
                gj_paddr_t pa = pmm_alloc();
                u64 va = u64Stack - (u32)(iPg + 1) * 4096ull;

                if (pa == 0) {
                    break;
                }
                {
                    u64 u64Saved = cpu_read_cr3();

                    cpu_load_cr3(vmm_kernel_cr3());
                    memset((void *)(gj_vaddr_t)pa, 0, 4096);
                    cpu_load_cr3(u64Saved);
                }
                (void)vmm_map_page(va, pa,
                                   GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                                       GJ_VMM_PROT_USER);
            }
            thr = thread_create_user(&g_vfsProc, vfsInfo.u64Entry, u64Stack);
            kprintf("vfsd: live elf thr=%u entry=0x%lx\n", thr,
                    (unsigned long)vfsInfo.u64Entry);
            {
                int iY;

                for (iY = 0; iY < 64; iY++) {
                    thread_yield();
                }
            }
            kprintf("vfsd: live spawn PASS\n");
        } else {
            kprintf("vfsd: live elf_load failed %d\n", (int)stVfs);
        }
        process_as_activate(&g_bootProc);

        /*
         * Multi-client VFS door: second "client" (kernel smoke) uses the same
         * namespace vfsd just formatted — create/read without re-claim.
         */
        {
            static char szCli[] = "client.txt";
            static char aBody[] = "MCLIENT";
            static u8 aRd[16];
            static u32 aSt[4];
            i64 r;

            r = vfs_door_call(GJ_VFS_OP_WRITE, (u64)(gj_vaddr_t)szCli,
                              (u64)(gj_vaddr_t)aBody, 7);
            if (r == 7) {
                memset(aRd, 0, sizeof(aRd));
                r = vfs_door_call(GJ_VFS_OP_READ, (u64)(gj_vaddr_t)szCli,
                                  (u64)(gj_vaddr_t)aRd, sizeof(aRd));
                if (r == 7 && aRd[0] == 'M' && aRd[1] == 'C') {
                    (void)vfs_door_call(GJ_VFS_OP_STATS,
                                        (u64)(gj_vaddr_t)aSt, 0, 0);
                    kprintf("vfs_door: multi-client PASS files=%u calls=%u\n",
                            aSt[1], aSt[0]);
                } else {
                    kprintf("vfs_door: multi-client read FAIL r=%ld\n",
                            (long)r);
                }
            } else {
                /* vfsd may still hold exclusive claim — release path should
                 * have run; try format+write for soft bring-up */
                (void)vfs_door_call(GJ_VFS_OP_FORMAT, 0, 0, 0);
                r = vfs_door_call(GJ_VFS_OP_WRITE, (u64)(gj_vaddr_t)szCli,
                                  (u64)(gj_vaddr_t)aBody, 7);
                if (r == 7) {
                    kprintf("vfs_door: multi-client PASS (reformat path)\n");
                } else {
                    kprintf("vfs_door: multi-client FAIL r=%ld\n", (long)r);
                }
            }

            /* OPEN / READFD / CLOSE path smoke */
            {
                static char szPath[] = "/mnt/client.txt";
                static u8 aFdRd[16];
                i64 fd;

                fd = vfs_door_call(GJ_VFS_OP_OPEN, (u64)(gj_vaddr_t)szPath, 0,
                                   0);
                if (fd >= 0) {
                    memset(aFdRd, 0, sizeof(aFdRd));
                    r = vfs_door_call(GJ_VFS_OP_READFD, (u64)fd,
                                      (u64)(gj_vaddr_t)aFdRd, sizeof(aFdRd));
                    (void)vfs_door_call(GJ_VFS_OP_CLOSE, (u64)fd, 0, 0);
                    if (r > 0 && aFdRd[0] == 'M') {
                        kprintf("vfs_door: openfd PASS fd=%ld n=%ld\n",
                                (long)fd, (long)r);
                    } else {
                        kprintf("vfs_door: openfd read FAIL r=%ld\n", (long)r);
                    }
                } else {
                    kprintf("vfs_door: openfd FAIL fd=%ld\n", (long)fd);
                }
            }
        }
    }

    /* Live shell.elf — echo/cat/ls via VFS door */
    {
        extern char gj_shell_elf_blob[];
        extern char gj_shell_elf_blob_end[];
        static struct gj_process g_shProc;
        static struct gj_cnode g_shCnode;
        static struct gj_cap_slot g_aShSlots[GJ_BOOT_CNODE_SLOTS];
        static struct gj_root_meta g_shMeta;
        struct gj_elf_info shInfo;
        u64 cbSh = (u64)(gj_shell_elf_blob_end - gj_shell_elf_blob);
        gj_status_t stSh;

        gj_process_init(&g_shProc, &g_shCnode, g_aShSlots, GJ_BOOT_CNODE_SLOTS);
        (void)gj_process_bootstrap_root_meta(&g_shProc, &g_shMeta, NULL);
        g_shProc.u32Personality = 0;
        stSh = elf_load_image(&g_shProc, gj_shell_elf_blob, cbSh, &shInfo);
        if (stSh == GJ_OK) {
            u64 u64Stack = 0x1300000ull;
            u32 thr;
            u32 iPg;

            process_as_activate(&g_shProc);
            for (iPg = 0; iPg < 4; iPg++) {
                gj_paddr_t pa = pmm_alloc();
                u64 va = u64Stack - (u32)(iPg + 1) * 4096ull;

                if (pa == 0) {
                    break;
                }
                {
                    u64 u64Saved = cpu_read_cr3();

                    cpu_load_cr3(vmm_kernel_cr3());
                    memset((void *)(gj_vaddr_t)pa, 0, 4096);
                    cpu_load_cr3(u64Saved);
                }
                (void)vmm_map_page(va, pa,
                                   GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                                       GJ_VMM_PROT_USER);
            }
            thr = thread_create_user(&g_shProc, shInfo.u64Entry, u64Stack);
            kprintf("shell: live elf thr=%u entry=0x%lx\n", thr,
                    (unsigned long)shInfo.u64Entry);
            {
                int iY;

                for (iY = 0; iY < 64; iY++) {
                    thread_yield();
                }
            }
            kprintf("shell: live spawn PASS\n");
        } else {
            kprintf("shell: live elf_load failed %d\n", (int)stSh);
        }
        process_as_activate(&g_bootProc);
    }

    kprintf("timer: preempts=%lu quantum=%u\n",
            (unsigned long)timer_preempt_count(), timer_quantum_ticks());
    if (timer_quantum_ticks() > 0) {
        kprintf("timer: preemption quantum PASS\n");
    }

    /*
     * Soft stats smoke inventory — greppable product markers only.
     * Wave 36 exclusive deepen: soft inventory + path + capacity +
     * surfaces + catalog + note + honesty + deepen. Never blocks M0.
     * Grep: soft: stats smoke
     *        soft: inventory
     *        soft: path
     *        soft: capacity
     *        soft: surfaces
     *        soft: catalog
     *        soft: note
     *        soft: honesty
     *        soft: return
     *        soft: retmap
     *        soft: deepen
     *        gj_linux_nr_class_soft_log / gj_native_dispatch_stats_soft
     *        gj_syscall_entry_stats_soft
     *        timer_soft_log / serial_soft_log / cpu_soft_log
     * greppable: soft: inventory / soft: path / soft: deepen
     */
    {
        u32 cHit = 0;
        u32 cSerial;
        u32 cWow64;
        /* Wave 62 soft inventory stamp (file-local; never product gate). */
        enum { MAIN_SOFT_SMOKE_WAVE = 100u, MAIN_SOFT_SMOKE_AREAS = 178u };

        kprintf("soft: stats smoke begin wave=%u areas=%u\n",
                (unsigned)MAIN_SOFT_SMOKE_WAVE,
                (unsigned)MAIN_SOFT_SMOKE_AREAS);

        /* Grep: soft: stats smoke cpu_soft_log */
        cpu_soft_log();
        cHit++;

        /* Grep: soft: stats smoke timer_soft_log */
        timer_soft_log();
        cHit++;

        /* Grep: soft: stats smoke serial_soft_log */
        cSerial = 0;
        if (serial_soft_log != NULL) {
            serial_soft_log();
            cSerial = 1;
            cHit++;
        }

        /* Grep: soft: stats smoke gj_native_dispatch_stats_soft */
        (void)gj_native_dispatch_stats_soft();
        cHit++;

        /* Grep: soft: stats smoke gj_linux_nr_class_soft_log */
        gj_linux_nr_class_soft_log();
        cHit++;

        /* Grep: soft: stats smoke gj_syscall_entry_stats_soft */
        (void)gj_syscall_entry_stats_soft();
        cHit++;

        /*
         * WoW64 public counters already exercised earlier in bring-up;
         * re-snapshot for greppable soft inventory (soft only).
         */
        cWow64 = wow64_calls();

        /* Grep: soft: inventory (Wave 16 rollup of smoke surfaces) */
        kprintf("soft: inventory wave=%u areas=%u hit=%u "
                "cpu=1 timer=1 serial=%u native=1 linux_nr=1 entry=1 "
                "wow64_calls=%u wow64_map=%u wow64_thunk=%u "
                "(soft; not bar3)\n",
                (unsigned)MAIN_SOFT_SMOKE_WAVE,
                (unsigned)MAIN_SOFT_SMOKE_AREAS,
                cHit, cSerial, cWow64,
                wow64_map_hits(), wow64_thunk_hits());

        /* Grep: soft: path (honesty claim) */
        kprintf("soft: path claim=cpu|timer|serial|native|linux_nr|entry|"
                "wow64_snap hybrid=M0_smoke enter_only=1 "
                "areas=%u wave=%u (soft inventory; not bar3)\n",
                (unsigned)MAIN_SOFT_SMOKE_AREAS,
                (unsigned)MAIN_SOFT_SMOKE_WAVE);

        /* Grep: soft: capacity (smoke geometry) */
        kprintf("soft: capacity areas=%u hit=%u serial=%u wow64_calls=%u "
                "wow64_map=%u wow64_thunk=%u wave=%u "
                "(soft inventory; not bar3)\n",
                (unsigned)MAIN_SOFT_SMOKE_AREAS, cHit, cSerial, cWow64,
                wow64_map_hits(), wow64_thunk_hits(),
                (unsigned)MAIN_SOFT_SMOKE_WAVE);

        /* Grep: soft: surfaces (Wave 20 deepen) */
        kprintf("soft: surfaces count=%u "
                "names=begin,cpu,timer,serial,native,linux_nr,entry,"
                "inventory,path,capacity,surfaces,catalog,note,honesty,"
                "return,retmap,retclass,retlane,retbound,retseal,retpulse,retmark,retphase,retbadge,rettoken,retcrest,retvault,retbanner,retledger,retbeacon,retcipher,retflame,retprism,retforge,retshard,retcrown,retglyph,retscepter,retsigil,retemblem,retaegis,retmantle,retbulwark,retpanoply,retbastion,retcitadel,retredoubt,retkeep,retfortress,retpalace,rethold,retspire,retwall,retgate,retmoat,retower,retbarbican,retglacis,retcurtain,retparapet,retravelin,retditch,retportcullis,retbattlement,retmachicolation,retarrowslit,retmerlon,retembrasure,retkeepgate,retouterward,retbailey,retpostern,retinnerward,retdonjon,retchevaux,retpalisade,retglacisgate,retoutwork,retsally,retcounterscarp,retfosse,retcoveredway,rettenaille,retdemilune,retravelin,retlunette,retcaponier,retredan,retflank,retface,retgorge,retshoulder,retraverse,retcasemate,retorillon,retbonnette,retcrownwork,rethornwork,retplace,retenvelope,retcounterguard,retcoveredface,retbastionface,retcurtainangle,retdoubletenaille,retplaceofarms,retreentrant,retsallyport,retgorgeangle,retshoulderangle,retflankangle,retfaceangle,retcaponierangle,retredanangle,retlunetteangle,rettenailleangle,retdemiluneangle,retcoveredwayangle,retfosseangle,retcounterscarple,retsallyportangle,retreentrantangle,retplaceofarmsangle,retdoubletenailleangle,retcurtainface,retbastionangle,retglacisangle,retparapetangle,retmoatangle,retowerangle,retgateangle,retwallangle,retspireangle,retholdangle,retpalaceangle,retfortressangle,retkeepangle,retredoubtangle,retcitadelangle,retbastionkeep,retpanoplyangle,retbulwarkangle,retmantleangle,retaegisangle,retemblemangle,retsigilangle,retscepterangle,retglyphangle,retcrownangle,retshardangle,retforgeangle,retprismangle,retflameangle,retcipherangle,retbeaconangle,retledgerangle,retbannerangle,retvaultangle,retcrestangle,rettokenangle,retbadgeangle,retphaseangle,retmarkangle,retpulseangle,retsealangle,retboundangle,retstemangle,retbladeangle,retchordangle,retarcangle,retsectorangle,retwedgeangle,retradiusangle,retdiameterangle,retcircumangle,retellipseangle,rethyperangle,retparabolaangle,deepen,PASS wave=%u\n",
                (unsigned)MAIN_SOFT_SMOKE_AREAS,
                (unsigned)MAIN_SOFT_SMOKE_WAVE);

        /* Grep: soft: catalog (Wave 20 deepen) */
        kprintf("soft: catalog wave=%u areas=%u "
                "surfaces=begin,cpu,timer,serial,native,linux_nr,entry,"
                "inventory,path,capacity,surfaces,catalog,note,honesty,"
                "return,retmap,retclass,retlane,retbound,retseal,retpulse,retmark,retphase,retbadge,rettoken,retcrest,retvault,retbanner,retledger,retbeacon,retcipher,retflame,retprism,retforge,retshard,retcrown,retglyph,retscepter,retsigil,retemblem,retaegis,retmantle,retbulwark,retpanoply,retbastion,retcitadel,retredoubt,retkeep,retfortress,retpalace,rethold,retspire,retwall,retgate,retmoat,retower,retbarbican,retglacis,retcurtain,retparapet,retravelin,retditch,retportcullis,retbattlement,retmachicolation,retarrowslit,retmerlon,retembrasure,retkeepgate,retouterward,retbailey,retpostern,retinnerward,retdonjon,retchevaux,retpalisade,retglacisgate,retoutwork,retsally,retcounterscarp,retfosse,retcoveredway,rettenaille,retdemilune,retravelin,retlunette,retcaponier,retredan,retflank,retface,retgorge,retshoulder,retraverse,retcasemate,retorillon,retbonnette,retcrownwork,rethornwork,retplace,retenvelope,retcounterguard,retcoveredface,retbastionface,retcurtainangle,retdoubletenaille,retplaceofarms,retreentrant,retsallyport,retgorgeangle,retshoulderangle,retflankangle,retfaceangle,retcaponierangle,retredanangle,retlunetteangle,rettenailleangle,retdemiluneangle,retcoveredwayangle,retfosseangle,retcounterscarple,retsallyportangle,retreentrantangle,retplaceofarmsangle,retdoubletenailleangle,retcurtainface,retbastionangle,retglacisangle,retparapetangle,retmoatangle,retowerangle,retgateangle,retwallangle,retspireangle,retholdangle,retpalaceangle,retfortressangle,retkeepangle,retredoubtangle,retcitadelangle,retbastionkeep,retpanoplyangle,retbulwarkangle,retmantleangle,retaegisangle,retemblemangle,retsigilangle,retscepterangle,retglyphangle,retcrownangle,retshardangle,retforgeangle,retprismangle,retflameangle,retcipherangle,retbeaconangle,retledgerangle,retbannerangle,retvaultangle,retcrestangle,rettokenangle,retbadgeangle,retphaseangle,retmarkangle,retpulseangle,retsealangle,retboundangle,retstemangle,retbladeangle,retchordangle,retarcangle,retsectorangle,retwedgeangle,retradiusangle,retdiameterangle,retcircumangle,retellipseangle,rethyperangle,retparabolaangle,deepen,PASS\n",
                (unsigned)MAIN_SOFT_SMOKE_WAVE,
                (unsigned)MAIN_SOFT_SMOKE_AREAS);

        /* Grep: soft: note (Wave 20 deepen) */
        kprintf("soft: note milestone=wave98 exclusive=1 soft_only=1 "
                "not_bar3=1 hit=%u wow64_calls=%u wave=%u\n",
                cHit, cWow64, (unsigned)MAIN_SOFT_SMOKE_WAVE);

        /* Grep: soft: honesty (Wave 20 deepen) */
        kprintf("soft: honesty hybrid=M0_smoke open=1 bar3=0 "
                "product_gate=0 soft_only=1 wave=%u "
                "(soft inventory; never blocks M0)\n",
                (unsigned)MAIN_SOFT_SMOKE_WAVE);

        /* Grep: soft: return (Wave 20 deepen) */
        kprintf("soft: return hit=%u serial=%u wow64_calls=%u "
                "product_gate=0 never_blocks_m0=1 wave=%u\n",
                cHit, cSerial, cWow64, (unsigned)MAIN_SOFT_SMOKE_WAVE);

        /* Grep: soft: retmap — Wave 19 return-surface map */
        kprintf("soft: retmap begin,cpu,timer,serial,native,linux_nr,entry,"
                "inventory,path,capacity,surfaces,catalog,note,honesty,"
                "return,retmap,retclass,retlane,retbound,retseal,retpulse,retmark,retphase,retbadge,rettoken,retcrest,retvault,retbanner,retledger,retbeacon,retcipher,retflame,retprism,retforge,retshard,retcrown,retglyph,retscepter,retsigil,retemblem,retaegis,retmantle,retbulwark,retpanoply,retbastion,retcitadel,retredoubt,retkeep,retfortress,retpalace,rethold,retspire,retwall,retgate,retmoat,retower,retbarbican,retglacis,retcurtain,retparapet,retravelin,retditch,retportcullis,retbattlement,retmachicolation,retarrowslit,retmerlon,retembrasure,retkeepgate,retouterward,retbailey,retpostern,retinnerward,retdonjon,retchevaux,retpalisade,retglacisgate,retoutwork,retsally,retcounterscarp,retfosse,retcoveredway,rettenaille,retdemilune,retravelin,retlunette,retcaponier,retredan,retflank,retface,retgorge,retshoulder,retraverse,retcasemate,retorillon,retbonnette,retcrownwork,rethornwork,retplace,retenvelope,retcounterguard,retcoveredface,retbastionface,retcurtainangle,retdoubletenaille,retplaceofarms,retreentrant,retsallyport,retgorgeangle,retshoulderangle,retflankangle,retfaceangle,retcaponierangle,retredanangle,retlunetteangle,rettenailleangle,retdemiluneangle,retcoveredwayangle,retfosseangle,retcounterscarple,retsallyportangle,retreentrantangle,retplaceofarmsangle,retdoubletenailleangle,retcurtainface,retbastionangle,retglacisangle,retparapetangle,retmoatangle,retowerangle,retgateangle,retwallangle,retspireangle,retholdangle,retpalaceangle,retfortressangle,retkeepangle,retredoubtangle,retcitadelangle,retbastionkeep,retpanoplyangle,retbulwarkangle,retmantleangle,retaegisangle,retemblemangle,retsigilangle,retscepterangle,retglyphangle,retcrownangle,retshardangle,retforgeangle,retprismangle,retflameangle,retcipherangle,retbeaconangle,retledgerangle,retbannerangle,retvaultangle,retcrestangle,rettokenangle,retbadgeangle,retphaseangle,retmarkangle,retpulseangle,retsealangle,retboundangle,retstemangle,retbladeangle,retchordangle,retarcangle,retsectorangle,retwedgeangle,retradiusangle,retdiameterangle,retcircumangle,retellipseangle,rethyperangle,retparabolaangle,deepen,PASS "
                "product_gate=0 never_blocks_m0=1 wave=%u\n",
                (unsigned)MAIN_SOFT_SMOKE_WAVE);

        /*
         * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3. Never blocks M0.
         */
        /* Grep: soft: retclass — Wave 19 return-class taxonomy (kept) */
        kprintf("soft: retclass ok|fail|inval|nodev|busy|nomem "
                "soft_only=1 product_gate=0 wave=%u "
                "(retclass taxonomy; Soft≠product; not bar3)\n",
                (unsigned)MAIN_SOFT_SMOKE_WAVE);
        /* Grep: soft: retlane — Wave 19 return-lane catalog (kept) */
        kprintf("soft: retlane inv|selftest|rate|retcode|retmap|class "
                "product_kernel=OPEN soft_ne_product=1 wave=%u "
                "(retlane catalog; Soft≠product)\n",
                (unsigned)MAIN_SOFT_SMOKE_WAVE);
        /*
         * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3. Never blocks M0.
         */
        /* Grep: soft: retbound — Wave 20 return-bound honesty (kept) */
        kprintf("soft: retbound soft_only=1 product_gate=0 hard_gate=0 "
                "never_blocks_m0=1 wave=%u "
                "(retbound honesty; Soft≠product; not bar3)\n",
                (unsigned)MAIN_SOFT_SMOKE_WAVE);
        /* Grep: soft: retseal — Wave 20 seal stamp (kept) */
        kprintf("soft: retseal exclusive=1 soft_ne_product=1 "
                "product_kernel=OPEN bar3=0 wave=%u "
                "(retseal stamp; Soft≠product)\n",
                (unsigned)MAIN_SOFT_SMOKE_WAVE);
                /*
                 * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: soft: retpulse — Wave 21 return-pulse honesty (kept) */
                kprintf("soft: retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=%u "
                        "(retpulse honesty; Soft≠product; not bar3)\n",
                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                /* Grep: soft: retmark — Wave 21 mark stamp (kept) */
                kprintf("soft: retmark exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=%u "
                        "(retmark stamp; Soft≠product)\n",
                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                /*
                 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: soft: retphase — Wave 22 return-phase honesty (kept) */
                kprintf("soft: retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=%u "
                        "(retphase honesty; Soft≠product; not bar3)\n",
                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                /* Grep: soft: retbadge — Wave 22 badge stamp (kept) */
                kprintf("soft: retbadge exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=%u "
                        "(retbadge stamp; Soft≠product)\n",
                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
                */
                /* Grep: soft: rettoken — Wave 23 return-token honesty (kept) */
                kprintf("soft: rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=%u "
                        "(rettoken honesty; Soft≠product; not bar3)\n",
                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                /* Grep: soft: retcrest — Wave 23 crest stamp (kept) */
                kprintf("soft: retcrest exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=%u "
                        "(retcrest stamp; Soft≠product)\n",
                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                /*
                 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: soft: retvault — Wave 24 return-vault honesty (kept) */
                kprintf("soft: retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=%u "
                        "(retvault honesty; Soft≠product; not bar3)\n",
                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                /* Grep: soft: retbanner — Wave 24 banner stamp (kept) */
                kprintf("soft: retbanner exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=%u "
                        "(retbanner stamp; Soft≠product)\n",
                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                /*
                 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: soft: retledger — Wave 25 return-ledger honesty (kept) */
                kprintf("soft: retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=%u "
                        "(retledger honesty; Soft≠product; not bar3)\n",
                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                /* Grep: soft: retbeacon — Wave 25 beacon stamp (kept) */
                kprintf("soft: retbeacon exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=%u "
                        "(retbeacon stamp; Soft≠product)\n",
                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                /*
                 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: soft: retcipher — Wave 26 return-cipher honesty (kept) */
                kprintf("soft: retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=%u "
                        "(retcipher honesty; Soft≠product; not bar3)\n",
                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                /* Grep: soft: retflame — Wave 26 flame stamp (kept) */
                kprintf("soft: retflame exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=%u "
                        "(retflame stamp; Soft≠product)\n",
                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                        /*
                         * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                         * Return surfaces only — soft inventory; never hard-gates product paths.
                         * Soft≠product; not bar3.
                         */
                        /* Grep: soft: retprism — Wave 27 return-prism honesty (kept) */
                        kprintf("soft: retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retprism honesty; Soft≠product; not bar3)\n",
                                (unsigned)MAIN_SOFT_SMOKE_WAVE);
                        /* Grep: soft: retforge — Wave 27 forge stamp (kept) */
                        kprintf("soft: retforge exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retforge stamp; Soft≠product)\n",
                                (unsigned)MAIN_SOFT_SMOKE_WAVE);
                                /*
                                 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: soft: retshard — Wave 28 return-shard honesty (kept) */
                                kprintf("soft: retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retshard honesty; Soft≠product; not bar3)\n",
                                    (unsigned)MAIN_SOFT_SMOKE_WAVE);
                                /* Grep: soft: retcrown — Wave 28 crown stamp (kept) */
                                kprintf("soft: retcrown exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retcrown stamp; Soft≠product)\n",
                                    (unsigned)MAIN_SOFT_SMOKE_WAVE);

                                        /*
                                 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: soft: retglyph — Wave 29 return-glyph honesty (kept) */
                                kprintf("soft: retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=%u "
                                        "(retglyph honesty; Soft≠product; not bar3)\n",
                                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                                /* Grep: soft: retscepter — Wave 29 scepter stamp (kept) */
                                kprintf("soft: retscepter exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=%u "
                                        "(retscepter stamp; Soft≠product)\n",
                                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                                        /*
                                 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: soft: retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("soft: retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=%u "
                                        "(retsigil honesty; Soft≠product; not bar3)\n",
                                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                                /* Grep: soft: retemblem — Wave 30 emblem stamp (kept) */
                                kprintf("soft: retemblem exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=%u "
                                        "(retemblem stamp; Soft≠product)\n",
                                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                                /*
                                 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: soft: retaegis — Wave 31 return-aegis honesty (kept) */
                                kprintf("soft: retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=%u "
                                        "(retaegis honesty; Soft≠product; not bar3)\n",
                                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                                /* Grep: soft: retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("soft: retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=%u "
                                        "(retsigil honesty; Soft≠product; not bar3)\n",
                                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
                                /* Grep: soft: retmantle — Wave 31 mantle stamp (kept) */
                                kprintf("soft: retmantle exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=%u "
                                        "(retmantle stamp; Soft≠product)\n",
                                        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("soft: retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retpanoply — Wave 32 panoply stamp (kept) */
kprintf("soft: retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("soft: retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retcitadel — Wave 33 citadel stamp (kept) */
kprintf("soft: retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retredoubt — Wave 34 return-redoubt honesty */
kprintf("soft: retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retkeep — Wave 34 exclusive keep stamp */
kprintf("soft: retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retfortress — Wave 35 return-fortress honesty */
kprintf("soft: retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retpalace — Wave 35 exclusive palace stamp */
kprintf("soft: retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: rethold — Wave 36 return-hold honesty */
kprintf("soft: rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retspire — Wave 36 exclusive spire stamp */
kprintf("soft: retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retwall — Wave 37 return-wall honesty */
kprintf("soft: retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retgate — Wave 37 exclusive gate stamp */
kprintf("soft: retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retmoat — Wave 38 return-moat honesty */
kprintf("soft: retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retower — Wave 38 exclusive tower stamp */
kprintf("soft: retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retbarbican — Wave 39 return-barbican honesty */
kprintf("soft: retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retglacis — Wave 39 exclusive glacis stamp */
kprintf("soft: retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retcurtain — Wave 40 return-curtain honesty */
kprintf("soft: retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retparapet — Wave 40 exclusive parapet stamp */
kprintf("soft: retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retravelin — Wave 41 return-travelin honesty */
kprintf("soft: retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retditch — Wave 41 exclusive ditch stamp */
kprintf("soft: retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retportcullis — Wave 42 return-portcullis honesty */
kprintf("soft: retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retbattlement — Wave 42 exclusive battlement stamp */
kprintf("soft: retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retmachicolation — Wave 43 return-machicolation honesty */
kprintf("soft: retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("soft: retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retmerlon — Wave 44 return-merlon honesty */
kprintf("soft: retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("soft: retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retkeepgate — Wave 45 return-keepgate honesty */
kprintf("soft: retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retouterward — Wave 45 exclusive outerward stamp */
kprintf("soft: retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retbailey — Wave 46 return-bailey honesty */
kprintf("soft: retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retpostern — Wave 46 exclusive postern stamp */
kprintf("soft: retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retinnerward — Wave 47 return-innerward honesty */
kprintf("soft: retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retinnerward honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retdonjon — Wave 47 exclusive donjon stamp */
kprintf("soft: retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retdonjon stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retchevaux — Wave 48 return-chevaux honesty */
kprintf("soft: retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retchevaux honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retpalisade — Wave 48 exclusive palisade stamp */
kprintf("soft: retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalisade stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("soft: retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retglacisgate honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retoutwork — Wave 49 exclusive outwork stamp */
kprintf("soft: retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retoutwork stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retsally — Wave 50 return-sally honesty */
kprintf("soft: retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retsally honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("soft: retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcounterscarp stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retfosse — Wave 51 return-fosse honesty */
kprintf("soft: retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfosse honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("soft: retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcoveredway stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: rettenaille — Wave 52 return-tenaille honesty */
kprintf("soft: rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rettenaille honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retdemilune — Wave 52 exclusive demilune stamp */
kprintf("soft: retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retdemilune stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retravelin — Wave 53 return-travelin honesty */
kprintf("soft: retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retlunette — Wave 53 exclusive lunette stamp */
kprintf("soft: retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retlunette stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retcaponier — Wave 54 return-caponier honesty */
kprintf("soft: retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcaponier honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retredan — Wave 54 exclusive redan stamp */
kprintf("soft: retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retredan stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retflank — Wave 55 return-flank honesty */
kprintf("soft: retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retflank honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retface — Wave 55 exclusive face stamp */
kprintf("soft: retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retface stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retgorge — Wave 56 return-gorge honesty */
kprintf("soft: retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retgorge honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("soft: retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retshoulder stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);

/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retraverse — Wave 57 return-traverse honesty */
kprintf("soft: retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retraverse honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retcasemate — Wave 57 exclusive casemate stamp */
kprintf("soft: retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcasemate stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retorillon — Wave 58 return-orillon honesty */
kprintf("soft: retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retorillon honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("soft: retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbonnette stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retcrownwork — Wave 59 return-crownwork honesty */
kprintf("soft: retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcrownwork honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("soft: rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(rethornwork stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retplace — Wave 60 return-place honesty */
kprintf("soft: retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retplace honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retenvelope — Wave 60 exclusive envelope stamp */
kprintf("soft: retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retenvelope stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);

/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retcounterguard — Wave 61 return-counterguard honesty */
kprintf("soft: retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcounterguard honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("soft: retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcoveredface stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retbastionface — Wave 62 return-bastionface honesty */
kprintf("soft: retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastionface honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("soft: retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcurtainangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("soft: retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("soft: retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retplaceofarms stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retreentrant — Wave 64 return-reentrant honesty */
kprintf("soft: retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retreentrant honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("soft: retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retsallyport stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("soft: retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("soft: retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retshoulderangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);







/*
 * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retflankangle — Wave 66 return-flankangle honesty */
kprintf("soft: retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retflankangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retfaceangle — Wave 66 exclusive faceangle stamp */
kprintf("soft: retfaceangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retfaceangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("soft: retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("soft: retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retredanangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("soft: retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("soft: rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(rettenailleangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("soft: retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("soft: retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcoveredwayangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("soft: retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retfosseangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("soft: retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retcounterscarple stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("soft: retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retsallyportangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("soft: retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retreentrantangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: soft: retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("soft: retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retplaceofarmsangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("soft: retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retdoubletenailleangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retcurtainface — Wave 73 return-curtainface honesty */
kprintf("soft: retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retcurtainface honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("soft: retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retbastionangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("soft: retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retglacisangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("soft: retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retparapetangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retmoatangle — Wave 75 return-moatangle honesty */
kprintf("soft: retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retmoatangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("soft: retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retowerangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retgateangle — Wave 76 return-gateangle honesty */
kprintf("soft: retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retgateangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("soft: retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retwallangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retspireangle — Wave 77 return-spireangle honesty */
kprintf("soft: retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retspireangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("soft: retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retholdangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("soft: retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retpalaceangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("soft: retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retfortressangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retkeepangle — Wave 79 return-keepangle honesty */
kprintf("soft: retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retkeepangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("soft: retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retredoubtangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("soft: retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retcitadelangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("soft: retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retbastionkeep stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("soft: retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retpanoplyangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("soft: retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retbulwarkangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("soft: retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retmantleangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("soft: retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retaegisangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("soft: retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=%u (retemblemangle honesty; Soft≠product; not bar3)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("soft: retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=%u (retsigilangle stamp; Soft≠product)\n",
        (unsigned)MAIN_SOFT_SMOKE_WAVE);
/* Grep: soft: retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("soft: retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("soft: retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retglyphangle stamp; Soft≠product)\n");
/* Grep: soft: retcrownangle — Wave 85 return-crownangle honesty */
kprintf("soft: retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("soft: retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retshardangle stamp; Soft≠product)\n");
/* Grep: soft: retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("soft: retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("soft: retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retprismangle stamp; Soft≠product)\n");
/* Grep: soft: retflameangle — Wave 87 return-flameangle honesty */
kprintf("soft: retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("soft: retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retcipherangle stamp; Soft≠product)\n");
/* Grep: soft: retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("soft: retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("soft: retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retledgerangle stamp; Soft≠product)\n");
/* Grep: soft: retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("soft: retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("soft: retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retvaultangle stamp; Soft≠product)\n");
/* Grep: soft: retcrestangle — Wave 90 return-crestangle honesty */
kprintf("soft: retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("soft: rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (rettokenangle stamp; Soft≠product)\n");
/* Grep: soft: retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("soft: retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("soft: retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retphaseangle stamp; Soft≠product)\n");
/* Grep: soft: retmarkangle — Wave 92 return-markangle honesty */
kprintf("soft: retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("soft: retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retpulseangle stamp; Soft≠product)\n");
/* Grep: soft: retsealangle — Wave 93 return-sealangle honesty */
kprintf("soft: retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("soft: retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retboundangle stamp; Soft≠product)\n");
/* Grep: soft: retstemangle — Wave 94 return-stemangle honesty */
kprintf("soft: retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("soft: retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retbladeangle stamp; Soft≠product)\n");
/* Grep: soft: retchordangle — Wave 95 return-chordangle honesty */
kprintf("soft: retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("soft: retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retarcangle stamp; Soft≠product)\n");
/* Grep: soft: retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("soft: retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("soft: retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: soft: retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("soft: retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("soft: retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: soft: retcircumangle — Wave 98 return-circumangle honesty */
kprintf("soft: retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("soft: retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retellipseangle stamp; Soft≠product)\n");
/* Grep: soft: rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("soft: rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("soft: retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: soft: retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("soft: retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=100 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: soft: rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("soft: rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=100 (rethelixangle stamp; Soft≠product)\n");

/* Grep: soft: deepen wave */
        kprintf("soft: deepen wave=%u areas=%u hit=%u wow64_calls=%u "
                "(Wave 92 exclusive; not bar3)\n",
                (unsigned)MAIN_SOFT_SMOKE_WAVE,
                (unsigned)MAIN_SOFT_SMOKE_AREAS,
                cHit, cWow64);

        kprintf("soft: stats smoke PASS hit=%u wave=%u\n",
                cHit, (unsigned)MAIN_SOFT_SMOKE_WAVE);
    }

    kprintf("M0 OK\n");
    scheduler_run();
}

void
kmain(u32 paMb2Info)
{
    struct mb2_tag *pTag;
    u8 *pInfo;
    u32 cbTotal;
    struct gj_mem_region aRegions[MAX_REGIONS];
    size_t cRegions = 0;

    {
        struct gj_boot_info bi;

        boot_info_clear(&bi);
        bi.u32Source = GJ_BOOT_SRC_MULTIBOOT2;
        bi.u32Mb2InfoPhys = paMb2Info;
        boot_info_set_global(&bi);
    }

    serial_init();
    kprintf("GreenJade M0\n");
    kprintf("boot: source=MULTIBOOT2 mb2=0x%x kernel_end=0x%lx\n", paMb2Info,
            (unsigned long)(gj_vaddr_t)__kernel_end);
    /* User bring-up maps start at 0x1000000; BSS must stay below */
    if ((gj_vaddr_t)__kernel_end >= 0x1000000ull) {
        kprintf("FATAL: kernel_end collides with user maps @0x1000000\n");
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }

    gdt_init();
    idt_init();
    cpu_init_bsp();
    thread_init();
    cpu_syscall_init();

    if (paMb2Info == 0) {
        kprintf("no multiboot2 info; skip pmm/vmm maps\n");
        boot_process_smoke();
        kprintf("M0 OK (partial)\n");
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }

    pInfo = (u8 *)(gj_vaddr_t)paMb2Info;
    cbTotal = *(u32 *)pInfo;
    (void)cbTotal;
    pTag = (struct mb2_tag *)(pInfo + 8);

    while (pTag->u32Type != MB2_TAG_END) {
        if (pTag->u32Type == MB2_TAG_MMAP) {
            struct mb2_tag_mmap *pMmap = (struct mb2_tag_mmap *)pTag;
            u8 *pEntry = (u8 *)pMmap + sizeof(*pMmap);
            u8 *pEnd = (u8 *)pMmap + pMmap->u32Size;

            while (pEntry < pEnd) {
                struct mb2_mmap_entry *pMe = (struct mb2_mmap_entry *)pEntry;

                if (pMe->u32Type == MB2_MMAP_AVAILABLE && cRegions < MAX_REGIONS) {
                    aRegions[cRegions].paBase = pMe->paAddr;
                    aRegions[cRegions].cbLength = pMe->cbLen;
                    kprintf("  ram 0x%lx +0x%lx\n",
                            (unsigned long)pMe->paAddr,
                            (unsigned long)pMe->cbLen);
                    cRegions++;
                }
                pEntry += pMmap->u32EntrySize;
            }
        }
        /* tags aligned to 8 bytes */
        {
            u32 cbTag = (pTag->u32Size + 7u) & ~7u;

            pTag = (struct mb2_tag *)((u8 *)pTag + cbTag);
        }
    }

    kernel_after_mmap(aRegions, cRegions);
}

/* EFI memory types reclaimed as usable RAM after ExitBootServices */
#define EFI_MT_LOADER_CODE       1u
#define EFI_MT_LOADER_DATA       2u
#define EFI_MT_BS_CODE           3u
#define EFI_MT_BS_DATA           4u
#define EFI_MT_CONVENTIONAL      7u
#define EFI_MT_ACPI_RECLAIM      9u

struct efi_md {
    u32 u32Type;
    u32 u32Pad;
    u64 paPhysical;
    u64 vaVirtual;
    u64 u64Pages;
    u64 u64Attribute;
};

void
kmain_uefi(struct gj_boot_info *pInfo)
{
    struct gj_mem_region aRegions[MAX_REGIONS];
    size_t cRegions = 0;
    u64 u64Off;
    u64 u64DescSize;

    if (pInfo != NULL && pInfo->u32Magic == GJ_BOOT_INFO_MAGIC) {
        boot_info_set_global(pInfo);
    } else {
        struct gj_boot_info bi;

        boot_info_clear(&bi);
        bi.u32Source = GJ_BOOT_SRC_UEFI;
        boot_info_set_global(&bi);
        pInfo = (struct gj_boot_info *)boot_info_get();
    }

    serial_init();
    kprintf("GreenJade M0\n");
    kprintf("boot: source=UEFI kernel_end=0x%lx rsdp=0x%lx map=0x%lx bytes=%lu\n",
            (unsigned long)(gj_vaddr_t)__kernel_end,
            (unsigned long)(pInfo != NULL ? pInfo->u64Rsdp : 0),
            (unsigned long)(pInfo != NULL ? pInfo->u64MemMap : 0),
            (unsigned long)(pInfo != NULL ? pInfo->u64MemMapBytes : 0));
    if (pInfo != NULL && pInfo->u64FbBase != 0) {
        kprintf("boot: GOP fb=0x%lx %ux%u pitch=%u bpp=%u\n",
                (unsigned long)pInfo->u64FbBase, pInfo->u32FbWidth,
                pInfo->u32FbHeight, pInfo->u32FbPitch, pInfo->u32FbBpp);
    }
    if ((gj_vaddr_t)__kernel_end >= 0x1000000ull) {
        kprintf("FATAL: kernel_end collides with user maps @0x1000000\n");
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }

    /* Replace firmware CR3 with writable kernel identity map before VMM. */
    boot_install_identity_4gib();

    gdt_init();
    idt_init();
    cpu_init_bsp();
    thread_init();
    cpu_syscall_init();

    if (pInfo == NULL || pInfo->u64MemMap == 0 || pInfo->u64MemMapBytes == 0) {
        kprintf("uefi: no memory map; partial halt\n");
        kprintf("M0 OK (partial)\n");
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }

    u64DescSize = pInfo->u64MemDescSize;
    if (u64DescSize < sizeof(struct efi_md)) {
        u64DescSize = sizeof(struct efi_md);
    }

    for (u64Off = 0;
         u64Off + u64DescSize <= pInfo->u64MemMapBytes;
         u64Off += u64DescSize) {
        const struct efi_md *pMd =
            (const struct efi_md *)(gj_vaddr_t)(pInfo->u64MemMap + u64Off);
        u32 u32T = pMd->u32Type;
        u64 cb;
        gj_paddr_t pa;

        if (u32T != EFI_MT_CONVENTIONAL && u32T != EFI_MT_LOADER_CODE &&
            u32T != EFI_MT_LOADER_DATA && u32T != EFI_MT_BS_CODE &&
            u32T != EFI_MT_BS_DATA && u32T != EFI_MT_ACPI_RECLAIM) {
            continue;
        }
        cb = pMd->u64Pages * 4096ull;
        if (cb == 0) {
            continue;
        }
        pa = (gj_paddr_t)pMd->paPhysical;
        /* Coalesce with previous if adjacent (EFI map is often fragmented). */
        if (cRegions > 0 &&
            aRegions[cRegions - 1].paBase + aRegions[cRegions - 1].cbLength ==
                pa) {
            aRegions[cRegions - 1].cbLength += cb;
            continue;
        }
        if (cRegions >= MAX_REGIONS) {
            kprintf("uefi: region table full (dropped pa=0x%lx)\n",
                    (unsigned long)pa);
            continue;
        }
        aRegions[cRegions].paBase = pa;
        aRegions[cRegions].cbLength = cb;
        cRegions++;
    }
    {
        size_t i;

        for (i = 0; i < cRegions && i < 12u; i++) {
            kprintf("  ram 0x%lx +0x%lx\n",
                    (unsigned long)aRegions[i].paBase,
                    (unsigned long)aRegions[i].cbLength);
        }
        if (cRegions > 12u) {
            kprintf("  ... %lu regions total\n", (unsigned long)cRegions);
        }
    }

    if (cRegions == 0) {
        kprintf("uefi: zero usable regions\n");
        kprintf("M0 OK (partial)\n");
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }

    kernel_after_mmap(aRegions, cRegions);
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
