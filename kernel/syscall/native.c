/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Native GreenJade syscalls (GJ_SYS_*; non-Linux numbers).
 *
 * Product path:
 *   - Entered only when process personality is NATIVE (or tests call us).
 *   - User pointers: user_range_ok + copy_{to,from}_user. The !user branch
 *     is for early kernel smokes that pass HHDM/static buffers.
 *   - Reserved CAP/VM/FUTEX numbers fall through to GJ_ERR_NOSUPPORT until
 *     their product wiring lands; numbers stay frozen (syscall.h).
 *   - Door façades (session/net/store/vfs) and platform ops are the main
 *     product surface for freestanding embeds (sessiond, scsi_mid, …).
 *
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 */
#include <gj/cold_ipc.h>
#include <gj/door.h>
#include <gj/error.h>
#include <gj/hda.h>
#include <gj/klog.h>
#include <gj/iommu.h>
#include <gj/net_door.h>
#include <gj/pci_caps.h>
#include <gj/scsi_mid.h>
#include <gj/session_door.h>
#include <gj/store_door.h>
#include <gj/vfs_door.h>
#include <gj/irq_msix.h>
#include <gj/linux_abi.h>
#include <gj/linux_dispatch.h>
#include <gj/memobj.h>
#include <gj/notify.h>
#include <gj/process.h>
#include <gj/spawn.h>
#include <gj/string.h>
#include <gj/syscall.h>
#include <gj/thread.h>
#include <gj/user_access.h>
#include <gj/virtio_gpu.h>
#include <gj/vmm.h>
#include <gj/wow64.h>

/* Cap DEBUG_LOG payload so a bad len cannot pin the CPU copying forever. */
#define GJ_NATIVE_DEBUG_LOG_MAX 4096u
/* Console non-blocking drain chunk (stack buffer). */
#define GJ_NATIVE_CONSOLE_CHUNK 64u
/* HDA write staging chunk. */
#define GJ_NATIVE_HDA_CHUNK     256u
/* SCSI xfer staging; one outstanding door submit at a time. */
#define GJ_NATIVE_SCSI_XFER_MAX 4096u

extern struct gj_process *g_pLinuxProc;

/**
 * Copy @cb bytes to caller buffer at @u64Dst.
 * Uses copy_to_user when the range is in the user VA window.
 */
static i64
native_copy_out(u64 u64Dst, const void *pSrc, u32 cb)
{
    if (u64Dst == 0 || pSrc == NULL || cb == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Dst, cb)) {
        if (copy_to_user(u64Dst, pSrc, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
    } else {
        /* Kernel-smoke path: destination is a trusted HHDM/static buffer. */
        memcpy((void *)(gj_vaddr_t)u64Dst, pSrc, cb);
    }
    return 0;
}

/**
 * Copy @cb bytes from caller buffer at @u64Src into @pDst.
 */
static i64
native_copy_in(void *pDst, u64 u64Src, u32 cb)
{
    if (pDst == NULL || u64Src == 0 || cb == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Src, cb)) {
        if (copy_from_user(pDst, u64Src, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
    } else {
        memcpy(pDst, (const void *)(gj_vaddr_t)u64Src, cb);
    }
    return 0;
}

/**
 * Copy a NUL-terminated name from @u64Src into @szDst (max @cbMax, incl NUL).
 * Always NUL-terminates on success. Returns 0 or GJ_ERR_*.
 */
static i64
native_copy_name(char *szDst, u64 u64Src, u32 cbMax)
{
    u32 i;
    i64 st;

    if (szDst == NULL || u64Src == 0 || cbMax < 2u) {
        return GJ_ERR_INVAL;
    }
    memset(szDst, 0, cbMax);
    for (i = 0; i + 1u < cbMax; i++) {
        char ch = 0;

        st = native_copy_in(&ch, u64Src + i, 1u);
        if (st != 0) {
            return st;
        }
        szDst[i] = ch;
        if (ch == '\0') {
            return 0;
        }
    }
    /* Truncated: force terminator (defensive; names must fit). */
    szDst[cbMax - 1u] = '\0';
    return 0;
}

/**
 * Resolve tid 0 to the calling thread id. Returns 0 if no current thread.
 */
static u32
native_tid_or_current(u32 u32Id)
{
    struct gj_thread *pCur;

    if (u32Id != 0) {
        return u32Id;
    }
    pCur = thread_current();
    if (pCur == NULL) {
        return 0;
    }
    return pCur->u32Id;
}

void
gj_native_syscall_dispatch(struct gj_syscall_regs *pRegs)
{
    if (pRegs == NULL) {
        return;
    }
    /* Native errors are GJ_ERR_*; unknown numbers land here until a case. */
    pRegs->i64Ret = GJ_ERR_NOSUPPORT;

    switch (pRegs->u64Nr) {
    case GJ_SYS_DEBUG_LOG: {
        char aKbuf[128];
        u64 u64Len = pRegs->u64Arg1;
        u64 u64Off = 0;
        u64 u64Src = pRegs->u64Arg0;
        size_t cbChunk;
        i64 st;

        if (u64Src == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (u64Len > GJ_NATIVE_DEBUG_LOG_MAX) {
            u64Len = GJ_NATIVE_DEBUG_LOG_MAX;
        }
        while (u64Off < u64Len) {
            cbChunk = (size_t)(u64Len - u64Off);
            if (cbChunk > sizeof(aKbuf) - 1u) {
                cbChunk = sizeof(aKbuf) - 1u;
            }
            st = native_copy_in(aKbuf, u64Src + u64Off, (u32)cbChunk);
            if (st != 0) {
                pRegs->i64Ret = u64Off ? (i64)u64Off : st;
                break;
            }
            aKbuf[cbChunk] = '\0';
            kprintf("%s", aKbuf);
            u64Off += cbChunk;
        }
        if (u64Off == u64Len) {
            pRegs->i64Ret = (i64)u64Len;
        }
        break;
    }
    case GJ_SYS_YIELD:
        thread_yield();
        pRegs->i64Ret = 0;
        break;
    case GJ_SYS_EXIT:
        kprintf("gj: SYS_EXIT %ld\n", (long)pRegs->u64Arg0);
        thread_exit();
        /* Not reached on a live path; keep a defined ret for smokes. */
        pRegs->i64Ret = 0;
        break;
    case GJ_SYS_PROCESS_SPAWN: {
        /* arg0 = entry VA (kernel smoke: function ptr), arg1 = arg, arg2 = flags */
        struct gj_spawn_args args;
        struct gj_cap_ref ref;
        struct gj_process *pParent = g_pLinuxProc;
        gj_status_t st;

        if (pParent == NULL || pRegs->u64Arg0 == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        memset(&args, 0, sizeof(args));
        memset(&ref, 0, sizeof(ref));
        args.pfnEntry = (void (*)(void *))(gj_vaddr_t)pRegs->u64Arg0;
        args.pArg = (void *)(gj_vaddr_t)pRegs->u64Arg1;
        args.u32Jit = (pRegs->u64Arg2 & 1ull) ? 1u : 0u;
        args.u32Personality = 1;
        st = process_spawn(pParent, &args, NULL, &ref);
        if (st != GJ_OK) {
            pRegs->i64Ret = st;
        } else {
            /* Return slot in low 32, gen in high — smoke ABI */
            pRegs->i64Ret = (i64)((ref.u64Slot & 0xffffffffull) |
                                  ((u64)ref.u32SlotGen << 32));
        }
        break;
    }
    case GJ_SYS_IPC_CALL: {
        /* Cold personality door call: arg0..5 = linux-ish regs payload */
        struct gj_linux_regs req;
        struct gj_door *pDoor;

        memset(&req, 0, sizeof(req));
        req.u64Nr = pRegs->u64Arg0;
        req.u64Arg0 = pRegs->u64Arg1;
        req.u64Arg1 = pRegs->u64Arg2;
        req.u64Arg2 = pRegs->u64Arg3;
        req.u64Arg3 = pRegs->u64Arg4;
        req.u64Arg4 = pRegs->u64Arg5;
        pDoor = door_cold_personality();
        if (pDoor == NULL) {
            pRegs->i64Ret = GJ_ERR_NODEV;
            break;
        }
        pRegs->i64Ret = door_call(pDoor, &req);
        break;
    }
    case GJ_SYS_IPC_RECV: {
        /* arg0 = user ptr to gj_linux_regs (or kernel for smoke) */
        struct gj_linux_regs req;
        struct gj_door *pDoor;
        int n;
        i64 st;

        pDoor = door_cold_personality();
        if (pDoor == NULL) {
            pRegs->i64Ret = GJ_ERR_NODEV;
            break;
        }
        memset(&req, 0, sizeof(req));
        n = door_recv(pDoor, &req);
        if (n != 0) {
            pRegs->i64Ret = GJ_ERR_AGAIN;
            break;
        }
        if (pRegs->u64Arg0 == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        st = native_copy_out(pRegs->u64Arg0, &req, (u32)sizeof(req));
        pRegs->i64Ret = (st == 0) ? 0 : st;
        break;
    }
    case GJ_SYS_IPC_REPLY: {
        struct gj_door *pDoor = door_cold_personality();

        if (pDoor == NULL) {
            pRegs->i64Ret = GJ_ERR_NODEV;
            break;
        }
        door_reply(pDoor, (i64)pRegs->u64Arg0);
        pRegs->i64Ret = 0;
        break;
    }
    case GJ_SYS_PERSONALITY_SERVE: {
        /* arg0 = user/kernel ptr to gj_linux_regs; run cold service, write ret */
        struct gj_linux_regs req;
        i64 st;

        if (pRegs->u64Arg0 == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (user_range_ok(pRegs->u64Arg0, sizeof(req))) {
            st = native_copy_in(&req, pRegs->u64Arg0, (u32)sizeof(req));
            if (st != 0) {
                pRegs->i64Ret = st;
                break;
            }
            (void)cold_ipc_service_local(&req);
            st = native_copy_out(pRegs->u64Arg0, &req, (u32)sizeof(req));
            pRegs->i64Ret = (st == 0) ? req.i64Ret : st;
        } else {
            struct gj_linux_regs *pK =
                (struct gj_linux_regs *)(gj_vaddr_t)pRegs->u64Arg0;

            pRegs->i64Ret = cold_ipc_service_local(pK);
        }
        break;
    }
    case GJ_SYS_COLD_DEQUEUE: {
        struct gj_cold_request *pOut;

        if (pRegs->u64Arg0 == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        pOut = (struct gj_cold_request *)(gj_vaddr_t)pRegs->u64Arg0;
        pRegs->i64Ret = cold_ipc_dequeue(pOut) ? 1 : 0;
        break;
    }
    case GJ_SYS_COLD_REPLY:
        pRegs->i64Ret = cold_ipc_reply(pRegs->u64Arg0, (i64)pRegs->u64Arg1)
                            ? 0
                            : GJ_ERR_NOENT;
        break;
    case GJ_SYS_THREAD_SET_QOS:
        /* arg0 = thr id (0 = current), arg1 = qos class */
        {
            u32 u32Id = native_tid_or_current((u32)pRegs->u64Arg0);

            if (u32Id == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            thread_set_qos(u32Id, (u32)pRegs->u64Arg1);
            pRegs->i64Ret = 0;
        }
        break;
    case GJ_SYS_THREAD_SET_CPU:
        /* arg0 = thr id (0 = current), arg1 = cpu slot */
        {
            u32 u32Id = native_tid_or_current((u32)pRegs->u64Arg0);

            if (u32Id == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            thread_set_cpu(u32Id, (u32)pRegs->u64Arg1);
            pRegs->i64Ret = 0;
        }
        break;
    case GJ_SYS_GPU_PRESENT: {
        /* Compositor / ICD present: width, height, fb VA, stride */
        u32 u32W = (u32)pRegs->u64Arg0;
        u32 u32H = (u32)pRegs->u64Arg1;
        void *pFb = (void *)(gj_vaddr_t)pRegs->u64Arg2;
        u32 u32Stride = (u32)pRegs->u64Arg3;

        if (!virtio_gpu_ready()) {
            pRegs->i64Ret = GJ_ERR_NODEV;
            break;
        }
        if (pFb == NULL || u32W == 0 || u32H == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        pRegs->i64Ret =
            virtio_gpu_present(u32W, u32H, pFb, u32Stride) == 0 ? 0 : GJ_ERR_IO;
        break;
    }
    case GJ_SYS_GPU_DISPLAY_INFO: {
        u32 aWH[2];
        u32 u32W = 0;
        u32 u32H = 0;
        i64 st;

        if (pRegs->u64Arg0 == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (!virtio_gpu_ready() ||
            virtio_gpu_get_display(&u32W, &u32H) != 0) {
            pRegs->i64Ret = GJ_ERR_NODEV;
            break;
        }
        aWH[0] = u32W;
        aWH[1] = u32H;
        st = native_copy_out(pRegs->u64Arg0, aWH, (u32)sizeof(aWH));
        pRegs->i64Ret = (st == 0) ? 0 : st;
        break;
    }
    case GJ_SYS_MEMOBJ_CREATE_NAMED: {
        char szName[GJ_MEMOBJ_NAME_MAX];
        struct gj_memobj *pObj;
        i64 st;

        if (pRegs->u64Arg0 == 0 || pRegs->u64Arg1 == 0) {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        st = native_copy_name(szName, pRegs->u64Arg0, (u32)sizeof(szName));
        if (st != 0) {
            pRegs->i64Ret = st;
            break;
        }
        if (szName[0] == '\0') {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        pObj = memobj_create_named(szName, (u32)pRegs->u64Arg1);
        pRegs->i64Ret = (pObj != NULL) ? 0 : GJ_ERR_NOMEM;
        break;
    }
    case GJ_SYS_SESSION:
        pRegs->i64Ret = session_door_call((u32)pRegs->u64Arg0, pRegs->u64Arg1,
                                          pRegs->u64Arg2, pRegs->u64Arg3);
        break;
    case GJ_SYS_NET:
        pRegs->i64Ret = net_door_call((u32)pRegs->u64Arg0, pRegs->u64Arg1,
                                      pRegs->u64Arg2, pRegs->u64Arg3);
        break;
    case GJ_SYS_STORE:
        pRegs->i64Ret = store_door_call((u32)pRegs->u64Arg0, pRegs->u64Arg1,
                                        pRegs->u64Arg2, pRegs->u64Arg3);
        break;
    case GJ_SYS_VFS:
        pRegs->i64Ret = vfs_door_call((u32)pRegs->u64Arg0, pRegs->u64Arg1,
                                      pRegs->u64Arg2, pRegs->u64Arg3);
        break;
    case GJ_SYS_CONSOLE: {
        u32 u32Op = (u32)pRegs->u64Arg0;

        if (u32Op == 0) {
            pRegs->i64Ret = serial_poll();
        } else if (u32Op == 1) {
            pRegs->i64Ret = serial_getchar();
        } else if (u32Op == 2) {
            u64 u64Buf = pRegs->u64Arg1;
            u32 cbMax = (u32)pRegs->u64Arg2;
            u32 n = 0;
            u8 aTmp[GJ_NATIVE_CONSOLE_CHUNK];
            i64 st;

            if (u64Buf == 0 || cbMax == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            if (cbMax > sizeof(aTmp)) {
                cbMax = (u32)sizeof(aTmp);
            }
            while (n < cbMax && serial_poll()) {
                aTmp[n++] = (u8)serial_getchar();
            }
            if (n > 0) {
                st = native_copy_out(u64Buf, aTmp, n);
                if (st != 0) {
                    pRegs->i64Ret = st;
                    break;
                }
            }
            pRegs->i64Ret = (i64)n;
        } else {
            pRegs->i64Ret = GJ_ERR_INVAL;
        }
        break;
    }
    case GJ_SYS_PLATFORM_INFO: {
        u32 u32Op = (u32)pRegs->u64Arg0;

        if (u32Op == 0) {
            struct gj_iommu_info info;
            i64 st;

            memset(&info, 0, sizeof(info));
            iommu_info_get(&info);
            if (pRegs->u64Arg1 != 0) {
                st = native_copy_out(pRegs->u64Arg1, &info, (u32)sizeof(info));
                if (st != 0) {
                    pRegs->i64Ret = st;
                    break;
                }
            }
            pRegs->i64Ret = info.u8Present ? 1 : 0;
        } else if (u32Op == 1) {
            struct gj_pci_msix_info aInfo[16];
            u32 n;
            u32 cb;

            memset(aInfo, 0, sizeof(aInfo));
            n = pci_msix_scan(aInfo, 16);
            if (pRegs->u64Arg1 != 0 && n > 0) {
                cb = n * (u32)sizeof(aInfo[0]);
                if (cb > sizeof(aInfo)) {
                    cb = (u32)sizeof(aInfo);
                }
                /* Best-effort inventory copy; count still returned. */
                (void)native_copy_out(pRegs->u64Arg1, aInfo, cb);
            }
            pRegs->i64Ret = (i64)n;
        } else if (u32Op == 2) {
            /* WoW64 personality control */
            u32 u32Act = (u32)pRegs->u64Arg1;

            if (u32Act == 1) {
                wow64_set(1);
            } else if (u32Act == 2) {
                wow64_set(0);
            }
            pRegs->i64Ret = (i64)wow64_calls() |
                            ((i64)(wow64_enabled() ? 1 : 0) << 32);
        } else if (u32Op == 3) {
            /* MSI-X Notification soft inject */
            irq_msix_soft_inject(pRegs->u64Arg1 ? pRegs->u64Arg1 : 1ull);
            pRegs->i64Ret = (i64)irq_msix_count();
        } else if (u32Op == 4) {
            iommu_enforce_set((int)pRegs->u64Arg1);
            pRegs->i64Ret = iommu_enforce_get();
        } else if (u32Op == 5) {
            u32 u32Bdf = (u32)pRegs->u64Arg1;
            u8 u8Bus = (u8)((u32Bdf >> 16) & 0xffu);
            u8 u8Slot = (u8)((u32Bdf >> 8) & 0xffu);
            u8 u8Func = (u8)(u32Bdf & 0xffu);

            if (pRegs->u64Arg3 == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            if (iommu_window_grant(u8Bus, u8Slot, u8Func, pRegs->u64Arg2,
                                   pRegs->u64Arg3) != 0) {
                pRegs->i64Ret = GJ_ERR_NOMEM;
            } else {
                pRegs->i64Ret = (i64)iommu_window_count();
            }
        } else {
            pRegs->i64Ret = GJ_ERR_INVAL;
        }
        break;
    }
    case GJ_SYS_NOTIFY_WAIT: {
        struct gj_notify *pN;
        u64 u64Mask = pRegs->u64Arg1;
        int fBlock = (int)pRegs->u64Arg2;

        if ((u32)pRegs->u64Arg0 == 0) {
            pN = notify_msix_global();
        } else {
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (pN == NULL) {
            pRegs->i64Ret = GJ_ERR_NODEV;
            break;
        }
        pRegs->i64Ret = (i64)notify_wait(pN, u64Mask, fBlock);
        break;
    }
    case GJ_SYS_SCSI: {
        /* arg0=op → scsi_door_submit / transport probe (userspace scsi_mid). */
        u32 u32Op = (u32)pRegs->u64Arg0;
        struct scsi_door_req dreq;
        static u8 aScsiXfer[GJ_NATIVE_SCSI_XFER_MAX];

        switch (u32Op) {
        case 5: /* ready */
            pRegs->i64Ret = scsi_mid_ready() ? 1 : 0;
            break;
        case 6: { /* stats */
            u32 aSt[2];
            i64 st;

            if (pRegs->u64Arg1 == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            aSt[0] = scsi_door_io_count();
            aSt[1] = scsi_mid_ready() ? 1u : 0u;
            st = native_copy_out(pRegs->u64Arg1, aSt, (u32)sizeof(aSt));
            pRegs->i64Ret = (st == 0) ? 0 : st;
            break;
        }
        case 0: { /* inquiry */
            u32 cb = (u32)pRegs->u64Arg2;
            i64 st;

            if (pRegs->u64Arg1 == 0 || cb == 0 || cb > sizeof(aScsiXfer)) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            memset(&dreq, 0, sizeof(dreq));
            dreq.u32Op = 0;
            dreq.cbData = cb;
            if (scsi_door_submit(&dreq, aScsiXfer, cb) != 0) {
                pRegs->i64Ret = GJ_ERR_IO;
                break;
            }
            st = native_copy_out(pRegs->u64Arg1, aScsiXfer, cb);
            pRegs->i64Ret = (st == 0) ? (i64)cb : st;
            break;
        }
        case 1: { /* read capacity → u32[2] */
            u32 aCap[2];
            i64 st;

            if (pRegs->u64Arg1 == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            memset(&dreq, 0, sizeof(dreq));
            dreq.u32Op = 1;
            dreq.cbData = 8;
            if (scsi_door_submit(&dreq, aScsiXfer, 8) != 0) {
                pRegs->i64Ret = GJ_ERR_IO;
                break;
            }
            /* big-endian last LBA + block len */
            aCap[0] = ((u32)aScsiXfer[0] << 24) | ((u32)aScsiXfer[1] << 16) |
                      ((u32)aScsiXfer[2] << 8) | (u32)aScsiXfer[3];
            aCap[1] = ((u32)aScsiXfer[4] << 24) | ((u32)aScsiXfer[5] << 16) |
                      ((u32)aScsiXfer[6] << 8) | (u32)aScsiXfer[7];
            st = native_copy_out(pRegs->u64Arg1, aCap, (u32)sizeof(aCap));
            pRegs->i64Ret = (st == 0) ? 0 : st;
            break;
        }
        case 2: /* read10 */
        case 3: { /* write10 */
            u32 u32Lba = (u32)pRegs->u64Arg1;
            u32 u32Blocks = (u32)pRegs->u64Arg3;
            u32 cb;
            i64 st;

            if (u32Blocks == 0) {
                u32Blocks = 1;
            }
            /* Guard multiply overflow before size check. */
            if (u32Blocks > (sizeof(aScsiXfer) / 512u)) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            cb = u32Blocks * 512u;
            if (pRegs->u64Arg2 == 0 || cb == 0 || cb > sizeof(aScsiXfer)) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            memset(&dreq, 0, sizeof(dreq));
            dreq.u32Op = u32Op;
            dreq.u32Lba = u32Lba;
            dreq.u16Blocks = (u16)u32Blocks;
            dreq.cbData = cb;
            if (u32Op == 3) {
                st = native_copy_in(aScsiXfer, pRegs->u64Arg2, cb);
                if (st != 0) {
                    pRegs->i64Ret = st;
                    break;
                }
            }
            if (scsi_door_submit(&dreq, aScsiXfer, cb) != 0) {
                pRegs->i64Ret = GJ_ERR_IO;
                break;
            }
            if (u32Op == 2) {
                st = native_copy_out(pRegs->u64Arg2, aScsiXfer, cb);
                if (st != 0) {
                    pRegs->i64Ret = st;
                    break;
                }
            }
            pRegs->i64Ret = (i64)cb;
            break;
        }
        default:
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
        break;
    }
    case GJ_SYS_HDA_STREAM: {
        /* arg0=op: 0 open, 1 write, 2 start, 3 tick, 4 close, 5 stats */
        u32 u32Op = (u32)pRegs->u64Arg0;

        switch (u32Op) {
        case 0:
            pRegs->i64Ret =
                hda_stream_open((u32)pRegs->u64Arg1, (u32)pRegs->u64Arg2,
                                (u32)pRegs->u64Arg3) == 0
                    ? 0
                    : GJ_ERR_INVAL;
            break;
        case 1: {
            u8 aTmp[GJ_NATIVE_HDA_CHUNK];
            u32 cbWant = (u32)pRegs->u64Arg2;
            u32 cbGot = 0;
            u32 cbChunk;
            i64 st;

            if (pRegs->u64Arg1 == 0 || cbWant == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            while (cbGot < cbWant) {
                cbChunk = cbWant - cbGot;
                if (cbChunk > sizeof(aTmp)) {
                    cbChunk = (u32)sizeof(aTmp);
                }
                st = native_copy_in(aTmp, pRegs->u64Arg1 + cbGot, cbChunk);
                if (st != 0) {
                    pRegs->i64Ret = (cbGot != 0) ? (i64)cbGot : st;
                    goto hda_done;
                }
                {
                    u32 n = hda_stream_write(aTmp, cbChunk);

                    cbGot += n;
                    if (n < cbChunk) {
                        break;
                    }
                }
            }
            pRegs->i64Ret = (i64)cbGot;
            break;
        }
        case 2:
            pRegs->i64Ret = hda_stream_start() == 0 ? 0 : GJ_ERR_INVAL;
            break;
        case 3:
            pRegs->i64Ret = (i64)hda_stream_tick((u32)pRegs->u64Arg1);
            break;
        case 4:
            hda_stream_close();
            pRegs->i64Ret = 0;
            break;
        case 5: {
            u32 aSt[3];
            i64 st;

            if (pRegs->u64Arg1 == 0) {
                pRegs->i64Ret = GJ_ERR_INVAL;
                break;
            }
            aSt[0] = hda_stream_bytes_queued();
            aSt[1] = hda_stream_bytes_played();
            aSt[2] = hda_stream_underruns();
            st = native_copy_out(pRegs->u64Arg1, aSt, (u32)sizeof(aSt));
            pRegs->i64Ret = (st == 0) ? 0 : st;
            break;
        }
        default:
            pRegs->i64Ret = GJ_ERR_INVAL;
            break;
        }
    hda_done:
        break;
    }
    case GJ_SYS_MEMOBJ_MAP_NAMED: {
        char szName[GJ_MEMOBJ_NAME_MAX];
        gj_vaddr_t va;
        struct gj_process *pProc = g_pLinuxProc;
        i64 st;

        /* ABI: failure returns 0 (not a negative errno). */
        if (pRegs->u64Arg0 == 0 || pProc == NULL) {
            pRegs->i64Ret = 0;
            break;
        }
        st = native_copy_name(szName, pRegs->u64Arg0, (u32)sizeof(szName));
        if (st != 0 || szName[0] == '\0') {
            pRegs->i64Ret = 0;
            break;
        }
        {
            u32 u32Prot = (u32)pRegs->u64Arg2
                              ? (u32)pRegs->u64Arg2
                              : (GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE);
            /* Userspace maps always need U/S so ring-3 can touch the pages. */
            u32Prot |= GJ_VMM_PROT_USER;
            va = memobj_map_named(pProc, szName, pRegs->u64Arg1, u32Prot);
        }
        pRegs->i64Ret = (i64)(u64)va;
        break;
    }
    default:
        pRegs->i64Ret = GJ_ERR_NOSUPPORT;
        break;
    }
}
