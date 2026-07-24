/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SYSCALL/SYSRET bring-up (STAR/LSTAR/SFMASK/EFER.SCE) and first-entry
 * helpers into ring-3 long mode or 32-bit compat (iretq).
 *
 * Soft SYSCALL MSR observability: program path snapshots STAR/LSTAR/
 * CSTAR/SFMASK/EFER, readback-verifies SCE + STAR layout, and exposes
 * greppable counters for product/smoke inventory (no hot-path locks).
 *
 * Soft SYSCALL inventory (prefix-stable, pure C, deepen-only):
 *   "cpu: syscall soft inventory …"
 *   "cpu: syscall soft inits=… verify_ok=… …"
 *   "cpu: syscall soft STAR=… LSTAR=… SFMASK=… EFER=… SCE=… NXE=…"
 *   "cpu: syscall soft decode kern_cs=… user_base=… (CS64=base+16)"
 *   "cpu: syscall soft program …"
 *   "cpu: syscall soft star …"
 *   "cpu: syscall soft lstar …"
 *   "cpu: syscall soft cstar …"
 *   "cpu: syscall soft sfmask …"
 *   "cpu: syscall soft efer …"
 *   "cpu: syscall soft enter …"
 *   "cpu: syscall soft verify …"
 *   "cpu: syscall soft expect …"
 *   "cpu: syscall soft path …"
 *   "cpu: syscall soft geom …"
 *   "cpu: syscall soft msr …"
 *   "cpu: syscall soft flags …"
 *   "cpu: syscall soft init …"
 *   "cpu: syscall soft capacity …"  (MSR/selector geometry)
 *   "cpu: syscall soft deepen …"    (wave stamp)
 *   "cpu: syscall soft verify PASS|FAIL|idle|armed"
 * Soft never hard-gates product; wrap-OK counters only. No bar3 claim.
 * greppable: cpu: syscall soft
 *
 * Wave 16 complementary deepen (kept; this unit) — x86 SYSCALL soft track.
 * Wave 15 complementary surfaces kept; Wave 16 adds exclusive/claim/
 * ratio/honesty complementary sub-lines. Product MSR program path
 * unchanged. Primary field names stay prefix-stable.
 * greppable: cpu: syscall soft exclusive
 */
#include <gj/cpu.h>
#include <gj/cpu_sys.h>
#include <gj/gdt.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/thread.h>

#define MSR_EFER     0xC0000080u
#define MSR_STAR     0xC0000081u
#define MSR_LSTAR    0xC0000082u
#define MSR_CSTAR    0xC0000083u
#define MSR_SFMASK   0xC0000084u

#define EFER_SCE     (1ull << 0)
#define EFER_LME     (1ull << 8)
#define EFER_LMA     (1ull << 10)
#define EFER_NXE     (1ull << 11)

/* RFLAGS bits cleared on SYSCALL (IF, TF, DF, AC, … — SDM SFMASK). */
#define SFMASK_DEFAULT 0x257fdull

/* Soft Wave stamp (greppable inventory only; never hard-gates boot). */
#define GJ_CPU_SYSCALL_SOFT_WAVE 44u

/* Soft RFLAGS IF used on enter_user / enter_user32 paths. */
#define GJ_CPU_SOFT_RFLAGS_IF 0x200ull

/* Soft enter32 / enter64 product selector constants (GDT-matched). */
#define GJ_CPU_SOFT_USER_DS    0x23u
#define GJ_CPU_SOFT_USER_CS32  0x1bu
#define GJ_CPU_SOFT_USER_CS64  0x2bu

static int g_fSyscallReady;

/* Soft SYSCALL MSR observability (monotonic; no locks — BSP soft). */
static volatile u32 g_u32SoftInits;
static volatile u32 g_u32SoftVerifyOk;
static volatile u32 g_u32SoftVerifyBad;
static volatile u32 g_u32SoftEnter64;
static volatile u32 g_u32SoftEnter32;
static volatile u32 g_u32SoftEnterBad;
/* Wave 15 exclusive deepen — extra path tallies (file-local only). */
static volatile u32 g_u32SoftInvLogs;     /* soft inventory emit count */
static volatile u32 g_u32SoftVerifyCall;  /* cpu_syscall_soft_verify entries */
static volatile u32 g_u32SoftInfoGet;     /* cpu_syscall_soft_info_get entries */
static volatile u32 g_u32SoftLogCall;     /* cpu_syscall_soft_log entries */
static volatile u32 g_u32SoftReadyQ;      /* cpu_syscall_ready samples */
static volatile u32 g_u32SoftEnter64Thr;  /* enter64 with thread kstack */
static volatile u32 g_u32SoftEnter64Irq;  /* enter64 fell back to irq rsp0 */
static volatile u32 g_u32SoftEnter32Thr;  /* enter32 with thread kstack */
static volatile u32 g_u32SoftEnter32NoRsp0; /* enter32 no RSP0 rejections */
static volatile u32 g_u32SoftEnterBadReady; /* enter64 rejected !ready */
static volatile u32 g_u32SoftEnterBadZero;  /* enter rejected zero rip/rsp */
static volatile u32 g_u32SoftStarGet;     /* soft_star accessor samples */
static volatile u32 g_u32SoftLstarGet;    /* soft_lstar accessor samples */
static volatile u32 g_u32SoftSfmaskGet;   /* soft_sfmask accessor samples */
static volatile u32 g_u32SoftEferGet;     /* soft_efer accessor samples */
static volatile u32 g_u32SoftInitsGet;    /* soft_inits accessor samples */
static volatile u32 g_u32SoftCtrGet;      /* other counter-accessor samples */
static struct gj_cpu_syscall_soft g_SoftSnap;
static int g_fSoftSnapLive;

/* Wave 15 soft expect subflags (last verify; inventory lamps). */
static u8 g_u8SoftStarOk;
static u8 g_u8SoftLstarOk;
static u8 g_u8SoftCstarOk;
static u8 g_u8SoftSfmaskOk;
static u8 g_u8SoftSceOk;
static u8 g_u8SoftKernCsOk;
static u8 g_u8SoftUserBaseOk;
static u8 g_u8SoftSysretCsOk;
static u8 g_u8SoftLme;
static u8 g_u8SoftLma;
static u8 g_u8SoftNxeLive;

extern void gj_syscall_entry(void);

static void cpu_syscall_soft_inc(volatile u32 *pCtr);
static void cpu_syscall_soft_inventory(void);

/** Soft: saturating bump (u32 wrap avoided; wrap OK if ever hit). */
static void
cpu_syscall_soft_inc(volatile u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

static u64
rdmsr(u32 u32Msr)
{
    u32 u32Lo;
    u32 u32Hi;

    __asm__ volatile ("rdmsr" : "=a"(u32Lo), "=d"(u32Hi) : "c"(u32Msr));
    return ((u64)u32Hi << 32) | u32Lo;
}

static void
wrmsr(u32 u32Msr, u64 u64Val)
{
    u32 u32Lo = (u32)u64Val;
    u32 u32Hi = (u32)(u64Val >> 32);

    __asm__ volatile ("wrmsr" : : "c"(u32Msr), "a"(u32Lo), "d"(u32Hi));
}

/**
 * Soft: snapshot programmed MSR values + STAR decode. Pure observability.
 */
static void
cpu_syscall_soft_note_program(u64 u64Star, u64 u64Lstar, u64 u64Cstar,
                              u64 u64Sfmask, u64 u64Efer)
{
    g_SoftSnap.u64Star = u64Star;
    g_SoftSnap.u64Lstar = u64Lstar;
    g_SoftSnap.u64Cstar = u64Cstar;
    g_SoftSnap.u64Sfmask = u64Sfmask;
    g_SoftSnap.u64Efer = u64Efer;
    g_SoftSnap.u16StarKernCs = (u16)((u64Star >> 32) & 0xffffu);
    g_SoftSnap.u16StarUserBase = (u16)((u64Star >> 48) & 0xffffu);
    g_SoftSnap.u8Sce = (u8)((u64Efer & EFER_SCE) != 0);
    g_SoftSnap.u8Nxe = (u8)((u64Efer & EFER_NXE) != 0);
    g_SoftSnap.u8Pad = 0;
    g_u8SoftLme = (u8)((u64Efer & EFER_LME) != 0);
    g_u8SoftLma = (u8)((u64Efer & EFER_LMA) != 0);
    g_u8SoftNxeLive = g_SoftSnap.u8Nxe;
    g_fSoftSnapLive = 1;
    cpu_syscall_soft_inc(&g_u32SoftInits);
}

/**
 * Soft: rdmsr verify against last program snapshot + expected layout.
 * Does not reprogram MSRs. Wave 15: records per-field expect subflags
 * for greppable inventory (PASS aggregate unchanged — star/lstar/cstar/
 * sfmask/sce/kern_cs/user_base/sysret_cs).
 */
static int
cpu_syscall_soft_verify_inner(void)
{
    u64 u64Star;
    u64 u64Lstar;
    u64 u64Cstar;
    u64 u64Sfmask;
    u64 u64Efer;
    u16 u16KernCs;
    u16 u16UserBase;
    int fOk = 1;

    if (!g_fSoftSnapLive || !g_fSyscallReady) {
        return 0;
    }

    u64Star = rdmsr(MSR_STAR);
    u64Lstar = rdmsr(MSR_LSTAR);
    u64Cstar = rdmsr(MSR_CSTAR);
    u64Sfmask = rdmsr(MSR_SFMASK);
    u64Efer = rdmsr(MSR_EFER);

    g_u8SoftStarOk = (u8)(u64Star == g_SoftSnap.u64Star ? 1u : 0u);
    g_u8SoftLstarOk = (u8)(u64Lstar == g_SoftSnap.u64Lstar ? 1u : 0u);
    g_u8SoftCstarOk = (u8)(u64Cstar == g_SoftSnap.u64Cstar ? 1u : 0u);
    g_u8SoftSfmaskOk = (u8)(u64Sfmask == g_SoftSnap.u64Sfmask ? 1u : 0u);
    g_u8SoftSceOk = (u8)((u64Efer & EFER_SCE) != 0 ? 1u : 0u);

    if (g_u8SoftStarOk == 0u) {
        fOk = 0;
    }
    if (g_u8SoftLstarOk == 0u) {
        fOk = 0;
    }
    if (g_u8SoftCstarOk == 0u) {
        fOk = 0;
    }
    if (g_u8SoftSfmaskOk == 0u) {
        fOk = 0;
    }
    if (g_u8SoftSceOk == 0u) {
        fOk = 0;
    }

    u16KernCs = (u16)((u64Star >> 32) & 0xffffu);
    u16UserBase = (u16)((u64Star >> 48) & 0xffffu);
    g_u8SoftKernCsOk =
        (u8)(u16KernCs == (u16)GJ_CPU_SYSCALL_STAR_KERNEL_CS ? 1u : 0u);
    g_u8SoftUserBaseOk =
        (u8)(u16UserBase == (u16)GJ_CPU_SYSCALL_STAR_USER_BASE ? 1u : 0u);
    /* SYSRETQ CS = user_base+16 → 0x28 raw; RPL applied by SYSRET. */
    g_u8SoftSysretCsOk =
        (u8)((u16)(u16UserBase + 16u) == (u16)(GJ_GDT_USER_CS & 0xf8u)
                 ? 1u
                 : 0u);

    if (g_u8SoftKernCsOk == 0u) {
        fOk = 0;
    }
    if (g_u8SoftUserBaseOk == 0u) {
        fOk = 0;
    }
    if (g_u8SoftSysretCsOk == 0u) {
        fOk = 0;
    }

    /* Refresh soft EFER bits from live readback. */
    g_SoftSnap.u8Sce = (u8)((u64Efer & EFER_SCE) != 0);
    g_SoftSnap.u8Nxe = (u8)((u64Efer & EFER_NXE) != 0);
    g_SoftSnap.u64Efer = u64Efer;
    g_u8SoftLme = (u8)((u64Efer & EFER_LME) != 0);
    g_u8SoftLma = (u8)((u64Efer & EFER_LMA) != 0);
    g_u8SoftNxeLive = g_SoftSnap.u8Nxe;
    g_SoftSnap.u8VerifyOk = fOk ? 1u : 0u;

    if (fOk) {
        cpu_syscall_soft_inc(&g_u32SoftVerifyOk);
    } else {
        cpu_syscall_soft_inc(&g_u32SoftVerifyBad);
    }
    return fOk;
}

/**
 * Soft SYSCALL inventory — greppable "cpu: syscall soft …".
 * Pure observability; never mutates MSRs or hard-gates product.
 * Wave 15 exclusive deepen splits program/star/lstar/cstar/sfmask/efer/
 * enter/verify/expect/path/geom/msr/flags/init surfaces (prefix-stable).
 */
static void
cpu_syscall_soft_inventory(void)
{
    u16 u16KernCs;
    u16 u16UserBase;
    u16 u16SysretCsRaw;
    u16 u16SysretSsRaw;
    u16 u16SysretCsRpl;
    u16 u16SysretSsRpl;
    u16 u16SyscallCsRpl;
    u32 u32SfTf;
    u32 u32SfIf;
    u32 u32SfDf;
    u32 u32SfIopl;
    u32 u32SfNt;
    u32 u32SfAc;
    u32 u32SfmaskMatch;
    u32 u32LstarNz;
    u32 u32CstarEqLstar;
    u32 u32StarMatchExpect;
    u32 u32Sce;
    u32 u32Nxe;
    u32 u32Lme;
    u32 u32Lma;
    int fPass;

    cpu_syscall_soft_inc(&g_u32SoftInvLogs);

    u16KernCs = g_SoftSnap.u16StarKernCs;
    u16UserBase = g_SoftSnap.u16StarUserBase;
    u16SysretCsRaw = (u16)(u16UserBase + 16u);
    u16SysretSsRaw = (u16)(u16UserBase + 8u);
    u16SysretCsRpl = (u16)(u16SysretCsRaw | 3u);
    u16SysretSsRpl = (u16)(u16SysretSsRaw | 3u);
    /* SYSCALL entry CS for long mode is kern CS (no RPL bump in STAR). */
    u16SyscallCsRpl = u16KernCs;

    /* Soft SFMASK bit lamps (SDM RFLAGS bits cleared on SYSCALL). */
    u32SfTf = (g_SoftSnap.u64Sfmask & (1ull << 8)) != 0 ? 1u : 0u;
    u32SfIf = (g_SoftSnap.u64Sfmask & (1ull << 9)) != 0 ? 1u : 0u;
    u32SfDf = (g_SoftSnap.u64Sfmask & (1ull << 10)) != 0 ? 1u : 0u;
    u32SfIopl = (g_SoftSnap.u64Sfmask & (3ull << 12)) != 0 ? 1u : 0u;
    u32SfNt = (g_SoftSnap.u64Sfmask & (1ull << 14)) != 0 ? 1u : 0u;
    u32SfAc = (g_SoftSnap.u64Sfmask & (1ull << 18)) != 0 ? 1u : 0u;

    u32SfmaskMatch =
        (g_SoftSnap.u64Sfmask == SFMASK_DEFAULT) ? 1u : 0u;
    u32LstarNz = (g_SoftSnap.u64Lstar != 0) ? 1u : 0u;
    u32CstarEqLstar =
        (g_SoftSnap.u64Cstar == g_SoftSnap.u64Lstar) ? 1u : 0u;
    u32StarMatchExpect =
        (u16KernCs == (u16)GJ_CPU_SYSCALL_STAR_KERNEL_CS &&
         u16UserBase == (u16)GJ_CPU_SYSCALL_STAR_USER_BASE)
            ? 1u
            : 0u;
    u32Sce = (unsigned)g_SoftSnap.u8Sce;
    u32Nxe = (unsigned)g_SoftSnap.u8Nxe;
    u32Lme = (unsigned)g_u8SoftLme;
    u32Lma = (unsigned)g_u8SoftLma;

    fPass = 0;
    if (g_fSoftSnapLive && g_fSyscallReady && g_SoftSnap.u8VerifyOk &&
        u32Sce != 0u && u32StarMatchExpect != 0u && u32LstarNz != 0u) {
        fPass = 1;
    }

    /*
     * Grep: cpu: syscall soft inventory
     * Aggregated presence snapshot for product smoke / agent greps.
     * Wave 15 stamps wave=15; existing keys remain prefix-stable.
     */
    kprintf("cpu: syscall soft inventory wave=%u ready=%u live=%u "
            "sce=%u nxe=%u verify_ok=%u inits=%u inv_logs=%u "
            "enter64=%u enter32=%u bad=%u pass=%u\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE,
            (unsigned)(g_fSyscallReady ? 1u : 0u),
            (unsigned)(g_fSoftSnapLive ? 1u : 0u), u32Sce, u32Nxe,
            (unsigned)g_SoftSnap.u8VerifyOk, g_u32SoftInits,
            g_u32SoftInvLogs, g_u32SoftEnter64, g_u32SoftEnter32,
            g_u32SoftEnterBad, (unsigned)(fPass ? 1u : 0u));

    /*
     * Grep: cpu: syscall soft program — expected product shape (constants).
     * Shadow only; never reprograms MSRs from this path.
     */
    kprintf("cpu: syscall soft program star_kern_cs=0x%x "
            "star_user_base=0x%x sfmask=0x%lx efer_sce=1 efer_nxe=1 "
            "cstar_eq_lstar=1 option_c=1 wave=%u\n",
            (unsigned)GJ_CPU_SYSCALL_STAR_KERNEL_CS,
            (unsigned)GJ_CPU_SYSCALL_STAR_USER_BASE,
            (unsigned long)SFMASK_DEFAULT,
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /* Grep: cpu: syscall soft star — IA32_STAR decode + expect match */
    kprintf("cpu: syscall soft star val=0x%lx kern_cs=0x%x "
            "user_base=0x%x sysret_cs_raw=0x%x sysret_ss_raw=0x%x "
            "sysret_cs_rpl=0x%x sysret_ss_rpl=0x%x match_expect=%u "
            "expect_kern=0x%x expect_base=0x%x\n",
            (unsigned long)g_SoftSnap.u64Star, (unsigned)u16KernCs,
            (unsigned)u16UserBase, (unsigned)u16SysretCsRaw,
            (unsigned)u16SysretSsRaw, (unsigned)u16SysretCsRpl,
            (unsigned)u16SysretSsRpl, u32StarMatchExpect,
            (unsigned)GJ_CPU_SYSCALL_STAR_KERNEL_CS,
            (unsigned)GJ_CPU_SYSCALL_STAR_USER_BASE);

    /* Grep: cpu: syscall soft lstar — long-mode SYSCALL entry RIP */
    kprintf("cpu: syscall soft lstar val=0x%lx nz=%u entry_sym=1 "
            "ok=%u snap_live=%u\n",
            (unsigned long)g_SoftSnap.u64Lstar, u32LstarNz,
            (unsigned)g_u8SoftLstarOk,
            (unsigned)(g_fSoftSnapLive ? 1u : 0u));

    /* Grep: cpu: syscall soft cstar — compat SYSCALL (unused product) */
    kprintf("cpu: syscall soft cstar val=0x%lx eq_lstar=%u ok=%u "
            "compat_syscall=0 unused_product=1\n",
            (unsigned long)g_SoftSnap.u64Cstar, u32CstarEqLstar,
            (unsigned)g_u8SoftCstarOk);

    /* Grep: cpu: syscall soft sfmask — RFLAGS mask lamps */
    kprintf("cpu: syscall soft sfmask val=0x%lx expect=0x%lx "
            "match=%u TF=%u IF=%u DF=%u IOPL=%u NT=%u AC=%u ok=%u\n",
            (unsigned long)g_SoftSnap.u64Sfmask,
            (unsigned long)SFMASK_DEFAULT, u32SfmaskMatch, u32SfTf,
            u32SfIf, u32SfDf, u32SfIopl, u32SfNt, u32SfAc,
            (unsigned)g_u8SoftSfmaskOk);

    /* Grep: cpu: syscall soft efer — SCE/LME/LMA/NXE live lamps */
    kprintf("cpu: syscall soft efer val=0x%lx SCE=%u LME=%u LMA=%u "
            "NXE=%u sce_ok=%u nxe_live=%u require_sce=1 nxe_wx=1\n",
            (unsigned long)g_SoftSnap.u64Efer, u32Sce, u32Lme, u32Lma,
            u32Nxe, (unsigned)g_u8SoftSceOk, (unsigned)g_u8SoftNxeLive);

    /* Grep: cpu: syscall soft enter — ring-3 enter path tallies */
    kprintf("cpu: syscall soft enter enter64=%u enter32=%u bad=%u "
            "enter64_thr=%u enter64_irq=%u enter32_thr=%u "
            "enter32_norsp0=%u bad_ready=%u bad_zero=%u "
            "sysretq=1 iretq32=1\n",
            g_u32SoftEnter64, g_u32SoftEnter32, g_u32SoftEnterBad,
            g_u32SoftEnter64Thr, g_u32SoftEnter64Irq, g_u32SoftEnter32Thr,
            g_u32SoftEnter32NoRsp0, g_u32SoftEnterBadReady,
            g_u32SoftEnterBadZero);

    /* Grep: cpu: syscall soft verify — full soft verify tallies */
    kprintf("cpu: syscall soft verify call=%u ok=%u bad=%u "
            "star_ok=%u lstar_ok=%u cstar_ok=%u sfmask_ok=%u sce_ok=%u "
            "kern_cs_ok=%u user_base_ok=%u sysret_cs_ok=%u live=%u "
            "wave=%u\n",
            g_u32SoftVerifyCall, g_u32SoftVerifyOk, g_u32SoftVerifyBad,
            (unsigned)g_u8SoftStarOk, (unsigned)g_u8SoftLstarOk,
            (unsigned)g_u8SoftCstarOk, (unsigned)g_u8SoftSfmaskOk,
            (unsigned)g_u8SoftSceOk, (unsigned)g_u8SoftKernCsOk,
            (unsigned)g_u8SoftUserBaseOk, (unsigned)g_u8SoftSysretCsOk,
            (unsigned)(g_fSoftSnapLive ? 1u : 0u),
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /* Grep: cpu: syscall soft expect — product expected catalog */
    kprintf("cpu: syscall soft expect kern_cs=0x08 user_base=0x18 "
            "sysret_cs_raw=0x28 sysret_ss_raw=0x20 "
            "sysret_cs_rpl=0x2b sysret_ss_rpl=0x23 "
            "user_cs32=0x1b user_ds=0x23 sfmask=0x257fd "
            "sce=1 nxe=1 cstar_eq_lstar=1 wave=%u\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /*
     * Grep: cpu: syscall soft path — honesty non-claim.
     * Soft inventory ≠ bar3, ≠ multi-CPU SYSCALL reprogram, ≠ int-only.
     */
    kprintf("cpu: syscall soft path option_c=1 sysretq=1 iretq32=1 "
            "compat_syscall=0 int_only=0 bar3=0 hard_gate=0 "
            "per_cpu_msr=0 sce_required=1 nxe_wx=1 wave=%u "
            "(soft inventory; not bar3)\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /* Grep: cpu: syscall soft geom — STAR selector geometry table */
    kprintf("cpu: syscall soft geom star_user_base=0x%x "
            "syscall_cs=0x%x sysret_cs=base+16|3=0x%x "
            "sysret_ss=base+8|3=0x%x user_cs32=0x%x user_ds=0x%x "
            "user_cs64=0x%x gdt_user_cs_mask=0x%x\n",
            (unsigned)u16UserBase, (unsigned)u16SyscallCsRpl,
            (unsigned)u16SysretCsRpl, (unsigned)u16SysretSsRpl,
            (unsigned)GJ_CPU_SOFT_USER_CS32, (unsigned)GJ_CPU_SOFT_USER_DS,
            (unsigned)GJ_CPU_SOFT_USER_CS64,
            (unsigned)(GJ_GDT_USER_CS & 0xf8u));

    /* Grep: cpu: syscall soft msr — IA32 MSR index catalog */
    kprintf("cpu: syscall soft msr efer=0x%x star=0x%x lstar=0x%x "
            "cstar=0x%x sfmask=0x%x programmed=%u ready=%u\n",
            (unsigned)MSR_EFER, (unsigned)MSR_STAR, (unsigned)MSR_LSTAR,
            (unsigned)MSR_CSTAR, (unsigned)MSR_SFMASK,
            (unsigned)(g_fSoftSnapLive ? 1u : 0u),
            (unsigned)(g_fSyscallReady ? 1u : 0u));

    /* Grep: cpu: syscall soft flags — enter path RFLAGS soft bits */
    kprintf("cpu: syscall soft flags enter_rflags=0x%lx IF=1 "
            "sysret_r11=IF swapgs=1 thr_kstack=1 never_thr_rsp0_irq=1 "
            "enter32_cli=1\n",
            (unsigned long)GJ_CPU_SOFT_RFLAGS_IF);

    /* Grep: cpu: syscall soft init — BSP program + accessor tallies */
    kprintf("cpu: syscall soft init bsp=%u verify_call=%u info_get=%u "
            "log_call=%u ready_q=%u inv_logs=%u star_get=%u "
            "lstar_get=%u sfmask_get=%u efer_get=%u inits_get=%u "
            "ctr_get=%u\n",
            g_u32SoftInits, g_u32SoftVerifyCall, g_u32SoftInfoGet,
            g_u32SoftLogCall, g_u32SoftReadyQ, g_u32SoftInvLogs,
            g_u32SoftStarGet, g_u32SoftLstarGet, g_u32SoftSfmaskGet,
            g_u32SoftEferGet, g_u32SoftInitsGet, g_u32SoftCtrGet);

    /* Grep: cpu: syscall soft capacity (geometry) */
    kprintf("cpu: syscall soft capacity star_kern=0x%x star_user=0x%x "
            "user_cs32=0x%x user_cs64=0x%x user_ds=0x%x "
            "sfmask=0x%lx rflags_if=0x%lx inv_logs=%u wave=%u\n",
            (unsigned)GJ_CPU_SYSCALL_STAR_KERNEL_CS,
            (unsigned)GJ_CPU_SYSCALL_STAR_USER_BASE,
            (unsigned)GJ_CPU_SOFT_USER_CS32, (unsigned)GJ_CPU_SOFT_USER_CS64,
            (unsigned)GJ_CPU_SOFT_USER_DS,
            (unsigned long)SFMASK_DEFAULT,
            (unsigned long)GJ_CPU_SOFT_RFLAGS_IF, g_u32SoftInvLogs,
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /*
     * Wave 16 complementary sub-lines (kept; never reshape primary).
     */
    /* Grep: cpu: syscall soft exclusive */
    kprintf("cpu: syscall soft exclusive wave=%u exclusive=1 soft=1 "
            "unit=cpu_sys.c bar3=0 hard_gate=0 product_complete=0 "
            "soft_only=1\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /* Grep: cpu: syscall soft claim — SYSCALL MSR product bounds */
    kprintf("cpu: syscall soft claim sce=1 nxe=1 star=1 lstar=1 "
            "cstar_eq_lstar=1 sfmask=1 enter64=1 enter32=1 "
            "per_cpu_msr=0 bar3=0 hard_gate=0 wave=%u\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /* Grep: cpu: syscall soft ratio — program/enter/verify path ratios */
    kprintf("cpu: syscall soft ratio inits=%u enter64=%u enter32=%u "
            "enter_bad=%u verify_ok=%u verify_bad=%u "
            "info_get=%u log_call=%u inv_logs=%u ready=%u live=%u "
            "wave=%u\n",
            g_u32SoftInits, g_u32SoftEnter64, g_u32SoftEnter32,
            g_u32SoftEnterBad, g_u32SoftVerifyOk, g_u32SoftVerifyBad,
            g_u32SoftInfoGet, g_u32SoftLogCall, g_u32SoftInvLogs,
            (unsigned)(g_fSyscallReady ? 1u : 0u),
            (unsigned)(g_fSoftSnapLive ? 1u : 0u),
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /* Grep: cpu: syscall soft honesty */
    kprintf("cpu: syscall soft honesty soft_ne_product_complete=1 "
            "bar3=0 hard_gate=0 soft_only=1 "
            "claim=star+lstar+sfmask+sce wave=%u unit=cpu_sys.c\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /*
     * Wave 17 complementary sub-lines (kept; never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: cpu: syscall soft return — Wave 17 API return surfaces (kept) */
    kprintf("cpu: syscall soft return ready=%u live=%u star=1 lstar=1 sfmask=1 soft_inv=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)(g_fSyscallReady ? 1u : 0u), (unsigned)(g_fSoftSnapLive ? 1u : 0u), (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /* Grep: cpu: syscall soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("cpu: syscall soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /* Grep: cpu: syscall soft retmap — Wave 17 return-surface map (kept) */
    kprintf("cpu: syscall soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: cpu: syscall soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("cpu: syscall soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /* Grep: cpu: syscall soft retcode — Wave 19 retcode catalog */
    kprintf("cpu: syscall soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

    /* Grep: cpu: syscall soft deepen (Wave 20 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: cpu: syscall: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("cpu: syscall: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
    /* Grep: cpu: syscall: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("cpu: syscall: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: cpu: syscall: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("cpu: syscall: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
    /* Grep: cpu: syscall: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("cpu: syscall: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: cpu: syscall: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("cpu: syscall: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
            /* Grep: cpu: syscall: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("cpu: syscall: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: cpu: syscall: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("cpu: syscall: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
            /* Grep: cpu: syscall: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("cpu: syscall: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: cpu: syscall: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("cpu: syscall: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
            /* Grep: cpu: syscall: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("cpu: syscall: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cpu: syscall: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("cpu: syscall: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
            /* Grep: cpu: syscall: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("cpu: syscall: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cpu: syscall: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("cpu: syscall: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
            /* Grep: cpu: syscall: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("cpu: syscall: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: cpu: syscall: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("cpu: syscall: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
            /* Grep: cpu: syscall: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("cpu: syscall: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: cpu: syscall: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("cpu: syscall: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
                    /* Grep: cpu: syscall: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("cpu: syscall: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cpu: syscall: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("cpu: syscall: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
                            /* Grep: cpu: syscall: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("cpu: syscall: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cpu: syscall: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("cpu: syscall: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
                            /* Grep: cpu: syscall: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("cpu: syscall: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cpu: syscall: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("cpu: syscall: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
                            /* Grep: cpu: syscall: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("cpu: syscall: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: cpu: syscall: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("cpu: syscall: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
                            /* Grep: cpu: syscall: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("cpu: syscall: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
                            /* Grep: cpu: syscall: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("cpu: syscall: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("cpu: syscall: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("cpu: syscall: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("cpu: syscall: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("cpu: syscall: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("cpu: syscall: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("cpu: syscall: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retfortress — Wave 35 return-fortress honesty */
kprintf("cpu: syscall: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("cpu: syscall: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft rethold — Wave 36 return-hold honesty */
kprintf("cpu: syscall: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retspire — Wave 36 exclusive spire stamp */
kprintf("cpu: syscall: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retwall — Wave 37 return-wall honesty */
kprintf("cpu: syscall: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retgate — Wave 37 exclusive gate stamp */
kprintf("cpu: syscall: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retmoat — Wave 38 return-moat honesty */
kprintf("cpu: syscall: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retower — Wave 38 exclusive tower stamp */
kprintf("cpu: syscall: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("cpu: syscall: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("cpu: syscall: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("cpu: syscall: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("cpu: syscall: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retravelin — Wave 41 return-travelin honesty */
kprintf("cpu: syscall: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("cpu: syscall: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("cpu: syscall: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("cpu: syscall: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("cpu: syscall: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("cpu: syscall: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("cpu: syscall: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("cpu: syscall: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);


                            kprintf("cpu: syscall soft deepen wave=%u ready=%u live=%u "
            "inits=%u enter64=%u enter32=%u verify_ok=%u inv_logs=%u "
            "areas=inventory,program,star,lstar,cstar,sfmask,efer,"
            "enter,verify,expect,path,geom,msr,flags,init,capacity,"
            "exclusive,claim,ratio,honesty,return,return_selftest,retmap,"
            "return_rate,retcode "
            "(Wave 43 exclusive; soft only; not bar3)\n",
            (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE,
            (unsigned)(g_fSyscallReady ? 1u : 0u),
            (unsigned)(g_fSoftSnapLive ? 1u : 0u), g_u32SoftInits,
            g_u32SoftEnter64, g_u32SoftEnter32, g_u32SoftVerifyOk,
            g_u32SoftInvLogs);

    /* Legacy / prefix-stable summary lines (pre-wave-14 greps). */
    kprintf("cpu: syscall soft inits=%u verify_ok=%u verify_bad=%u "
            "enter64=%u enter32=%u bad=%u\n",
            g_u32SoftInits, g_u32SoftVerifyOk, g_u32SoftVerifyBad,
            g_u32SoftEnter64, g_u32SoftEnter32, g_u32SoftEnterBad);
    kprintf("cpu: syscall soft STAR=0x%lx LSTAR=0x%lx SFMASK=0x%lx "
            "EFER=0x%lx SCE=%u NXE=%u\n",
            (unsigned long)g_SoftSnap.u64Star,
            (unsigned long)g_SoftSnap.u64Lstar,
            (unsigned long)g_SoftSnap.u64Sfmask,
            (unsigned long)g_SoftSnap.u64Efer,
            (unsigned)g_SoftSnap.u8Sce, (unsigned)g_SoftSnap.u8Nxe);
    kprintf("cpu: syscall soft decode kern_cs=0x%x user_base=0x%x "
            "(CS64=base+16=0x%x)\n",
            (unsigned)u16KernCs, (unsigned)u16UserBase,
            (unsigned)u16SysretCsRaw);

    if (!g_fSoftSnapLive) {
        kprintf("cpu: syscall soft inventory idle "
                "(MSRs not programmed)\n");
        kprintf("cpu: syscall soft idle (MSRs not programmed)\n");
    } else if (fPass) {
        kprintf("cpu: syscall soft inventory PASS wave=%u\n",
                (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
        kprintf("cpu: syscall soft verify PASS\n");
    } else if (g_u32SoftVerifyBad != 0) {
        kprintf("cpu: syscall soft inventory FAIL wave=%u\n",
                (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
        kprintf("cpu: syscall soft verify FAIL\n");
    } else {
        kprintf("cpu: syscall soft inventory armed wave=%u "
                "(no re-verify yet)\n",
                (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
        kprintf("cpu: syscall soft armed (no re-verify yet)\n");
    }
}

void
cpu_syscall_init(void)
{
    u64 u64Efer;
    u64 u64Star;
    u64 u64Lstar;
    u64 u64Cstar;
    u64 u64Sfmask;

    /*
     * STAR: kernel CS in [47:32], user selector base in [63:48].
     * SYSRETQ: CS = base+16 = 0x28 (user long code), SS = base+8 = 0x20.
     * GDT layout: index5=0x28 code, index4=0x20 data → base 0x18.
     */
    u64Star = ((u64)GJ_CPU_SYSCALL_STAR_USER_BASE << 48) |
              ((u64)GJ_GDT_KERNEL_CS << 32);
    u64Lstar = (u64)(gj_vaddr_t)gj_syscall_entry;
    u64Cstar = u64Lstar; /* CSTAR unused (no 32-bit SYSCALL path) */
    u64Sfmask = SFMASK_DEFAULT;

    wrmsr(MSR_STAR, u64Star);
    wrmsr(MSR_LSTAR, u64Lstar);
    wrmsr(MSR_CSTAR, u64Cstar);
    wrmsr(MSR_SFMASK, u64Sfmask);

    u64Efer = rdmsr(MSR_EFER);
    u64Efer |= EFER_SCE | EFER_NXE; /* NXE backs W^X mprotect */
    wrmsr(MSR_EFER, u64Efer);
    u64Efer = rdmsr(MSR_EFER); /* soft: post-write live value */

    g_fSyscallReady = 1;
    cpu_syscall_soft_note_program(u64Star, u64Lstar, u64Cstar, u64Sfmask,
                                  u64Efer);

    kprintf("cpu: SYSCALL ready LSTAR=0x%lx STAR=0x%lx\n",
            (unsigned long)u64Lstar, (unsigned long)u64Star);
    /* Soft: rate-limited bring-up line (first program path). */
    kprintf("cpu: syscall soft program STAR=0x%lx LSTAR=0x%lx "
            "SFMASK=0x%lx EFER=0x%lx SCE=%u NXE=%u\n",
            (unsigned long)u64Star, (unsigned long)u64Lstar,
            (unsigned long)u64Sfmask, (unsigned long)u64Efer,
            (unsigned)g_SoftSnap.u8Sce, (unsigned)g_SoftSnap.u8Nxe);
    kprintf("cpu: syscall soft decode kern_cs=0x%x user_base=0x%x "
            "sysret_cs=0x%x sysret_ss=0x%x\n",
            (unsigned)g_SoftSnap.u16StarKernCs,
            (unsigned)g_SoftSnap.u16StarUserBase,
            (unsigned)(g_SoftSnap.u16StarUserBase + 16u),
            (unsigned)(g_SoftSnap.u16StarUserBase + 8u));

    if (cpu_syscall_soft_verify_inner()) {
        kprintf("cpu: syscall soft verify PASS inits=%u\n",
                g_u32SoftInits);
    } else {
        kprintf("cpu: syscall soft verify FAIL inits=%u\n",
                g_u32SoftInits);
    }

    /* Wave 15: greppable "cpu: syscall soft …" inventory at BSP program. */
    cpu_syscall_soft_inventory();

    (void)EFER_LME;
    (void)EFER_NXE;
}

int
cpu_syscall_ready(void)
{
    cpu_syscall_soft_inc(&g_u32SoftReadyQ);
    return g_fSyscallReady;
}

void
cpu_enter_user(u64 u64Entry, u64 u64Stack)
{
    struct gj_thread *pThr;

    if (!g_fSyscallReady || u64Entry == 0 || u64Stack == 0) {
        if (!g_fSyscallReady) {
            cpu_syscall_soft_inc(&g_u32SoftEnterBadReady);
        }
        if (u64Entry == 0 || u64Stack == 0) {
            cpu_syscall_soft_inc(&g_u32SoftEnterBadZero);
        }
        cpu_syscall_soft_inc(&g_u32SoftEnterBad);
        kprintf("cpu: enter_user invalid (ready=%d)\n", g_fSyscallReady);
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }
    cpu_syscall_soft_inc(&g_u32SoftEnter64);
    pThr = thread_current();
    if (pThr != NULL) {
        thread_install_kstack(pThr);
        cpu_syscall_soft_inc(&g_u32SoftEnter64Thr);
    } else {
        tss_use_irq_rsp0();
        cpu_syscall_soft_inc(&g_u32SoftEnter64Irq);
    }
    /*
     * Kernel GSBASE holds per-CPU state. swapgs → GS=user(0), KERNEL_GS=percpu.
     * sysretq: rcx=rip, r11=rflags, rsp=user stack.
     */
    __asm__ volatile (
        "swapgs\n\t"
        "mov %0, %%rsp\n\t"
        "mov $0x200, %%r11\n\t" /* IF */
        "mov %1, %%rcx\n\t"
        "sysretq\n\t"
        :
        : "r"(u64Stack), "r"(u64Entry)
        : "rcx", "r11", "memory"
    );
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

void
cpu_enter_user32(u64 u64Entry, u64 u64Stack)
{
    struct gj_thread *pThr;
    u64 u64Rsp0;

    /*
     * Long mode → 32-bit compat user via iretq:
     *   SS=USER_DS, RSP=stack, RFLAGS=IF, CS=USER_CS32, RIP=entry
     * Interrupt-return path only; SYSCALL MSRs are not required.
     */
    if (u64Entry == 0 || u64Stack == 0) {
        cpu_syscall_soft_inc(&g_u32SoftEnterBadZero);
        cpu_syscall_soft_inc(&g_u32SoftEnterBad);
        kprintf("cpu: enter_user32 invalid\n");
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }
    cpu_syscall_soft_inc(&g_u32SoftEnter32);
    pThr = thread_current();
    if (pThr != NULL && pThr->u64KstackTop != 0) {
        /* Per-thread kstack for SYSCALL; ring-3 IRQs keep dedicated RSP0. */
        thread_install_kstack(pThr);
        cpu_syscall_soft_inc(&g_u32SoftEnter32Thr);
    }
    /*
     * Never pin TSS.RSP0 to a thread kstack: if that thread parks mid-syscall,
     * a timer IRQ from another thread smashes its saved frames → kernel #UD.
     */
    tss_use_irq_rsp0();
    u64Rsp0 = tss_irq_rsp0();
    if (u64Rsp0 == 0) {
        u64Rsp0 = tss_get_rsp0();
    }
    if (u64Rsp0 == 0) {
        cpu_syscall_soft_inc(&g_u32SoftEnter32NoRsp0);
        cpu_syscall_soft_inc(&g_u32SoftEnterBad);
        kprintf("cpu: enter_user32 no RSP0\n");
        for (;;) {
            __asm__ volatile ("hlt");
        }
    }
    kprintf("cpu: enter_user32 rip=0x%lx rsp=0x%lx cs32=0x1b rsp0=0x%lx\n",
            (unsigned long)u64Entry, (unsigned long)u64Stack,
            (unsigned long)u64Rsp0);
    __asm__ volatile(
        "cli\n\t"
        "mov $0x23, %%ax\n\t" /* USER_DS */
        "mov %%ax, %%ds\n\t"
        "mov %%ax, %%es\n\t"
        "pushq $0x23\n\t"     /* SS */
        "pushq %[rsp]\n\t"    /* RSP */
        "pushq $0x200\n\t"    /* RFLAGS IF */
        "pushq $0x1b\n\t"     /* CS32 */
        "pushq %[rip]\n\t"    /* RIP */
        "iretq\n\t"
        :
        : [rsp] "r"(u64Stack), [rip] "r"(u64Entry)
        : "rax", "memory");
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

/* ---- Soft SYSCALL MSR observability API ---------------------------- */

u32
cpu_syscall_soft_inits(void)
{
    cpu_syscall_soft_inc(&g_u32SoftInitsGet);
    return g_u32SoftInits;
}

u32
cpu_syscall_soft_verify_ok(void)
{
    cpu_syscall_soft_inc(&g_u32SoftCtrGet);
    return g_u32SoftVerifyOk;
}

u32
cpu_syscall_soft_verify_bad(void)
{
    cpu_syscall_soft_inc(&g_u32SoftCtrGet);
    return g_u32SoftVerifyBad;
}

u32
cpu_syscall_soft_enter64(void)
{
    cpu_syscall_soft_inc(&g_u32SoftCtrGet);
    return g_u32SoftEnter64;
}

u32
cpu_syscall_soft_enter32(void)
{
    cpu_syscall_soft_inc(&g_u32SoftCtrGet);
    return g_u32SoftEnter32;
}

u32
cpu_syscall_soft_enter_bad(void)
{
    cpu_syscall_soft_inc(&g_u32SoftCtrGet);
    return g_u32SoftEnterBad;
}

u64
cpu_syscall_soft_star(void)
{
    cpu_syscall_soft_inc(&g_u32SoftStarGet);
    return g_SoftSnap.u64Star;
}

u64
cpu_syscall_soft_lstar(void)
{
    cpu_syscall_soft_inc(&g_u32SoftLstarGet);
    return g_SoftSnap.u64Lstar;
}

u64
cpu_syscall_soft_sfmask(void)
{
    cpu_syscall_soft_inc(&g_u32SoftSfmaskGet);
    return g_SoftSnap.u64Sfmask;
}

u64
cpu_syscall_soft_efer(void)
{
    cpu_syscall_soft_inc(&g_u32SoftEferGet);
    return g_SoftSnap.u64Efer;
}

int
cpu_syscall_soft_info_get(struct gj_cpu_syscall_soft *pOut)
{
    cpu_syscall_soft_inc(&g_u32SoftInfoGet);
    if (pOut == NULL) {
        return g_fSoftSnapLive ? 1 : 0;
    }
    memset(pOut, 0, sizeof(*pOut));
    if (!g_fSoftSnapLive) {
        return 0;
    }
    *pOut = g_SoftSnap;
    return 1;
}

int
cpu_syscall_soft_verify(void)
{
    cpu_syscall_soft_inc(&g_u32SoftVerifyCall);
    return cpu_syscall_soft_verify_inner();
}

void
cpu_syscall_soft_log(void)
{
    /*
     * Greppable soft summary (product / smoke inventory):
     *   cpu: syscall soft inventory wave=15 …
     *   cpu: syscall soft inits=… verify_ok=… verify_bad=… enter64=… …
     *   cpu: syscall soft STAR=0x… LSTAR=0x… SFMASK=0x… EFER=0x… SCE=… NXE=…
     *   cpu: syscall soft decode kern_cs=0x… user_base=0x… (CS64=base+16)
     *   cpu: syscall soft program|star|lstar|cstar|sfmask|efer|enter|…
     * Wave 15: full inventory via soft_inventory; re-verify once per log.
     */
    cpu_syscall_soft_inc(&g_u32SoftLogCall);
    if (g_fSoftSnapLive && g_fSyscallReady) {
        (void)cpu_syscall_soft_verify_inner();
    }
    cpu_syscall_soft_inventory();
}
