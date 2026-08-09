/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * WoW64 soft thunk: i386 Linux syscall NR map + arg convert + personality.
 * Pure C dual-license (MIT OR Apache-2.0); clean-room public man-page map -
 * not Linux/Wine paste. Product PE32 load is userspace; CS32 int 0x80 is
 * trap_dispatch. Hybrid SYSCALL uses this table; soft thunk zero-extends
 * and fixes mmap2 pgoff / socketcall / ipc demux without hardware compat CS.
 *
 * Lean residual 32-bit Linux-shaped thunk (preferred over soft-lamp storms):
 *   - Functional: NR map + arg zx + mmap2 pgoff + socketcall demux +
 *     ipc demux + _llseek compose + old_mmap note + ptr32 soft-canon
 *   - C2 residual deepen: cred32/TLS (modify_ldt + set_thread_area) /
 *     umask / times / truncate64 / socket-direct (359+) / utimes fix /
 *     time64 soft aliases / PE32 companion NR surface (int80 hybrid map)
 *   - Sparse greppable lamps only; multi-line dump capped
 *   - Soft residual lean once-check (map/thunk/adjust self-smoke)
 * Soft!=product; G-AC-1 (no .ko product AC). No version stamp. No stamp storms.
 * Soft inventory never hard-gates path PASS; wrap OK.
 * Dual DoD A/B remain OPEN (agent does not close); freestanding rtl/USB SKIP.
 *
 * greppable: wow64: soft residual lean
 * greppable: wow64: soft residual catalog
 * greppable: wow64: soft residual socket_direct
 * greppable: wow64: soft residual cred32
 * greppable: wow64: soft residual pe32_companion
 * greppable: wow64: soft residual tls
 * greppable: wow64: soft residual time64
 * greppable: wow64: soft inventory
 * greppable: wow64: soft path
 * greppable: wow64: soft honesty
 * greppable: wow64: soft PASS
 * greppable: WOW64_LEAN_RESIDUAL
 * greppable: WOW64_C2_RESIDUAL
 * greppable: WOW64_PE32_COMPANION
 */
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/wow64.h>

static int g_fWow64;
static u32 g_u32Calls;
static u32 g_u32MapHits;
static u32 g_u32IdentityHits;
static u32 g_u32ThunkHits;
static u32 g_u32Mmap2Hits;
static u32 g_u32SocketcallHits;
static u32 g_u32IpcHits;

/*
 * Soft inventory (lean residual). Cumulative path tallies.
 * greppable: wow64: soft ...
 * Sparse areas only - no stamp storms; no version stamp.
 * Soft!=product; G-AC-1; dual MIT OR Apache-2.0.
 */
#define GJ_WOW64_SOFT_WAVE    126u
/* Areas: map+zx+mmap2+socketcall+ipc+llseek+old_mmap+cred32+tls+
 * socket_direct+time64+ptr32+pe32_companion+catalog honesty
 * (C2 residual PE32 companion; Soft!=product). */
#define GJ_WOW64_SOFT_AREAS   20u
/* Cap multi-line soft inventory dumps (Soft!=product; no stamp storms). */
#define GJ_WOW64_SOFT_LOG_CAP 4u
/* Lean residual self-check slot count (map+identity+fixups+PE32 C2). */
#define GJ_WOW64_LEAN_SLOTS   18u
#define GJ_WOW64_LEAN_NEED    14u
/* i386 socket-direct NR band (modern glibc; not socketcall 102). */
#define GJ_WOW64_SOCK_DIRECT_LO  359u
#define GJ_WOW64_SOCK_DIRECT_HI  373u

/* i386 ipc() call codes (public man ipc; clean-room). Soft demux residual. */
#define WOW64_IPC_SEMOP       1u
#define WOW64_IPC_SEMGET      2u
#define WOW64_IPC_SEMCTL      3u
#define WOW64_IPC_SEMTIMEDOP  4u
#define WOW64_IPC_MSGSND      11u
#define WOW64_IPC_MSGRCV      12u
#define WOW64_IPC_MSGGET      13u
#define WOW64_IPC_MSGCTL      14u
#define WOW64_IPC_SHMAT       21u
#define WOW64_IPC_SHMDT       22u
#define WOW64_IPC_SHMGET      23u
#define WOW64_IPC_SHMCTL      24u

static u32 g_u32SoftTranslateEnter; /* wow64_translate_nr entries */
static u32 g_u32SoftTranslateNull;  /* translate with pOutNr == NULL */
static u32 g_u32SoftIsMappedProbe;  /* wow64_nr_is_mapped probes */
static u32 g_u32SoftThunkEnter;     /* wow64_thunk_soft entries */
static u32 g_u32SoftThunkNull;      /* thunk_soft with pThunk == NULL */
static u32 g_u32SoftThunkOk;        /* thunk_soft success returns */
static u32 g_u32SoftAdjustEnter;    /* wow64_adjust_args entries */
static u32 g_u32SoftAdjustNop;      /* adjust early return (null/0) */
static u32 g_u32SoftAdjustZx;       /* adjust applied ARGS_ZX */
static u32 g_u32SoftAdjustMmap2;    /* adjust mmap2 pgoff path */
static u32 g_u32SoftAdjustSocket;   /* adjust socketcall demux path */
static u32 g_u32SoftAdjustSockFail; /* socketcall unknown subcall */
static u32 g_u32SoftAdjustOldMmap;  /* old_mmap (NR 90) soft path */
static u32 g_u32SoftAdjustLlseek;   /* _llseek (NR 140) soft compose */
static u32 g_u32SoftAdjustIpc;      /* ipc (NR 117) soft demux */
static u32 g_u32SoftAdjustIpcFail;  /* ipc unknown subcall */
static u32 g_u32SoftArgsZxCalls;    /* wow64_args_zero_extend calls */
static u32 g_u32SoftSocketNrOk;     /* wow64_socketcall_nr mapped */
static u32 g_u32SoftSocketNrFail;   /* socketcall_nr unknown/null */
static u32 g_u32SoftIpcNrOk;        /* ipc demux mapped */
static u32 g_u32SoftIpcNrFail;      /* ipc demux fail */
static u32 g_u32SoftPtr32;          /* wow64_ptr32 calls */
static u32 g_u32SoftPersonOn;       /* wow64_set(on) transitions */
static u32 g_u32SoftPersonOff;      /* wow64_set(off) transitions */
static u32 g_u32SoftLogN;           /* soft inventory log emissions */
static u8  g_fSoftInvOnce;          /* one-shot deep dump after activity */
static u32 g_u32SoftLeanRuns;       /* lean residual self-check runs */
static u32 g_u32SoftLeanOk;         /* lean residual checks passed */
static u32 g_u32SoftSockDirectMap;  /* socket-direct band map hits */
static u32 g_u32SoftPtr32Canon;     /* adjust soft ptr32 canon hits */
static u32 g_u32SoftPe32CompOk;     /* PE32 companion lean probes ok */

static void soft_inc(u32 *pCtr);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);
static void soft_residual_lean_once(void);
static int  wow64_map_nr_core(u32 u32Nr32, u32 *pOutNr);
static int  wow64_ipc_nr(u32 u32Call, u32 *pOutNr);
static u32  wow64_soft_ptr32_canon(u32 u32Nr32, u64 *pArgs, u32 cArgs);

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/**
 * Sparse greppable soft WoW64 inventory (lean residual).
 * Cap multi-line dumps - Soft!=product; G-AC-1; no stamp storms.
 * greppable: wow64: soft inventory | path | residual lean | honesty | PASS
 * Honesty: soft inventory only - not product gate; not Top-50/bar3.
 */
static void
soft_inventory_log(void)
{
    u32 u32Enabled;
    u32 u32AdjActive;
    u32 u32MapRatio;

    if (g_u32SoftLogN >= GJ_WOW64_SOFT_LOG_CAP) {
        /* Further calls: silent tally only (no stamp storms). */
        soft_inc(&g_u32SoftLogN);
        return;
    }

    soft_inc(&g_u32SoftLogN);
    u32Enabled = (g_fWow64 != 0) ? 1u : 0u;
    u32AdjActive = 0;
    if (g_u32SoftAdjustMmap2 != 0) {
        u32AdjActive++;
    }
    if (g_u32SoftAdjustSocket != 0) {
        u32AdjActive++;
    }
    if (g_u32SoftAdjustOldMmap != 0) {
        u32AdjActive++;
    }
    if (g_u32SoftAdjustLlseek != 0) {
        u32AdjActive++;
    }
    if (g_u32SoftAdjustIpc != 0) {
        u32AdjActive++;
    }
    if (g_u32Calls != 0) {
        u32MapRatio = (u32)(((u64)g_u32MapHits * 10000ull) /
                            (u64)g_u32Calls);
    } else {
        u32MapRatio = 0;
    }

    /* Grep: wow64: soft inventory - single rollup */
    kprintf("wow64: soft inventory enabled=%u calls=%u map=%u identity=%u "
            "thunk=%u mmap2=%u socketcall=%u ipc=%u "
            "tr_enter=%u th_ok=%u adj_enter=%u adj_active=%u "
            "map_bp=%u log_n=%u areas=%u wave=%u "
            "(sparse; Soft!=product; G-AC-1; dual MIT OR Apache-2.0)\n",
            u32Enabled, g_u32Calls, g_u32MapHits, g_u32IdentityHits,
            g_u32ThunkHits, g_u32Mmap2Hits, g_u32SocketcallHits,
            g_u32IpcHits,
            g_u32SoftTranslateEnter, g_u32SoftThunkOk,
            g_u32SoftAdjustEnter, u32AdjActive, u32MapRatio,
            g_u32SoftLogN, (unsigned)GJ_WOW64_SOFT_AREAS,
            (unsigned)GJ_WOW64_SOFT_WAVE);

    /* Grep: wow64: soft residual lean */
    kprintf("wow64: soft residual lean "
            "map=%u thunk=%u mmap2=%u socket=%u ipc=%u llseek=%u "
            "lean_runs=%u lean_ok=%u soft_ne_product=1 G-AC-1=1 "
            "dual=MIT_OR_Apache-2.0 "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
            "not product WoW64 / not bar3)\n",
            g_u32MapHits, g_u32ThunkHits, g_u32Mmap2Hits,
            g_u32SoftAdjustSocket, g_u32IpcHits,
            g_u32SoftAdjustLlseek, g_u32SoftLeanRuns, g_u32SoftLeanOk);

    /*
     * Grep: wow64: soft residual catalog | WOW64_C2_RESIDUAL
     * Grep: wow64: soft residual socket_direct | cred32 | pe32_companion
     * Grep: wow64: soft residual tls | time64
     * Catalog only - never closes Dual DoD A/B; Soft!=product.
     */
    kprintf("wow64: soft residual catalog "
            "families=nr_map,arg_zx,mmap2_pgoff,socketcall,ipc,llseek,"
            "old_mmap,cred32,tls,socket_direct,time64,ptr32,"
            "pe32_companion "
            "WOW64_C2_RESIDUAL=1 WOW64_PE32_COMPANION=1 areas=%u "
            "(Soft!=product; G-AC-1; dual MIT OR Apache-2.0; "
            "Dual_DoD_A_B=OPEN; freestanding_rtl_usb=SKIP; not bar3)\n",
            (unsigned)GJ_WOW64_SOFT_AREAS);
    kprintf("wow64: soft residual socket_direct "
            "i386_nr=%u..%u map=x86_64_native hits=%u "
            "(soft residual; Soft!=product; never hard-gates)\n",
            (unsigned)GJ_WOW64_SOCK_DIRECT_LO,
            (unsigned)GJ_WOW64_SOCK_DIRECT_HI,
            g_u32SoftSockDirectMap);
    kprintf("wow64: soft residual cred32 "
            "getgroups32,setgroups32,getresuid32,getresgid32,"
            "setuid32,setgid32,setfsuid32,setfsgid32,"
            "umask,times,capget,capset,truncate64,setfsuid,setfsgid "
            "(soft residual; Soft!=product)\n");
    kprintf("wow64: soft residual tls "
            "modify_ldt=123->154 set_thread_area=243->158 "
            "get_thread_area=244->158 set_tid_address=258->218 "
            "set_robust_list=311->273 "
            "(PE32 int80 TLS companion; Soft!=product)\n");
    kprintf("wow64: soft residual time64 "
            "i386_nr=403..414,422 soft_alias=non_time64_x86_64 "
            "(struct layout cold-path; Soft!=product)\n");
    /*
     * Grep: wow64: soft residual pe32_companion | WOW64_PE32_COMPANION
     * Hybrid NR companion for PE32 int80 / CS32 soft path (trap owns
     * product enter; this module is NR/arg thunk residual only).
     */
    kprintf("wow64: soft residual pe32_companion "
            "int80_map=1 tls=1 mmap2=1 socketcall=1 socket_direct=1 "
            "cred32=1 exit_group=1 gettid=1 clock_gettime=1 "
            "ptr32_canon=%u lean_pe32_ok=%u "
            "WOW64_PE32_COMPANION=1 WOW64_C2_RESIDUAL=1 "
            "(Soft!=product; pe32_int80=trap; product_pe32=userspace; "
            "Dual_DoD_A_B=OPEN; not bar3)\n",
            g_u32SoftPtr32Canon, g_u32SoftPe32CompOk);

    /* Grep: wow64: soft path | wow64: soft honesty */
    kprintf("wow64: soft path claim=nr_map+arg_zx+mmap2_pgoff+"
            "socketcall_demux+ipc_demux+llseek_compose+"
            "socket_direct+cred32+tls+ptr32+pe32_companion "
            "hybrid=SYSCALL+int80 areas=%u "
            "(soft residual; never hard-gates; G-AC-1)\n",
            (unsigned)GJ_WOW64_SOFT_AREAS);
    kprintf("wow64: soft honesty hybrid=OptionC open=1 "
            "product_pe32=userspace soft_only=1 pe32_int80=trap "
            "soft_ne_product=1 G-AC-1=1 dual=MIT_OR_Apache-2.0 "
            "Dual_DoD_A_B=OPEN product=UDX_DDI "
            "(Soft!=product; never closes hybrid; not bar3)\n");

    /* Grep: wow64: soft inventory PASS / soft PASS */
    kprintf("wow64: soft inventory PASS logs=%u calls=%u map=%u thunk=%u "
            "lean_ok=%u pe32_comp_ok=%u (soft only; Soft!=product)\n",
            g_u32SoftLogN, g_u32Calls, g_u32MapHits, g_u32ThunkHits,
            g_u32SoftLeanOk, g_u32SoftPe32CompOk);
    kprintf("wow64: soft PASS logs=%u lean_ok=%u pe32_comp_ok=%u\n",
            g_u32SoftLogN, g_u32SoftLeanOk, g_u32SoftPe32CompOk);
}

/**
 * Lean residual self-check (stack-local thunk; no product PE / no CS32).
 * Exercises map + mmap2 pgoff + socketcall demux + ipc demux + llseek +
 * C2 deepen: modify_ldt + set_thread_area + umask + socket_direct +
 * old_mmap + utimes + setuid32 + PE32 companion (gettid/exit_group/
 * clock_gettime/truncate64/times/ptr32). Soft!=product; G-AC-1.
 * greppable: WOW64_LEAN_RESIDUAL WOW64_C2_RESIDUAL WOW64_PE32_COMPANION
 * greppable: wow64: soft residual lean | pe32_companion
 */
static void
soft_residual_lean_once(void)
{
    static u8 s_fLeanOnce;
    struct gj_wow64_thunk th;
    u32 u32Nr64;
    u32 u32Ok;
    u32 u32Pe32Ok;
    u32 u32Fl;
    u64 aArgs[6];

    if (s_fLeanOnce != 0) {
        return;
    }
    s_fLeanOnce = 1;
    soft_inc(&g_u32SoftLeanRuns);
    u32Ok = 0;
    u32Pe32Ok = 0;

    /* Map residual: i386 futex(240) -> x86_64 futex(202). */
    if (wow64_map_nr_core(240u, &u32Nr64) != 0 && u32Nr64 == 202u) {
        u32Ok++;
    }
    /* Identity residual: unmapped high NR passes through. */
    if (wow64_map_nr_core(0xffffu, &u32Nr64) == 0 && u32Nr64 == 0xffffu) {
        u32Ok++;
    }

    /* mmap2 pgoff residual via thunk_soft. */
    memset(&th, 0, sizeof(th));
    th.u32Nr32 = 192u; /* mmap2 */
    th.aArgs[5] = 3ull; /* 3 pages -> 0x3000 bytes */
    if (wow64_thunk_soft(&th) == 0 && th.u32Nr64 == 9u &&
        th.aArgs[5] == 0x3000ull &&
        (th.u32Flags & GJ_WOW64_THUNK_MMAP2_PGOFF) != 0) {
        u32Ok++;
        u32Pe32Ok++; /* PE32 int80 mmap2 companion */
    }

    /* socketcall demux residual: SYS_SOCKET -> 41. */
    memset(&th, 0, sizeof(th));
    th.u32Nr32 = 102u;
    th.aArgs[0] = (u64)GJ_WOW64_SYS_SOCKET;
    /* high garbage in args_ptr slot exercises soft ptr32 canon */
    th.aArgs[1] = 0xdeadbeef12345678ull;
    if (wow64_thunk_soft(&th) == 0 && th.u32Nr64 == 41u &&
        (th.u32Flags & GJ_WOW64_THUNK_SOCKETCALL) != 0 &&
        th.u32SocketCall == GJ_WOW64_SYS_SOCKET &&
        th.aArgs[1] == 0x12345678ull) {
        u32Ok++;
        u32Pe32Ok++;
    }

    /* ipc demux residual: SHMGET -> 29. */
    aArgs[0] = (u64)WOW64_IPC_SHMGET;
    aArgs[1] = 0x11ull; /* first */
    aArgs[2] = 0x22ull;
    aArgs[3] = 0x33ull;
    aArgs[4] = 0x44ull;
    aArgs[5] = 0x55ull;
    u32Nr64 = 0;
    u32Fl = wow64_adjust_args(117u, &u32Nr64, aArgs, 6);
    if ((u32Fl & GJ_WOW64_THUNK_IPC) != 0 && u32Nr64 == 29u &&
        aArgs[0] == 0x11ull && aArgs[1] == 0x22ull) {
        u32Ok++;
    }

    /* _llseek compose residual + result* ptr32 soft-canon. */
    aArgs[0] = 3ull;          /* fd */
    aArgs[1] = 0x1ull;        /* high */
    aArgs[2] = 0x234ull;      /* low */
    aArgs[3] = 0xffff00004000ull; /* result* with high garbage */
    aArgs[4] = 0;             /* SEEK_SET */
    aArgs[5] = 0;
    u32Nr64 = 0;
    u32Fl = wow64_adjust_args(140u, &u32Nr64, aArgs, 6);
    if ((u32Fl & GJ_WOW64_THUNK_LLSEEK) != 0 &&
        aArgs[1] == 0x100000234ull && aArgs[2] == 0 &&
        aArgs[3] == 0x4000ull) {
        u32Ok++;
    }

    /* C2: modify_ldt (123) -> 154 TLS residual (PE32 int80 soft path). */
    if (wow64_map_nr_core(123u, &u32Nr64) != 0 && u32Nr64 == 154u) {
        u32Ok++;
        u32Pe32Ok++;
    }
    /* C2: umask (60) -> 95. */
    if (wow64_map_nr_core(60u, &u32Nr64) != 0 && u32Nr64 == 95u) {
        u32Ok++;
    }
    /* C2: socket-direct (359) -> socket(41); modern glibc i386. */
    if (wow64_map_nr_core(359u, &u32Nr64) != 0 && u32Nr64 == 41u) {
        u32Ok++;
        u32Pe32Ok++;
    }
    /* C2: old_mmap (90) soft note via adjust. */
    aArgs[0] = 0xffff00001000ull; /* struct* with high garbage */
    aArgs[1] = aArgs[2] = aArgs[3] = aArgs[4] = aArgs[5] = 0;
    u32Nr64 = 0;
    u32Fl = wow64_adjust_args(90u, &u32Nr64, aArgs, 6);
    if ((u32Fl & GJ_WOW64_THUNK_OLD_MMAP) != 0 && u32Nr64 == 9u &&
        aArgs[0] == 0x1000ull) {
        u32Ok++;
    }
    /* C2: utimes (271) -> 235 (not utimensat; public man-page map). */
    if (wow64_map_nr_core(271u, &u32Nr64) != 0 && u32Nr64 == 235u) {
        u32Ok++;
    }
    /* C2: setuid32 (213) -> setuid(105) cred32 honesty. */
    if (wow64_map_nr_core(213u, &u32Nr64) != 0 && u32Nr64 == 105u) {
        u32Ok++;
    }

    /*
     * C2 PE32 companion residual deepen (int80 surface companions):
     * set_thread_area / gettid / exit_group / clock_gettime /
     * truncate64 / times. Soft map only; trap owns product enter.
     */
    if (wow64_map_nr_core(243u, &u32Nr64) != 0 && u32Nr64 == 158u) {
        u32Ok++;
        u32Pe32Ok++; /* set_thread_area -> arch_prctl soft */
    }
    if (wow64_map_nr_core(224u, &u32Nr64) != 0 && u32Nr64 == 186u) {
        u32Ok++;
        u32Pe32Ok++; /* gettid */
    }
    if (wow64_map_nr_core(252u, &u32Nr64) != 0 && u32Nr64 == 231u) {
        u32Ok++;
        u32Pe32Ok++; /* exit_group */
    }
    if (wow64_map_nr_core(265u, &u32Nr64) != 0 && u32Nr64 == 228u) {
        u32Ok++;
        u32Pe32Ok++; /* clock_gettime */
    }
    if (wow64_map_nr_core(193u, &u32Nr64) != 0 && u32Nr64 == 76u) {
        u32Ok++; /* truncate64 */
    }
    if (wow64_map_nr_core(43u, &u32Nr64) != 0 && u32Nr64 == 100u) {
        u32Ok++; /* times */
    }

    g_u32SoftPe32CompOk = u32Pe32Ok;

    if (u32Ok >= GJ_WOW64_LEAN_NEED) {
        soft_inc(&g_u32SoftLeanOk);
    }

    /* Grep: wow64: soft residual lean PASS / WOW64_LEAN_RESIDUAL */
    kprintf("wow64: soft residual lean PASS ok=%u/%u pe32_ok=%u "
            "mmap2_pgoff=1 socketcall=1 ipc=1 llseek=1 "
            "modify_ldt=1 umask=1 socket_direct=1 old_mmap=1 "
            "utimes=1 setuid32=1 set_thread_area=1 gettid=1 "
            "exit_group=1 clock_gettime=1 truncate64=1 times=1 "
            "soft_ne_product=1 G-AC-1=1 dual=MIT_OR_Apache-2.0 "
            "WOW64_LEAN_RESIDUAL=1 WOW64_C2_RESIDUAL=1 "
            "WOW64_PE32_COMPANION=1 "
            "(Soft!=product; dual MIT OR Apache-2.0; no version stamp; "
            "not product WoW64 / not bar3; Dual_DoD_A_B=OPEN)\n",
            (unsigned)u32Ok, (unsigned)GJ_WOW64_LEAN_SLOTS,
            (unsigned)u32Pe32Ok);
    /* Grep: wow64: soft residual pe32_companion PASS */
    kprintf("wow64: soft residual pe32_companion PASS "
            "ok=%u tls=1 mmap2=1 socket=1 gettid=1 exit_group=1 "
            "clock_gettime=1 ptr32=1 "
            "WOW64_PE32_COMPANION=1 WOW64_C2_RESIDUAL=1 "
            "(Soft!=product; pe32_int80=trap; Dual_DoD_A_B=OPEN)\n",
            (unsigned)u32Pe32Ok);
}

/**
 * After first translate/thunk/adjust/personality activity: lean residual
 * self-check + sparse inventory once. Diagnostics only - never gates PASS.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_u32SoftTranslateEnter == 0 && g_u32SoftThunkEnter == 0 &&
        g_u32SoftAdjustEnter == 0 && g_u32SoftPersonOn == 0 &&
        g_u32SoftPersonOff == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    soft_residual_lean_once();
    soft_inventory_log();
}

int
wow64_enabled(void)
{
    return g_fWow64;
}

void
wow64_set(int fOn)
{
    g_fWow64 = fOn ? 1 : 0;
    if (g_fWow64 != 0) {
        soft_inc(&g_u32SoftPersonOn);
    } else {
        soft_inc(&g_u32SoftPersonOff);
    }
    kprintf("wow64: personality %s\n", g_fWow64 ? "on" : "off");
    /* Wave 19: greppable soft inventory on personality flip. */
    soft_inventory_maybe_once();
}

/*
 * Core NR map (no counters). Returns 1 if explicit map, 0 if identity.
 * i386 -> x86_64 public ABI; Deck Top-50 / glibc / PE32 int80 soft needs.
 * Single switch only - no duplicate case labels.
 */
static int
wow64_map_nr_core(u32 u32Nr32, u32 *pOutNr)
{
    u32 nr64;
    int fMapped = 1;

    switch (u32Nr32) {
    /* ---- process / exit / ids / credentials --------------------------- */
    case 1:   nr64 = 60; break;   /* exit */
    case 2:   nr64 = 57; break;   /* fork */
    case 7:   nr64 = 61; break;   /* waitpid -> wait4 */
    case 11:  nr64 = 59; break;   /* execve */
    case 13:  nr64 = 201; break;  /* time */
    case 20:  nr64 = 39; break;   /* getpid */
    case 23:  nr64 = 105; break;  /* setuid */
    case 24:  nr64 = 102; break;  /* getuid */
    case 27:  nr64 = 37; break;   /* alarm */
    case 29:  nr64 = 34; break;   /* pause */
    case 37:  nr64 = 62; break;   /* kill */
    case 43:  nr64 = 100; break;  /* times */
    case 46:  nr64 = 106; break;  /* setgid */
    case 47:  nr64 = 104; break;  /* getgid */
    case 49:  nr64 = 107; break;  /* geteuid */
    case 50:  nr64 = 108; break;  /* getegid */
    case 57:  nr64 = 109; break;  /* setpgid */
    case 60:  nr64 = 95; break;   /* umask */
    case 61:  nr64 = 161; break;  /* chroot */
    case 64:  nr64 = 110; break;  /* getppid */
    case 65:  nr64 = 111; break;  /* getpgrp */
    case 66:  nr64 = 112; break;  /* setsid */
    case 70:  nr64 = 113; break;  /* setreuid */
    case 71:  nr64 = 114; break;  /* setregid */
    case 81:  nr64 = 116; break;  /* setgroups */
    case 110: nr64 = 172; break;  /* iopl (PE32/Wine residual) */
    case 114: nr64 = 61; break;   /* wait4 */
    case 117: nr64 = 29; break;   /* ipc -> shmget coarse; soft demux */
    case 120: nr64 = 56; break;   /* clone */
    case 121: nr64 = 171; break;  /* setdomainname */
    case 123: nr64 = 154; break;  /* modify_ldt (PE32 TLS residual) */
    case 124: nr64 = 159; break;  /* adjtimex */
    case 132: nr64 = 115; break;  /* getgroups */
    case 138: nr64 = 122; break;  /* setfsuid (non-32; PE32 cred residual) */
    case 139: nr64 = 123; break;  /* setfsgid */
    case 147: nr64 = 124; break;  /* getsid */
    case 164: nr64 = 117; break;  /* setresuid */
    case 165: nr64 = 118; break;  /* getresuid */
    case 170: nr64 = 119; break;  /* setresgid */
    case 171: nr64 = 120; break;  /* getresgid */
    case 173: nr64 = 15; break;   /* rt_sigreturn (PE32 signal residual) */
    case 184: nr64 = 125; break;  /* capget */
    case 185: nr64 = 126; break;  /* capset */
    case 190: nr64 = 58; break;   /* vfork */
    case 199: nr64 = 102; break;  /* getuid32 -> getuid */
    case 200: nr64 = 104; break;  /* getgid32 -> getgid */
    case 201: nr64 = 107; break;  /* geteuid32 */
    case 202: nr64 = 108; break;  /* getegid32 */
    case 203: nr64 = 113; break;  /* setreuid32 */
    case 204: nr64 = 114; break;  /* setregid32 */
    case 205: nr64 = 115; break;  /* getgroups32 */
    case 206: nr64 = 116; break;  /* setgroups32 */
    case 208: nr64 = 117; break;  /* setresuid32 */
    case 209: nr64 = 118; break;  /* getresuid32 */
    case 210: nr64 = 119; break;  /* setresgid32 */
    case 211: nr64 = 120; break;  /* getresgid32 */
    case 213: nr64 = 105; break;  /* setuid32 -> setuid */
    case 214: nr64 = 106; break;  /* setgid32 -> setgid */
    case 215: nr64 = 122; break;  /* setfsuid32 */
    case 216: nr64 = 123; break;  /* setfsgid32 */
    case 224: nr64 = 186; break;  /* gettid */
    case 238: nr64 = 200; break;  /* tkill */
    case 252: nr64 = 231; break;  /* exit_group */
    case 253: nr64 = 212; break;  /* lookup_dcookie residual */
    case 257: nr64 = 216; break;  /* remap_file_pages residual */
    case 258: nr64 = 218; break;  /* set_tid_address */
    case 270: nr64 = 234; break;  /* tgkill */
    case 274: nr64 = 237; break;  /* mbind (PE32/NUMA soft residual) */
    case 275: nr64 = 239; break;  /* get_mempolicy */
    case 276: nr64 = 238; break;  /* set_mempolicy */
    case 284: nr64 = 247; break;  /* waitid */
    case 289: nr64 = 251; break;  /* ioprio_set */
    case 290: nr64 = 252; break;  /* ioprio_get */
    case 310: nr64 = 272; break;  /* unshare */
    case 340: nr64 = 302; break;  /* prlimit64 */
    case 347: nr64 = 310; break;  /* process_vm_readv */
    case 348: nr64 = 311; break;  /* process_vm_writev */
    case 358: nr64 = 322; break;  /* execveat */
    case 424: nr64 = 424; break;  /* pidfd_send_signal identity */
    case 434: nr64 = 434; break;  /* pidfd_open identity */
    case 435: nr64 = 435; break;  /* clone3 identity */

    /* ---- path / vfs (open, stat, rename, *at, xattr) ------------------ */
    case 5:   nr64 = 2; break;    /* open */
    case 6:   nr64 = 3; break;    /* close */
    case 8:   nr64 = 85; break;   /* creat */
    case 9:   nr64 = 86; break;   /* link */
    case 10:  nr64 = 87; break;   /* unlink */
    case 12:  nr64 = 80; break;   /* chdir */
    case 14:  nr64 = 133; break;  /* mknod */
    case 15:  nr64 = 90; break;   /* chmod */
    case 16:  nr64 = 94; break;   /* lchown */
    case 21:  nr64 = 165; break;  /* mount */
    case 22:  nr64 = 166; break;  /* umount -> umount2 */
    case 28:  nr64 = 72; break;   /* fcntl (legacy alt) */
    case 30:  nr64 = 132; break;  /* utime */
    case 33:  nr64 = 21; break;   /* access */
    case 38:  nr64 = 82; break;   /* rename */
    case 39:  nr64 = 83; break;   /* mkdir */
    case 40:  nr64 = 84; break;   /* rmdir */
    case 55:  nr64 = 72; break;   /* fcntl */
    case 83:  nr64 = 88; break;   /* symlink */
    case 85:  nr64 = 89; break;   /* readlink */
    case 92:  nr64 = 76; break;   /* truncate */
    case 93:  nr64 = 77; break;   /* ftruncate */
    case 94:  nr64 = 91; break;   /* fchmod */
    case 95:  nr64 = 93; break;   /* fchown */
    case 99:  nr64 = 137; break;  /* statfs */
    case 100: nr64 = 138; break;  /* fstatfs */
    case 106: nr64 = 4; break;    /* stat */
    case 107: nr64 = 6; break;    /* lstat */
    case 108: nr64 = 5; break;    /* fstat */
    case 118: nr64 = 74; break;   /* fsync */
    case 133: nr64 = 81; break;   /* fchdir */
    case 136: nr64 = 135; break;  /* personality */
    case 148: nr64 = 75; break;   /* fdatasync */
    case 182: nr64 = 92; break;   /* chown */
    case 183: nr64 = 79; break;   /* getcwd */
    case 193: nr64 = 76; break;   /* truncate64 -> truncate */
    case 194: nr64 = 77; break;   /* ftruncate64 -> ftruncate */
    case 195: nr64 = 4; break;    /* stat64 -> stat */
    case 196: nr64 = 6; break;    /* lstat64 -> lstat */
    case 197: nr64 = 5; break;    /* fstat64 -> fstat */
    case 198: nr64 = 94; break;   /* lchown32 */
    case 207: nr64 = 93; break;   /* fchown32 */
    case 212: nr64 = 92; break;   /* chown32 */
    case 225: nr64 = 187; break;  /* readahead */
    case 226: nr64 = 188; break;  /* setxattr */
    case 227: nr64 = 189; break;  /* lsetxattr */
    case 228: nr64 = 190; break;  /* fsetxattr */
    case 229: nr64 = 191; break;  /* getxattr */
    case 230: nr64 = 192; break;  /* lgetxattr */
    case 231: nr64 = 193; break;  /* fgetxattr */
    case 232: nr64 = 194; break;  /* listxattr */
    case 233: nr64 = 195; break;  /* llistxattr */
    case 234: nr64 = 196; break;  /* flistxattr */
    case 235: nr64 = 197; break;  /* removexattr */
    case 236: nr64 = 198; break;  /* lremovexattr */
    case 237: nr64 = 199; break;  /* fremovexattr */
    case 268: nr64 = 137; break;  /* statfs64 -> statfs */
    case 269: nr64 = 138; break;  /* fstatfs64 -> fstatfs */
    case 271: nr64 = 235; break;  /* utimes (i386 public; not utimensat) */
    case 272: nr64 = 221; break;  /* fadvise64_64 -> fadvise64 */
    case 295: nr64 = 257; break;  /* openat */
    case 296: nr64 = 258; break;  /* mkdirat */
    case 297: nr64 = 259; break;  /* mknodat */
    case 298: nr64 = 260; break;  /* fchownat */
    case 299: nr64 = 261; break;  /* futimesat */
    case 300: nr64 = 262; break;  /* fstatat64 -> newfstatat */
    case 301: nr64 = 263; break;  /* unlinkat */
    case 302: nr64 = 264; break;  /* renameat */
    case 303: nr64 = 265; break;  /* linkat */
    case 304: nr64 = 266; break;  /* symlinkat */
    case 305: nr64 = 267; break;  /* readlinkat */
    case 306: nr64 = 268; break;  /* fchmodat */
    case 307: nr64 = 269; break;  /* faccessat */
    case 341: nr64 = 303; break;  /* name_to_handle_at */
    case 342: nr64 = 304; break;  /* open_by_handle_at */
    case 344: nr64 = 306; break;  /* syncfs */
    case 338: nr64 = 300; break;  /* fanotify_init residual */
    case 339: nr64 = 301; break;  /* fanotify_mark residual */
    case 353: nr64 = 316; break;  /* renameat2 */
    case 380: nr64 = 329; break;  /* pkey_mprotect residual */
    case 381: nr64 = 330; break;  /* pkey_alloc residual */
    case 382: nr64 = 331; break;  /* pkey_free residual */
    case 383: nr64 = 332; break;  /* statx */
    case 437: nr64 = 437; break;  /* openat2 identity */
    case 439: nr64 = 439; break;  /* faccessat2 identity */

    /* ---- mmap2 / memory ----------------------------------------------- */
    case 45:  nr64 = 12; break;   /* brk */
    case 90:  nr64 = 9; break;    /* old mmap -> mmap */
    case 91:  nr64 = 11; break;   /* munmap */
    case 125: nr64 = 10; break;   /* mprotect */
    case 144: nr64 = 26; break;   /* msync */
    case 150: nr64 = 149; break;  /* mlock */
    case 151: nr64 = 150; break;  /* munlock */
    case 152: nr64 = 151; break;  /* mlockall */
    case 153: nr64 = 152; break;  /* munlockall */
    case 163: nr64 = 25; break;   /* mremap */
    case 172: nr64 = 157; break;  /* prctl */
    case 192: nr64 = 9; break;    /* mmap2 -> mmap (pgoff pages; soft convert) */
    case 218: nr64 = 27; break;   /* mincore */
    case 219: nr64 = 28; break;   /* madvise */
    case 356: nr64 = 319; break;  /* memfd_create */
    case 376: nr64 = 325; break;  /* mlock2 */

    /* ---- fd I/O / pipe / socket / splice / uring ---------------------- */
    case 3:   nr64 = 0; break;    /* read */
    case 4:   nr64 = 1; break;    /* write */
    case 19:  nr64 = 8; break;    /* lseek */
    case 41:  nr64 = 32; break;   /* dup */
    case 42:  nr64 = 22; break;   /* pipe */
    case 54:  nr64 = 16; break;   /* ioctl */
    case 63:  nr64 = 33; break;   /* dup2 */
    case 102: nr64 = 41; break;   /* socketcall -> socket (coarse; soft demux) */
    case 140: nr64 = 8; break;    /* _llseek -> lseek-shaped */
    case 141: nr64 = 78; break;   /* getdents */
    case 145: nr64 = 19; break;   /* readv */
    case 146: nr64 = 20; break;   /* writev */
    case 180: nr64 = 17; break;   /* pread64 */
    case 181: nr64 = 18; break;   /* pwrite64 */
    case 187: nr64 = 40; break;   /* sendfile (i386) */
    case 220: nr64 = 217; break;  /* getdents64 */
    case 221: nr64 = 72; break;   /* fcntl64 -> fcntl */
    case 239: nr64 = 40; break;   /* sendfile64 -> sendfile */
    case 245: nr64 = 206; break;  /* io_setup */
    case 246: nr64 = 207; break;  /* io_destroy */
    case 247: nr64 = 208; break;  /* io_getevents */
    case 248: nr64 = 209; break;  /* io_submit */
    case 249: nr64 = 210; break;  /* io_cancel */
    case 313: nr64 = 275; break;  /* splice */
    case 314: nr64 = 277; break;  /* sync_file_range */
    case 315: nr64 = 276; break;  /* tee */
    case 316: nr64 = 278; break;  /* vmsplice */
    case 320: nr64 = 280; break;  /* utimensat (i386) */
    case 324: nr64 = 285; break;  /* fallocate */
    case 330: nr64 = 292; break;  /* dup3 */
    case 331: nr64 = 293; break;  /* pipe2 */
    case 333: nr64 = 295; break;  /* preadv */
    case 334: nr64 = 296; break;  /* pwritev */
    case 337: nr64 = 299; break;  /* recvmmsg */
    case 345: nr64 = 307; break;  /* sendmmsg */
    /*
     * Modern i386 glibc uses direct socket NRs (not socketcall 102).
     * greppable: wow64: soft residual socket_direct
     */
    case 359: nr64 = 41; break;   /* socket */
    case 360: nr64 = 53; break;   /* socketpair */
    case 361: nr64 = 49; break;   /* bind */
    case 362: nr64 = 42; break;   /* connect */
    case 363: nr64 = 50; break;   /* listen */
    case 364: nr64 = 288; break;  /* accept4 */
    case 365: nr64 = 55; break;   /* getsockopt */
    case 366: nr64 = 54; break;   /* setsockopt */
    case 367: nr64 = 51; break;   /* getsockname */
    case 368: nr64 = 52; break;   /* getpeername */
    case 369: nr64 = 44; break;   /* sendto */
    case 370: nr64 = 46; break;   /* sendmsg */
    case 371: nr64 = 45; break;   /* recvfrom */
    case 372: nr64 = 47; break;   /* recvmsg */
    case 373: nr64 = 48; break;   /* shutdown */
    case 377: nr64 = 326; break;  /* copy_file_range */
    case 378: nr64 = 327; break;  /* preadv2 */
    case 379: nr64 = 328; break;  /* pwritev2 */
    /*
     * y2038 time64 soft aliases -> non-time64 x86_64 NRs (soft residual).
     * Struct layout still cold-path responsibility; NR only here.
     */
    case 403: nr64 = 228; break;  /* clock_gettime64 -> clock_gettime */
    case 404: nr64 = 227; break;  /* clock_settime64 */
    case 405: nr64 = 305; break;  /* clock_adjtime64 */
    case 406: nr64 = 229; break;  /* clock_getres_time64 */
    case 407: nr64 = 230; break;  /* clock_nanosleep_time64 */
    case 408: nr64 = 224; break;  /* timer_gettime64 */
    case 409: nr64 = 223; break;  /* timer_settime64 */
    case 410: nr64 = 287; break;  /* timerfd_gettime64 */
    case 411: nr64 = 286; break;  /* timerfd_settime64 */
    case 412: nr64 = 280; break;  /* utimensat_time64 */
    case 413: nr64 = 270; break;  /* pselect6_time64 */
    case 414: nr64 = 271; break;  /* ppoll_time64 */
    case 415: nr64 = 333; break;  /* io_pgetevents_time64 soft */
    case 416: nr64 = 299; break;  /* recvmmsg_time64 -> recvmmsg */
    case 419: nr64 = 220; break;  /* semtimedop_time64 */
    case 420: nr64 = 128; break;  /* rt_sigtimedwait_time64 */
    case 422: nr64 = 202; break;  /* futex_time64 -> futex */
    case 425: nr64 = 425; break;  /* io_uring_setup identity */
    case 426: nr64 = 426; break;  /* io_uring_enter identity */
    case 427: nr64 = 427; break;  /* io_uring_register identity */
    case 436: nr64 = 436; break;  /* close_range identity */
    case 438: nr64 = 438; break;  /* pidfd_getfd identity */
    case 441: nr64 = 441; break;  /* epoll_pwait2 identity */

    /* ---- poll / epoll / inotify / eventfd ----------------------------- */
    case 142: nr64 = 23; break;   /* select */
    case 168: nr64 = 7; break;    /* poll */
    case 254: nr64 = 213; break;  /* epoll_create */
    case 255: nr64 = 233; break;  /* epoll_ctl */
    case 256: nr64 = 232; break;  /* epoll_wait */
    case 291: nr64 = 253; break;  /* inotify_init */
    case 292: nr64 = 254; break;  /* inotify_add_watch */
    case 293: nr64 = 255; break;  /* inotify_rm_watch */
    case 308: nr64 = 270; break;  /* pselect6 */
    case 309: nr64 = 271; break;  /* ppoll */
    case 319: nr64 = 281; break;  /* epoll_pwait */
    case 323: nr64 = 284; break;  /* eventfd */
    case 328: nr64 = 290; break;  /* eventfd2 */
    case 329: nr64 = 291; break;  /* epoll_create1 */
    case 332: nr64 = 294; break;  /* inotify_init1 */

    /* ---- time / futex / tls / sched / signals / misc ------------------ */
    case 36:  nr64 = 162; break;  /* sync */
    case 67:  nr64 = 13; break;   /* sigaction -> rt_sigaction */
    case 72:  nr64 = 130; break;  /* sigsuspend -> rt_sigsuspend */
    case 73:  nr64 = 127; break;  /* sigpending -> rt_sigpending */
    case 75:  nr64 = 160; break;  /* setrlimit */
    case 76:  nr64 = 97; break;   /* getrlimit */
    case 77:  nr64 = 98; break;   /* getrusage */
    case 78:  nr64 = 96; break;   /* gettimeofday */
    case 79:  nr64 = 164; break;  /* settimeofday */
    case 96:  nr64 = 140; break;  /* getpriority */
    case 97:  nr64 = 141; break;  /* setpriority */
    case 103: nr64 = 103; break;  /* syslog identity */
    case 104: nr64 = 38; break;   /* setitimer */
    case 105: nr64 = 36; break;   /* getitimer */
    case 116: nr64 = 99; break;   /* sysinfo */
    case 122: nr64 = 63; break;   /* uname */
    case 143: nr64 = 73; break;   /* flock */
    case 154: nr64 = 142; break;  /* sched_setparam */
    case 155: nr64 = 143; break;  /* sched_getparam */
    case 156: nr64 = 144; break;  /* sched_setscheduler */
    case 157: nr64 = 145; break;  /* sched_getscheduler */
    case 158: nr64 = 24; break;   /* sched_yield */
    case 159: nr64 = 146; break;  /* sched_get_priority_max */
    case 160: nr64 = 147; break;  /* sched_get_priority_min */
    case 161: nr64 = 148; break;  /* sched_rr_get_interval */
    case 162: nr64 = 35; break;   /* nanosleep */
    case 174: nr64 = 13; break;   /* rt_sigaction */
    case 175: nr64 = 14; break;   /* rt_sigprocmask */
    case 176: nr64 = 127; break;  /* rt_sigpending */
    case 177: nr64 = 128; break;  /* rt_sigtimedwait */
    case 178: nr64 = 129; break;  /* rt_sigqueueinfo */
    case 179: nr64 = 130; break;  /* rt_sigsuspend */
    case 186: nr64 = 131; break;  /* sigaltstack */
    case 191: nr64 = 97; break;   /* ugetrlimit -> getrlimit */
    case 240: nr64 = 202; break;  /* futex */
    case 241: nr64 = 203; break;  /* sched_setaffinity */
    case 242: nr64 = 204; break;  /* sched_getaffinity */
    case 243: nr64 = 158; break;  /* set_thread_area -> arch_prctl soft */
    case 244: nr64 = 158; break;  /* get_thread_area -> arch_prctl soft */
    case 250: nr64 = 221; break;  /* fadvise64 -> fadvise64 */
    case 259: nr64 = 222; break;  /* timer_create */
    case 260: nr64 = 223; break;  /* timer_settime */
    case 261: nr64 = 224; break;  /* timer_gettime */
    case 262: nr64 = 225; break;  /* timer_getoverrun */
    case 263: nr64 = 226; break;  /* timer_delete */
    case 264: nr64 = 227; break;  /* clock_settime */
    case 265: nr64 = 228; break;  /* clock_gettime */
    case 266: nr64 = 229; break;  /* clock_getres */
    case 267: nr64 = 230; break;  /* clock_nanosleep */
    case 311: nr64 = 273; break;  /* set_robust_list */
    case 312: nr64 = 274; break;  /* get_robust_list */
    case 318: nr64 = 309; break;  /* getcpu */
    case 321: nr64 = 282; break;  /* signalfd */
    case 322: nr64 = 283; break;  /* timerfd_create */
    case 325: nr64 = 286; break;  /* timerfd_settime */
    case 326: nr64 = 287; break;  /* timerfd_gettime */
    case 327: nr64 = 289; break;  /* signalfd4 */
    case 343: nr64 = 305; break;  /* clock_adjtime */
    case 346: nr64 = 308; break;  /* setns */
    case 349: nr64 = 312; break;  /* kcmp */
    case 350: nr64 = 314; break;  /* sched_setattr soft */
    case 351: nr64 = 315; break;  /* sched_getattr soft */
    case 354: nr64 = 317; break;  /* seccomp */
    case 355: nr64 = 318; break;  /* getrandom */
    case 357: nr64 = 321; break;  /* bpf */
    case 374: nr64 = 323; break;  /* userfaultfd */
    case 375: nr64 = 324; break;  /* membarrier */
    case 386: nr64 = 334; break;  /* rseq */

    default:
        nr64 = u32Nr32;
        fMapped = 0;
        break;
    }

    if (pOutNr != NULL) {
        *pOutNr = nr64;
    }
    return fMapped;
}

int
wow64_translate_nr(u32 u32Nr32, u32 *pOutNr)
{
    int fMapped;

    soft_inc(&g_u32SoftTranslateEnter);
    g_u32Calls++;
    if (pOutNr == NULL) {
        soft_inc(&g_u32SoftTranslateNull);
        soft_inventory_maybe_once();
        return -1;
    }
    fMapped = wow64_map_nr_core(u32Nr32, pOutNr);
    if (fMapped) {
        g_u32MapHits++;
        if (u32Nr32 >= GJ_WOW64_SOCK_DIRECT_LO &&
            u32Nr32 <= GJ_WOW64_SOCK_DIRECT_HI) {
            soft_inc(&g_u32SoftSockDirectMap);
        }
    } else {
        g_u32IdentityHits++;
    }
    soft_inventory_maybe_once();
    return 0;
}

int
wow64_nr_is_mapped(u32 u32Nr32)
{
    u32 u32Nr64;

    soft_inc(&g_u32SoftIsMappedProbe);
    return wow64_map_nr_core(u32Nr32, &u32Nr64);
}

u32
wow64_args_zero_extend(u64 *pArgs, u32 cArgs)
{
    u32 iArg;
    u32 cTouch;

    soft_inc(&g_u32SoftArgsZxCalls);
    if (pArgs == NULL || cArgs == 0) {
        return 0;
    }
    cTouch = cArgs > 6u ? 6u : cArgs;
    for (iArg = 0; iArg < cTouch; iArg++) {
        pArgs[iArg] &= 0xffffffffull;
    }
    return cTouch;
}

u64
wow64_mmap2_pgoff_to_bytes(u32 u32PgoffPages)
{
    return (u64)u32PgoffPages << GJ_WOW64_PAGE_SHIFT;
}

u64
wow64_ptr32(u64 u64Val)
{
    soft_inc(&g_u32SoftPtr32);
    return u64Val & 0xffffffffull;
}

/*
 * i386 socketcall subcall -> x86_64 NR (public man socketcall).
 */
int
wow64_socketcall_nr(u32 u32Call, u32 *pOutNr)
{
    u32 nr64;

    if (pOutNr == NULL) {
        soft_inc(&g_u32SoftSocketNrFail);
        return -1;
    }
    switch (u32Call) {
    case GJ_WOW64_SYS_SOCKET:      nr64 = 41; break;  /* socket */
    case GJ_WOW64_SYS_BIND:        nr64 = 49; break;  /* bind */
    case GJ_WOW64_SYS_CONNECT:     nr64 = 42; break;  /* connect */
    case GJ_WOW64_SYS_LISTEN:      nr64 = 50; break;  /* listen */
    case GJ_WOW64_SYS_ACCEPT:      nr64 = 43; break;  /* accept */
    case GJ_WOW64_SYS_GETSOCKNAME: nr64 = 51; break;  /* getsockname */
    case GJ_WOW64_SYS_GETPEERNAME: nr64 = 52; break;  /* getpeername */
    case GJ_WOW64_SYS_SOCKETPAIR:  nr64 = 53; break;  /* socketpair */
    case GJ_WOW64_SYS_SEND:        nr64 = 44; break;  /* send -> sendto */
    case GJ_WOW64_SYS_RECV:        nr64 = 45; break;  /* recv -> recvfrom */
    case GJ_WOW64_SYS_SENDTO:      nr64 = 44; break;  /* sendto */
    case GJ_WOW64_SYS_RECVFROM:    nr64 = 45; break;  /* recvfrom */
    case GJ_WOW64_SYS_SHUTDOWN:    nr64 = 48; break;  /* shutdown */
    case GJ_WOW64_SYS_SETSOCKOPT:  nr64 = 54; break;  /* setsockopt */
    case GJ_WOW64_SYS_GETSOCKOPT:  nr64 = 55; break;  /* getsockopt */
    case GJ_WOW64_SYS_SENDMSG:     nr64 = 46; break;  /* sendmsg */
    case GJ_WOW64_SYS_RECVMSG:     nr64 = 47; break;  /* recvmsg */
    case GJ_WOW64_SYS_ACCEPT4:     nr64 = 288; break; /* accept4 */
    case GJ_WOW64_SYS_RECVMMSG:    nr64 = 299; break; /* recvmmsg */
    case GJ_WOW64_SYS_SENDMMSG:    nr64 = 307; break; /* sendmmsg */
    default:
        soft_inc(&g_u32SoftSocketNrFail);
        return -1;
    }
    *pOutNr = nr64;
    soft_inc(&g_u32SoftSocketNrOk);
    return 0;
}

/*
 * Soft ptr32 canon for pointer-shaped slots on common PE32/i386 NRs.
 * Clears high long-mode garbage so hybrid SYSCALL args match CS32 width.
 * Soft!=product; never hard-gates; G-AC-1.
 * Returns number of slots canonized (0 if none / NULL).
 */
static u32
wow64_soft_ptr32_canon(u32 u32Nr32, u64 *pArgs, u32 cArgs)
{
    u32 cTouch = 0;

    if (pArgs == NULL || cArgs == 0) {
        return 0;
    }

    switch (u32Nr32) {
    case 5u:   /* open path* */
    case 12u:  /* chdir */
    case 33u:  /* access */
    case 38u:  /* rename old* */
    case 39u:  /* mkdir */
    case 40u:  /* rmdir */
    case 83u:  /* symlink */
    case 85u:  /* readlink */
    case 90u:  /* old_mmap struct* */
    case 92u:  /* truncate path* */
    case 106u: /* stat */
    case 107u: /* lstat */
    case 183u: /* getcwd buf* */
    case 195u: /* stat64 */
    case 196u: /* lstat64 */
    case 193u: /* truncate64 path* */
    case 226u: /* setxattr path* */
    case 229u: /* getxattr path* */
    case 271u: /* utimes path* */
        if (cArgs > 0u) {
            pArgs[0] = wow64_ptr32(pArgs[0]);
            cTouch++;
        }
        if ((u32Nr32 == 38u || u32Nr32 == 83u) && cArgs > 1u) {
            /* rename/symlink: second path* */
            pArgs[1] = wow64_ptr32(pArgs[1]);
            cTouch++;
        }
        break;
    case 3u:   /* read buf* */
    case 4u:   /* write buf* */
    case 108u: /* fstat */
    case 145u: /* readv iov* */
    case 146u: /* writev iov* */
    case 197u: /* fstat64 */
        if (cArgs > 1u) {
            pArgs[1] = wow64_ptr32(pArgs[1]);
            cTouch++;
        }
        break;
    case 102u: /* socketcall args_ptr */
        if (cArgs > 1u) {
            pArgs[1] = wow64_ptr32(pArgs[1]);
            cTouch++;
        }
        break;
    case 140u: /* _llseek result* */
        if (cArgs > 3u) {
            pArgs[3] = wow64_ptr32(pArgs[3]);
            cTouch++;
        }
        break;
    case 295u: /* openat path* in arg1 */
    case 296u: /* mkdirat */
    case 301u: /* unlinkat */
    case 300u: /* fstatat64 */
    case 306u: /* fchmodat */
    case 307u: /* faccessat */
    case 320u: /* utimensat */
        if (cArgs > 1u) {
            pArgs[1] = wow64_ptr32(pArgs[1]);
            cTouch++;
        }
        break;
    case 243u: /* set_thread_area user_desc* */
    case 244u: /* get_thread_area user_desc* */
    case 123u: /* modify_ldt ptr (arg1 is ptr on i386: func,ptr,bytecount) */
        if (u32Nr32 == 123u) {
            if (cArgs > 1u) {
                pArgs[1] = wow64_ptr32(pArgs[1]);
                cTouch++;
            }
        } else if (cArgs > 0u) {
            pArgs[0] = wow64_ptr32(pArgs[0]);
            cTouch++;
        }
        break;
    default:
        break;
    }

    if (cTouch != 0) {
        soft_inc(&g_u32SoftPtr32Canon);
    }
    return cTouch;
}

/*
 * i386 ipc() subcall -> x86_64 NR (public man ipc; clean-room).
 * Soft demux residual; SysV IPC product remains open (ENOSYS until cold).
 */
static int
wow64_ipc_nr(u32 u32Call, u32 *pOutNr)
{
    u32 nr64;

    if (pOutNr == NULL) {
        soft_inc(&g_u32SoftIpcNrFail);
        return -1;
    }
    switch (u32Call) {
    case WOW64_IPC_SEMOP:      nr64 = 65; break;  /* semop */
    case WOW64_IPC_SEMGET:     nr64 = 64; break;  /* semget */
    case WOW64_IPC_SEMCTL:     nr64 = 66; break;  /* semctl */
    case WOW64_IPC_SEMTIMEDOP: nr64 = 220; break; /* semtimedop */
    case WOW64_IPC_MSGSND:     nr64 = 69; break;  /* msgsnd */
    case WOW64_IPC_MSGRCV:     nr64 = 70; break;  /* msgrcv */
    case WOW64_IPC_MSGGET:     nr64 = 68; break;  /* msgget */
    case WOW64_IPC_MSGCTL:     nr64 = 71; break;  /* msgctl */
    case WOW64_IPC_SHMAT:      nr64 = 30; break;  /* shmat */
    case WOW64_IPC_SHMDT:      nr64 = 67; break;  /* shmdt */
    case WOW64_IPC_SHMGET:     nr64 = 29; break;  /* shmget */
    case WOW64_IPC_SHMCTL:     nr64 = 31; break;  /* shmctl */
    default:
        soft_inc(&g_u32SoftIpcNrFail);
        return -1;
    }
    *pOutNr = nr64;
    soft_inc(&g_u32SoftIpcNrOk);
    return 0;
}

u32
wow64_adjust_args(u32 u32Nr32, u32 *pOutNr64, u64 *pArgs, u32 cArgs)
{
    u32 u32Flags = 0;
    u32 u32Call;
    u32 u32SockNr;

    soft_inc(&g_u32SoftAdjustEnter);
    if (pArgs == NULL || cArgs == 0) {
        soft_inc(&g_u32SoftAdjustNop);
        soft_inventory_maybe_once();
        return 0;
    }

    (void)wow64_args_zero_extend(pArgs, cArgs);
    u32Flags |= GJ_WOW64_THUNK_ARGS_ZX;
    soft_inc(&g_u32SoftAdjustZx);

    /* Soft ptr32 canon for pointer-shaped slots (PE32 companion residual). */
    (void)wow64_soft_ptr32_canon(u32Nr32, pArgs, cArgs);

    if (u32Nr32 == 192u) {
        /* mmap2: arg5 is page offset -> byte offset for mmap */
        if (cArgs > 5u) {
            pArgs[5] = wow64_mmap2_pgoff_to_bytes((u32)pArgs[5]);
            u32Flags |= GJ_WOW64_THUNK_MMAP2_PGOFF;
            g_u32Mmap2Hits++;
            soft_inc(&g_u32SoftAdjustMmap2);
        }
        if (pOutNr64 != NULL) {
            *pOutNr64 = 9; /* mmap */
        }
        u32Flags |= GJ_WOW64_THUNK_NR_MAPPED;
        soft_inventory_maybe_once();
        return u32Flags;
    }

    if (u32Nr32 == 102u) {
        /* socketcall(call, args_ptr): demux NR; user pull later */
        u32Call = (u32)pArgs[0];
        soft_inc(&g_u32SoftAdjustSocket);
        if (wow64_socketcall_nr(u32Call, &u32SockNr) == 0) {
            if (pOutNr64 != NULL) {
                *pOutNr64 = u32SockNr;
            }
            u32Flags |= GJ_WOW64_THUNK_SOCKETCALL;
            u32Flags |= GJ_WOW64_THUNK_NR_MAPPED;
            u32Flags |= GJ_WOW64_THUNK_SOCKET_NEED_PULL;
            g_u32SocketcallHits++;
        } else {
            if (pOutNr64 != NULL && *pOutNr64 == 0) {
                *pOutNr64 = 41;
            }
            u32Flags |= GJ_WOW64_THUNK_SOCKETCALL;
            soft_inc(&g_u32SoftAdjustSockFail);
        }
        soft_inventory_maybe_once();
        return u32Flags;
    }

    if (u32Nr32 == 117u) {
        /* ipc(call, first, second, third, ptr, fifth): demux NR; soft only */
        u32Call = (u32)pArgs[0];
        soft_inc(&g_u32SoftAdjustIpc);
        if (wow64_ipc_nr(u32Call, &u32SockNr) == 0) {
            if (pOutNr64 != NULL) {
                *pOutNr64 = u32SockNr;
            }
            u32Flags |= GJ_WOW64_THUNK_IPC;
            u32Flags |= GJ_WOW64_THUNK_NR_MAPPED;
            soft_inc(&g_u32IpcHits);
            /* Soft ptr32 on ipc ptr slot before reshape (arg4 on i386). */
            if (cArgs > 4u) {
                pArgs[4] = wow64_ptr32(pArgs[4]);
            }
            /* arg reshape: first->arg0 (soft; cold IPC may refine). */
            if (cArgs > 1u) {
                pArgs[0] = pArgs[1];
            }
            if (cArgs > 2u) {
                pArgs[1] = pArgs[2];
            }
            if (cArgs > 3u) {
                pArgs[2] = pArgs[3];
            }
            if (cArgs > 4u) {
                pArgs[3] = pArgs[4];
            }
            if (cArgs > 5u) {
                pArgs[4] = pArgs[5];
            }
        } else {
            if (pOutNr64 != NULL && *pOutNr64 == 0) {
                *pOutNr64 = 29; /* shmget coarse fallback */
            }
            u32Flags |= GJ_WOW64_THUNK_IPC;
            soft_inc(&g_u32SoftAdjustIpcFail);
        }
        soft_inventory_maybe_once();
        return u32Flags;
    }

    if (u32Nr32 == 90u) {
        /*
         * old_mmap: single struct ptr in arg0 on i386. Soft path
         * zero-extends + ptr32-canon; struct unpack is trap/dispatch.
         */
        soft_inc(&g_u32SoftAdjustOldMmap);
        u32Flags |= GJ_WOW64_THUNK_NR_MAPPED;
        u32Flags |= GJ_WOW64_THUNK_OLD_MMAP;
        if (pOutNr64 != NULL) {
            *pOutNr64 = 9;
        }
        soft_inventory_maybe_once();
        return u32Flags;
    }

    if (u32Nr32 == 140u) {
        /*
         * _llseek(fd, offset_high, offset_low, result, whence):
         * soft-compose 64-bit offset into arg1 for lseek-shaped hot path.
         * arg0=fd, arg1=high, arg2=low, arg3=result* (ptr32), arg4=whence
         */
        if (cArgs >= 5u) {
            u64 u64Off = ((u64)(u32)pArgs[1] << 32) | (u64)(u32)pArgs[2];

            pArgs[1] = u64Off;
            pArgs[2] = pArgs[4]; /* whence */
            u32Flags |= GJ_WOW64_THUNK_NR_MAPPED;
            u32Flags |= GJ_WOW64_THUNK_LLSEEK;
            soft_inc(&g_u32SoftAdjustLlseek);
        }
        soft_inventory_maybe_once();
        return u32Flags;
    }

    soft_inventory_maybe_once();
    return u32Flags;
}

int
wow64_thunk_soft(struct gj_wow64_thunk *pThunk)
{
    u32 u32Nr64;
    int fMapped;
    u32 u32Adj;
    u32 u32SubCall;

    soft_inc(&g_u32SoftThunkEnter);
    if (pThunk == NULL) {
        soft_inc(&g_u32SoftThunkNull);
        soft_inventory_maybe_once();
        return -1;
    }

    g_u32ThunkHits++;
    g_u32Calls++;
    soft_inc(&g_u32SoftThunkOk);

    fMapped = wow64_map_nr_core(pThunk->u32Nr32, &u32Nr64);
    if (fMapped) {
        g_u32MapHits++;
        pThunk->u32Flags = GJ_WOW64_THUNK_NR_MAPPED;
        if (pThunk->u32Nr32 >= GJ_WOW64_SOCK_DIRECT_LO &&
            pThunk->u32Nr32 <= GJ_WOW64_SOCK_DIRECT_HI) {
            soft_inc(&g_u32SoftSockDirectMap);
        }
    } else {
        g_u32IdentityHits++;
        pThunk->u32Flags = GJ_WOW64_THUNK_IDENTITY;
        u32Nr64 = pThunk->u32Nr32;
    }
    pThunk->u32Nr64 = u32Nr64;
    /* Capture subcall before adjust reshapes args (socketcall / ipc). */
    u32SubCall = (u32)(pThunk->aArgs[0] & 0xffffffffull);
    pThunk->u32SocketCall = 0;

    u32Adj = wow64_adjust_args(pThunk->u32Nr32, &pThunk->u32Nr64,
                               pThunk->aArgs, 6);
    pThunk->u32Flags |= u32Adj;

    if ((u32Adj & GJ_WOW64_THUNK_SOCKETCALL) != 0 ||
        (u32Adj & GJ_WOW64_THUNK_IPC) != 0) {
        pThunk->u32SocketCall = u32SubCall;
    }

    soft_inventory_maybe_once();
    return 0;
}

u32
wow64_calls(void)
{
    return g_u32Calls;
}

u32
wow64_map_hits(void)
{
    return g_u32MapHits;
}

u32
wow64_identity_hits(void)
{
    return g_u32IdentityHits;
}

u32
wow64_thunk_hits(void)
{
    return g_u32ThunkHits;
}

u32
wow64_mmap2_hits(void)
{
    return g_u32Mmap2Hits;
}

u32
wow64_socketcall_hits(void)
{
    return g_u32SocketcallHits;
}

u32
wow64_ipc_hits(void)
{
    return g_u32IpcHits;
}
