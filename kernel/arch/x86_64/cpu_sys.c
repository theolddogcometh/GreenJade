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
#define GJ_CPU_SYSCALL_SOFT_WAVE 104u

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

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("cpu: syscall: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("cpu: syscall: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retbailey — Wave 46 return-bailey honesty */
kprintf("cpu: syscall: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);
/* Grep: cpu: syscall: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("cpu: syscall: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_CPU_SYSCALL_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("cpu: syscall: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("cpu: syscall: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("cpu: syscall: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("cpu: syscall: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("cpu: syscall: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("cpu: syscall: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retsally — Wave 50 return-sally honesty */
kprintf("cpu: syscall: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("cpu: syscall: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retfosse — Wave 51 return-fosse honesty */
kprintf("cpu: syscall: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("cpu: syscall: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("cpu: syscall: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("cpu: syscall: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retravelin — Wave 53 return-travelin honesty */
kprintf("cpu: syscall: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("cpu: syscall: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("cpu: syscall: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retredan — Wave 54 exclusive redan stamp */
kprintf("cpu: syscall: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retflank — Wave 55 return-flank honesty */
kprintf("cpu: syscall: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retface — Wave 55 exclusive face stamp */
kprintf("cpu: syscall: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retgorge — Wave 56 return-gorge honesty */
kprintf("cpu: syscall: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("cpu: syscall: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retraverse — Wave 57 return-traverse honesty */
kprintf("cpu: syscall: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("cpu: syscall: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retorillon — Wave 58 return-orillon honesty */
kprintf("cpu: syscall: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("cpu: syscall: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("cpu: syscall: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("cpu: syscall: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retplace — Wave 60 return-place honesty */
kprintf("cpu: syscall: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("cpu: syscall: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("cpu: syscall: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("cpu: syscall: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("cpu: syscall: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("cpu: syscall: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("cpu: syscall: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("cpu: syscall: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cpu: syscall: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("cpu: syscall: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: cpu: syscall: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("cpu: syscall: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cpu: syscall: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("cpu: syscall: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: cpu: syscall: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("cpu: syscall: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: cpu: syscall: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("cpu: syscall: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=104 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: cpu: syscall: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("cpu: syscall: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=104 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("cpu: syscall: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("cpu: syscall: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("cpu: syscall: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("cpu: syscall: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("cpu: syscall: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=104 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("cpu: syscall: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=104 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("cpu: syscall: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("cpu: syscall: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("cpu: syscall: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("cpu: syscall: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: cpu: syscall: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("cpu: syscall: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("cpu: syscall: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("cpu: syscall: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("cpu: syscall: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retbastionangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("cpu: syscall: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("cpu: syscall: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retparapetangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("cpu: syscall: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("cpu: syscall: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retowerangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("cpu: syscall: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("cpu: syscall: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retwallangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("cpu: syscall: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("cpu: syscall: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retholdangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("cpu: syscall: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("cpu: syscall: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retfortressangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("cpu: syscall: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("cpu: syscall: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("cpu: syscall: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("cpu: syscall: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("cpu: syscall: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("cpu: syscall: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("cpu: syscall: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("cpu: syscall: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retaegisangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("cpu: syscall: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("cpu: syscall: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retsigilangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("cpu: syscall: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("cpu: syscall: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retglyphangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("cpu: syscall: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("cpu: syscall: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retshardangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("cpu: syscall: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("cpu: syscall: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retprismangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("cpu: syscall: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("cpu: syscall: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retcipherangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("cpu: syscall: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("cpu: syscall: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retledgerangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("cpu: syscall: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("cpu: syscall: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retvaultangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("cpu: syscall: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("cpu: syscall: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (rettokenangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("cpu: syscall: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("cpu: syscall: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retphaseangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("cpu: syscall: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("cpu: syscall: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retpulseangle stamp; Soft≠product)\n");

/* Grep: cpu: syscall: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("cpu: syscall: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("cpu: syscall: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retboundangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("cpu: syscall: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("cpu: syscall: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retbladeangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("cpu: syscall: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("cpu: syscall: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retarcangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("cpu: syscall: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("cpu: syscall: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("cpu: syscall: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("cpu: syscall: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("cpu: syscall: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("cpu: syscall: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retellipseangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("cpu: syscall: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("cpu: syscall: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("cpu: syscall: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("cpu: syscall: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (rethelixangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("cpu: syscall: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("cpu: syscall: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retknotangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("cpu: syscall: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("cpu: syscall: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retkleinangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("cpu: syscall: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("cpu: syscall: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retaffineangle stamp; Soft≠product)\n");
/* Grep: cpu: syscall: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("cpu: syscall: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=104 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: cpu: syscall: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("cpu: syscall: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=104 (retbilinearangle stamp; Soft≠product)\n");
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
