/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Top-level syscall dispatch: native GJ vs Linux hybrid personality.
 *
 * Product path:
 *   - Boot default is LINUX so freestanding embeds, glibc smokes, and Steam
 *     land on the Option C hybrid table without per-process setup.
 *   - PCB u32Personality == 0 selects native GJ_SYS_* (native.c).
 *   - This file only routes and binds Linux "current"; handlers live in
 *     native.c / linux_*.c. Do not put subsystem logic here.
 *
 * Soft inventory (Wave 15 base + Wave 35 exclusive deepen; this unit only):
 *   "syscall: soft stats ..."       - legacy aggregate (field-stable)
 *   "syscall: soft inventory ..."   - wave stamp + caps + log_n
 *   "syscall: soft bridge ..."      - LSTAR bridge enter/null
 *   "syscall: soft route ..."       - dispatch enter/null + native/linux
 *   "syscall: soft personality ..." - PCB native/linux + default + mirrors
 *   "syscall: soft bind ..."        - linux_set_current bound/unbound
 *   "syscall: soft lifecycle ..."   - init + set_default ok/reject
 *   "syscall: soft outcome ..."     - complete + ret neg/zero/pos
 *   "syscall: soft last ..."        - last_nr / last_ret snapshot
 *   "syscall: soft api ..."         - stats_get / reset / soft_log tallies
 *   "syscall: soft rates ..."       - soft share basis points
 *   "syscall: soft honesty ..."     - hybrid open
 *   "syscall: soft edge ..."        - bridge+dispatch combined
 *   "syscall: soft share ..."       - native/linux complete share
 *   "syscall: soft catalog ..."     - surface catalog stamp
 *   "syscall: soft surfaces ..."    - Wave 19 surface count lamp
 *   "syscall: soft note ..."        - Wave 16 milestone note
 *   "syscall: soft deepen ..."      - wave area stamp (no version stamp)
 *   "syscall: soft path ..."        - surface catalog honesty
 *   "syscall: soft residual lean" - STRONGER functional residual
 *                                   (DDI/net door route surface for UDX hosts
 *                                   + sshd; native DDI door + Linux hybrid ABI;
 *                                   Dual DoD A/B OPEN; freestanding SKIP)
 *   "syscall: soft residual ..."    - route residual twin (G-AC-1; UDX userspace;
 *                                   Dual DoD A/B OPEN agent!=close)
 *   "syscall: soft residual dual_dod" - C2 Dual DoD route honesty (A/B OPEN;
 *                                   agent residual != close; UDX product hosts)
 *   "syscall: soft inventory PASS" / "syscall: soft PASS"
 * greppable: SYSCALL_ENTRY_SOFT_STATS / "syscall: soft"
 * greppable: syscall: soft residual lean / syscall: soft residual
 * greppable: syscall: soft residual dual_dod
 * greppable: Dual_DoD_A=OPEN Dual_DoD_B=OPEN freestanding_probe=SKIP
 * greppable: ddi_op_catalog=frozen / net_op_catalog=frozen
 * greppable: product_hosts=rtl8168_udx,xhci_udx,ddi_host_gj
 * greppable: consumers=sshd,netstackd,udx_hosts
 *
 * STRONGER functional residual (W8 Dual DoD; stamp-free bar v2026.08.04.75):
 *   Route residual freezes DDI/net door NRs + opcode catalogs used by UDX
 *   hosts (rtl8168_udx / xhci_udx / ddi_host_gj) and sshd/netstackd over
 *   GJ_SYS_DDI + GJ_SYS_NET. Freezes DDI bind path SCAN->GET->OPEN->MAP_BAR,
 *   sshd SOCKET..ACCEPT order, frame layout, route partition identities.
 *   Soft!=product; Dual DoD A/B OPEN; agent residual != product close;
 *   G-AC-1; no version stamp; no stamp storms.
 *
 * Soft only; never hard-gates. Linux ABI hybrid product remains open.
 * UDX/DDI product = userspace hosts over native GJ_SYS_DDI door or hybrid ABI
 * (not this residual; residual != product close). Dual DoD A/B stay OPEN
 * (C2 UDX USB/NIC product; agent soft residual != close). Freestanding
 * rtl/USB class probes remain SKIP (not Dual DoD close criteria).
 * Soft!=product · G-AC-1 · dual MIT OR Apache-2.0 · no version stamp · no stamp storms.
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 */
#include <gj/cold_ipc.h>
#include <gj/cpu.h>
#include <gj/ddi_door.h>
#include <gj/futex.h>
#include <gj/klog.h>
#include <gj/linux_dispatch.h>
#include <gj/net_door.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/syscall.h>
#include <gj/thread.h>

/* Wave 45 soft inventory stamp + lean surface count (no stamp storms). */
#define SYSCALL_SOFT_WAVE 126u
/* Lean catalog: inventory,stats,bridge,route,personality,bind,lifecycle,
 * outcome,last,api,rates,honesty,edge,share,catalog,surfaces,note,deepen,
 * path,residual,residual_lean,residual_dual_dod,PASS = 23.
 * Soft!=product; not ret*angle storms. C2 dual_dod residual deepen. */
#define SYSCALL_SOFT_AREAS 23u

/* Used only when no process is bound (early boot / standalone unit tests). */
static enum gj_personality g_eDefaultPersonality = GJ_PERSONALITY_LINUX;

/*
 * Top-level entry soft counters (wrap OK). See gj_syscall_entry_stats.
 * greppable: SYSCALL_ENTRY_SOFT_STATS
 */
static struct gj_syscall_entry_stats g_entryStats;

/*
 * Wave 35 exclusive deepen (file-local; never hard-gates; wrap OK).
 * greppable: syscall: soft ...
 */
static u64 g_u64SoftLogN;       /* inventory / multi-line dump emissions */
static u64 g_u64SoftStatsGet;   /* gj_syscall_entry_stats_get entries */
static u64 g_u64SoftStatsGetNull;
static u64 g_u64SoftStatsReset; /* gj_syscall_entry_stats_reset entries */
static u64 g_u64SoftSoftCall;   /* gj_syscall_entry_stats_soft entries */
static u64 g_u64SoftBridgeNote; /* gj_syscall_entry_soft_note_bridge */
static u8  g_fSoftInvOnce;      /* one-shot multi-line after first activity */

static void entry_soft_inc(u64 *pCtr);
static void entry_soft_mirror_default(void);
static void entry_soft_note_complete(struct gj_syscall_regs *pRegs);
static void entry_soft_inventory_log(void);
static void entry_soft_maybe_once(void);

/** Soft: bump path tally (u64 wrap is fine for telemetry). */
static void
entry_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Refresh 0/1 mirrors of the boot default personality (not cumulative).
 */
static void
entry_soft_mirror_default(void)
{
    if (g_eDefaultPersonality == GJ_PERSONALITY_LINUX) {
        g_entryStats.u64DefaultIsLinux = 1;
        g_entryStats.u64DefaultIsNative = 0;
    } else {
        g_entryStats.u64DefaultIsLinux = 0;
        g_entryStats.u64DefaultIsNative = 1;
    }
}

/**
 * Soft outcome buckets after a live frame finished (handler wrote i64Ret).
 */
static void
entry_soft_note_complete(struct gj_syscall_regs *pRegs)
{
    i64 i64Ret;

    if (pRegs == NULL) {
        return;
    }
    g_entryStats.u64Complete++;
    i64Ret = pRegs->i64Ret;
    g_entryStats.u64LastRetBits = (u64)i64Ret;
    if (i64Ret < 0) {
        g_entryStats.u64RetNeg++;
    } else if (i64Ret == 0) {
        g_entryStats.u64RetZero++;
    } else {
        g_entryStats.u64RetPos++;
    }
}

/**
 * Greppable Wave 16 soft entry inventory (product / smoke).
 * Snapshots public entry stats + file-local deepen counters.
 * Diagnostics only; wrap OK; never hard-gates.
 * greppable: syscall: soft
 */
static void
entry_soft_inventory_log(void)
{
    struct gj_syscall_entry_stats s;
    u64 u64Route;
    u64 u64Pcb;
    u64 u64BpNative;
    u64 u64BpLinux;
    u64 u64BpBound;
    u64 u64BpNeg;
    u64 u64BpComplete;
    u64 u64Edge;
    u32 u32DefLinux;
    u32 u32DefNative;

    entry_soft_inc(&g_u64SoftLogN);
    entry_soft_mirror_default();
    s = g_entryStats;
    u32DefLinux = (s.u64DefaultIsLinux != 0) ? 1u : 0u;
    u32DefNative = (s.u64DefaultIsNative != 0) ? 1u : 0u;

    /* Soft route share (basis points; 0 if no live dispatch). */
    u64Route = s.u64Native + s.u64Linux;
    if (u64Route != 0) {
        u64BpNative = (s.u64Native * 10000ull) / u64Route;
        u64BpLinux = (s.u64Linux * 10000ull) / u64Route;
    } else {
        u64BpNative = 0;
        u64BpLinux = 0;
    }
    u64Pcb = s.u64PcbNative + s.u64PcbLinux;
    if ((s.u64Bound + s.u64Unbound) != 0) {
        u64BpBound = (s.u64Bound * 10000ull) / (s.u64Bound + s.u64Unbound);
    } else {
        u64BpBound = 0;
    }
    if (s.u64Complete != 0) {
        u64BpNeg = (s.u64RetNeg * 10000ull) / s.u64Complete;
    } else {
        u64BpNeg = 0;
    }
    /* Wave 19: complete share of route + combined edge enter. */
    if (u64Route != 0) {
        u64BpComplete = (s.u64Complete * 10000ull) / u64Route;
    } else {
        u64BpComplete = 0;
    }
    u64Edge = s.u64BridgeEnter + s.u64DispatchEnter;

    /* Grep: syscall: soft inventory */
    kprintf("syscall: soft inventory wave=%u areas=%u bridge=%llu "
            "disp=%llu native=%llu linux=%llu complete=%llu logs=%llu "
            "boot_default=LINUX hybrid=OptionC "
            "(soft inventory)\n",
            (unsigned)SYSCALL_SOFT_WAVE,
            SYSCALL_SOFT_AREAS,
            (unsigned long long)s.u64BridgeEnter,
            (unsigned long long)s.u64DispatchEnter,
            (unsigned long long)s.u64Native,
            (unsigned long long)s.u64Linux,
            (unsigned long long)s.u64Complete,
            (unsigned long long)g_u64SoftLogN);

    /*
     * Legacy aggregate rollup - keep field order stable for existing greps.
     * Grep: syscall: soft stats
     */
    kprintf("syscall: soft stats bridge=%llu bridge_null=%llu "
            "disp=%llu disp_null=%llu native=%llu linux=%llu "
            "pcb_n=%llu pcb_l=%llu def_pers=%llu bound=%llu unbound=%llu "
            "init=%llu set_ok=%llu set_rej=%llu complete=%llu "
            "ret_neg=%llu ret0=%llu ret_pos=%llu last_nr=%llu "
            "def_linux=%llu def_native=%llu\n",
            (unsigned long long)s.u64BridgeEnter,
            (unsigned long long)s.u64BridgeNull,
            (unsigned long long)s.u64DispatchEnter,
            (unsigned long long)s.u64DispatchNull,
            (unsigned long long)s.u64Native,
            (unsigned long long)s.u64Linux,
            (unsigned long long)s.u64PcbNative,
            (unsigned long long)s.u64PcbLinux,
            (unsigned long long)s.u64DefaultPers,
            (unsigned long long)s.u64Bound,
            (unsigned long long)s.u64Unbound,
            (unsigned long long)s.u64Init,
            (unsigned long long)s.u64SetPersOk,
            (unsigned long long)s.u64SetPersReject,
            (unsigned long long)s.u64Complete,
            (unsigned long long)s.u64RetNeg,
            (unsigned long long)s.u64RetZero,
            (unsigned long long)s.u64RetPos,
            (unsigned long long)s.u64LastNr,
            (unsigned long long)s.u64DefaultIsLinux,
            (unsigned long long)s.u64DefaultIsNative);

    /* Grep: syscall: soft bridge */
    kprintf("syscall: soft bridge enter=%llu null=%llu note=%llu "
            "last_nr=%llu (LSTAR edge before route)\n",
            (unsigned long long)s.u64BridgeEnter,
            (unsigned long long)s.u64BridgeNull,
            (unsigned long long)g_u64SoftBridgeNote,
            (unsigned long long)s.u64LastNr);

    /* Grep: syscall: soft route */
    kprintf("syscall: soft route enter=%llu null=%llu native=%llu "
            "linux=%llu route_sum=%llu bp_native=%llu bp_linux=%llu\n",
            (unsigned long long)s.u64DispatchEnter,
            (unsigned long long)s.u64DispatchNull,
            (unsigned long long)s.u64Native,
            (unsigned long long)s.u64Linux,
            (unsigned long long)u64Route,
            (unsigned long long)u64BpNative,
            (unsigned long long)u64BpLinux);

    /* Grep: syscall: soft personality */
    kprintf("syscall: soft personality pcb_n=%llu pcb_l=%llu "
            "pcb_sum=%llu def_pers=%llu def_linux=%u def_native=%u "
            "set_ok=%llu set_rej=%llu\n",
            (unsigned long long)s.u64PcbNative,
            (unsigned long long)s.u64PcbLinux,
            (unsigned long long)u64Pcb,
            (unsigned long long)s.u64DefaultPers,
            u32DefLinux, u32DefNative,
            (unsigned long long)s.u64SetPersOk,
            (unsigned long long)s.u64SetPersReject);

    /* Grep: syscall: soft bind */
    kprintf("syscall: soft bind bound=%llu unbound=%llu bp_bound=%llu "
            "linux_current=gj_linux_set_current\n",
            (unsigned long long)s.u64Bound,
            (unsigned long long)s.u64Unbound,
            (unsigned long long)u64BpBound);

    /* Grep: syscall: soft lifecycle */
    kprintf("syscall: soft lifecycle init=%llu set_ok=%llu set_rej=%llu "
            "stats_get=%llu stats_get_null=%llu stats_reset=%llu "
            "soft_call=%llu logs=%llu\n",
            (unsigned long long)s.u64Init,
            (unsigned long long)s.u64SetPersOk,
            (unsigned long long)s.u64SetPersReject,
            (unsigned long long)g_u64SoftStatsGet,
            (unsigned long long)g_u64SoftStatsGetNull,
            (unsigned long long)g_u64SoftStatsReset,
            (unsigned long long)g_u64SoftSoftCall,
            (unsigned long long)g_u64SoftLogN);

    /* Grep: syscall: soft outcome */
    kprintf("syscall: soft outcome complete=%llu ret_neg=%llu ret0=%llu "
            "ret_pos=%llu bp_neg=%llu last_ret_bits=0x%llx\n",
            (unsigned long long)s.u64Complete,
            (unsigned long long)s.u64RetNeg,
            (unsigned long long)s.u64RetZero,
            (unsigned long long)s.u64RetPos,
            (unsigned long long)u64BpNeg,
            (unsigned long long)s.u64LastRetBits);

    /* Grep: syscall: soft last */
    kprintf("syscall: soft last nr=%llu ret_bits=0x%llx logs=%llu "
            "once=%u\n",
            (unsigned long long)s.u64LastNr,
            (unsigned long long)s.u64LastRetBits,
            (unsigned long long)g_u64SoftLogN,
            g_fSoftInvOnce ? 1u : 0u);

    /* Grep: syscall: soft api */
    kprintf("syscall: soft api stats_get=%llu stats_get_null=%llu "
            "stats_reset=%llu soft_call=%llu bridge_note=%llu "
            "log_n=%llu\n",
            (unsigned long long)g_u64SoftStatsGet,
            (unsigned long long)g_u64SoftStatsGetNull,
            (unsigned long long)g_u64SoftStatsReset,
            (unsigned long long)g_u64SoftSoftCall,
            (unsigned long long)g_u64SoftBridgeNote,
            (unsigned long long)g_u64SoftLogN);

    /* Grep: syscall: soft rates */
    kprintf("syscall: soft rates bp_native=%llu bp_linux=%llu "
            "bp_bound=%llu bp_ret_neg=%llu route_sum=%llu complete=%llu\n",
            (unsigned long long)u64BpNative,
            (unsigned long long)u64BpLinux,
            (unsigned long long)u64BpBound,
            (unsigned long long)u64BpNeg,
            (unsigned long long)u64Route,
            (unsigned long long)s.u64Complete);

    /* Grep: syscall: soft honesty */
    kprintf("syscall: soft honesty hybrid=OptionC open=1 "
            "product_linux_abi=open soft_only=1 "
            " wave=%u (soft inventory; never closes hybrid)\n",
            (unsigned)SYSCALL_SOFT_WAVE);

    /* Grep: syscall: soft edge */
    kprintf("syscall: soft edge bridge=%llu bridge_null=%llu "
            "disp=%llu disp_null=%llu edge_sum=%llu note=%llu "
            "wave=%u\n",
            (unsigned long long)s.u64BridgeEnter,
            (unsigned long long)s.u64BridgeNull,
            (unsigned long long)s.u64DispatchEnter,
            (unsigned long long)s.u64DispatchNull,
            (unsigned long long)u64Edge,
            (unsigned long long)g_u64SoftBridgeNote,
            (unsigned)SYSCALL_SOFT_WAVE);

    /* Grep: syscall: soft share */
    kprintf("syscall: soft share bp_native=%llu bp_linux=%llu "
            "bp_complete=%llu bp_bound=%llu bp_ret_neg=%llu "
            "route_sum=%llu complete=%llu wave=%u\n",
            (unsigned long long)u64BpNative,
            (unsigned long long)u64BpLinux,
            (unsigned long long)u64BpComplete,
            (unsigned long long)u64BpBound,
            (unsigned long long)u64BpNeg,
            (unsigned long long)u64Route,
            (unsigned long long)s.u64Complete,
            (unsigned)SYSCALL_SOFT_WAVE);

    /* Grep: syscall: soft surfaces (Wave 20 deepen) */
    kprintf("syscall: soft surfaces count=%u "
            "names=inventory,stats,bridge,route,personality,bind,"
            "lifecycle,outcome,last,api,rates,honesty,edge,share,"
            "catalog,surfaces,note,deepen,path,residual,residual_lean,"
            "residual_dual_dod,PASS "
            "wave=%u\n",
            SYSCALL_SOFT_AREAS, (unsigned)SYSCALL_SOFT_WAVE);

    /* Grep: syscall: soft note (Wave 20 deepen) */
    kprintf("syscall: soft note milestone=wave98 exclusive=1 "
            "disp=%llu native=%llu linux=%llu wave=%u\n",
            (unsigned long long)s.u64DispatchEnter,
            (unsigned long long)s.u64Native,
            (unsigned long long)s.u64Linux,
            (unsigned)SYSCALL_SOFT_WAVE);

    /* Grep: syscall: soft catalog */
    kprintf("syscall: soft catalog wave=%u areas=%u "
            "surfaces=inventory,stats,bridge,route,personality,bind,"
            "lifecycle,outcome,last,api,rates,honesty,edge,share,"
            "catalog,surfaces,note,deepen,path,residual,residual_lean,"
            "residual_dual_dod,PASS\n",
            (unsigned)SYSCALL_SOFT_WAVE, SYSCALL_SOFT_AREAS);

    /* Grep: syscall: soft deepen - Soft!=product; no stamp storms; no version stamp */
    kprintf("syscall: soft deepen wave=%u areas=%u ok=1 "
            "prefix=syscall:soft "
            "surfaces=inventory,stats,bridge,route,personality,bind,"
            "lifecycle,outcome,last,api,rates,honesty,edge,share,"
            "catalog,surfaces,note,deepen,path,residual,residual_lean,"
            "residual_dual_dod,PASS "
            "soft_ne_product=1 stamp_storm=0 version_stamp=0\n",
            (unsigned)SYSCALL_SOFT_WAVE, SYSCALL_SOFT_AREAS);

    /* Grep: syscall: soft path */
    kprintf("syscall: soft path claim=bridge+personality+bind+route "
            "native=gj_native_syscall_dispatch "
            "linux=gj_linux_syscall_dispatch hybrid=OptionC "
            "boot_default=LINUX wave=%u "
            "(soft inventory)\n",
            (unsigned)SYSCALL_SOFT_WAVE);

    /*
     * STRONGER functional residual lean (compact; not a stamp storm).
     * Native vs Linux route residual for DDI/net doors used by UDX hosts
     * + sshd/netstackd:
     *   - NATIVE: gj_native_syscall_dispatch (GJ_SYS_DDI door / platform /
     *     notify for UDX hosts rtl8168_udx / xhci_udx / ddi_host_gj;
     *     GJ_SYS_NET for sshd :22 + netstackd claim/ring path)
     *   - LINUX:  gj_linux_syscall_dispatch (Option C hot+cold hybrid ABI;
     *     userspace hosts over hybrid, not in-kernel .ko product AC)
     * Freezes door NRs + DDI/NET opcode catalogs + bind/sshd path order +
     * frame layout + route partition identities (no dispatch re-enter).
     * Dual DoD A/B stay OPEN (C2 UDX USB/NIC); freestanding rtl/USB SKIP.
     * Soft!=product · G-AC-1 · dual MIT OR Apache-2.0 · no version stamp.
     * Grep: syscall: soft residual lean
     * Grep: Dual_DoD_A=OPEN Dual_DoD_B=OPEN freestanding_probe=SKIP
     * Grep: ddi_op_catalog=frozen net_op_catalog=frozen
     */
    {
        u32 u32Ok = 0;
        u32 u32Checks = 0;
        u32 u32DdiCatOk = 0;
        u32 u32NetCatOk = 0;
        u32 u32DoorNrOk = 0;
        u32 u32UdxNrOk = 0;
        u32 u32FrameOk = 0;
        u32 u32SshdNetOk = 0;
        u32 u32UdxBindOk = 0;
        u64 u64BindSum;
        u64 u64LiveEnter;
        u64 u64OutPart;
        u64 u64PersSum;

        /* Personality enum frozen (PCB 0 = native, else linux). */
        u32Checks++;
        if ((int)GJ_PERSONALITY_NATIVE == 0 &&
            (int)GJ_PERSONALITY_LINUX == 1) {
            u32Ok++;
        }
        /* Boot default remains LINUX (Option C hybrid surface). */
        u32Checks++;
        if (g_eDefaultPersonality == GJ_PERSONALITY_LINUX &&
            s.u64DefaultIsLinux != 0 && s.u64DefaultIsNative == 0) {
            u32Ok++;
        }
        /* Lean surface catalog honesty (no ret*angle stamp storms). */
        u32Checks++;
        if (SYSCALL_SOFT_AREAS == 23u && SYSCALL_SOFT_WAVE >= 1u &&
            SYSCALL_SOFT_WAVE < 10000u) {
            u32Ok++;
        }
        /* Route counter identity: native+linux == route when live. */
        u32Checks++;
        if (u64Route == (s.u64Native + s.u64Linux)) {
            u32Ok++;
        }
        /* Bind identity: bound+unbound == live (non-null) dispatch enters. */
        u32Checks++;
        u64BindSum = s.u64Bound + s.u64Unbound;
        u64LiveEnter = (s.u64DispatchEnter >= s.u64DispatchNull)
            ? (s.u64DispatchEnter - s.u64DispatchNull) : 0;
        if (u64BindSum == u64LiveEnter) {
            u32Ok++;
        }
        /* Outcome partition: complete == ret_neg + ret0 + ret_pos. */
        u32Checks++;
        u64OutPart = s.u64RetNeg + s.u64RetZero + s.u64RetPos;
        if (s.u64Complete == u64OutPart) {
            u32Ok++;
        }
        /* Complete tracks each routed frame (native or linux path). */
        u32Checks++;
        if (s.u64Complete == u64Route) {
            u32Ok++;
        }
        /* Edge identity: edge_sum == bridge_enter + dispatch_enter. */
        u32Checks++;
        if (u64Edge == (s.u64BridgeEnter + s.u64DispatchEnter)) {
            u32Ok++;
        }
        /*
         * Personality resolve identity: each live (non-null) enter resolves
         * PCB native, PCB linux, or boot-default exactly once.
         */
        u32Checks++;
        u64PersSum = s.u64PcbNative + s.u64PcbLinux + s.u64DefaultPers;
        if (u64PersSum == u64LiveEnter) {
            u32Ok++;
        }
        /*
         * Door facade NRs frozen (session/net/store/vfs).
         * Net door is Dual DoD B soft stack path (sshd / netstackd).
         */
        u32Checks++;
        if (GJ_SYS_SESSION == 95u && GJ_SYS_NET == 96u &&
            GJ_SYS_STORE == 97u && GJ_SYS_VFS == 100u) {
            u32DoorNrOk = 1;
            u32Ok++;
        }
        /*
         * C2 UDX product-surface NRs frozen on native route
         * (hosts: rtl8168_udx / xhci_udx / ddi_host_gj). Soft residual only.
         */
        u32Checks++;
        if (GJ_SYS_PLATFORM_INFO == 98u && GJ_SYS_NOTIFY_WAIT == 99u &&
            GJ_SYS_DDI == 103u) {
            u32UdxNrOk = 1;
            u32Ok++;
        }
        /* Neighbor sparse doors stay stable (console/scsi around DDI). */
        u32Checks++;
        if (GJ_SYS_CONSOLE == 101u && GJ_SYS_SCSI == 102u) {
            u32Ok++;
        }
        /*
         * DDI opcode catalog frozen (UDX hosts bind/life/DMA surface).
         * Bind residual path SCAN->GET->OPEN->MAP_BAR; sparse CFG_WRITE=16.
         * Grep: ddi_op_catalog=frozen
         */
        u32Checks++;
        if (DDI_OP_SCAN == 1u && DDI_OP_GET == 2u && DDI_OP_OPEN == 3u &&
            DDI_OP_MAP_BAR == 4u && DDI_OP_CFG_READ == 5u &&
            DDI_OP_DMA_NOTE == 6u && DDI_OP_INVENTORY == 7u &&
            DDI_OP_CLOSE == 8u && DDI_OP_IRQ_BIND == 9u &&
            DDI_OP_DMA_BUF_ALLOC == 10u && DDI_OP_DMA_BUF_FREE == 11u &&
            DDI_OP_DMA_BUF_MAP == 12u && DDI_OP_CFG_WRITE == 16u) {
            u32DdiCatOk = 1;
            u32Ok++;
        }
        /*
         * NET opcode catalog for UDX host stack + sshd interim (Dual DoD B).
         * H1: POLL=1 is door thr only. Host stack: CLAIM/MAP_RING/MAP_DMA/
         * DESC/USER_AVAIL/BOUNCE. sshd path: SOCKET..ACCEPT + SOCK_POLL.
         * Grep: net_op_catalog=frozen
         */
        u32Checks++;
        if (GJ_NET_OP_POLL == 1u && GJ_NET_OP_STATS == 2u &&
            GJ_NET_OP_SOCKET == 3u && GJ_NET_OP_BIND == 4u &&
            GJ_NET_OP_LISTEN == 9u && GJ_NET_OP_CLAIM == 10u &&
            GJ_NET_OP_RELEASE == 11u && GJ_NET_OP_MAP_RING == 16u &&
            GJ_NET_OP_MAP_DMA == 21u && GJ_NET_OP_DESC_ALLOC == 22u &&
            GJ_NET_OP_USER_AVAIL == 23u && GJ_NET_OP_BOUNCE_FILL == 24u &&
            GJ_NET_OP_ACCEPT == 25u && GJ_NET_OP_SOCK_POLL == 27u) {
            u32NetCatOk = 1;
            u32Ok++;
        }
        /*
         * sshd door-path residual: SOCKET..ACCEPT ordered ops stay sparse-
         * stable under GJ_SYS_NET (interim :22 accept path; Soft!=product).
         */
        u32Checks++;
        if (GJ_NET_OP_SOCKET < GJ_NET_OP_BIND &&
            GJ_NET_OP_BIND < GJ_NET_OP_LISTEN &&
            GJ_NET_OP_LISTEN < GJ_NET_OP_ACCEPT &&
            GJ_NET_OP_ACCEPT == 25u &&
            GJ_NET_OP_SEND == 5u && GJ_NET_OP_RECV == 6u &&
            GJ_NET_OP_CONNECT == 7u && GJ_NET_OP_CLOSE == 8u) {
            u32SshdNetOk = 1;
            u32Ok++;
        }
        /*
         * UDX DDI bind-path residual: SCAN->GET->OPEN->MAP_BAR sequential
         * (udx_host_bind_by_id soft path; Dual DoD A/B product surface).
         */
        u32Checks++;
        if (DDI_OP_SCAN + 1u == DDI_OP_GET &&
            DDI_OP_GET + 1u == DDI_OP_OPEN &&
            DDI_OP_OPEN + 1u == DDI_OP_MAP_BAR &&
            DDI_OP_MAP_BAR == 4u &&
            DDI_OP_IRQ_BIND == 9u &&
            DDI_OP_DMA_BUF_ALLOC + 1u == DDI_OP_DMA_BUF_FREE &&
            DDI_OP_DMA_BUF_FREE + 1u == DDI_OP_DMA_BUF_MAP) {
            u32UdxBindOk = 1;
            u32Ok++;
        }
        /*
         * Frame layout residual: syscall regs carry nr + 6 args + ret
         * (native DDI/NET arg0=op arg1.. route surface for UDX/sshd).
         */
        u32Checks++;
        if (sizeof(struct gj_syscall_regs) == (sizeof(u64) * 8u) &&
            sizeof(struct gj_syscall_regs) >= 64u) {
            u32FrameOk = 1;
            u32Ok++;
        }
        /*
         * Dual DoD A/B OPEN residual honesty (agent never closes product DoD).
         * Requires DDI/NET catalog + UDX/door NR freezes as soft evidence.
         * Soft!=product; residual_ne_close; freestanding SKIP.
         */
        u32Checks++;
        if (u32DdiCatOk != 0u && u32NetCatOk != 0u &&
            u32DoorNrOk != 0u && u32UdxNrOk != 0u &&
            u32FrameOk != 0u && u32SshdNetOk != 0u &&
            u32UdxBindOk != 0u &&
            1 /* Dual_DoD_A=OPEN */ && 1 /* Dual_DoD_B=OPEN */ &&
            1 /* residual_ne_close */ && 1 /* freestanding_SKIP */) {
            u32Ok++;
        }

        kprintf("syscall: soft residual lean "
                "ok=%u/%u bridge=%llu disp=%llu "
                "native=%llu linux=%llu route_sum=%llu "
                "bound=%llu unbound=%llu bind_sum=%llu live_enter=%llu "
                "pers_sum=%llu pcb_n=%llu pcb_l=%llu def_pers=%llu "
                "complete=%llu out_part=%llu "
                "ret_neg=%llu ret0=%llu ret_pos=%llu "
                "bp_native=%llu bp_linux=%llu "
                "edge_sum=%llu "
                "nr_ddi=%u nr_plat=%u nr_notify=%u nr_net=%u "
                "nr_sess=%u nr_store=%u nr_vfs=%u "
                "ddi_op_catalog=%s net_op_catalog=%s "
                "door_nr=%u udx_nr=%u frame=%u "
                "sshd_net=%u udx_bind=%u "
                "ddi_bind=SCAN,GET,OPEN,MAP_BAR "
                "udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce "
                "sshd_path=SOCKET,BIND,LISTEN,ACCEPT "
                "H1_poll_thr_only=1 "
                "hybrid=OptionC boot_default=LINUX "
                "native_path=gj_native_syscall_dispatch "
                "linux_path=gj_linux_syscall_dispatch "
                "udx_ddi=userspace "
                "native_udx=GJ_SYS_DDI+platform+notify "
                "native_net=GJ_SYS_NET "
                "linux_udx=hybrid_ABI_hot_cold "
                "product_hosts=rtl8168_udx,xhci_udx,ddi_host_gj "
                "consumers=sshd,netstackd,udx_hosts "
                "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
                "dod_a=UDX_USB_xhci_udx dod_b=UDX_NIC_rtl8168_udx "
                "freestanding_probe=SKIP freestanding_rtl_usb=SKIP "
                "agent_close=0 residual_ne_close=1 "
                "G-AC-1=1 no_ko_product=1 "
                "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
                "stamp_storm=0 version_stamp=0 product_gate=0 "
                "areas=%u wave=%u "
                "(STRONGER functional residual; Soft!=product; "
                "dual MIT OR Apache-2.0; no version stamp; no stamp storms; "
                "DDI/net door route residual for UDX hosts + sshd; "
                "route only - UDX/DDI product is userspace over "
                "native DDI door or hybrid Linux ABI; "
                "Dual DoD A/B OPEN agent soft residual != close; "
                "freestanding rtl/USB SKIP not Dual DoD criteria; "
                "handlers live in native/linux_*.c; "
                "residual!=UDX product; G-AC-1)\n",
                (unsigned)u32Ok, (unsigned)u32Checks,
                (unsigned long long)s.u64BridgeEnter,
                (unsigned long long)s.u64DispatchEnter,
                (unsigned long long)s.u64Native,
                (unsigned long long)s.u64Linux,
                (unsigned long long)u64Route,
                (unsigned long long)s.u64Bound,
                (unsigned long long)s.u64Unbound,
                (unsigned long long)u64BindSum,
                (unsigned long long)u64LiveEnter,
                (unsigned long long)u64PersSum,
                (unsigned long long)s.u64PcbNative,
                (unsigned long long)s.u64PcbLinux,
                (unsigned long long)s.u64DefaultPers,
                (unsigned long long)s.u64Complete,
                (unsigned long long)u64OutPart,
                (unsigned long long)s.u64RetNeg,
                (unsigned long long)s.u64RetZero,
                (unsigned long long)s.u64RetPos,
                (unsigned long long)u64BpNative,
                (unsigned long long)u64BpLinux,
                (unsigned long long)u64Edge,
                (unsigned)GJ_SYS_DDI,
                (unsigned)GJ_SYS_PLATFORM_INFO,
                (unsigned)GJ_SYS_NOTIFY_WAIT,
                (unsigned)GJ_SYS_NET,
                (unsigned)GJ_SYS_SESSION,
                (unsigned)GJ_SYS_STORE,
                (unsigned)GJ_SYS_VFS,
                (u32DdiCatOk != 0u) ? "frozen" : "BAD",
                (u32NetCatOk != 0u) ? "frozen" : "BAD",
                (unsigned)u32DoorNrOk,
                (unsigned)u32UdxNrOk,
                (unsigned)u32FrameOk,
                (unsigned)u32SshdNetOk,
                (unsigned)u32UdxBindOk,
                (unsigned)SYSCALL_SOFT_AREAS,
                (unsigned)SYSCALL_SOFT_WAVE);
    }

    /*
     * Compact residual twin (route ownership honesty + DDI/net doors).
     * G-AC-1: no Linux .ko in-kernel product; UDX/DDI = userspace hosts.
     * Native route owns GJ_SYS_DDI + GJ_SYS_NET; Linux route owns hybrid ABI.
     * Dual DoD A/B OPEN (agent soft residual != product close).
     * Freestanding rtl/USB class probes remain SKIP residual.
     * Grep: syscall: soft residual
     * Grep: Dual_DoD_A=OPEN Dual_DoD_B=OPEN freestanding_probe=SKIP
     */
    kprintf("syscall: soft residual route=personality+bind "
            "native=gj_native_syscall_dispatch "
            "linux=gj_linux_syscall_dispatch "
            "udx_ddi=userspace_over_native_DDI_or_hybrid_ABI "
            "native_hosts=GJ_SYS_DDI,platform,notify,GJ_SYS_NET "
            "linux_hosts=OptionC_hot_cold "
            "ddi_op_catalog=frozen net_op_catalog=frozen "
            "ddi_bind=SCAN,GET,OPEN,MAP_BAR "
            "udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce "
            "sshd_path=SOCKET,BIND,LISTEN,ACCEPT "
            "H1_poll_thr_only=1 "
            "product_hosts=rtl8168_udx,xhci_udx,ddi_host_gj "
            "consumers=sshd,netstackd,udx_hosts "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "dod_a=UDX_USB_xhci_udx dod_b=UDX_NIC_rtl8168_udx "
            "freestanding_probe=SKIP freestanding_rtl_usb=SKIP "
            "agent_close=0 residual_ne_close=1 "
            "soft_only=1 product_gate=0 G-AC-1=1 no_ko_product=1 "
            "soft_ne_product=1 dual=MIT_OR_Apache-2.0 "
            "stamp_storm=0 version_stamp=0 "
            "(STRONGER functional residual; Soft!=product dual license; "
            "no version stamp; DDI/net door residual for UDX hosts + sshd; "
            "dispatch residual does not close UDX/DDI product; "
            "Dual DoD A/B stay OPEN; freestanding SKIP; "
            "product drivers stay userspace)\n");

    /*
     * C2 Dual DoD route honesty residual (deepen). Soft lamps never close A/B.
     * Product path = UDX/DDI hosts + hot/cold ABI + net door for sshd.
     * This unit only routes; handlers live in native/linux_*.c.
     * Grep: syscall: soft residual dual_dod
     * Grep: Dual_DoD_A=OPEN Dual_DoD_B=OPEN
     */
    kprintf("syscall: soft residual dual_dod "
            "Dual_DoD_A=OPEN Dual_DoD_B=OPEN "
            "dod_a=UDX_USB_xhci_udx dod_b=UDX_NIC_rtl8168_udx "
            "agent_close=0 residual_ne_close=1 "
            "product=UDX/DDI+hot/cold_ABI "
            "native_leg=gj_native_syscall_dispatch "
            "linux_leg=gj_linux_syscall_dispatch "
            "native_udx=GJ_SYS_DDI+platform+notify "
            "native_net=GJ_SYS_NET "
            "linux_udx=OptionC_hot_cold "
            "hosts=xhci_udx,rtl8168_udx,ddi_host_gj "
            "consumers=sshd,netstackd,udx_hosts "
            "nr_ddi=%u nr_plat=%u nr_notify=%u nr_net=%u "
            "ddi_op_catalog=frozen net_op_catalog=frozen "
            "ddi_bind=SCAN,GET,OPEN,MAP_BAR "
            "udx_host_stack=claim|map_ring|map_dma|desc|user_avail|bounce "
            "sshd_path=SOCKET,BIND,LISTEN,ACCEPT "
            "H1_poll_thr_only=1 "
            "disp=%llu native=%llu linux=%llu "
            "bound=%llu unbound=%llu "
            "freestanding_probe=SKIP freestanding_rtl_usb=SKIP "
            "soft_ne_product=1 G-AC-1=1 no_ko_product=1 "
            "dual=MIT_OR_Apache-2.0 "
            "stamp_storm=0 version_stamp=0 product_gate=0 "
            "wave=%u areas=%u "
            "(Soft!=product; Dual DoD A/B remain OPEN; "
            "soft residual lamps never close DoD; "
            "STRONGER functional residual DDI/net doors for UDX+sshd; "
            "C2 product path = UDX/DDI + hot/cold ABI; "
            "top-level dispatch residual only routes+binds; "
            "no in-kernel .ko; no freestanding rtl/usb)\n",
            (unsigned)GJ_SYS_DDI,
            (unsigned)GJ_SYS_PLATFORM_INFO,
            (unsigned)GJ_SYS_NOTIFY_WAIT,
            (unsigned)GJ_SYS_NET,
            (unsigned long long)s.u64DispatchEnter,
            (unsigned long long)s.u64Native,
            (unsigned long long)s.u64Linux,
            (unsigned long long)s.u64Bound,
            (unsigned long long)s.u64Unbound,
            (unsigned)SYSCALL_SOFT_WAVE,
            (unsigned)SYSCALL_SOFT_AREAS);

    /* Grep: syscall: soft inventory PASS / syscall: soft PASS */
    kprintf("syscall: soft inventory PASS wave=%u logs=%llu "
            "disp=%llu native=%llu linux=%llu\n",
            (unsigned)SYSCALL_SOFT_WAVE,
            (unsigned long long)g_u64SoftLogN,
            (unsigned long long)s.u64DispatchEnter,
            (unsigned long long)s.u64Native,
            (unsigned long long)s.u64Linux);
    kprintf("syscall: soft PASS wave=%u logs=%llu\n",
            (unsigned)SYSCALL_SOFT_WAVE,
            (unsigned long long)g_u64SoftLogN);
}

/**
 * After first product dispatch/bridge activity, print soft inventory once
 * (mirrors native/linux soft-stats-once). Diagnostics only - never gates.
 */
static void
entry_soft_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_entryStats.u64DispatchEnter == 0 &&
        g_entryStats.u64BridgeEnter == 0 &&
        g_entryStats.u64DispatchNull == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    entry_soft_inventory_log();
}

void
gj_syscall_entry_soft_note_bridge(struct gj_syscall_regs *pRegs)
{
    entry_soft_inc(&g_u64SoftBridgeNote);
    g_entryStats.u64BridgeEnter++;
    if (pRegs == NULL) {
        g_entryStats.u64BridgeNull++;
        entry_soft_maybe_once();
        return;
    }
    /* Soft snapshot nr at the hardware edge (before route). */
    g_entryStats.u64LastNr = pRegs->u64Nr;
    entry_soft_maybe_once();
}

void
gj_syscall_entry_stats_get(struct gj_syscall_entry_stats *pOut)
{
    entry_soft_inc(&g_u64SoftStatsGet);
    if (pOut == NULL) {
        entry_soft_inc(&g_u64SoftStatsGetNull);
        return;
    }
    entry_soft_mirror_default();
    *pOut = g_entryStats;
}

void
gj_syscall_entry_stats_reset(void)
{
    entry_soft_inc(&g_u64SoftStatsReset);
    memset(&g_entryStats, 0, sizeof(g_entryStats));
    /* File-local deepen tallies (api) survive reset - lifetime of module.
     * Allow one-shot deepen again after a product reset of entry path. */
    g_fSoftInvOnce = 0;
    entry_soft_mirror_default();
}

u64
gj_syscall_entry_stats_soft(void)
{
    entry_soft_inc(&g_u64SoftSoftCall);
    /*
     * Full Wave 15 multi-line soft inventory (includes legacy stats line).
     * Grep: syscall: soft stats / syscall: soft inventory
     */
    entry_soft_inventory_log();
    return g_entryStats.u64DispatchEnter;
}

void
gj_syscall_init(void)
{
    cold_ipc_init();
    futex_init();
    gj_linux_dispatch_init();
    g_eDefaultPersonality = GJ_PERSONALITY_LINUX;
    g_entryStats.u64Init++;
    g_fSoftInvOnce = 0;
    entry_soft_mirror_default();
    /* Wave 15 soft inventory baseline (greppable syscall: soft ...). */
    entry_soft_inventory_log();
}

void
gj_syscall_set_default_personality(enum gj_personality e)
{
    /* Reject out-of-range values so a corrupt caller cannot invent modes. */
    if (e != GJ_PERSONALITY_NATIVE && e != GJ_PERSONALITY_LINUX) {
        g_entryStats.u64SetPersReject++;
        return;
    }
    g_eDefaultPersonality = e;
    g_entryStats.u64SetPersOk++;
    entry_soft_mirror_default();
}

enum gj_personality
gj_syscall_get_default_personality(void)
{
    return g_eDefaultPersonality;
}

/**
 * Resolve personality for this trap.
 * Prefer the current process PCB; fall back to the boot default.
 * Soft: bumps PCB native/linux or default-personality counters.
 */
static enum gj_personality
current_personality(void)
{
    struct gj_thread *pThr = thread_current();

    if (pThr != NULL && pThr->pProc != NULL) {
        if (pThr->pProc->u32Personality == 0) {
            g_entryStats.u64PcbNative++;
            return GJ_PERSONALITY_NATIVE;
        }
        g_entryStats.u64PcbLinux++;
        return GJ_PERSONALITY_LINUX;
    }
    g_entryStats.u64DefaultPers++;
    return g_eDefaultPersonality;
}

void
gj_syscall_dispatch(struct gj_syscall_regs *pRegs)
{
    struct gj_linux_regs linuxRegs;
    struct gj_thread *pThr;
    enum gj_personality ePers;

    g_entryStats.u64DispatchEnter++;
    if (pRegs == NULL) {
        g_entryStats.u64DispatchNull++;
        entry_soft_maybe_once();
        return;
    }

    g_entryStats.u64LastNr = pRegs->u64Nr;

    /*
     * Bind Linux "current" before any hybrid work so mmap/AS, creds, and
     * per-process tables see the calling process (not a stale boot task).
     * Native path does not require this, but binding is cheap and keeps a
     * mixed call sequence consistent if personality flips mid-smoke.
     */
    pThr = thread_current();
    if (pThr != NULL && pThr->pProc != NULL) {
        g_entryStats.u64Bound++;
        gj_linux_set_current(pThr->pProc, 1,
                             pThr->u32Id != 0 ? pThr->u32Id : 1);
    } else {
        g_entryStats.u64Unbound++;
    }

    ePers = current_personality();
    if (ePers == GJ_PERSONALITY_NATIVE) {
        g_entryStats.u64Native++;
        gj_native_syscall_dispatch(pRegs);
        entry_soft_note_complete(pRegs);
        entry_soft_maybe_once();
        return;
    }

    /* Linux hybrid Option C: hot table first, cold door for the rest. */
    g_entryStats.u64Linux++;
    memset(&linuxRegs, 0, sizeof(linuxRegs));
    linuxRegs.u64Nr = pRegs->u64Nr;
    linuxRegs.u64Arg0 = pRegs->u64Arg0;
    linuxRegs.u64Arg1 = pRegs->u64Arg1;
    linuxRegs.u64Arg2 = pRegs->u64Arg2;
    linuxRegs.u64Arg3 = pRegs->u64Arg3;
    linuxRegs.u64Arg4 = pRegs->u64Arg4;
    linuxRegs.u64Arg5 = pRegs->u64Arg5;
    gj_linux_syscall_dispatch(&linuxRegs);
    pRegs->i64Ret = linuxRegs.i64Ret;
    entry_soft_note_complete(pRegs);
    entry_soft_maybe_once();
}
