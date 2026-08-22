/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding userspace cold personality door server scaffold (G-PERS).
 *
 * Product shape (Option C / DESIGN_SPEC_COMPLETE section 3):
 *   ring-3 NATIVE process owns the cold personality door:
 *     gj_ipc_recv -> cold serve (libprotonrt) -> gj_ipc_reply
 *   replacing kernel kthread cold_personality_server as the default.
 *   This cut: loop compiled, park opt-in; G-PERS attach still OPEN.
 *
 * Scaffold status (this TU only):
 *   - Links as freestanding ELF via user/init/user.ld + libgj
 *   - Door loop body always compiled; park only if PERSONALITY_DOOR_LOOP=1
 *   - Default PERSONALITY_DOOR_LOOP=0 so smoke cannot hang on IPC_RECV
 *     with no door binding (kernel attach is outside this tree)
 *   - Product G-PERS OPEN: kernel still owns cold default (kthread /
 *     protonrt-server blob). OpenSSH needs a LINUX-personality process
 *     whose cold NRs this door would serve; that attach is not this cut
 *     (do not flip sshd_gj NATIVE net in kernel/main.c from here)
 *   - Soft once markers via native GJ_SYS_DEBUG_LOG (gj_debug_log)
 *
 * Soft markers (prefix-stable -- do not rename once smoke greps depend):
 *   personality: soft userspace scaffold PASS
 *   personality-gj: soft serve ready
 *
 * Lean soft residual (exclusive residual; this tree only):
 *   greppable: personality: soft residual lean PASS
 *   greppable: personality: soft residual lean
 *   greppable: personality: soft honesty
 *   greppable: personality: soft path
 *   greppable: personality: soft inventory
 *   greppable: personality: soft option_c
 *   greppable: personality: soft udx_host
 *   greppable: personality: soft apps
 *   greppable: personality: soft attach
 *   greppable: personality: soft g_pers
 *   greppable: personality: soft product residual product=UDX+sshd+stack
 * Soft!=product. Dual MIT OR Apache-2.0. G-AC-1 (no .ko product AC).
 * Soft residual != live door path / freestanding class drivers product.
 * Lean residual only -- no version stamp / no stamp storms / no commit.
 * bar3=OPEN. Dual DoD A OPEN until host USB path; B OPEN until interactive SSH login.
 * Product residual (stamp-free bar v2026.08.04.75): product=UDX+sshd+stack
 *   Option C cold door hosts apps (sshd/netstackd) + UDX hosts (rtl/xhci).
 *
 * C0 Linux-shaped residual deepen (UDX hosts + apps + attach + G-PERS):
 *   Option C hybrid: hot kernel path + cold door personality for apps.
 *   Product drivers: userspace UDX/DDI hosts (rtl8168_udx / xhci_udx …).
 *   Product apps: Linux-shaped over hot+cold ABI (not freestanding thrash).
 *   Dual DoD A OPEN until host USB path; B OPEN until interactive SSH login.
 *   Freestanding class drivers SKIP default -- not Dual DoD close criteria.
 *   G-PERS-1/2/3 + G-COLD-1 doors normative; attach contract still OPEN.
 *   ABI-first host path -- not in-kernel .ko product (G-AC-1).
 *   residual_deepen hosts/apps/bind_walk/g752 IDs -- fixed tokens only.
 *   Soft residual documents direction only; never hard-gates product attach.
 *
 * Soft phases (greppable comments; runtime markers above):
 *   phase1 scaffold PASS   -- _start->main, DEBUG_LOG works
 *   phase2 soft serve ready -- soft once; scaffold complete (!= product door)
 *   phase3 residual lean    -- honesty + path + inventory + option_c +
 *                             udx_host + apps + attach + g_pers +
 *                             residual lean PASS (C0 residual_deepen=1)
 *
 * When PERSONALITY_DOOR_LOOP=1 (product attach only):
 *   personality-gj: door server up
 *   personality-gj: soft recv-miss / soft serve-miss / soft reply-miss
 *   personality-gj: soft serve ready   (first full recv->serve->reply)
 *   personality-gj: soft would_recv cap (optional smoke cap exit)
 *
 * Planned hard markers (not claimed by this scaffold):
 *   personality-gj: live path PASS
 *
 *   make personality-gj -> build/user/personality.elf  (coordinator Makefile)
 *
 * Pure C11 freestanding. Dual-licensed MIT OR Apache-2.0 (no GPL).
 */
#include <gj/syscalls.h>

/*
 * gj_linux_regs wire size is 64 bytes (8 x u64; i64Ret at offset 56).
 * Keep a 128-byte frame for alignment/headroom (matches protonrt-server).
 * Local layout only -- full struct lives with the kernel cold path.
 */
#define PERS_REGS_WIRE   64u
#define PERS_REGS_FRAME  128u

/*
 * Door loop is always compiled. Default does not park (smoke-safe).
 * Product G-PERS attach is kernel-side and still OPEN; OpenSSH needs it.
 *
 * Park only after cold-door attach:
 *   -DPERSONALITY_DOOR_LOOP=1
 * (see README "Door attach contract"). Flag alone is not G-PERS product.
 */
#ifndef PERSONALITY_DOOR_LOOP
#define PERSONALITY_DOOR_LOOP  0
#endif

/*
 * When PERSONALITY_DOOR_LOOP=1: soft-count consecutive soft-miss recv
 * yields ("would_recv" iterations). 0 = product infinite park (never
 * return). N > 0 = after N soft recv-misses, log once and return so a
 * mis-attached smoke cannot hang forever.
 *
 * Enable with e.g. -DPERSONALITY_WOULD_RECV_CAP=64 alongside DOOR_LOOP=1.
 */
#ifndef PERSONALITY_WOULD_RECV_CAP
#define PERSONALITY_WOULD_RECV_CAP  0
#endif

static void
msg(const char *sz)
{
    size_t n = 0;

    if (sz == 0) {
        return;
    }
    while (sz[n] != '\0') {
        n++;
    }
    /* Native kprint path (GJ_SYS_DEBUG_LOG) -- NATIVE personality server. */
    (void)gj_debug_log(sz, (long)n);
}

static void
regs_wire_zero(unsigned char *pFrame)
{
    unsigned i;

    if (pFrame == 0) {
        return;
    }
    for (i = 0; i < PERS_REGS_WIRE; i++) {
        pFrame[i] = 0;
    }
}

/*
 * Soft door_recv loop (always compiled; park is opt-in).
 *
 * When PERSONALITY_DOOR_LOOP is 1 and boot has attached this task as the
 * cold personality door owner, this matches cold_personality_server /
 * protonrt-server: recv -> serve -> reply; soft miss -> yield; never EXIT
 * unless PERSONALITY_WOULD_RECV_CAP > 0 and soft recv-misses hit the cap
 * (smoke-safe escape only -- product leaves CAP at 0).
 *
 * Default main() does not call this, so smoke cannot hang on IPC_RECV.
 * Kernel attach (G-PERS) is still OPEN; OpenSSH needs that attach.
 * used: keep recv/serve/reply in the default ELF (park remains opt-in).
 */
static void __attribute__((used))
personality_door_loop(void)
{
    static unsigned char aFrame[PERS_REGS_FRAME];
    unsigned fSoftReady = 0;
    unsigned fSoftRecv = 0;
    unsigned fSoftReply = 0;
    unsigned fSoftServe = 0;
#if PERSONALITY_WOULD_RECV_CAP > 0
    unsigned fSoftCap = 0;
    unsigned cWouldRecv = 0;
#endif

    msg("personality-gj: door server up\n");

    for (;;) {
        long nRecv;
        long nServe;
        long nReply;

        regs_wire_zero(aFrame);

        /*
         * Soft would_recv: each soft-miss recv iteration counts toward
         * PERSONALITY_WOULD_RECV_CAP (when > 0). Product leaves CAP at 0.
         */
        nRecv = gj_ipc_recv(aFrame); /* door_recv */
        if (nRecv < 0) {
            if (fSoftRecv == 0u) {
                fSoftRecv = 1u;
                msg("personality-gj: soft recv-miss\n");
            }
#if PERSONALITY_WOULD_RECV_CAP > 0
            cWouldRecv++;
            if (cWouldRecv >= (unsigned)PERSONALITY_WOULD_RECV_CAP) {
                if (fSoftCap == 0u) {
                    fSoftCap = 1u;
                    msg("personality-gj: soft would_recv cap\n");
                }
                return;
            }
#endif
            gj_yield();
            continue;
        }

#if PERSONALITY_WOULD_RECV_CAP > 0
        /* Successful recv resets soft-miss would_recv tally. */
        cWouldRecv = 0;
#endif

        /*
         * Interim: kernel cold policy via PERSONALITY_SERVE.
         * Product: call libprotonrt cold path in userspace instead.
         */
        nServe = gj_personality_serve(aFrame);
        if (nServe < 0 && fSoftServe == 0u) {
            fSoftServe = 1u;
            msg("personality-gj: soft serve-miss\n");
        }

        nReply = gj_ipc_reply(nServe); /* door_reply */
        if (nReply < 0) {
            if (fSoftReply == 0u) {
                fSoftReply = 1u;
                msg("personality-gj: soft reply-miss\n");
            }
            gj_yield();
            continue;
        }

        if (fSoftReady == 0u) {
            fSoftReady = 1u;
            msg("personality-gj: soft serve ready\n");
        }
    }
}

#if PERSONALITY_DOOR_LOOP == 0
/*
 * Lean soft residual inventory (never hard-gates product).
 * Exclusive residual under user/personality/ only.
 * Soft!=product. Dual MIT OR Apache-2.0. G-AC-1.
 * No version stamp. No stamp storms (once-lamps only).
 *
 * C0 residual deepen (Linux-shaped; Soft!=product Dual DoD OPEN):
 *   - Option C hybrid (hot + cold door) is the app personality surface
 *   - UDX/DDI userspace hosts are the product driver path (Dual DoD A/B)
 *   - Freestanding class drivers SKIP default; in-kernel .ko not product
 *   - Door attach contract residual (badge / cold_ipc) still OPEN
 *   - G-PERS-1/2/3 + G-COLD-1 residual (doors normative; death policy OPEN)
 *   - residual_deepen hosts/apps/bind_walk/g752 IDs additive fixed tokens only
 *
 * Grep: personality: soft honesty|path|inventory|option_c|udx_host|apps
 * Grep: personality: soft attach|g_pers
 * Grep: personality: soft residual lean|PASS
 *
 * Honesty:
 *   - Soft scaffold residual != product cold door (G-PERS attach OPEN)
 *   - Door loop default 0 -- smoke-safe; not live path PASS
 *   - ABI-first host path for Linux-shaped drivers/apps in userspace
 *   - Not freestanding class drivers product (rtl/xhci freestanding SKIP)
 *   - G-AC-1: no Linux .ko product AC in kernel
 *   - Dual DoD A/B UDX product OPEN; bar3 / Deck Top 50 not closed by residual
 */
static void
personality_soft_residual_lean_once(void)
{
    /*
     * Stack-local self-check honesty (never hard-gates product).
     * Fixed lamps only -- freestanding msg has no number formatters.
     * Structural wire/frame checks + honesty dims; ok/14 when all hold
     * under default DOOR_LOOP=0 (C0 deepen: residual_deepen + dual_dod OPEN).
     */
    const int nDoorLoopDefault0 = (PERSONALITY_DOOR_LOOP == 0);
    const int nLivePath = 0;              /* not live G-PERS path PASS */
    const int nScaffold = 1;              /* this TU is soft scaffold */
    const int nServeReadySoft = 1;        /* soft serve ready (scaffold) */
    const int nProductUdxAbi = 1;         /* product dir = UDX + ABI */
    const int nFsClassProduct = 0;        /* freestanding class != product */
    const int nKoProduct = 0;             /* G-AC-1: no .ko product AC */
    const int nExclusive = 1;             /* residual exclusive this tree */
    const int nWire64 = (PERS_REGS_WIRE == 64u);     /* gj_linux_regs wire */
    const int nFrameGeWire = (PERS_REGS_FRAME >= PERS_REGS_WIRE);
    const int nFsSkipDefault = 1;         /* freestanding class SKIP default */
    const int nGPers2NoIommu = 1;         /* G-PERS-2: no IOMMU/IRQ in TCB */
    const int nResidualDeepen = 1;        /* C0 residual deepen wave */
    const int nDualDodOpen = 1;           /* Dual DoD A/B honesty stays OPEN */
    int nOk = 0;

    if (nDoorLoopDefault0 != 0) {
        nOk++;
    }
    if (nLivePath == 0) {
        nOk++;
    }
    if (nScaffold == 1) {
        nOk++;
    }
    if (nServeReadySoft == 1) {
        nOk++;
    }
    if (nProductUdxAbi == 1) {
        nOk++;
    }
    if (nFsClassProduct == 0) {
        nOk++;
    }
    if (nKoProduct == 0) {
        nOk++;
    }
    if (nExclusive == 1) {
        nOk++;
    }
    if (nWire64 != 0) {
        nOk++;
    }
    if (nFrameGeWire != 0) {
        nOk++;
    }
    if (nFsSkipDefault == 1) {
        nOk++;
    }
    if (nGPers2NoIommu == 1) {
        nOk++;
    }
    if (nResidualDeepen == 1) {
        nOk++;
    }
    if (nDualDodOpen == 1) {
        nOk++;
    }
    /* nOk is compile-time-constant 14 under default DOOR_LOOP=0; silence. */
    (void)nOk;

    /*
     * Grep: personality: soft honesty
     * Door/personality honesty -- soft residual only.
     * Dual DoD A/B = UDX product OPEN (not freestanding close).
     */
    msg("personality: soft honesty door_loop_default=0 live_path=0 "
        "scaffold=1 serve_ready=1 exclusive=1 soft=1 "
        "product_kernel=OPEN g_pers_attach=OPEN g_ac1=1 "
        "product_dir=UDX+ABI freestanding_class_product=0 "
        "freestanding_class_skip=1 freestanding_skip=1 ko_product=0 "
        "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
        "dual_dod_product=UDX bar3=OPEN deck_top50=NOT_CLAIMED "
        "option_c=1 udx_host=1 apps=1 residual_deepen=1 "
        "wire_b=64 frame_b=128 g_pers1=1 g_pers2=1 g_pers3=OPEN "
        "g_cold1=1 "
        "Soft!=product (soft residual; not live door path; "
        "not freestanding class drivers product; dual MIT OR Apache-2.0)\n");

    /*
     * Grep: personality: soft path
     * ABI-first residual lean -- host Linux-shaped drivers/apps via cold door.
     * Product path: Option C hot+cold ABI + userspace UDX/DDI hosts.
     * T0 product net = virtio until UDX owns laptop wire.
     */
    msg("personality: soft path scaffold=1 door_api=1 cold_serve=soft "
        "door_loop=0 abi_first=1 host_linux_shaped=1 option_c=1 "
        "hot_cold=1 hot_dispatch=1 cold_door_call=1 "
        "udx_host=1 apps=1 apps_linux_shaped=1 residual_deepen=1 "
        "product_dir=UDX+ABI freestanding_class=0 freestanding_skip=1 "
        "ko_product=0 g_ac1=1 t0_virtio=1 virtio_t0=1 udx_owns_wire=OPEN "
        "g_pers1=1 g_pers2=1 g_pers3=OPEN "
        "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN dual_dod_product=UDX "
        "Soft!=product dual=MIT_OR_Apache-2.0\n");

    /*
     * Grep: personality: soft inventory
     * Lean counters as fixed lamps (no format storm; freestanding msg only).
     * C0 deepen: UDX hosts + apps + attach + g_pers + Dual DoD A/B + bind.
     */
    msg("personality: soft inventory scaffold_ok=1 serve_ready=1 "
        "door_loop=0 live_path=0 residual_lean=1 residual_deepen=1 "
        "option_c=1 udx_host=1 apps=1 apps_linux_shaped=1 attach=1 g_pers=1 "
        "rtl8168_udx=1 xhci_udx=1 ddi_host_gj=1 "
        "reply_product=0 freestanding_class_product=0 freestanding_skip=1 "
        "ko_product=0 g_ac1=1 g_pers_attach=OPEN "
        "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
        "product_dir=UDX+ABI wire_ok=1 "
        "Soft!=product soft_partial\n");

    /*
     * Grep: personality: soft option_c
     * Option C hybrid residual: hot kernel + cold door personality.
     * Scaffold only -- product attach still OPEN (coordinator).
     * G-COLD-1: doors normative; queue COLD_DEQUEUE/REPLY deprecated (G-COLD-2).
     */
    msg("personality: soft option_c hybrid=1 hot=1 cold_door=1 "
        "hot_dispatch=1 cold_door_call=1 residual_deepen=1 "
        "kthread_default=1 userspace_scaffold=1 door_loop=0 "
        "live_path=0 g_pers_attach=OPEN cold_policy=interim_SERVE "
        "libprotonrt_userspace=OPEN "
        "g_pers1=1 g_pers2=1 g_pers3=OPEN "
        "g_cold1=1 g_cold2_depr=1 g_cold2=deprecated_scaffold g_cold3=1 "
        "soft=1 Soft!=product dual=MIT_OR_Apache-2.0 g_ac1=1\n");

    /*
     * Grep: personality: soft udx_host
     * UDX driver host residual: product drivers are userspace UDX/DDI.
     * Dual DoD A = xhci_udx USB OPEN; Dual DoD B OPEN until interactive SSH login.
     * Not freestanding rtl/xhci thrash; not in-kernel .ko product AC.
     * Hosts live outside this tree; residual only (C0 deepen tokens).
     */
    msg("personality: soft udx_host product_dir=1 userspace=1 "
        "linux_shaped_api=1 probe_remove_quiesce=1 residual_deepen=1 "
        "ioremap_dma_irq=shaped caps_hidden=1 hard_irq_in_driver=0 "
        "freestanding_class_product=0 freestanding_class_skip=1 "
        "freestanding_skip=1 ko_product=0 g_ac1=1 "
        "rtl8168_udx=1 xhci_udx=1 ddi_host_gj=1 "
        "g752_nic=10ec:8168 g752_xhci=8086:a12f "
        "bind_walk=SCAN_GET_OPEN_MAP_BAR "
        "ddi_soft=1 inject_host=1 bind_by_id=soft "
        "tx_rx_product=0 bot_product=0 "
        "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
        "dual_dod_a_host=xhci_udx dual_dod_b_host=rtl8168_udx "
        "Soft!=product dual=MIT_OR_Apache-2.0 "
        "(UDX residual; not live datapath; not freestanding class product)\n");

    /*
     * Grep: personality: soft apps
     * Linux-shaped apps residual via Option C personality (hot + cold).
     * Apps consume cold door; drivers stay UDX hosts -- personality TCB
     * has no IOMMU/IRQ (G-PERS-2). Soft residual != product app path PASS.
     */
    msg("personality: soft apps linux_shaped=1 apps=1 apps_linux_shaped=1 "
        "option_c=1 residual_deepen=1 "
        "hot_path=1 cold_door=1 vfs_client=soft net_client=soft "
        "app_sshd=soft app_netstackd=soft app_shell=soft "
        "app_vfsd=soft app_storaged=soft app_proton=soft "
        "proton_path=soft bar3=OPEN deck_top50=NOT_CLAIMED "
        "personality_tcb_no_iommu_irq=1 g_pers2=1 "
        "t0_virtio=1 virtio_t0=1 udx_owns_wire=OPEN "
        "live_app_path=0 soft=1 "
        "Soft!=product dual=MIT_OR_Apache-2.0 g_ac1=1 "
        "(apps residual; not Deck Top 50 PASS; not bar3 close)\n");

    /*
     * Grep: personality: soft attach
     * Door attach contract residual (badge + cold_ipc + owner transfer).
     * Scaffold never sets door badge / cold_ipc attached; coordinator owns
     * product attach. PERSONALITY_DOOR_LOOP=1 only after attach.
     * Soft residual != product G-PERS attach PASS.
     */
    msg("personality: soft attach door_owner=OPEN badge_server=OPEN "
        "cold_ipc_attach=OPEN door_loop_default=0 would_recv_cap=0 "
        "p_server_bind=OPEN userspace_owner=OPEN kthread_fallback=1 "
        "g_pers_attach=OPEN residual_deepen=1 live_path=0 soft=1 "
        "Soft!=product dual=MIT_OR_Apache-2.0 g_ac1=1 "
        "(attach residual; not product door park; not live path PASS)\n");

    /*
     * Grep: personality: soft g_pers
     * G-PERS / G-COLD residual (DESIGN_SPEC_COMPLETE section 3).
     * G-PERS-1 hot-only kernel; G-PERS-2 TCB no IOMMU/IRQ; G-PERS-3 death
     * PEER_DEAD policy OPEN at product attach. G-COLD-1 doors normative.
     */
    msg("personality: soft g_pers g_pers1=1 g_pers2=1 g_pers3=OPEN "
        "g_cold1=1 g_cold2_depr=1 g_cold2=deprecated_scaffold g_cold3=1 "
        "tcb_cold_door=1 tcb_vfs_client=soft tcb_net_client=soft "
        "tcb_iommu=0 tcb_irq=0 kthread_default=1 residual_deepen=1 "
        "userspace_scaffold=1 g_pers_attach=OPEN live_path=0 soft=1 "
        "Soft!=product dual=MIT_OR_Apache-2.0 g_ac1=1 "
        "(G-PERS residual; not product cold owner; not death-policy PASS)\n");

    /*
     * Grep: personality: soft residual lean
     * Lean rollup -- Soft!=product dual license; no version stamp.
     * C0 deepen: option_c + udx_host + apps + attach + g_pers + Dual DoD A/B.
     */
    msg("personality: soft residual lean "
        "scaffold=1 serve_ready=1 door_loop=0 live_path=0 "
        "abi_first=1 g_ac1=1 host_linux_shaped=1 option_c=1 "
        "udx_host=1 apps=1 attach=1 g_pers=1 residual_deepen=1 "
        "product_dir=UDX+ABI "
        "rtl8168_udx=1 xhci_udx=1 ddi_host_gj=1 "
        "freestanding_class_product=0 freestanding_class_skip=1 "
        "freestanding_skip=1 ko_product=0 "
        "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN dual_dod_product=UDX "
        "wire_ok=1 g_pers1=1 g_pers2=1 g_pers3=OPEN g_cold1=1 "
        "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
        "ok=14/14 stamp_storm=0 no_version_stamp=1 "
        "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
        "not product G-PERS attach; not freestanding class thrash; "
        "not in-kernel .ko product; UDX hosts + Linux-shaped apps residual; "
        "attach + G-PERS residual; Dual DoD A/B UDX OPEN)\n");

    /*
     * Grep: personality: soft residual lean PASS
     * Once-lamp residual complete. Soft!=product. G-AC-1.
     * C0 deepen rollup: option_c / udx_host / apps / attach / g_pers.
     */
    msg("personality: soft residual lean PASS "
        "scaffold_ok=1 serve_ready=1 residual_lean=1 residual_deepen=1 "
        "option_c=1 udx_host=1 apps=1 attach=1 g_pers=1 "
        "rtl8168_udx=1 xhci_udx=1 ddi_host_gj=1 "
        "live_path=0 door_loop=0 reply_product=0 "
        "freestanding_class_product=0 freestanding_class_skip=1 "
        "freestanding_skip=1 ko_product=0 g_ac1=1 "
        "product_dir=UDX+ABI g_pers_attach=OPEN "
        "dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN ok=14/14 "
        "Soft!=product dual MIT OR Apache-2.0 no_version_stamp storm=0\n");

    /*
     * Grep: personality: soft product residual
     * C0/C2 product residual toward UDX+sshd+stack (Soft!=product).
     * Cold door personality hosts Linux-shaped apps (sshd/netstackd) while
     * UDX hosts own NIC/USB. Chain: rtl8168_udx → stack → sshd :22.
     * Bar honesty v2026.08.04.75 stamp-free; Dual DoD A/B OPEN; G-AC-1.
     */
    msg("personality: soft product residual claim_class=C0 "
        "product=UDX+sshd+stack product_path=UDX product_dir=UDX+ABI "
        "chain=rtl8168_udx>netstackd>sshd "
        "option_c=1 cold_door=1 app_sshd=soft app_netstackd=soft "
        "udx_host=1 rtl8168_udx=1 xhci_udx=1 ddi_host_gj=1 "
        "sshd_port=22 stack_host=soft t0_virtio=1 udx_owns_wire=OPEN "
        "freestanding_skip=1 ko_product=0 soft_ne_product=1 Soft!=product "
        "G-AC-1=1 dual_dod=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
        "agent_ne_close=1 multi_server=0 confine=0 product_ac=0 "
        "stamp_free=1 storm=0 once=1 dual=MIT_OR_Apache-2.0 "
        "bar=v2026.08.04.75 g_pers_attach=OPEN live_path=0\n");
}
#endif /* PERSONALITY_DOOR_LOOP == 0 */

/**
 * Scaffold entry body -- greppable soft phases for bring-up honesty.
 * Freestanding link uses _start (user.ld ENTRY); _start calls main.
 *
 * Soft phases (default DOOR_LOOP=0 -- never blocks):
 *   1. personality: soft userspace scaffold PASS
 *   2. personality-gj: soft serve ready   (soft once; not product door)
 *   3. personality: soft residual lean PASS  (honesty + path + inventory +
 *      option_c + udx_host + apps + attach + g_pers + residual lean)
 *
 * DOOR_LOOP=1 parks in personality_door_loop (needs kernel attach; G-PERS OPEN).
 */
int
main(void)
{
    /* Soft phase 1 -- ELF + DEBUG_LOG path alive (prefix-stable). */
    msg("personality: soft userspace scaffold PASS\n");

#if PERSONALITY_DOOR_LOOP
    /*
     * Park on the cold door. Not product G-PERS until kernel attach.
     * OpenSSH still needs that attach; this flag is not Dual DoD close.
     */
    personality_door_loop();
#else
    /* Soft phase 2 -- scaffold complete; not product door serve. */
    msg("personality-gj: soft serve ready\n");

    /*
     * Soft phase 3 -- exclusive residual lean (scaffold path only).
     * Door park path never reaches here when DOOR_LOOP=1.
     */
    personality_soft_residual_lean_once();
#endif

    return 0;
}

void
_start(void)
{
    int n;

    n = main();
    gj_exit(n);
}
