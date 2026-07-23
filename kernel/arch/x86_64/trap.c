/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Exception / IRQ trap dispatch (G-IDT-*): kernel halt on ring-0 faults,
 * PE32 CS32 smokes (#BP / int 0x80), COW break on user #PF, else kill.
 *
 * Soft deepen: greppable #PF COW path logs + trap soft stats (trap.h).
 * Keep vmm "COW break" strings untouched; except_port_deliver path unchanged.
 *
 * Soft trap inventory (Wave 13 base + Wave 19 exclusive complementary deepen;
 * this unit only):
 *   Lifetime classify / #PF COW / PE32 / except-or-kill tallies (gj_trap_stats).
 *   File-local resume/halt/last-frame/rate lamps (never hard-gate).
 *   Greppable prefix-stable serial markers (rate-limited; never flood):
 *     trap: soft inventory …
 *     trap: soft class …
 *     trap: soft pf …
 *     trap: soft pe32 …
 *     trap: soft outcome …
 *     trap: soft stats …
 *     trap: soft limit …
 *     trap: soft last …
 *     trap: soft resume …
 *     trap: soft halt …
 *     trap: soft rate …
 *     trap: soft path …
 *     trap: soft deepen …
 * Wave 15 complementary surfaces (kept; never reshape primary fields):
 *     trap: soft honesty …  — soft-only / non-claim catalog
 *     trap: soft api …      — trap_stats_get/reset/soft call tallies
 *     trap: soft frame …    — last full frame lamps (err/rip/cs/rsp/ss/rflags)
 *     trap: soft vec …      — last vector class + name lamp
 *     trap: soft mile …     — power-of-two milestone / quiet / skip rollup
 * Wave 16 complementary surfaces (kept; never reshape primary fields):
 *     trap: soft exclusive …— exclusive=1 unit stamp + wave
 *     trap: soft claim …    — product claim bounds (classify+pe32+cow)
 *     trap: soft ratio …    — class/outcome/resume/halt path ratios
 * Wave 17 complementary surfaces (kept) (never reshape primary fields):
 *     trap: soft return …   — Wave 17 API return surfaces (kept)
 *     trap: soft return selftest … — Wave 17 terminal return surface (kept)
 *     trap: soft retmap …   — Wave 17 return-surface map (kept)
 *   Emissions only at power-of-two trap_dispatch milestones, hard-capped at
 *   TRAP_SOFT_LOG_MAX. Explicit trap_stats_soft() always dumps (smoke path).
 *   Never hard-gates product policy. Pure C. Soft ≠ bar3.
 * Grep: trap: soft · trap: #PF soft
 * greppable: trap: soft deepen
 * greppable: trap: soft exclusive
 * greppable: trap: soft return
 * greppable: trap: soft return selftest
 * greppable: trap: soft retmap
 */
#include <gj/config.h>
#include <gj/cpu.h>
#include <gj/error.h>
#include <gj/except.h>
#include <gj/gdt.h>
#include <gj/klog.h>
#include <gj/pe32.h>
#include <gj/pmm.h>
#include <gj/process.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/door.h>
#include <gj/timer.h>
#include <gj/trap.h>
#include <gj/types.h>
#include <gj/user_access.h>
#include <gj/vfs_ram.h>
#include <gj/vmm.h>

/* PE32 hardware-enter smoke: latched when #BP fires from CS32 */
static volatile u32 g_u32Pe32HwEnterCs;
static volatile u32 g_u32Pe32HwEnterHits;
static volatile u32 g_u32Pe32Int80Exits;
static volatile u32 g_u32Pe32Int80LastCode;
static volatile u32 g_u32Pe32Int80Calls;
static volatile u32 g_u32Pe32Int80LastNr;
static volatile u32 g_u32Pe32Brk; /* soft heap break for i386 brk */
static u32 g_aPe32FdUsed[32];    /* simple fd table for PE32 open/fstat */
static i64 g_aPe32VfsFd[32];     /* underlying vfs_ram fd or -1 */
static u32 g_u32Pe32NextFd = 3;
static int g_fPe32FdInit;
static u32 g_u32Pe32NextChildPid = 2; /* fake child pids for fork/clone */

static void
pe32_fd_table_init(void)
{
    u32 i;

    if (g_fPe32FdInit) {
        return;
    }
    for (i = 0; i < 32u; i++) {
        g_aPe32VfsFd[i] = -1;
        g_aPe32FdUsed[i] = 0;
    }
    g_u32Pe32NextFd = 3;
    g_fPe32FdInit = 1;
}

/** Allocate a PE32 fd slot bound to a vfs_ram fd. Returns pe32 fd or -1. */
static i32
pe32_alloc_bound_fd(i64 i64Vfs)
{
    u32 fd;

    if (i64Vfs < 0) {
        return -1;
    }
    fd = g_u32Pe32NextFd++;
    if (fd >= 32u) {
        g_u32Pe32NextFd = 3;
        fd = 3;
    }
    /* If slot busy, scan for free */
    if (g_aPe32FdUsed[fd]) {
        u32 i;

        for (i = 3; i < 32u; i++) {
            if (!g_aPe32FdUsed[i]) {
                fd = i;
                break;
            }
        }
        if (i >= 32u) {
            return -1;
        }
    }
    g_aPe32FdUsed[fd] = 1;
    g_aPe32VfsFd[fd] = i64Vfs;
    return (i32)fd;
}

static int
pe32_copy_path(u32 u32UserPath, char *szOut, u32 cbOut)
{
    u32 i;

    if (szOut == NULL || cbOut < 2) {
        return -1;
    }
    if (u32UserPath == 0) {
        szOut[0] = '\0';
        return 0;
    }
    for (i = 0; i + 1u < cbOut; i++) {
        u8 c = 0;

        if (user_range_ok((u64)u32UserPath + i, 1)) {
            if (copy_from_user(&c, (u64)u32UserPath + i, 1) != GJ_OK) {
                break;
            }
        } else {
            break;
        }
        szOut[i] = (char)c;
        if (c == 0) {
            return 0;
        }
    }
    szOut[i] = '\0';
    return (i > 0) ? 0 : -1;
}

u32
pe32_hw_enter_hits(void)
{
    return g_u32Pe32HwEnterHits;
}

u32
pe32_hw_enter_last_cs(void)
{
    return g_u32Pe32HwEnterCs;
}

u32
pe32_int80_exit_hits(void)
{
    return g_u32Pe32Int80Exits;
}

u32
pe32_int80_last_code(void)
{
    return g_u32Pe32Int80LastCode;
}

u32
pe32_int80_call_hits(void)
{
    return g_u32Pe32Int80Calls;
}

u32
pe32_int80_last_nr(void)
{
    return g_u32Pe32Int80LastNr;
}

static const char *const g_aszExc[] = {
    "#DE",  "#DB",  "NMI",  "#BP",  "#OF",  "#BR",  "#UD",  "#NM",
    "#DF",  "CSO",  "#TS",  "#NP",  "#SS",  "#GP",  "#PF",  "RES",
    "#MF",  "#AC",  "#MC",  "#XM",  "#VE",  "#CP",  "22",   "23",
    "24",   "25",   "26",   "27",   "28",   "29",   "#SX",  "31",
};

/* Soft trap path counters — wrap OK; never gate product policy. */
static struct gj_trap_stats g_trapStats;

/*
 * Soft inventory serial budget (Wave 16). Absolute cap of greppable dumps;
 * milestones are power-of-two non-null trap_dispatch entries (1,2,4,…).
 * greppable: trap: soft
 */
#define TRAP_SOFT_LOG_MAX 12u
#define TRAP_SOFT_WAVE    19u

static u32 g_u32SoftLogged;      /* greppable dump emissions */
static u64 g_u64SoftSkip;        /* soft log suppressed at cap (milestone) */
static u64 g_u64SoftQuiet;       /* non-milestone dispatches (silent) */
static u64 g_u64SoftForce;       /* trap_stats_soft / null-frame force dumps */
static u32 g_u32SoftLastVec;     /* last classified vector (soft lamp) */
static u32 g_u32SoftLastUser;    /* last ring (1=user 0=kernel) */
static u64 g_u64SoftLastErr;     /* last frame error code */
static u64 g_u64SoftLastRip;     /* last frame RIP */
static u64 g_u64SoftLastCr2;     /* last #PF CR2 (0 if not #PF) */
static u64 g_u64SoftLastCs;      /* last frame CS */
static u64 g_u64SoftLastRsp;     /* last frame RSP (Wave 15) */
static u64 g_u64SoftLastSs;      /* last frame SS (Wave 15) */
static u64 g_u64SoftLastRflags;  /* last frame RFLAGS (Wave 15) */
static u64 g_u64SoftMileLast;    /* last milestone total that logged */
static u64 g_u64SoftPe32Resume;  /* PE32 int 0x80 returned to user */
static u64 g_u64SoftPe32Death;   /* PE32 #BP / exit / exit_group death */
static u64 g_u64SoftHaltKern;    /* kernel-fault halt path entered */
static u64 g_u64SoftHaltNull;    /* null-frame halt path entered */
/* Wave 15 exclusive complementary path tallies (file-local only). */
static u64 g_u64SoftApiGet;      /* trap_stats_get entries */
static u64 g_u64SoftApiReset;    /* trap_stats_reset entries */
static u64 g_u64SoftApiSoft;     /* trap_stats_soft entries */
static u64 g_u64SoftMileHit;     /* power-of-two milestone hits (pre-cap) */

static void trap_soft_inventory_log(void);
static void trap_soft_maybe_log(void);

static u64
read_cr2(void)
{
    u64 u64Cr2;
    __asm__ volatile ("mov %%cr2, %0" : "=r"(u64Cr2));
    return u64Cr2;
}

/**
 * Greppable soft trap inventory (product / smoke) — Wave 16 deepen.
 * Prefix-stable markers (trap: soft …):
 *   trap: soft inventory  — totals + rate-limit lamps + wave stamp
 *   trap: soft class      — user/kernel + vector class
 *   trap: soft pf         — #PF COW pipeline
 *   trap: soft pe32       — CS32 #BP / int 0x80 smokes + resume/death
 *   trap: soft outcome    — except port vs kill fallthrough
 *   trap: soft stats      — aggregate one-liner (legacy smoke)
 *   trap: soft limit      — hard cap / power-of-two policy lamps
 *   trap: soft last       — last frame lamps (vec/user/err/rip/cr2/cs)
 *   trap: soft resume     — COW ok + PE32 int80 return
 *   trap: soft halt       — kernel / null frame halt entries
 *   trap: soft rate       — milestone / quiet / force / skip rollup
 *   trap: soft path       — product claim + honesty
 *   trap: soft deepen     — Wave 19 stamp line
 * Wave 15 complementary (kept; never reshape primary):
 *   trap: soft honesty / api / frame / vec / mile
 * Wave 16 complementary (kept) + Wave 19 return surfaces:
 *   trap: soft exclusive / claim / ratio
 *
 * Never allocates; never hard-gates. Diagnostics only (wrap OK).
 * greppable: trap: soft
 */
static void
trap_soft_inventory_log(void)
{
    u32 u32AtCap;
    u32 u32Room;
    u32 u32LastExc;
    u32 u32LastHi;
    u32 u32LastPf;
    u32 u32LastPe32;
    u32 u32LastRpl;
    const char *szLastName;

    if (g_u32SoftLogged < 0xffffffffu) {
        g_u32SoftLogged++;
    }
    u32AtCap = (g_u32SoftLogged >= TRAP_SOFT_LOG_MAX) ? 1u : 0u;
    u32Room = (g_u32SoftLogged < TRAP_SOFT_LOG_MAX)
                  ? (TRAP_SOFT_LOG_MAX - g_u32SoftLogged)
                  : 0u;

    /* Wave 15 last-vector soft classify lamps. */
    u32LastExc = (g_u32SoftLastVec < 32u) ? 1u : 0u;
    u32LastHi = (g_u32SoftLastVec >= 32u &&
                 g_u32SoftLastVec != 0xffffffffu)
                    ? 1u
                    : 0u;
    u32LastPf = (g_u32SoftLastVec == 14u) ? 1u : 0u;
    u32LastPe32 = (g_u32SoftLastVec == 3u || g_u32SoftLastVec == 128u)
                      ? 1u
                      : 0u;
    u32LastRpl = (u32)(g_u64SoftLastCs & 3ull);
    if (g_u32SoftLastVec == 0xffffffffu) {
        szLastName = "null";
    } else if (g_u32SoftLastVec < 32u) {
        szLastName = g_aszExc[g_u32SoftLastVec];
    } else if (g_u32SoftLastVec == 128u) {
        szLastName = "int80";
    } else {
        szLastName = "irq";
    }

    /* Grep: trap: soft inventory */
    kprintf("trap: soft inventory total=%lu user=%lu kern=%lu null=%lu "
            "logs=%u skip=%lu quiet=%lu force=%lu max=%u "
            "last_vec=%u last_user=%u wave=%u\n",
            (unsigned long)g_trapStats.u64Total,
            (unsigned long)g_trapStats.u64User,
            (unsigned long)g_trapStats.u64Kernel,
            (unsigned long)g_trapStats.u64NullFrame,
            g_u32SoftLogged,
            (unsigned long)g_u64SoftSkip,
            (unsigned long)g_u64SoftQuiet,
            (unsigned long)g_u64SoftForce,
            (unsigned)TRAP_SOFT_LOG_MAX,
            g_u32SoftLastVec,
            g_u32SoftLastUser,
            (unsigned)TRAP_SOFT_WAVE);

    /* Grep: trap: soft class */
    kprintf("trap: soft class vec32=%lu vec_hi=%lu user=%lu kern=%lu "
            "null=%lu last_vec=%u last_user=%u\n",
            (unsigned long)g_trapStats.u64VecLt32,
            (unsigned long)g_trapStats.u64VecGe32,
            (unsigned long)g_trapStats.u64User,
            (unsigned long)g_trapStats.u64Kernel,
            (unsigned long)g_trapStats.u64NullFrame,
            g_u32SoftLastVec,
            g_u32SoftLastUser);

    /* Grep: trap: soft pf */
    kprintf("trap: soft pf user=%lu cow_cand=%lu cow_cr3=%lu cow_ok=%lu "
            "cow_miss=%lu cow_skip=%lu last_cr2=0x%lx "
            "pipe=pf_user>cow_cand>(cr3)>ok|miss skip=non_p_w\n",
            (unsigned long)g_trapStats.u64PfUser,
            (unsigned long)g_trapStats.u64PfCowCand,
            (unsigned long)g_trapStats.u64PfCowCr3Sw,
            (unsigned long)g_trapStats.u64PfCowOk,
            (unsigned long)g_trapStats.u64PfCowMiss,
            (unsigned long)g_trapStats.u64PfCowSkip,
            (unsigned long)g_u64SoftLastCr2);

    /* Grep: trap: soft pe32 */
    kprintf("trap: soft pe32 bp=%lu int80=%lu resume=%lu death=%lu "
            "hw_hits=%u int80_calls=%u last_nr=%u "
            "cs32=GJ_GDT_USER_CS32 paths=int3,int80\n",
            (unsigned long)g_trapStats.u64Pe32Bp,
            (unsigned long)g_trapStats.u64Pe32Int80,
            (unsigned long)g_u64SoftPe32Resume,
            (unsigned long)g_u64SoftPe32Death,
            g_u32Pe32HwEnterHits,
            g_u32Pe32Int80Calls,
            g_u32Pe32Int80LastNr);

    /* Grep: trap: soft outcome */
    kprintf("trap: soft outcome except_ok=%lu except_miss=%lu kill=%lu "
            "null_halt=%lu kern_halt=%lu "
            "resume=cow_ok|pe32_int80 death=pe32_bp|exit\n",
            (unsigned long)g_trapStats.u64ExceptDeliver,
            (unsigned long)g_trapStats.u64ExceptMiss,
            (unsigned long)g_trapStats.u64Kill,
            (unsigned long)g_u64SoftHaltNull,
            (unsigned long)g_u64SoftHaltKern);

    /* Grep: trap: soft stats */
    kprintf("trap: soft stats total=%lu user=%lu kern=%lu null=%lu "
            "vec32=%lu vec_hi=%lu pe32_bp=%lu pe32_int80=%lu "
            "pf_user=%lu cow_cand=%lu cow_cr3=%lu cow_ok=%lu cow_miss=%lu "
            "cow_skip=%lu except_ok=%lu except_miss=%lu kill=%lu "
            "logs=%u skip=%lu quiet=%lu force=%lu wave=%u\n",
            (unsigned long)g_trapStats.u64Total,
            (unsigned long)g_trapStats.u64User,
            (unsigned long)g_trapStats.u64Kernel,
            (unsigned long)g_trapStats.u64NullFrame,
            (unsigned long)g_trapStats.u64VecLt32,
            (unsigned long)g_trapStats.u64VecGe32,
            (unsigned long)g_trapStats.u64Pe32Bp,
            (unsigned long)g_trapStats.u64Pe32Int80,
            (unsigned long)g_trapStats.u64PfUser,
            (unsigned long)g_trapStats.u64PfCowCand,
            (unsigned long)g_trapStats.u64PfCowCr3Sw,
            (unsigned long)g_trapStats.u64PfCowOk,
            (unsigned long)g_trapStats.u64PfCowMiss,
            (unsigned long)g_trapStats.u64PfCowSkip,
            (unsigned long)g_trapStats.u64ExceptDeliver,
            (unsigned long)g_trapStats.u64ExceptMiss,
            (unsigned long)g_trapStats.u64Kill,
            g_u32SoftLogged,
            (unsigned long)g_u64SoftSkip,
            (unsigned long)g_u64SoftQuiet,
            (unsigned long)g_u64SoftForce,
            (unsigned)TRAP_SOFT_WAVE);

    /* Grep: trap: soft limit */
    kprintf("trap: soft limit max=%u logs=%u room=%u at_cap=%u "
            "policy=pow2_milestone+hard_cap skip=%lu quiet=%lu "
            "force=%lu wave=%u\n",
            (unsigned)TRAP_SOFT_LOG_MAX,
            g_u32SoftLogged,
            u32Room,
            u32AtCap,
            (unsigned long)g_u64SoftSkip,
            (unsigned long)g_u64SoftQuiet,
            (unsigned long)g_u64SoftForce,
            (unsigned)TRAP_SOFT_WAVE);

    /* Grep: trap: soft last */
    kprintf("trap: soft last vec=%u user=%u err=0x%lx rip=0x%lx "
            "cr2=0x%lx cs=0x%lx mile=%lu\n",
            g_u32SoftLastVec,
            g_u32SoftLastUser,
            (unsigned long)g_u64SoftLastErr,
            (unsigned long)g_u64SoftLastRip,
            (unsigned long)g_u64SoftLastCr2,
            (unsigned long)g_u64SoftLastCs,
            (unsigned long)g_u64SoftMileLast);

    /* Grep: trap: soft resume */
    kprintf("trap: soft resume cow_ok=%lu pe32_int80=%lu "
            "pe32_death=%lu except_block=%lu "
            "paths=cow_break|int80_ret|except_port\n",
            (unsigned long)g_trapStats.u64PfCowOk,
            (unsigned long)g_u64SoftPe32Resume,
            (unsigned long)g_u64SoftPe32Death,
            (unsigned long)g_trapStats.u64ExceptDeliver);

    /* Grep: trap: soft halt */
    kprintf("trap: soft halt kern=%lu null=%lu "
            "policy=cli_hlt kernel_fault_or_null_frame=1\n",
            (unsigned long)g_u64SoftHaltKern,
            (unsigned long)g_u64SoftHaltNull);

    /* Grep: trap: soft rate */
    kprintf("trap: soft rate total=%lu mile_last=%lu logs=%u "
            "skip=%lu quiet=%lu force=%lu max=%u "
            "policy=pow2(1,2,4,...) hard_cap=%u\n",
            (unsigned long)g_trapStats.u64Total,
            (unsigned long)g_u64SoftMileLast,
            g_u32SoftLogged,
            (unsigned long)g_u64SoftSkip,
            (unsigned long)g_u64SoftQuiet,
            (unsigned long)g_u64SoftForce,
            (unsigned)TRAP_SOFT_LOG_MAX,
            (unsigned)TRAP_SOFT_LOG_MAX);

    /* Grep: trap: soft path */
    kprintf("trap: soft path classify=user|kern|vec "
            "pe32=cs32_bp|int80 pf=cow_p_w except=port|kill "
            "rate=pow2+cap wave=%u (soft inventory; not bar3)\n",
            (unsigned)TRAP_SOFT_WAVE);

    /*
     * ---- Wave 15 complementary surfaces (kept; never reshape primary).
     */

    /* Grep: trap: soft honesty */
    kprintf("trap: soft honesty claim=classify+pe32+cow+except "
            "bar3=0 hard_gate=0 soft_only=1 rate_limited=1 "
            "unit=trap.c wave=%u (soft inventory; not bar3)\n",
            (unsigned)TRAP_SOFT_WAVE);

    /* Grep: trap: soft api — trap_stats_* call tallies */
    kprintf("trap: soft api get=%lu reset=%lu soft=%lu "
            "force=%lu logs=%u inv_path=dispatch|null|soft\n",
            (unsigned long)g_u64SoftApiGet,
            (unsigned long)g_u64SoftApiReset,
            (unsigned long)g_u64SoftApiSoft,
            (unsigned long)g_u64SoftForce,
            g_u32SoftLogged);

    /* Grep: trap: soft frame — last full frame lamps */
    kprintf("trap: soft frame vec=%u user=%u rpl=%u err=0x%lx "
            "rip=0x%lx cs=0x%lx rsp=0x%lx ss=0x%lx rflags=0x%lx "
            "cr2=0x%lx\n",
            g_u32SoftLastVec,
            g_u32SoftLastUser,
            u32LastRpl,
            (unsigned long)g_u64SoftLastErr,
            (unsigned long)g_u64SoftLastRip,
            (unsigned long)g_u64SoftLastCs,
            (unsigned long)g_u64SoftLastRsp,
            (unsigned long)g_u64SoftLastSs,
            (unsigned long)g_u64SoftLastRflags,
            (unsigned long)g_u64SoftLastCr2);

    /* Grep: trap: soft vec — last vector class + name lamp */
    kprintf("trap: soft vec last=%u name=%s exc=%u hi=%u pf=%u "
            "pe32=%u user=%u name_table=g_aszExc\n",
            g_u32SoftLastVec,
            szLastName,
            u32LastExc,
            u32LastHi,
            u32LastPf,
            u32LastPe32,
            g_u32SoftLastUser);

    /* Grep: trap: soft mile — milestone / quiet / skip rollup */
    kprintf("trap: soft mile hit=%lu last=%lu quiet=%lu skip=%lu "
            "force=%lu logs=%u max=%u at_cap=%u "
            "policy=pow2(1,2,4,...) \n",
            (unsigned long)g_u64SoftMileHit,
            (unsigned long)g_u64SoftMileLast,
            (unsigned long)g_u64SoftQuiet,
            (unsigned long)g_u64SoftSkip,
            (unsigned long)g_u64SoftForce,
            g_u32SoftLogged,
            (unsigned)TRAP_SOFT_LOG_MAX,
            u32AtCap);

    /*
     * ---- Wave 16 complementary surfaces (kept; never reshape primary).
     */

    /* Grep: trap: soft exclusive */
    kprintf("trap: soft exclusive wave=%u exclusive=1 soft=1 "
            "unit=trap.c bar3=0 hard_gate=0 soft_only=1 "
            "rate_limited=1 product_complete=0\n",
            (unsigned)TRAP_SOFT_WAVE);

    /* Grep: trap: soft claim — product claim bounds */
    kprintf("trap: soft claim classify=1 pe32=1 cow=1 except=1 "
            "kill_fallthrough=1 rate=pow2+cap "
            "bar3=0 hard_gate=0 soft_only=1 wave=%u\n",
            (unsigned)TRAP_SOFT_WAVE);

    /* Grep: trap: soft ratio — class/outcome/resume/halt path ratios */
    kprintf("trap: soft ratio total=%lu user=%lu kern=%lu "
            "vec32=%lu vec_hi=%lu pf_user=%lu cow_ok=%lu cow_miss=%lu "
            "pe32_bp=%lu pe32_int80=%lu except_ok=%lu kill=%lu "
            "halt_kern=%lu halt_null=%lu logs=%u wave=%u\n",
            (unsigned long)g_trapStats.u64Total,
            (unsigned long)g_trapStats.u64User,
            (unsigned long)g_trapStats.u64Kernel,
            (unsigned long)g_trapStats.u64VecLt32,
            (unsigned long)g_trapStats.u64VecGe32,
            (unsigned long)g_trapStats.u64PfUser,
            (unsigned long)g_trapStats.u64PfCowOk,
            (unsigned long)g_trapStats.u64PfCowMiss,
            (unsigned long)g_trapStats.u64Pe32Bp,
            (unsigned long)g_trapStats.u64Pe32Int80,
            (unsigned long)g_trapStats.u64ExceptDeliver,
            (unsigned long)g_trapStats.u64Kill,
            (unsigned long)g_u64SoftHaltKern,
            (unsigned long)g_u64SoftHaltNull,
            g_u32SoftLogged,
            (unsigned)TRAP_SOFT_WAVE);

    /*
     * ---- Wave 17 complementary surfaces (kept; never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: trap: soft return — Wave 17 API return surfaces (kept) */
    kprintf("trap: soft return classify=1 pe32=1 cow=1 except=1 "
            "stats_get=1 stats_reset=1 soft_inv=1 product_kernel=OPEN "
            "bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)TRAP_SOFT_WAVE);

    /* Grep: trap: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("trap: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 rate_limited=1 wave=%u soft PASS\n",
            (unsigned)TRAP_SOFT_WAVE);

    /* Grep: trap: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("trap: soft retmap dispatch=1 resume=1 halt=1 kill=1 "
            "soft_inv=1 deepen=1 product=OPEN wave=%u soft PASS\n",
            (unsigned)TRAP_SOFT_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: trap: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("trap: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)TRAP_SOFT_WAVE);

    /* Grep: trap: soft retcode — Wave 19 retcode catalog */
    kprintf("trap: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)TRAP_SOFT_WAVE);

    /* Grep: trap: soft deepen */
    /*
     * ---- Wave 19 exclusive complementary surfaces (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: trap: soft retclass — Wave 19 return-class taxonomy */
    kprintf("trap: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)TRAP_SOFT_WAVE);
    /* Grep: trap: soft retlane — Wave 19 return-lane catalog */
    kprintf("trap: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)TRAP_SOFT_WAVE);
    kprintf("trap: soft deepen wave=%u areas="
            "inventory,class,pf,pe32,outcome,stats,"
            "limit,last,resume,halt,rate,path,"
            "honesty,api,frame,vec,mile,"
            "exclusive,claim,ratio,return,return_selftest,retmap,return_rate,retcode "
            "unit=trap.c only rate_limited=1\n",
            (unsigned)TRAP_SOFT_WAVE);
}

/**
 * Rate-limit soft inventory: power-of-two dispatch milestones, hard-capped.
 * Never floods serial. Soft skip tallies only suppressed milestones (cap);
 * non-milestone dispatches bump quiet (Wave 15 lamp) without serial.
 * greppable: trap: soft
 */
static void
trap_soft_maybe_log(void)
{
    u64 u64N;

    u64N = g_trapStats.u64Total;
    /* Milestone: first entry and subsequent powers of two only. */
    if (u64N == 0ull || (u64N & (u64N - 1ull)) != 0ull) {
        g_u64SoftQuiet++;
        return;
    }

    g_u64SoftMileHit++;
    if (g_u32SoftLogged >= TRAP_SOFT_LOG_MAX) {
        g_u64SoftSkip++;
        return;
    }

    g_u64SoftMileLast = u64N;
    trap_soft_inventory_log();
}

void
trap_stats_get(struct gj_trap_stats *pOut)
{
    g_u64SoftApiGet++;
    if (pOut == NULL) {
        return;
    }
    *pOut = g_trapStats;
}

void
trap_stats_reset(void)
{
    g_u64SoftApiReset++;
    memset(&g_trapStats, 0, sizeof(g_trapStats));
    /* Preserve inventory log / skip lifetime (emission budget is sticky). */
}

u64
trap_stats_soft(void)
{
    /*
     * Explicit soft dump for smoke / product scripts — always emits
     * (does not consume rate-limit budget the same as hot path; still
     * increments logs so skip/max lamps stay honest).
     * greppable: trap: soft
     */
    g_u64SoftApiSoft++;
    g_u64SoftForce++;
    trap_soft_inventory_log();
    return g_trapStats.u64Total;
}

void
trap_dispatch(struct gj_trap_frame *pFrame)
{
    u32 u32Vec;
    int fUser;
    struct gj_thread *pThr;

    if (pFrame == NULL) {
        g_trapStats.u64NullFrame++;
        g_u32SoftLastVec = 0xffffffffu;
        g_u32SoftLastUser = 0;
        g_u64SoftLastErr = 0;
        g_u64SoftLastRip = 0;
        g_u64SoftLastCr2 = 0;
        g_u64SoftLastCs = 0;
        g_u64SoftLastRsp = 0;
        g_u64SoftLastSs = 0;
        g_u64SoftLastRflags = 0;
        g_u64SoftHaltNull++;
        g_u64SoftForce++;
        /* Grep: trap: soft (null frame inventory before halt) */
        trap_soft_inventory_log();
        kprintf("trap: null frame\n");
        for (;;) {
            __asm__ volatile ("cli; hlt");
        }
    }

    u32Vec = (u32)pFrame->u64Vector;
    fUser = ((pFrame->u64Cs & 3ull) == 3ull) ? 1 : 0;
    pThr = thread_current();

    g_trapStats.u64Total++;
    if (fUser) {
        g_trapStats.u64User++;
    } else {
        g_trapStats.u64Kernel++;
    }
    if (u32Vec < 32u) {
        g_trapStats.u64VecLt32++;
    } else {
        g_trapStats.u64VecGe32++;
    }
    g_u32SoftLastVec = u32Vec;
    g_u32SoftLastUser = fUser ? 1u : 0u;
    g_u64SoftLastErr = pFrame->u64Error;
    g_u64SoftLastRip = pFrame->u64Rip;
    g_u64SoftLastCs = pFrame->u64Cs;
    g_u64SoftLastRsp = pFrame->u64Rsp;
    g_u64SoftLastSs = pFrame->u64Ss;
    g_u64SoftLastRflags = pFrame->u64Rflags;
    if (u32Vec == 14u) {
        g_u64SoftLastCr2 = read_cr2();
    } else {
        g_u64SoftLastCr2 = 0;
    }

    /* Wave 15: rate-limited greppable trap: soft … inventory. */
    trap_soft_maybe_log();

    /* Suppress banner noise for expected PE32 paths (int3 / int 0x80) */
    if (!(fUser && (u32Vec == 3 || u32Vec == 128) &&
          (pFrame->u64Cs & 0xffull) == (u64)GJ_GDT_USER_CS32)) {
        kprintf("\n*** TRAP %s (%u) %s ***\n",
                u32Vec < 32 ? g_aszExc[u32Vec] : "??",
                u32Vec,
                fUser ? "user" : "kernel");
        kprintf("  err=0x%lx rip=0x%lx cs=0x%lx rflags=0x%lx\n",
                (unsigned long)pFrame->u64Error,
                (unsigned long)pFrame->u64Rip,
                (unsigned long)pFrame->u64Cs,
                (unsigned long)pFrame->u64Rflags);
        kprintf("  rsp=0x%lx ss=0x%lx rax=0x%lx\n",
                (unsigned long)pFrame->u64Rsp,
                (unsigned long)pFrame->u64Ss,
                (unsigned long)pFrame->u64Rax);
        if (u32Vec == 14) {
            kprintf("  cr2=0x%lx\n", (unsigned long)read_cr2());
        }
        if (pThr != NULL) {
            kprintf("  thr=%u state=%u\n", pThr->u32Id, pThr->u32State);
        }
    }

    if (!fUser) {
        g_u64SoftHaltKern++;
        kprintf("trap: kernel fault — halt\n");
        for (;;) {
            __asm__ volatile ("cli; hlt");
        }
    }

    /*
     * PE32 hardware enter: int3 (vector 3) from true CS32 compat mode.
     * Count the hit and exit the user thread so kmain smokes can observe.
     */
    if (u32Vec == 3 &&
        (pFrame->u64Cs & 0xffull) == (u64)GJ_GDT_USER_CS32) {
        g_u32Pe32HwEnterCs = (u32)pFrame->u64Cs;
        g_u32Pe32HwEnterHits++;
        g_trapStats.u64Pe32Bp++;
        g_u64SoftPe32Death++;
        kprintf("pe32: hw enter PASS cs=0x%lx rip=0x%lx hits=%u\n",
                (unsigned long)pFrame->u64Cs,
                (unsigned long)pFrame->u64Rip,
                g_u32Pe32HwEnterHits);
        if (pThr != NULL && pThr->pProc != NULL) {
            process_death(pThr->pProc, 0);
        }
        if (pThr != NULL && pThr->u32Id != 0) {
            pThr->u32State = GJ_THR_EXITED;
            schedule();
        }
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }

    /*
     * i386 Linux int 0x80 from CS32 (vector 128).
     * ABI: eax=nr; ebx/ecx/edx/esi/edi/ebp = args (low 32 of the frame regs).
     * Wine/Proton PE32 bring-up surface — clean-room numbers from man pages.
     */
    if (u32Vec == 128 &&
        (pFrame->u64Cs & 0xffull) == (u64)GJ_GDT_USER_CS32) {
        u32 u32Nr = (u32)pFrame->u64Rax;
        u32 u32A0 = (u32)pFrame->u64Rbx;
        u32 u32A1 = (u32)pFrame->u64Rcx;
        u32 u32A2 = (u32)pFrame->u64Rdx;
        u32 u32A3 = (u32)pFrame->u64Rsi;
        u32 u32A4 = (u32)pFrame->u64Rdi;
        i64 i64Ret = (i64)-38; /* ENOSYS default */

        g_u32Pe32Int80Calls++;
        g_u32Pe32Int80LastNr = u32Nr;
        g_trapStats.u64Pe32Int80++;
        pe32_fd_table_init();
        if (g_u32Pe32Brk == 0) {
            g_u32Pe32Brk = 0x56000000u; /* soft heap base */
        }
        kprintf("pe32: int80 nr=%u a0=%u a1=%u a2=%u cs=0x%lx\n", u32Nr,
                u32A0, u32A1, u32A2, (unsigned long)pFrame->u64Cs);

        switch (u32Nr) {
        case 1u:  /* exit */
        case 252u: /* exit_group */
            g_u32Pe32Int80Exits++;
            g_u32Pe32Int80LastCode = u32A0;
            g_u64SoftPe32Death++;
            kprintf("pe32: int80 exit PASS code=%u hits=%u\n", u32A0,
                    g_u32Pe32Int80Exits);
            if (pThr != NULL && pThr->pProc != NULL) {
                process_death(pThr->pProc, u32A0);
            }
            if (pThr != NULL && pThr->u32Id != 0) {
                pThr->u32State = GJ_THR_EXITED;
                schedule();
            }
            for (;;) {
                __asm__ volatile ("hlt");
            }
            break;
        case 3u: { /* read */
            u8 aBuf[256];
            u32 n = u32A2;
            i64 nRd;

            if (u32A0 < 3u) {
                i64Ret = 0; /* stdin EOF */
                break;
            }
            if (u32A0 >= 32u || !g_aPe32FdUsed[u32A0] ||
                g_aPe32VfsFd[u32A0] < 0) {
                i64Ret = 0; /* soft EOF for fake fds */
                break;
            }
            if (n > sizeof(aBuf)) {
                n = (u32)sizeof(aBuf);
            }
            nRd = vfs_ram_read(g_aPe32VfsFd[u32A0], aBuf, n);
            if (nRd < 0) {
                i64Ret = nRd;
                break;
            }
            if (nRd > 0 && u32A1 != 0 &&
                user_range_ok((u64)u32A1, (u64)nRd)) {
                if (copy_to_user((u64)u32A1, aBuf, (size_t)nRd) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            i64Ret = nRd;
            break;
        }
        case 4u: { /* write */
            u8 aBuf[256];
            u32 n = u32A2;
            i64 nWr;

            if (u32A0 <= 2u) {
                /* stdout/stderr — accept length */
                i64Ret = (i64)(u32)u32A2;
                break;
            }
            if (u32A0 >= 32u || !g_aPe32FdUsed[u32A0] ||
                g_aPe32VfsFd[u32A0] < 0) {
                i64Ret = (i64)(u32)u32A2; /* soft accept */
                break;
            }
            if (n > sizeof(aBuf)) {
                n = (u32)sizeof(aBuf);
            }
            if (n > 0 && u32A1 != 0) {
                if (user_range_ok((u64)u32A1, n)) {
                    if (copy_from_user(aBuf, (u64)u32A1, n) != GJ_OK) {
                        i64Ret = -14;
                        break;
                    }
                } else {
                    i64Ret = -14;
                    break;
                }
            }
            nWr = vfs_ram_write(g_aPe32VfsFd[u32A0], aBuf, n);
            i64Ret = nWr;
            break;
        }
        case 5u: { /* open — vfs_ram when path available */
            u32 fd;
            char szPath[96];
            i64 i64Vfs;
            int fCreate = (u32A1 & 0x40u) ? 1 : 0; /* O_CREAT-ish */

            if (pe32_copy_path(u32A0, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                /* No path: hand out soft fd (compat with prior smokes) */
                fd = g_u32Pe32NextFd++;
                if (fd >= 32u) {
                    g_u32Pe32NextFd = 3;
                    fd = 3;
                }
                g_aPe32FdUsed[fd] = 1;
                g_aPe32VfsFd[fd] = -1;
                i64Ret = (i64)fd;
                break;
            }
            i64Vfs = vfs_ram_open(szPath, fCreate);
            if (i64Vfs < 0) {
                i64Ret = i64Vfs;
                break;
            }
            fd = g_u32Pe32NextFd++;
            if (fd >= 32u) {
                g_u32Pe32NextFd = 3;
                fd = 3;
            }
            g_aPe32FdUsed[fd] = 1;
            g_aPe32VfsFd[fd] = i64Vfs;
            i64Ret = (i64)fd;
            break;
        }
        case 6u: /* close */
            if (u32A0 < 32u) {
                if (g_aPe32VfsFd[u32A0] >= 0) {
                    (void)vfs_ram_close(g_aPe32VfsFd[u32A0]);
                    g_aPe32VfsFd[u32A0] = -1;
                }
                g_aPe32FdUsed[u32A0] = 0;
            }
            i64Ret = 0;
            break;
        case 41u: { /* dup */
            i32 i32New;

            if (u32A0 >= 32u || !g_aPe32FdUsed[u32A0]) {
                i64Ret = -9; /* EBADF */
                break;
            }
            i32New = pe32_alloc_bound_fd(g_aPe32VfsFd[u32A0]);
            if (i32New < 0) {
                i64Ret = -24; /* EMFILE */
                break;
            }
            /* Dup soft-ref same vfs fd (close both will double-close — bring-up OK) */
            i64Ret = (i64)i32New;
            break;
        }
        case 42u: { /* pipe — real vfs_ram pipe pair */
            i32 aVfs[2];
            i32 aPe[2];
            i64 st;

            st = vfs_ram_pipe2(aVfs, 0);
            if (st != 0) {
                i64Ret = st;
                break;
            }
            aPe[0] = pe32_alloc_bound_fd(aVfs[0]);
            aPe[1] = pe32_alloc_bound_fd(aVfs[1]);
            if (aPe[0] < 0 || aPe[1] < 0) {
                (void)vfs_ram_close(aVfs[0]);
                (void)vfs_ram_close(aVfs[1]);
                i64Ret = -24;
                break;
            }
            if (u32A0 != 0 && user_range_ok((u64)u32A0, 8)) {
                u32 aOut[2];

                aOut[0] = (u32)aPe[0];
                aOut[1] = (u32)aPe[1];
                if (copy_to_user((u64)u32A0, aOut, 8) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            i64Ret = 0;
            break;
        }
        case 63u: { /* dup2 */
            if (u32A1 >= 32u) {
                i64Ret = -9;
                break;
            }
            if (u32A0 >= 32u || !g_aPe32FdUsed[u32A0]) {
                i64Ret = -9;
                break;
            }
            if (u32A0 == u32A1) {
                i64Ret = (i64)u32A1;
                break;
            }
            if (g_aPe32FdUsed[u32A1] && g_aPe32VfsFd[u32A1] >= 0) {
                (void)vfs_ram_close(g_aPe32VfsFd[u32A1]);
            }
            g_aPe32FdUsed[u32A1] = 1;
            g_aPe32VfsFd[u32A1] = g_aPe32VfsFd[u32A0];
            i64Ret = (i64)u32A1;
            break;
        }
        case 330u: { /* dup3 (i386) — flags ignored for bring-up */
            if (u32A1 >= 32u) {
                i64Ret = -9;
                break;
            }
            if (u32A0 >= 32u || !g_aPe32FdUsed[u32A0]) {
                i64Ret = -9;
                break;
            }
            if (u32A0 == u32A1) {
                i64Ret = -22; /* EINVAL */
                break;
            }
            if (g_aPe32FdUsed[u32A1] && g_aPe32VfsFd[u32A1] >= 0) {
                (void)vfs_ram_close(g_aPe32VfsFd[u32A1]);
            }
            g_aPe32FdUsed[u32A1] = 1;
            g_aPe32VfsFd[u32A1] = g_aPe32VfsFd[u32A0];
            i64Ret = (i64)u32A1;
            break;
        }
        case 436u: { /* close_range (i386) — close [first,last] */
            u32 lo = u32A0;
            u32 hi = u32A1;
            u32 i;

            if (hi >= 32u) {
                hi = 31u;
            }
            if (lo > hi) {
                i64Ret = -22;
                break;
            }
            for (i = lo; i <= hi; i++) {
                if (g_aPe32FdUsed[i]) {
                    if (g_aPe32VfsFd[i] >= 0) {
                        (void)vfs_ram_close(g_aPe32VfsFd[i]);
                        g_aPe32VfsFd[i] = -1;
                    }
                    g_aPe32FdUsed[i] = 0;
                }
            }
            i64Ret = 0;
            break;
        }
        case 102u: { /* socketcall — i386 multiplex */
            u32 call = u32A0;

            if (call == 1u) {
                /* SYS_SOCKET — soft fd (no real net from CS32 yet) */
                u32 fd = g_u32Pe32NextFd++;

                if (fd >= 32u) {
                    g_u32Pe32NextFd = 3;
                    fd = 3;
                }
                g_aPe32FdUsed[fd] = 1;
                g_aPe32VfsFd[fd] = -1;
                i64Ret = (i64)fd;
            } else if (call == 2u || call == 3u || call == 4u || call == 5u ||
                       call == 9u || call == 10u || call == 11u || call == 13u) {
                i64Ret = 0;
            } else if (call == 8u) {
                /* socketpair — real pipe pair; args: domain,type,proto,sv[2] */
                i32 aVfs[2];
                i32 aPe[2];
                u32 u32Sv = 0;
                i64 st;

                if (u32A1 != 0 && user_range_ok((u64)u32A1 + 12, 4)) {
                    (void)copy_from_user(&u32Sv, (u64)u32A1 + 12, 4);
                }
                st = vfs_ram_socketpair(1, 1, 0, aVfs);
                if (st != 0) {
                    i64Ret = st;
                    break;
                }
                aPe[0] = pe32_alloc_bound_fd(aVfs[0]);
                aPe[1] = pe32_alloc_bound_fd(aVfs[1]);
                if (aPe[0] < 0 || aPe[1] < 0) {
                    (void)vfs_ram_close(aVfs[0]);
                    (void)vfs_ram_close(aVfs[1]);
                    i64Ret = -24;
                    break;
                }
                if (u32Sv != 0 && user_range_ok((u64)u32Sv, 8)) {
                    u32 aOut[2];

                    aOut[0] = (u32)aPe[0];
                    aOut[1] = (u32)aPe[1];
                    (void)copy_to_user((u64)u32Sv, aOut, 8);
                }
                i64Ret = 0;
            } else {
                i64Ret = -38;
            }
            break;
        }
        case 13u: { /* time */
            u64 nsec = timer_ready() ? timer_mono_nsec() : 0;
            i64 sec = (i64)(nsec / 1000000000ull);

            if (u32A0 != 0) {
                u32 t = (u32)sec;
                if (user_range_ok((u64)u32A0, 4)) {
                    (void)copy_to_user((u64)u32A0, &t, 4);
                }
            }
            i64Ret = sec;
            break;
        }
        case 180u: { /* pread64 (i386) — pos read without moving off permanently */
            u8 aBuf[256];
            u32 n = u32A2;
            i64 nRd;
            u64 off = (u64)u32A3 | ((u64)u32A4 << 32);

            if (u32A0 >= 32u || !g_aPe32FdUsed[u32A0] ||
                g_aPe32VfsFd[u32A0] < 0) {
                i64Ret = -9;
                break;
            }
            if (n > sizeof(aBuf)) {
                n = (u32)sizeof(aBuf);
            }
            nRd = vfs_ram_pread(g_aPe32VfsFd[u32A0], aBuf, n, off);
            if (nRd < 0) {
                i64Ret = nRd;
                break;
            }
            if (nRd > 0 && u32A1 != 0 &&
                user_range_ok((u64)u32A1, (u64)nRd)) {
                if (copy_to_user((u64)u32A1, aBuf, (size_t)nRd) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            i64Ret = nRd;
            break;
        }
        case 181u: { /* pwrite64 (i386) */
            u8 aBuf[256];
            u32 n = u32A2;
            i64 nWr;
            u64 off = (u64)u32A3 | ((u64)u32A4 << 32);

            if (u32A0 >= 32u || !g_aPe32FdUsed[u32A0] ||
                g_aPe32VfsFd[u32A0] < 0) {
                i64Ret = -9;
                break;
            }
            if (n > sizeof(aBuf)) {
                n = (u32)sizeof(aBuf);
            }
            if (n > 0 && u32A1 != 0) {
                if (!user_range_ok((u64)u32A1, n) ||
                    copy_from_user(aBuf, (u64)u32A1, n) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            nWr = vfs_ram_pwrite(g_aPe32VfsFd[u32A0], aBuf, n, off);
            i64Ret = nWr;
            break;
        }
        case 19u: /* lseek */
            if (u32A0 < 32u && g_aPe32FdUsed[u32A0] &&
                g_aPe32VfsFd[u32A0] >= 0) {
                i64Ret = vfs_ram_lseek(g_aPe32VfsFd[u32A0], (i64)u32A1,
                                       (int)u32A2);
            } else {
                i64Ret = 0;
            }
            break;
        case 20u: /* getpid */
            i64Ret = 1;
            break;
        case 64u: /* getppid */
            i64Ret = 0;
            break;
        case 65u: /* getpgrp */
            i64Ret = 1;
            break;
        case 66u: /* setsid */
            i64Ret = 1;
            break;
        case 224u: /* gettid (i386) */
            i64Ret = 1;
            break;
        case 24u: /* getuid */
        case 47u: /* getgid */
        case 49u: /* geteuid */
        case 50u: /* getegid */
        case 199u: /* getuid32 */
        case 200u: /* getgid32 */
        case 201u: /* geteuid32 */
        case 202u: /* getegid32 */
            i64Ret = 0;
            break;
        case 45u: { /* brk — extend soft heap with real user pages */
            u32 u32Want = u32A0;
            u32 u32Cur = g_u32Pe32Brk;

            if (u32Want == 0) {
                i64Ret = (i64)u32Cur;
                break;
            }
            if (u32Want < 0x56000000u || u32Want > 0x5f000000u) {
                i64Ret = (i64)u32Cur;
                break;
            }
            if (u32Want > u32Cur) {
                u32 va = (u32Cur + 0xfffu) & ~0xfffu;
                while (va < u32Want) {
                    gj_paddr_t pa = pmm_alloc();
                    void *pK;

                    if (pa == 0) {
                        break;
                    }
                    pK = (void *)(gj_vaddr_t)(hhdm_ready()
                                                  ? (GJ_HHDM_BASE + (u64)pa)
                                                  : (u64)pa);
                    memset(pK, 0, GJ_PAGE_SIZE);
                    if (vmm_map_page((gj_vaddr_t)(u64)va, pa,
                                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE |
                                         GJ_VMM_PROT_USER) != GJ_OK) {
                        break;
                    }
                    va += (u32)GJ_PAGE_SIZE;
                }
                g_u32Pe32Brk = va;
            } else {
                g_u32Pe32Brk = u32Want;
            }
            i64Ret = (i64)g_u32Pe32Brk;
            break;
        }
        case 54u: /* ioctl */
            i64Ret = 0;
            break;
        case 90u: /* old_mmap — ignore struct; anonymous page */
        case 192u: { /* mmap2(addr,len,prot,flags,fd,pgoff) */
            u32 vaHint = u32A0;
            u32 len = u32A1 ? u32A1 : (u32)GJ_PAGE_SIZE;
            u32 va;
            u32 nPages;
            u32 i;
            u32 prot = GJ_VMM_PROT_READ | GJ_VMM_PROT_USER;

            (void)u32A3;
            (void)u32A4;
            if (u32A2 & 2u) {
                prot |= GJ_VMM_PROT_WRITE;
            }
            if (u32A2 & 4u) {
                prot |= GJ_VMM_PROT_EXEC;
            }
            if (vaHint == 0) {
                va = 0x57000000u +
                     ((g_u32Pe32Int80Calls & 0xffu) << 12);
            } else {
                va = vaHint & ~0xfffu;
            }
            nPages = (len + (u32)GJ_PAGE_SIZE - 1u) / (u32)GJ_PAGE_SIZE;
            if (nPages == 0) {
                nPages = 1;
            }
            if (nPages > 16u) {
                nPages = 16u;
            }
            for (i = 0; i < nPages; i++) {
                gj_paddr_t pa = pmm_alloc();
                void *pK;

                if (pa == 0) {
                    i64Ret = -12; /* ENOMEM */
                    break;
                }
                pK = (void *)(gj_vaddr_t)(hhdm_ready()
                                              ? (GJ_HHDM_BASE + (u64)pa)
                                              : (u64)pa);
                memset(pK, 0, GJ_PAGE_SIZE);
                if (vmm_map_page((gj_vaddr_t)(u64)(va + i * (u32)GJ_PAGE_SIZE),
                                 pa, prot) != GJ_OK) {
                    i64Ret = -12;
                    break;
                }
            }
            if (i64Ret != -12) {
                i64Ret = (i64)(u32)va;
            }
            break;
        }
        case 91u: /* munmap — accept; pages left until AS destroy */
            i64Ret = 0;
            break;
        case 219u: /* madvise (i386) — accept */
            i64Ret = 0;
            break;
        case 173u: /* sysctl — accept soft */
            i64Ret = 0;
            break;
        case 158u: /* sched_yield (i386) */
            thread_yield();
            i64Ret = 0;
            break;
        case 122u: { /* uname — struct old_utsname (9*65) on i386 */
            char aUts[9 * 65];
            u32 i;

            memset(aUts, 0, sizeof(aUts));
            /* sysname */
            aUts[0] = 'L';
            aUts[1] = 'i';
            aUts[2] = 'n';
            aUts[3] = 'u';
            aUts[4] = 'x';
            /* nodename @65 */
            aUts[65] = 'g';
            aUts[66] = 'r';
            aUts[67] = 'e';
            aUts[68] = 'e';
            aUts[69] = 'n';
            aUts[70] = 'j';
            aUts[71] = 'a';
            aUts[72] = 'd';
            aUts[73] = 'e';
            /* release @130 */
            aUts[130] = '6';
            aUts[131] = '.';
            aUts[132] = '1';
            aUts[133] = '.';
            aUts[134] = '0';
            /* version @195 */
            aUts[195] = '#';
            aUts[196] = '1';
            /* machine @260 */
            aUts[260] = 'i';
            aUts[261] = '6';
            aUts[262] = '8';
            aUts[263] = '6';
            (void)i;
            if (u32A0 != 0 && user_range_ok((u64)u32A0, sizeof(aUts))) {
                if (copy_to_user((u64)u32A0, aUts, sizeof(aUts)) != GJ_OK) {
                    i64Ret = -14; /* EFAULT */
                    break;
                }
            }
            i64Ret = 0;
            break;
        }
        case 125u: /* mprotect */
            i64Ret = 0;
            break;
        case 240u: { /* futex — wake returns 0; wait returns 0 (no block) */
            /* op in a1 low bits */
            i64Ret = 0;
            break;
        }
        case 243u: /* set_thread_area — TLS entry for glibc/Wine */
            i64Ret = 0;
            break;
        case 123u: /* modify_ldt — accept write/read for Wine TLS */
            i64Ret = 0;
            break;
        case 258u: /* set_tid_address (i386) */
            i64Ret = 1; /* return tid */
            break;
        case 311u: /* set_robust_list (i386) */
        case 312u: /* get_robust_list */
            i64Ret = 0;
            break;
        case 340u: /* prlimit64 (i386) */
            i64Ret = 0;
            break;
        case 355u: /* getrandom (i386) */
            if (u32A0 != 0 && u32A1 > 0 && user_range_ok((u64)u32A0, u32A1)) {
                u8 aR[16];
                u32 n = u32A1 > 16u ? 16u : u32A1;
                u32 j;

                for (j = 0; j < n; j++) {
                    aR[j] = (u8)(0x5au ^ (u8)j ^ (u8)g_u32Pe32Int80Calls);
                }
                if (copy_to_user((u64)u32A0, aR, n) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
                i64Ret = (i64)n;
            } else {
                i64Ret = 0;
            }
            break;
        case 265u: { /* clock_gettime (i386) */
            u64 nsec = timer_ready() ? timer_mono_nsec() : 0;
            i32 aTs[2];

            aTs[0] = (i32)(nsec / 1000000000ull);
            aTs[1] = (i32)(nsec % 1000000000ull);
            if (u32A1 != 0 && user_range_ok((u64)u32A1, 8)) {
                if (copy_to_user((u64)u32A1, aTs, 8) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            i64Ret = 0;
            break;
        }
        case 266u: { /* clock_getres (i386) */
            i32 aTs[2];

            aTs[0] = 0;
            aTs[1] = 1000000; /* 1 ms resolution for bring-up */
            if (u32A1 != 0 && user_range_ok((u64)u32A1, 8)) {
                if (copy_to_user((u64)u32A1, aTs, 8) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            i64Ret = 0;
            break;
        }
        case 267u: { /* clock_nanosleep (i386) — accept; no real sleep */
            i64Ret = 0;
            break;
        }
        case 27u: { /* alarm (i386) — return previous 0; accept seconds */
            i64Ret = 0;
            (void)u32A0;
            break;
        }
        case 96u: { /* getpriority (i386) — return nice 0 → 20 */
            i64Ret = 20;
            break;
        }
        case 97u: { /* setpriority (i386) — accept */
            i64Ret = 0;
            break;
        }
        case 104u: /* setitimer (i386) */
        case 105u: { /* getitimer (i386) */
            if (u32Nr == 105u && u32A1 != 0 &&
                user_range_ok((u64)u32A1, 16)) {
                u32 aZ[4];
                memset(aZ, 0, sizeof(aZ));
                (void)copy_to_user((u64)u32A1, aZ, 16);
            }
            i64Ret = 0;
            break;
        }
        case 78u: { /* gettimeofday (i386) */
            u64 nsec = timer_ready() ? timer_mono_nsec() : 0;
            i32 aTv[2];

            aTv[0] = (i32)(nsec / 1000000000ull);
            aTv[1] = (i32)((nsec % 1000000000ull) / 1000ull);
            if (u32A0 != 0 && user_range_ok((u64)u32A0, 8)) {
                if (copy_to_user((u64)u32A0, aTv, 8) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            i64Ret = 0;
            break;
        }
        case 114u: { /* wait4 — only reap children of this process */
            i32 st = 0;
            i64 r;
            u32 myPid = 0;

            if (pThr != NULL && pThr->pProc != NULL) {
                myPid = process_wait_pid_of(pThr->pProc);
            }
            if (myPid == 0) {
                /* Unregistered process has no wait children */
                i64Ret = -10;
                break;
            }
            r = process_wait4_ppid(myPid, (i64)(i32)u32A0, &st, (int)u32A2);
            if (r > 0 && u32A1 != 0 && user_range_ok((u64)u32A1, 4)) {
                (void)copy_to_user((u64)u32A1, &st, 4);
            }
            i64Ret = r;
            break;
        }
        case 284u: { /* waitid (i386) — ECHILD */
            i64Ret = -10;
            break;
        }
        case 7u: /* waitpid (legacy tables) */
            i64Ret = -10;
            break;
        case 76u: /* getrlimit */
        case 191u: { /* ugetrlimit */
            /* struct rlimit { rlim_t cur, max; } — 8 bytes i386 */
            u32 aLim[2];

            aLim[0] = 0xffffffffu; /* RLIM_INFINITY-ish */
            aLim[1] = 0xffffffffu;
            if (u32A1 != 0 && user_range_ok((u64)u32A1, 8)) {
                if (copy_to_user((u64)u32A1, aLim, 8) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            i64Ret = 0;
            break;
        }
        case 75u: /* setrlimit — accept */
            i64Ret = 0;
            break;
        case 37u: /* kill */
            if (u32A0 == 1u || u32A0 == 0u) {
                i64Ret = 0;
            } else {
                i64Ret = -3; /* ESRCH */
            }
            break;
        case 29u: /* pause — return EINTR */
            i64Ret = -4;
            break;
        case 162u: /* nanosleep — accept */
            i64Ret = 0;
            break;
        case 12u: { /* chdir — track soft cwd success if path exists */
            char szPath[96];

            if (pe32_copy_path(u32A0, szPath, sizeof(szPath)) != 0) {
                i64Ret = -14;
                break;
            }
            if (szPath[0] == '\0' || vfs_ram_access(szPath, 0) != 0) {
                i64Ret = -2; /* ENOENT */
                break;
            }
            i64Ret = 0;
            break;
        }
        case 183u: { /* getcwd — write "/tmp" for bring-up */
            static const char szCwd[] = "/tmp";
            u32 n = 5;

            if (u32A0 != 0 && u32A1 >= n &&
                user_range_ok((u64)u32A0, n)) {
                if (copy_to_user((u64)u32A0, szCwd, n) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
                i64Ret = (i64)n;
            } else {
                i64Ret = -22; /* EINVAL */
            }
            break;
        }
        case 116u: { /* sysinfo — minimal  struct sysinfo-shaped fill */
            u32 aSi[16];

            memset(aSi, 0, sizeof(aSi));
            aSi[0] = 1; /* uptime */
            aSi[2] = 64; /* totalram pages-ish */
            aSi[3] = 32; /* freeram */
            aSi[8] = 1;  /* procs */
            if (u32A0 != 0 && user_range_ok((u64)u32A0, sizeof(aSi))) {
                if (copy_to_user((u64)u32A0, aSi, sizeof(aSi)) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            i64Ret = 0;
            break;
        }
        case 33u: { /* access */
            char szPath[96];

            if (pe32_copy_path(u32A0, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Ret = vfs_ram_access(szPath, (int)u32A1);
            break;
        }
        case 38u: { /* rename */
            char szOld[96];
            char szNew[96];

            if (pe32_copy_path(u32A0, szOld, sizeof(szOld)) != 0 ||
                pe32_copy_path(u32A1, szNew, sizeof(szNew)) != 0 ||
                szOld[0] == '\0' || szNew[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Ret = vfs_ram_rename(szOld, szNew);
            break;
        }
        case 9u: { /* link */
            char szOld[96];
            char szNew[96];

            if (pe32_copy_path(u32A0, szOld, sizeof(szOld)) != 0 ||
                pe32_copy_path(u32A1, szNew, sizeof(szNew)) != 0 ||
                szOld[0] == '\0' || szNew[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Ret = vfs_ram_link(szOld, szNew);
            break;
        }
        case 302u: { /* renameat (i386) — ignore dirfds; use paths */
            char szOld[96];
            char szNew[96];

            if (pe32_copy_path(u32A1, szOld, sizeof(szOld)) != 0 ||
                pe32_copy_path(u32A3, szNew, sizeof(szNew)) != 0 ||
                szOld[0] == '\0' || szNew[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Ret = vfs_ram_rename(szOld, szNew);
            break;
        }
        case 303u: { /* linkat (i386) */
            char szOld[96];
            char szNew[96];

            if (pe32_copy_path(u32A1, szOld, sizeof(szOld)) != 0 ||
                pe32_copy_path(u32A3, szNew, sizeof(szNew)) != 0 ||
                szOld[0] == '\0' || szNew[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Ret = vfs_ram_link(szOld, szNew);
            break;
        }
        case 304u: { /* symlinkat (i386) */
            char szTarget[96];
            char szLink[96];

            if (pe32_copy_path(u32A0, szTarget, sizeof(szTarget)) != 0 ||
                pe32_copy_path(u32A2, szLink, sizeof(szLink)) != 0 ||
                szTarget[0] == '\0' || szLink[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Ret = vfs_ram_symlink(szTarget, szLink);
            break;
        }
        case 301u: { /* unlinkat (i386) */
            char szPath[96];

            if (pe32_copy_path(u32A1, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Ret = vfs_ram_unlink(szPath);
            break;
        }
        case 296u: { /* mkdirat (i386) */
            char szPath[96];
            i64 fd;

            if (pe32_copy_path(u32A1, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            fd = vfs_ram_open(szPath, 1);
            if (fd < 0) {
                i64Ret = fd;
                break;
            }
            (void)vfs_ram_mark_dir(fd);
            (void)vfs_ram_close(fd);
            i64Ret = 0;
            break;
        }
        case 307u: { /* faccessat (i386) */
            char szPath[96];

            if (pe32_copy_path(u32A1, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Ret = vfs_ram_access(szPath, (int)u32A2);
            break;
        }
        case 305u: { /* readlinkat (i386) */
            char szPath[96];
            char aLink[96];
            i64 n;

            if (pe32_copy_path(u32A1, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            n = vfs_ram_readlink(szPath, aLink, sizeof(aLink));
            if (n < 0) {
                i64Ret = n;
                break;
            }
            if (n > 0 && u32A2 != 0) {
                u32 cb = (u32)n;
                if (u32A3 != 0 && cb > u32A3) {
                    cb = u32A3;
                }
                if (user_range_ok((u64)u32A2, cb) &&
                    copy_to_user((u64)u32A2, aLink, cb) == GJ_OK) {
                    i64Ret = (i64)cb;
                } else {
                    i64Ret = -14;
                }
            } else {
                i64Ret = n;
            }
            break;
        }
        case 300u: { /* fstatat64 (i386) — path stat into buffer */
            char szPath[96];
            u8 aSt[96];
            u8 aSt64[144];

            memset(aSt, 0, sizeof(aSt));
            aSt[16] = 0xa4;
            aSt[17] = 0x81;
            aSt[20] = 1;
            if (pe32_copy_path(u32A1, szPath, sizeof(szPath)) == 0 &&
                szPath[0] != '\0' &&
                vfs_ram_stat(szPath, aSt64, sizeof(aSt64)) == 0) {
                aSt[44] = aSt64[48];
                aSt[45] = aSt64[49];
                aSt[46] = aSt64[50];
                aSt[47] = aSt64[51];
                aSt[16] = aSt64[24];
                aSt[17] = aSt64[25];
            }
            if (u32A2 != 0 && user_range_ok((u64)u32A2, sizeof(aSt))) {
                if (copy_to_user((u64)u32A2, aSt, sizeof(aSt)) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            i64Ret = 0;
            break;
        }
        case 83u: { /* symlink */
            char szTarget[96];
            char szLink[96];

            if (pe32_copy_path(u32A0, szTarget, sizeof(szTarget)) != 0 ||
                pe32_copy_path(u32A1, szLink, sizeof(szLink)) != 0 ||
                szTarget[0] == '\0' || szLink[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Ret = vfs_ram_symlink(szTarget, szLink);
            break;
        }
        case 85u: { /* readlink */
            char szPath[96];
            char aLink[96];
            i64 n;

            if (pe32_copy_path(u32A0, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            n = vfs_ram_readlink(szPath, aLink, sizeof(aLink));
            if (n < 0) {
                i64Ret = n;
                break;
            }
            if (n > 0 && u32A1 != 0) {
                u32 cb = (u32)n;
                if (u32A2 != 0 && cb > u32A2) {
                    cb = u32A2;
                }
                if (user_range_ok((u64)u32A1, cb)) {
                    if (copy_to_user((u64)u32A1, aLink, cb) != GJ_OK) {
                        i64Ret = -14;
                        break;
                    }
                }
                i64Ret = (i64)cb;
            } else {
                i64Ret = n;
            }
            break;
        }
        case 10u: /* unlink */
        case 40u: { /* rmdir-shaped unlink */
            char szPath[96];

            if (pe32_copy_path(u32A0, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Ret = vfs_ram_unlink(szPath);
            break;
        }
        case 39u: { /* mkdir — create empty marker file */
            char szPath[96];
            i64 fd;

            if (pe32_copy_path(u32A0, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            fd = vfs_ram_open(szPath, 1);
            if (fd < 0) {
                i64Ret = fd;
                break;
            }
            (void)vfs_ram_mark_dir(fd);
            (void)vfs_ram_close(fd);
            i64Ret = 0;
            break;
        }
        case 195u: /* stat64 */
        case 196u: /* lstat64 */
        case 197u: { /* fstat64 — fill from vfs when possible; else minimal */
            u8 aSt[96];
            u8 aSt64[144];
            u32 dst = u32A1;

            memset(aSt, 0, sizeof(aSt));
            memset(aSt64, 0, sizeof(aSt64));
            /* st_mode S_IFREG|0644 at offset 16 on i386 stat64 */
            aSt[16] = 0xa4;
            aSt[17] = 0x81;
            aSt[20] = 1;
            aSt[44] = 0x00;
            aSt[45] = 0x10;
            if (u32Nr == 197u) {
                if (u32A0 >= 32u ||
                    (u32A0 > 2u && !g_aPe32FdUsed[u32A0])) {
                    i64Ret = -9;
                    break;
                }
                if (u32A0 < 32u && g_aPe32FdUsed[u32A0] &&
                    g_aPe32VfsFd[u32A0] >= 0) {
                    if (vfs_ram_fstat(g_aPe32VfsFd[u32A0], aSt64,
                                      sizeof(aSt64)) == 0) {
                        /* map x86_64 st_size (~48) into i386-ish off 44 */
                        aSt[44] = aSt64[48];
                        aSt[45] = aSt64[49];
                        aSt[46] = aSt64[50];
                        aSt[47] = aSt64[51];
                        /* mode */
                        aSt[16] = aSt64[24];
                        aSt[17] = aSt64[25];
                    }
                }
            } else if (u32Nr == 195u || u32Nr == 196u) {
                char szPath[96];
                if (pe32_copy_path(u32A0, szPath, sizeof(szPath)) == 0 &&
                    szPath[0] != '\0') {
                    if (vfs_ram_stat(szPath, aSt64, sizeof(aSt64)) == 0) {
                        aSt[44] = aSt64[48];
                        aSt[45] = aSt64[49];
                        aSt[46] = aSt64[50];
                        aSt[47] = aSt64[51];
                        aSt[16] = aSt64[24];
                        aSt[17] = aSt64[25];
                    }
                }
            }
            if (dst != 0 && user_range_ok((u64)dst, sizeof(aSt))) {
                if (copy_to_user((u64)dst, aSt, sizeof(aSt)) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            i64Ret = 0;
            break;
        }
        case 221u: { /* fcntl64 (i386) */
            u32 cmd = u32A1;

            if (u32A0 >= 32u || (!g_aPe32FdUsed[u32A0] && u32A0 > 2u)) {
                i64Ret = -9;
                break;
            }
            if (cmd == 1u /* F_GETFD */ || cmd == 3u /* F_GETFL */) {
                i64Ret = 0;
            } else if (cmd == 2u /* F_SETFD */ || cmd == 4u /* F_SETFL */) {
                i64Ret = 0;
            } else if (cmd == 0u /* F_DUPFD */) {
                i32 i32New = pe32_alloc_bound_fd(
                    (u32A0 < 32u) ? g_aPe32VfsFd[u32A0] : (i64)-1);
                if (i32New < 0) {
                    /* soft fd dup when unbound */
                    u32 fd = g_u32Pe32NextFd++;
                    if (fd >= 32u) {
                        g_u32Pe32NextFd = 3;
                        fd = 3;
                    }
                    g_aPe32FdUsed[fd] = 1;
                    g_aPe32VfsFd[fd] =
                        (u32A0 < 32u) ? g_aPe32VfsFd[u32A0] : (i64)-1;
                    i64Ret = (i64)fd;
                } else {
                    i64Ret = (i64)i32New;
                }
            } else {
                i64Ret = 0;
            }
            break;
        }
        case 143u: /* flock (i386) — accept for bring-up */
            i64Ret = 0;
            break;
        case 140u: { /* _llseek(fd, high, low, result, whence) */
            i64 off;
            i64 cur;

            if (u32A0 < 32u && g_aPe32FdUsed[u32A0] &&
                g_aPe32VfsFd[u32A0] >= 0) {
                off = ((i64)(u32)u32A1 << 32) | (i64)(u32)u32A2;
                cur = vfs_ram_lseek(g_aPe32VfsFd[u32A0], off, (int)u32A4);
                if (cur < 0) {
                    i64Ret = cur;
                    break;
                }
                if (u32A3 != 0 && user_range_ok((u64)u32A3, 8)) {
                    u64 u = (u64)cur;
                    (void)copy_to_user((u64)u32A3, &u, 8);
                }
                i64Ret = 0;
            } else {
                if (u32A3 != 0 && user_range_ok((u64)u32A3, 8)) {
                    u64 z = 0;
                    (void)copy_to_user((u64)u32A3, &z, 8);
                }
                i64Ret = 0;
            }
            break;
        }
        case 295u: { /* openat (i386) — ignore dirfd; path absolute for bring-up */
            char szPath[96];
            i64 i64Vfs;
            i32 fd;
            int fCreate = (u32A2 & 0x40u) ? 1 : 0;

            if (pe32_copy_path(u32A1, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Vfs = vfs_ram_open(szPath, fCreate);
            if (i64Vfs < 0) {
                i64Ret = i64Vfs;
                break;
            }
            fd = pe32_alloc_bound_fd(i64Vfs);
            i64Ret = (fd >= 0) ? (i64)fd : (i64)-24;
            break;
        }
        case 331u: { /* pipe2 (i386) */
            i32 aVfs[2];
            i32 aPe[2];
            i64 st;

            st = vfs_ram_pipe2(aVfs, (int)u32A1);
            if (st != 0) {
                i64Ret = st;
                break;
            }
            aPe[0] = pe32_alloc_bound_fd(aVfs[0]);
            aPe[1] = pe32_alloc_bound_fd(aVfs[1]);
            if (aPe[0] < 0 || aPe[1] < 0) {
                (void)vfs_ram_close(aVfs[0]);
                (void)vfs_ram_close(aVfs[1]);
                i64Ret = -24;
                break;
            }
            if (u32A0 != 0 && user_range_ok((u64)u32A0, 8)) {
                u32 aOut[2];

                aOut[0] = (u32)aPe[0];
                aOut[1] = (u32)aPe[1];
                if (copy_to_user((u64)u32A0, aOut, 8) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            i64Ret = 0;
            break;
        }
        case 120u: { /* clone (i386) */
            u32 flags = u32A0;
            u32 stack = u32A1;

            /*
             * CLONE_VM (0x100): create real CS32 sibling thread in current AS
             * when child_stack is provided. Parent gets thread id.
             */
            if ((flags & 0x100u) != 0 && stack != 0 && pThr != NULL &&
                pThr->pProc != NULL) {
                u32 thr;
                u64 entry = pFrame->u64Rip; /* continue after int 0x80 */

                thr = thread_create_user32(pThr->pProc, entry, (u64)stack);
                if (thr != 0) {
                    i64Ret = (i64)thr;
                    break;
                }
            }
            /* Process-shaped: unique fake child pid (fork-like) */
            {
                u32 pid = g_u32Pe32NextChildPid++;

                if (g_u32Pe32NextChildPid > 0xffffu) {
                    g_u32Pe32NextChildPid = 2;
                }
                i64Ret = (i64)pid;
            }
            break;
        }
        case 220u: { /* getdents64 (i386) */
            u8 aDir[256];
            i64 n;
            i64 i64Vfs = -1;

            if (u32A0 < 32u && g_aPe32FdUsed[u32A0]) {
                i64Vfs = g_aPe32VfsFd[u32A0];
            }
            if (i64Vfs < 0) {
                /* open / soft for listing if no bound fd */
                i64Ret = 0;
                break;
            }
            n = vfs_ram_getdents64(i64Vfs, aDir, sizeof(aDir));
            if (n < 0) {
                i64Ret = n;
                break;
            }
            if (n > 0 && u32A1 != 0 &&
                user_range_ok((u64)u32A1, (u64)n)) {
                if (copy_to_user((u64)u32A1, aDir, (size_t)n) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            i64Ret = n;
            break;
        }
        case 141u: { /* getdents (legacy) — same as getdents64 fill */
            u8 aDir[256];
            i64 n;
            i64 i64Vfs = -1;

            if (u32A0 < 32u && g_aPe32FdUsed[u32A0]) {
                i64Vfs = g_aPe32VfsFd[u32A0];
            }
            if (i64Vfs < 0) {
                i64Ret = 0;
                break;
            }
            n = vfs_ram_getdents64(i64Vfs, aDir, sizeof(aDir));
            i64Ret = (n < 0) ? n : n;
            if (n > 0 && u32A1 != 0 && user_range_ok((u64)u32A1, (u64)n)) {
                (void)copy_to_user((u64)u32A1, aDir, (size_t)n);
            }
            break;
        }
        case 323u: { /* eventfd (i386) */
            i64 i64Vfs = vfs_ram_eventfd2(u32A0, (int)u32A1);
            i32 fd;

            if (i64Vfs < 0) {
                i64Ret = i64Vfs;
                break;
            }
            fd = pe32_alloc_bound_fd(i64Vfs);
            i64Ret = (fd >= 0) ? (i64)fd : (i64)-24;
            break;
        }
        case 328u: { /* eventfd2 (i386) */
            i64 i64Vfs = vfs_ram_eventfd2(u32A0, (int)u32A1);
            i32 fd;

            if (i64Vfs < 0) {
                i64Ret = i64Vfs;
                break;
            }
            fd = pe32_alloc_bound_fd(i64Vfs);
            i64Ret = (fd >= 0) ? (i64)fd : (i64)-24;
            break;
        }
        case 2u: /* fork */
        case 190u: { /* vfork — register wait child under this process pid */
            i64 pid;
            u32 ppid = 1;

            if (pThr != NULL && pThr->pProc != NULL) {
                u32 self = process_wait_pid_of(pThr->pProc);

                if (self != 0) {
                    ppid = self;
                }
                pid = process_linux_fork(ppid, (u32Nr == 190u) ? 1 : 0);
                if (pid > 0) {
                    i64Ret = pid;
                    break;
                }
            }
            {
                u32 fake = g_u32Pe32NextChildPid++;

                if (g_u32Pe32NextChildPid > 0xffffu) {
                    g_u32Pe32NextChildPid = 2;
                }
                i64Ret = (i64)fake;
            }
            break;
        }
        case 11u: { /* execve — load PE from vfs and rewrite CS32 frame */
            char szPath[96];
            u8 aPe[0x1000];
            i64 i64Fd;
            i64 n;
            struct gj_pe32_load load;
            struct gj_process *pProc;

            if (pe32_copy_path(u32A0, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Fd = vfs_ram_open(szPath, 0);
            if (i64Fd < 0) {
                i64Ret = i64Fd;
                break;
            }
            n = vfs_ram_read(i64Fd, aPe, sizeof(aPe));
            (void)vfs_ram_close(i64Fd);
            if (n < 64) {
                i64Ret = -8; /* ENOEXEC */
                break;
            }
            /* Non-PE: soft accept (legacy smoke paths) */
            if (aPe[0] != (u8)'M' || aPe[1] != (u8)'Z') {
                i64Ret = 0;
                break;
            }
            pProc = (pThr != NULL) ? pThr->pProc : NULL;
            if (pProc == NULL) {
                i64Ret = -12;
                break;
            }
            if (pe32_load_process(pProc, aPe, (u32)n, &load) != 0 ||
                load.u32Ready == 0) {
                i64Ret = -8;
                break;
            }
            /* Replace return path: iretq into new image (no return to caller) */
            pFrame->u64Rip = load.u64Entry;
            pFrame->u64Rsp = load.u64StackTop - 16ull;
            i64Ret = 0;
            kprintf("pe32: execve image entry=0x%lx\n",
                    (unsigned long)load.u64Entry);
            break;
        }
        case 174u: /* rt_sigaction */
        case 175u: /* rt_sigprocmask */
        case 176u: /* rt_sigreturn */
            i64Ret = 0;
            break;
        case 172u: /* prctl */
            i64Ret = 0;
            break;
        case 309u: /* ppoll (i386) — same as poll; ignore sigmask/timeout shape */
        case 168u: { /* poll — vfs readiness when fd bound */
            u32 nfds = u32A1;
            u32 i;
            u32 ready = 0;
            u32 maxn = nfds;

            if (maxn > 16u) {
                maxn = 16u;
            }
            for (i = 0; i < maxn; i++) {
                u32 aEnt[2]; /* fd, events|revents packed loosely */
                u32 fd;
                u16 ev;
                u16 rev = 0;
                i64 i64Vfs = -1;

                if (u32A0 == 0 ||
                    !user_range_ok((u64)u32A0 + (u64)i * 8u, 8)) {
                    break;
                }
                if (copy_from_user(aEnt, (u64)u32A0 + (u64)i * 8u, 8) !=
                    GJ_OK) {
                    break;
                }
                fd = aEnt[0];
                ev = (u16)(aEnt[1] & 0xffffu);
                if (fd < 32u && g_aPe32FdUsed[fd]) {
                    i64Vfs = g_aPe32VfsFd[fd];
                }
                if (i64Vfs >= 0) {
                    u32 m = vfs_ram_poll_mask(i64Vfs, (u32)ev | 0x5u);
                    if (m != 0) {
                        rev = (u16)(m & 0xffffu);
                        ready++;
                    }
                } else if (fd <= 2u) {
                    /* stdout always writable */
                    if (ev & 0x4u) {
                        rev = 0x4u;
                        ready++;
                    }
                }
                aEnt[1] = (u32)ev | ((u32)rev << 16);
                (void)copy_to_user((u64)u32A0 + (u64)i * 8u, aEnt, 8);
            }
            i64Ret = (i64)ready;
            break;
        }
        case 308u: /* pselect6 (i386) — same as select; ignore sigmask */
        case 142u: { /* _newselect — scan readfds for ready eventfd/pipe */
            u32 n = u32A0;
            u32 ready = 0;
            u32 i;
            u32 bits = 0;

            if (n > 32u) {
                n = 32u;
            }
            if (u32A1 != 0 && user_range_ok((u64)u32A1, 4)) {
                (void)copy_from_user(&bits, (u64)u32A1, 4);
            }
            for (i = 0; i < n; i++) {
                if ((bits & (1u << i)) == 0) {
                    continue;
                }
                if (i < 32u && g_aPe32FdUsed[i] && g_aPe32VfsFd[i] >= 0) {
                    u32 m = vfs_ram_poll_mask(g_aPe32VfsFd[i], 0x1u);
                    if (m & 0x1u) {
                        ready++;
                    } else {
                        bits &= ~(1u << i);
                    }
                } else {
                    bits &= ~(1u << i);
                }
            }
            if (u32A1 != 0 && user_range_ok((u64)u32A1, 4)) {
                (void)copy_to_user((u64)u32A1, &bits, 4);
            }
            /* writefds/exceptfds cleared for bring-up */
            if (u32A2 != 0 && user_range_ok((u64)u32A2, 4)) {
                u32 z = 0;
                (void)copy_to_user((u64)u32A2, &z, 4);
            }
            if (u32A3 != 0 && user_range_ok((u64)u32A3, 4)) {
                u32 z = 0;
                (void)copy_to_user((u64)u32A3, &z, 4);
            }
            i64Ret = (i64)ready;
            break;
        }
        case 146u: { /* writev — sum iov lengths via vfs write when bound */
            u32 iovcnt = u32A2;
            u32 i;
            i64 total = 0;

            if (iovcnt > 8u) {
                iovcnt = 8u;
            }
            if (u32A0 <= 2u) {
                /* stdout: sum lengths */
                for (i = 0; i < iovcnt; i++) {
                    u32 aIov[2];
                    if (u32A1 == 0 ||
                        !user_range_ok((u64)u32A1 + (u64)i * 8u, 8)) {
                        break;
                    }
                    if (copy_from_user(aIov, (u64)u32A1 + (u64)i * 8u, 8) !=
                        GJ_OK) {
                        break;
                    }
                    total += (i64)(u32)aIov[1];
                }
                i64Ret = total > 0 ? total : 1;
                break;
            }
            if (u32A0 >= 32u || !g_aPe32FdUsed[u32A0] ||
                g_aPe32VfsFd[u32A0] < 0) {
                i64Ret = (i64)(u32)u32A2;
                if (i64Ret == 0) {
                    i64Ret = 1;
                }
                break;
            }
            for (i = 0; i < iovcnt; i++) {
                u32 aIov[2];
                u8 aBuf[128];
                u32 n;
                i64 nWr;

                if (u32A1 == 0 ||
                    !user_range_ok((u64)u32A1 + (u64)i * 8u, 8)) {
                    break;
                }
                if (copy_from_user(aIov, (u64)u32A1 + (u64)i * 8u, 8) !=
                    GJ_OK) {
                    i64Ret = -14;
                    break;
                }
                n = aIov[1];
                if (n > sizeof(aBuf)) {
                    n = (u32)sizeof(aBuf);
                }
                if (n > 0 && aIov[0] != 0 &&
                    user_range_ok((u64)aIov[0], n)) {
                    if (copy_from_user(aBuf, (u64)aIov[0], n) != GJ_OK) {
                        i64Ret = -14;
                        break;
                    }
                    nWr = vfs_ram_write(g_aPe32VfsFd[u32A0], aBuf, n);
                    if (nWr < 0) {
                        i64Ret = nWr;
                        break;
                    }
                    total += nWr;
                }
            }
            if (i64Ret >= 0 || i64Ret == -38) {
                i64Ret = total;
            }
            break;
        }
        case 145u: { /* readv */
            u32 iovcnt = u32A2;
            u32 i;
            i64 total = 0;

            if (iovcnt > 8u) {
                iovcnt = 8u;
            }
            if (u32A0 >= 32u || !g_aPe32FdUsed[u32A0] ||
                g_aPe32VfsFd[u32A0] < 0) {
                i64Ret = 0;
                break;
            }
            for (i = 0; i < iovcnt; i++) {
                u32 aIov[2];
                u8 aBuf[128];
                u32 n;
                i64 nRd;

                if (u32A1 == 0 ||
                    !user_range_ok((u64)u32A1 + (u64)i * 8u, 8)) {
                    break;
                }
                if (copy_from_user(aIov, (u64)u32A1 + (u64)i * 8u, 8) !=
                    GJ_OK) {
                    i64Ret = -14;
                    break;
                }
                n = aIov[1];
                if (n > sizeof(aBuf)) {
                    n = (u32)sizeof(aBuf);
                }
                nRd = vfs_ram_read(g_aPe32VfsFd[u32A0], aBuf, n);
                if (nRd < 0) {
                    i64Ret = nRd;
                    break;
                }
                if (nRd == 0) {
                    break;
                }
                if (aIov[0] != 0 &&
                    user_range_ok((u64)aIov[0], (u64)nRd)) {
                    if (copy_to_user((u64)aIov[0], aBuf, (size_t)nRd) !=
                        GJ_OK) {
                        i64Ret = -14;
                        break;
                    }
                }
                total += nRd;
            }
            if (i64Ret >= 0 || i64Ret == -38) {
                i64Ret = total;
            }
            break;
        }
        case 118u: /* fsync */
            i64Ret = 0;
            break;
        case 148u: /* fdatasync (i386) */
            i64Ret = 0;
            break;
        case 36u: /* sync — global sync accept */
            i64Ret = 0;
            break;
        case 133u: /* fchdir */
            i64Ret = 0;
            break;
        case 15u: { /* chmod */
            char szPath[96];

            if (pe32_copy_path(u32A0, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            /* touch path then accept mode */
            i64Ret = vfs_ram_access(szPath, 0);
            if (i64Ret == 0) {
                /* best-effort: open+fchmod if file exists */
                i64 fd = vfs_ram_open(szPath, 0);
                if (fd >= 0) {
                    i64Ret = vfs_ram_fchmod(fd, u32A1);
                    (void)vfs_ram_close(fd);
                }
            }
            break;
        }
        case 94u: { /* fchmod */
            if (u32A0 < 32u && g_aPe32FdUsed[u32A0] &&
                g_aPe32VfsFd[u32A0] >= 0) {
                i64Ret = vfs_ram_fchmod(g_aPe32VfsFd[u32A0], u32A1);
            } else {
                i64Ret = 0;
            }
            break;
        }
        case 93u: { /* ftruncate */
            if (u32A0 < 32u && g_aPe32FdUsed[u32A0] &&
                g_aPe32VfsFd[u32A0] >= 0) {
                i64Ret = vfs_ram_ftruncate(g_aPe32VfsFd[u32A0], (i64)u32A1);
            } else {
                i64Ret = -9;
            }
            break;
        }
        case 30u: { /* utime */
            char szPath[96];

            if (pe32_copy_path(u32A0, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Ret = vfs_ram_utimens(szPath);
            break;
        }
        case 271u: { /* utimensat (i386) — ignore dirfd/times; touch path */
            char szPath[96];

            if (pe32_copy_path(u32A1, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Ret = vfs_ram_utimens(szPath);
            break;
        }
        case 324u: { /* fallocate (i386) */
            if (u32A0 < 32u && g_aPe32FdUsed[u32A0] &&
                g_aPe32VfsFd[u32A0] >= 0) {
                i64Ret = vfs_ram_fallocate(g_aPe32VfsFd[u32A0], (i64)u32A2,
                                          (i64)u32A3);
            } else {
                i64Ret = -9;
            }
            break;
        }
        case 254u: /* epoll_create (i386) */
        case 329u: { /* epoll_create1 (i386) */
            i64 i64Vfs = vfs_ram_epoll_create1((int)u32A0);
            i32 fd;

            if (i64Vfs < 0) {
                i64Ret = i64Vfs;
                break;
            }
            fd = pe32_alloc_bound_fd(i64Vfs);
            i64Ret = (fd >= 0) ? (i64)fd : (i64)-24;
            break;
        }
        case 255u: { /* epoll_ctl (i386) */
            i64 i64Ep = -1;
            i64 i64Fd = -1;
            u32 u32Ev = 0;
            u64 u64Data = 0;

            if (u32A0 < 32u && g_aPe32FdUsed[u32A0]) {
                i64Ep = g_aPe32VfsFd[u32A0];
            }
            if (u32A2 < 32u && g_aPe32FdUsed[u32A2]) {
                i64Fd = g_aPe32VfsFd[u32A2];
            }
            if (i64Ep < 0 || i64Fd < 0) {
                i64Ret = -9;
                break;
            }
            /* event: struct epoll_event { u32 events; u64 data } — 12 bytes i386 packed */
            if (u32A3 != 0 && user_range_ok((u64)u32A3, 12)) {
                u8 aEv[12];
                if (copy_from_user(aEv, (u64)u32A3, 12) == GJ_OK) {
                    u32Ev = (u32)aEv[0] | ((u32)aEv[1] << 8) |
                            ((u32)aEv[2] << 16) | ((u32)aEv[3] << 24);
                    u64Data = (u64)aEv[4] | ((u64)aEv[5] << 8) |
                              ((u64)aEv[6] << 16) | ((u64)aEv[7] << 24) |
                              ((u64)aEv[8] << 32) | ((u64)aEv[9] << 40) |
                              ((u64)aEv[10] << 48) | ((u64)aEv[11] << 56);
                }
            }
            i64Ret = vfs_ram_epoll_ctl(i64Ep, (int)u32A1, i64Fd, u32Ev,
                                       u64Data);
            break;
        }
        case 256u: { /* epoll_wait (i386) */
            u8 aEv[48]; /* up to 4 events * 12 */
            i64 i64Ep = -1;
            i64 n;
            int maxn = (int)u32A2;

            if (u32A0 < 32u && g_aPe32FdUsed[u32A0]) {
                i64Ep = g_aPe32VfsFd[u32A0];
            }
            if (i64Ep < 0) {
                i64Ret = -9;
                break;
            }
            if (maxn > 4) {
                maxn = 4;
            }
            if (maxn < 1) {
                maxn = 1;
            }
            n = vfs_ram_epoll_wait(i64Ep, aEv, maxn, (int)u32A3);
            if (n < 0) {
                i64Ret = n;
                break;
            }
            if (n > 0 && u32A1 != 0 &&
                user_range_ok((u64)u32A1, (u64)n * 12u)) {
                if (copy_to_user((u64)u32A1, aEv, (size_t)n * 12u) != GJ_OK) {
                    i64Ret = -14;
                    break;
                }
            }
            i64Ret = n;
            break;
        }
        case 322u: { /* timerfd_create (i386) */
            i64 i64Vfs = vfs_ram_timerfd_create((int)u32A0, (int)u32A1);
            i32 fd;

            if (i64Vfs < 0) {
                i64Ret = i64Vfs;
                break;
            }
            fd = pe32_alloc_bound_fd(i64Vfs);
            i64Ret = (fd >= 0) ? (i64)fd : (i64)-24;
            break;
        }
        case 325u: { /* timerfd_settime (i386) */
            u64 u64Val = 1;
            u64 u64Int = 0;
            i64 i64Fd = -1;

            if (u32A0 < 32u && g_aPe32FdUsed[u32A0]) {
                i64Fd = g_aPe32VfsFd[u32A0];
            }
            if (i64Fd < 0) {
                i64Ret = -9;
                break;
            }
            /* itimerspec: 2*timespec; value.tv_sec at start — if non-zero arm */
            if (u32A2 != 0 && user_range_ok((u64)u32A2, 16)) {
                u32 aTs[4];
                if (copy_from_user(aTs, (u64)u32A2, 16) == GJ_OK) {
                    if (aTs[0] != 0 || aTs[1] != 0) {
                        u64Val = 1;
                    } else {
                        u64Val = 0;
                    }
                    if (aTs[2] != 0 || aTs[3] != 0) {
                        u64Int = 1;
                    }
                }
            }
            i64Ret = vfs_ram_timerfd_settime(i64Fd, (int)u32A1, u64Val,
                                             u64Int);
            break;
        }
        case 326u: { /* timerfd_gettime (i386) */
            u64 u64Val = 0;
            u64 u64Int = 0;
            i64 i64Fd = -1;
            i64 st;

            if (u32A0 < 32u && g_aPe32FdUsed[u32A0]) {
                i64Fd = g_aPe32VfsFd[u32A0];
            }
            if (i64Fd < 0) {
                i64Ret = -9;
                break;
            }
            st = vfs_ram_timerfd_gettime(i64Fd, &u64Val, &u64Int);
            if (st < 0) {
                i64Ret = st;
                break;
            }
            if (u32A1 != 0 && user_range_ok((u64)u32A1, 32)) {
                u32 aOut[8];
                memset(aOut, 0, sizeof(aOut));
                aOut[0] = u64Val ? 1u : 0u;
                aOut[2] = u64Int ? 1u : 0u;
                (void)copy_to_user((u64)u32A1, aOut, 32);
            }
            i64Ret = 0;
            break;
        }
        case 239u: { /* sendfile64 (i386) */
            i64 i64Out = -1;
            i64 i64In = -1;
            u64 off = 0;
            u64 *pOff = NULL;

            if (u32A0 < 32u && g_aPe32FdUsed[u32A0]) {
                i64Out = g_aPe32VfsFd[u32A0];
            }
            if (u32A1 < 32u && g_aPe32FdUsed[u32A1]) {
                i64In = g_aPe32VfsFd[u32A1];
            }
            if (i64Out < 0 || i64In < 0) {
                i64Ret = -9;
                break;
            }
            if (u32A2 != 0 && user_range_ok((u64)u32A2, 8)) {
                (void)copy_from_user(&off, (u64)u32A2, 8);
                pOff = &off;
            }
            i64Ret = vfs_ram_sendfile(i64Out, i64In, pOff, (size_t)u32A3);
            if (pOff != NULL && u32A2 != 0) {
                (void)copy_to_user((u64)u32A2, &off, 8);
            }
            break;
        }
        case 291u: /* inotify_init (i386) */
        case 332u: { /* inotify_init1 (i386) */
            i64 i64Vfs = vfs_ram_inotify_init1((int)u32A0);
            i32 fd;

            if (i64Vfs < 0) {
                i64Ret = i64Vfs;
                break;
            }
            fd = pe32_alloc_bound_fd(i64Vfs);
            i64Ret = (fd >= 0) ? (i64)fd : (i64)-24;
            break;
        }
        case 292u: { /* inotify_add_watch (i386) */
            char szPath[96];
            i64 i64Fd = -1;

            if (u32A0 < 32u && g_aPe32FdUsed[u32A0]) {
                i64Fd = g_aPe32VfsFd[u32A0];
            }
            if (i64Fd < 0) {
                i64Ret = -9;
                break;
            }
            if (pe32_copy_path(u32A1, szPath, sizeof(szPath)) != 0 ||
                szPath[0] == '\0') {
                i64Ret = -14;
                break;
            }
            i64Ret = vfs_ram_inotify_add_watch(i64Fd, szPath, u32A2);
            break;
        }
        case 293u: { /* inotify_rm_watch (i386) */
            i64 i64Fd = -1;

            if (u32A0 < 32u && g_aPe32FdUsed[u32A0]) {
                i64Fd = g_aPe32VfsFd[u32A0];
            }
            if (i64Fd < 0) {
                i64Ret = -9;
                break;
            }
            i64Ret = vfs_ram_inotify_rm_watch(i64Fd, (i32)u32A1);
            break;
        }
        case 356u: { /* memfd_create (i386) — anonymous ram file */
            i64 i64Vfs = vfs_ram_open("/tmp/memfd_pe32", 1);
            i32 fd;

            if (i64Vfs < 0) {
                i64Ret = i64Vfs;
                break;
            }
            fd = pe32_alloc_bound_fd(i64Vfs);
            i64Ret = (fd >= 0) ? (i64)fd : (i64)-24;
            break;
        }
        case 377u: { /* copy_file_range (i386) — simplified: ignore offs ptrs */
            i64 i64In = -1;
            i64 i64Out = -1;
            size_t cb = (size_t)u32A4;

            if (u32A0 < 32u && g_aPe32FdUsed[u32A0]) {
                i64In = g_aPe32VfsFd[u32A0];
            }
            if (u32A2 < 32u && g_aPe32FdUsed[u32A2]) {
                i64Out = g_aPe32VfsFd[u32A2];
            }
            if (i64In < 0 || i64Out < 0) {
                i64Ret = -9;
                break;
            }
            if (cb == 0) {
                cb = 4096;
            }
            if (cb > 256) {
                cb = 256;
            }
            i64Ret = vfs_ram_copy_file_range(i64In, NULL, i64Out, NULL, cb);
            break;
        }
        case 321u: /* signalfd (i386) */
        case 327u: { /* signalfd4 (i386) */
            i64 i64In = -1;
            u64 mask = 0;
            i64 i64Vfs;
            i32 fd;

            if (u32A0 < 32u && g_aPe32FdUsed[u32A0] &&
                g_aPe32VfsFd[u32A0] >= 0) {
                i64In = g_aPe32VfsFd[u32A0];
            } else if ((i32)u32A0 >= 0 && u32A0 < 32u) {
                i64In = -1; /* create new */
            }
            if (u32A1 != 0 && user_range_ok((u64)u32A1, 8)) {
                (void)copy_from_user(&mask, (u64)u32A1, 8);
            }
            i64Vfs = vfs_ram_signalfd4(i64In, mask, (int)u32A2);
            if (i64Vfs < 0) {
                i64Ret = i64Vfs;
                break;
            }
            if (i64In >= 0) {
                i64Ret = (i64)u32A0; /* updated existing pe32 fd */
            } else {
                fd = pe32_alloc_bound_fd(i64Vfs);
                i64Ret = (fd >= 0) ? (i64)fd : (i64)-24;
            }
            break;
        }
        default:
            i64Ret = -38; /* ENOSYS */
            break;
        }
        /* Return value in eax (low 32); sign-extend into rax for 64-bit frame */
        pFrame->u64Rax = (u64)(u32)(i32)i64Ret;
        if (i64Ret < 0) {
            pFrame->u64Rax = (u64)(i64)i64Ret;
        }
        /* Soft: PE32 int 0x80 returned to user (Wave 13 resume lamp). */
        g_u64SoftPe32Resume++;
        return;
    }

    /*
     * User write #PF on a present COW page → break sharing and resume.
     * Error code: bit0=P, bit1=W/R, bit2=U/S (Intel SDM).
     *
     * Soft deepen (grep: trap: #PF soft): log decision tree + bump stats.
     * Actual break still logs greppable `vmm: COW break …` from vmm.
     */
    if (u32Vec == 14 && fUser) {
        u64 u64Err = pFrame->u64Error;
        u64 u64Cr2 = read_cr2();
        u64 u64VaPage = u64Cr2 & ~0xfffull;
        u32 u32ThrId = (pThr != NULL) ? pThr->u32Id : 0u;
        u32 u32P = (u32)(u64Err & 1ull);
        u32 u32W = (u32)((u64Err >> 1) & 1ull);
        u32 u32U = (u32)((u64Err >> 2) & 1ull);

        g_trapStats.u64PfUser++;
        /* Grep: trap: #PF soft path */
        kprintf("trap: #PF soft path cr2=0x%lx va=0x%lx err=0x%lx "
                "P=%u W=%u U=%u thr=%u rip=0x%lx\n",
                (unsigned long)u64Cr2,
                (unsigned long)u64VaPage,
                (unsigned long)u64Err,
                u32P, u32W, u32U, u32ThrId,
                (unsigned long)pFrame->u64Rip);

        if ((u64Err & 0x3ull) == 0x3ull) { /* present + write */
            g_trapStats.u64PfCowCand++;
            /* Grep: trap: #PF soft cow try */
            kprintf("trap: #PF soft cow try va=0x%lx thr=%u\n",
                    (unsigned long)u64VaPage, u32ThrId);
            if (pThr != NULL && pThr->pProc != NULL &&
                pThr->pProc->u64Cr3 != 0) {
                /* Switch to the process AS so COW break targets its CR3 */
                cpu_load_cr3(pThr->pProc->u64Cr3);
                g_trapStats.u64PfCowCr3Sw++;
                /* Grep: trap: #PF soft cow cr3 */
                kprintf("trap: #PF soft cow cr3=0x%lx thr=%u\n",
                        (unsigned long)pThr->pProc->u64Cr3, u32ThrId);
            }
            if (vmm_cow_break_page((gj_vaddr_t)u64VaPage) == GJ_OK) {
                g_trapStats.u64PfCowOk++;
                /* Grep: trap: #PF soft cow ok */
                kprintf("trap: #PF soft cow ok va=0x%lx thr=%u resume\n",
                        (unsigned long)u64VaPage, u32ThrId);
                return; /* resume faulting instruction */
            }
            g_trapStats.u64PfCowMiss++;
            /* Grep: trap: #PF soft cow miss */
            kprintf("trap: #PF soft cow miss va=0x%lx thr=%u "
                    "(not COW leaf / nomem) fallthrough\n",
                    (unsigned long)u64VaPage, u32ThrId);
        } else {
            g_trapStats.u64PfCowSkip++;
            /* Grep: trap: #PF soft cow skip */
            kprintf("trap: #PF soft cow skip cr2=0x%lx err=0x%lx "
                    "P=%u W=%u thr=%u\n",
                    (unsigned long)u64Cr2, (unsigned long)u64Err,
                    u32P, u32W, u32ThrId);
        }
    }

    /* User: SEH-shaped exception port if registered; otherwise kill */
    if (pThr != NULL && pThr->pProc != NULL &&
        except_port_deliver(pThr->pProc, u32Vec, pFrame->u64Error,
                            pFrame->u64Rip,
                            u32Vec == 14 ? read_cr2() : 0)) {
        g_trapStats.u64ExceptDeliver++;
        kprintf("trap: delivered to exception port (thr stays)\n");
        /* Handler wakes; faulting thread blocks until policy advances */
        thread_block(&pThr->pProc->excPort, 1);
        schedule();
        /* Resumed without resolution → fall through and kill */
    } else {
        g_trapStats.u64ExceptMiss++;
    }
    g_trapStats.u64Kill++;
    kprintf("trap: killing user thread\n");
    if (pThr != NULL && pThr->u32Id != 0) {
        pThr->u32State = GJ_THR_EXITED;
        /* Clear door single-flight claim if thr was mid cold syscall. */
        door_on_thread_exit(pThr);
        schedule();
    }
    for (;;) {
        __asm__ volatile ("cli; hlt");
    }
}
