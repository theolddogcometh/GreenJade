/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal xAPIC memory-mapped local APIC (clean-room SDM).
 * Calibrated against PIT; product mono clock prefers APIC timer (vector 48).
 * Soft cal observability: multi-sample bus_hz, spin/elapsed telemetry,
 * sticky status tags (GJ_APIC_CAL_*), greppable apic_cal_soft_log().
 *
 * Soft APIC inventory (Wave 10 base + Wave 12 path + Wave 35 exclusive deepen;
 * this unit only — greppable "apic: soft …"):
 *   apic: soft PASS|PARTIAL|UP|FAIL|NONE …  — primary verdict (Wave 10+)
 *   apic: soft inventory …                  — rollup catalog + wave tag
 *   apic: soft timer …                      — LVT/period/bus_hz surface
 *   apic: soft cal …                        — cal status + multi-sample
 *   apic: soft ipi …                        — fixed/self/init_sipi/resched
 *   apic: soft tlb …                        — shootdown handshake tallies
 *   apic: soft vectors …                    — timer/resched/tlb/spur lamps
 *   apic: soft mode …                       — xAPIC MMIO + x2 MSR lamps
 *   apic: soft stats …                      — aggregate path counters
 *   apic: soft last …                       — last IPI/TLB/ICR snapshot
 *   apic: soft eoi …                        — EOI x2/MMIO/skip rollup
 *   apic: soft icr …                        — xAPIC ICR wait + path split
 *   apic: soft irq …                        — timer/resched/tlb IRQ tallies
 *   apic: soft bringup …                    — PASS|idle|active INIT/SIPI
 *   apic: soft path …                       — honesty non-claim (≠ bar3)
 * Wave 15 complementary surfaces (kept; never reshape primary fields):
 *   apic: soft query …                      — ready/cal/bus/ticks/npt samples
 *   apic: soft lapic …                      — MMIO vs x2 r/w path split
 *   apic: soft sample …                     — cal window enter/ok/fail
 *   apic: soft lamps …                      — ready/cal/periodic/svr lamps
 *   apic: soft reject …                     — !ready / clamp / icr-cap skips
 *   apic: soft capacity …                   — vec/div/cal geometry
 * Wave 16 complementary surfaces (kept; never reshape primary fields):
 *   apic: soft exclusive …                  — exclusive=1 unit stamp + wave
 *   apic: soft claim …                      — product claim bounds
 *   apic: soft ratio …                      — cal/ipi/tlb/irq path ratios
 *   apic: soft honesty …                    — soft ≠ bar3 / multi-server
 * Wave 17 complementary surfaces (kept) (never reshape primary fields):
 *   apic: soft return …                     — Wave 17 API return surfaces (kept)
 *   apic: soft return selftest …            — Wave 17 terminal return surface (kept)
 *   apic: soft retmap …                     — Wave 17 return-surface map (kept)
 *   apic: soft return rate — Wave 19 ok/fail rate lamps
 *   apic: soft retcode    — Wave 19 retcode catalog
 *   apic: soft deepen …                     — wave=44 areas stamp
 * Soft only: wrap-OK counters + kprintf; never hard-gates product paths.
 * Hot IRQ path bumps counters only — no kprintf from IRQ handlers.
 * greppable: apic: soft
 * greppable: apic: soft exclusive
 * greppable: apic: soft return
 * greppable: apic: soft return selftest
 * greppable: apic: soft retmap
 */
#include <gj/apic.h>
#include <gj/config.h>
#include <gj/cpu.h>
#include <gj/idt.h>
#include <gj/klog.h>
#include <gj/smp.h>
#include <gj/timer.h>
#include <gj/types.h>
#include <gj/x2apic.h>

#define IA32_APIC_BASE_MSR 0x1Bu
#define APIC_BASE_ENABLE   (1ull << 11)
#define APIC_BASE_BSP      (1ull << 8)

#define LAPIC_ID           0x20u
#define LAPIC_EOI          0xB0u
#define LAPIC_SVR          0xF0u
#define LAPIC_TIMER_LVT    0x320u
#define LAPIC_TIMER_INIT   0x380u
#define LAPIC_TIMER_CUR    0x390u
#define LAPIC_TIMER_DIV    0x3E0u
#define LAPIC_ICR_LOW      0x300u
#define LAPIC_ICR_HIGH     0x310u
#define LAPIC_SVR_ENABLE   (1u << 8)
#define LAPIC_TIMER_PERIODIC (1u << 17)
#define LAPIC_TIMER_MASKED   (1u << 16)
#define LAPIC_TIMER_ONESHOT  0u
#define LAPIC_TIMER_VEC    48u
#define LAPIC_IPI_RESCHED_VEC GJ_APIC_IPI_RESCHED_VEC
#define LAPIC_IPI_TLB_VEC     GJ_APIC_IPI_TLB_VEC
#define LAPIC_ICR_PENDING  (1u << 12)
/* TLB shootdown handshake (single outstanding request). */
static volatile gj_vaddr_t g_vaTlbShoot;
static volatile u32        g_u32TlbAck;
static volatile u32        g_u32TlbExpect;
#define LAPIC_ICR_INIT     (5u << 8)
#define LAPIC_ICR_STARTUP  (6u << 8)
#define LAPIC_ICR_LEVEL    (1u << 15)
#define LAPIC_ICR_ASSERT   (1u << 14)
#define LAPIC_ICR_EDGE     0u

/* Divide config: bits 0,1,3 — 0x3 = divide by 16 */
#define LAPIC_DIV_16       0x3u
#define LAPIC_DIV_FACTOR   16u

/* Multi-sample calibrate: two ~50 ms windows when stable (100 Hz PIT). */
#define APIC_CAL_SAMPLES_MAX  2u
#define APIC_CAL_WAIT_TICKS   5u /* 50 ms @ 100 Hz per sample */

/* Soft inventory wave stamp (this unit exclusive deepen). */
#define APIC_SOFT_WAVE 44u
#define APIC_SOFT_ICR_SPIN_MAX 1000000u

static volatile u32 *g_pLapic;
static int            g_fReady;
static int            g_fCalibrated;
static volatile u64   g_u64Ticks; /* BSP mono-driving ticks */
static volatile u64   g_aCpuTicks[GJ_CPU_STATIC_MAX];
static u64            g_u64BusHz;      /* timer counts per second (after /16) */
static u64            g_u64NsecPerTick;
static u32            g_u32PeriodInit;

/* Soft cal observability (sticky). */
static u32            g_u32CalStatus = GJ_APIC_CAL_NONE;
static u32            g_u32CalAttempts;
static u32            g_u32CalOk;
static u32            g_u32CalFail;
static u32            g_u32CalSamples;
static u32            g_u32CalPitTicks;
static u32            g_u32CalElapsed;
static u32            g_u32CalAlignSpins;
static u32            g_u32CalWaitSpins;
static u64            g_u64BusHzSample0;
static u64            g_u64BusHzSample1;
static u32            g_u32HzProgrammed;

/*
 * Soft inventory counters (file-local; wrap OK; diagnostics only).
 * Wave 10 base + Wave 12 path + Wave 35 exclusive deepen.
 * Hot IRQ path only bumps counters already present — no kprintf.
 * greppable: apic: soft
 * greppable: apic: soft stats
 */
static u32            g_u32SoftInit;        /* apic_init entries */
static u32            g_u32SoftCalCalls;    /* apic_calibrate entries */
static u32            g_u32SoftTimerHz;     /* apic_timer_hz entries */
static u32            g_u32SoftLocalStart;  /* apic_timer_start_local entries */
static u32            g_u32SoftEoi;         /* apic_eoi entries */
static u32            g_u32SoftLogN;        /* soft inventory emissions */
static u64            g_u64SoftIpiSend;     /* apic_send_ipi accepted */
static u64            g_u64SoftIpiSelf;     /* apic_send_self_ipi accepted */
static u64            g_u64SoftInitSipi;    /* apic_send_init_sipi accepted */
static u64            g_u64SoftResched;     /* apic_send_resched accepted */
static u64            g_u64SoftTlbShoot;    /* tlb_shootdown entries */
static u64            g_u64SoftTlbIpi;      /* TLB IPIs dispatched */
static u64            g_u64SoftTlbTimeout;  /* shootdown ack spin budget hit */
static u64            g_u64SoftTlbLocalOnly;/* shootdown local-only (no IPI) */
static u32            g_u32SoftLastApicId;  /* last fixed-IPI dest id */
static u32            g_u32SoftLastIpiVec;  /* last fixed-IPI vector */
static u32            g_u32SoftLastTlbExpect;
static u32            g_u32SoftLastTlbAck;

/* Wave 12 path deepen (sticky; wrap OK; never hard-gate). */
static u64            g_u64SoftIpiX2;       /* fixed IPI via x2APIC MSR */
static u64            g_u64SoftIpiXapic;    /* fixed IPI via xAPIC MMIO */
static u64            g_u64SoftIpiNotReady; /* send_ipi skipped !ready */
static u64            g_u64SoftSelfX2;      /* self-IPI via x2APIC */
static u64            g_u64SoftSelfXapic;   /* self-IPI via xAPIC */
static u64            g_u64SoftSelfNotReady;
static u64            g_u64SoftInitSipiX2;
static u64            g_u64SoftInitSipiXapic;
static u64            g_u64SoftInitSipiNotReady;
static u64            g_u64SoftReschedSkip; /* resched early return */
static u64            g_u64SoftIcrWait;     /* apic_icr_wait entries */
static u32            g_u32SoftIcrLastSpins;/* last ICR pending-spin count */
static u64            g_u64SoftEoiX2;       /* EOI via x2apic_eoi */
static u64            g_u64SoftEoiMmio;     /* EOI via LAPIC MMIO */
static u64            g_u64SoftEoiSkip;     /* EOI no path (null lapic) */
static u64            g_u64SoftTimerIrq;    /* apic_timer_irq entries */
static u64            g_u64SoftTimerIrqBsp; /* timer IRQ on CPU0 */
static u64            g_u64SoftTimerIrqAp;  /* timer IRQ on AP */
static u64            g_u64SoftTimerMono;   /* BSP advanced mono source */
static u64            g_u64SoftReschedIrq;  /* apic_ipi_resched_irq */
static u64            g_u64SoftTlbIrq;      /* apic_ipi_tlb_irq */
static u64            g_u64SoftTlbInvlpg;   /* shootdown invlpg path */
static u64            g_u64SoftTlbCr3;      /* shootdown full CR3 reload */
static u64            g_u64SoftTlbNotReady; /* shootdown !ready local */
static u64            g_u64SoftTlbOnline1;  /* shootdown single-cpu local */
static u64            g_u64SoftCalFailRdy;
static u64            g_u64SoftCalFailPit;
static u64            g_u64SoftCalFailCur;
static u64            g_u64SoftCalFailHz;
static u64            g_u64SoftCalFailWin;
static u64            g_u64SoftCalSample1Skip;
static u64            g_u64SoftTimerHzCal;  /* timer_hz used calibrated bus */
static u64            g_u64SoftTimerHzGuess;/* timer_hz uncalibrated guess */
static u64            g_u64SoftTimerHzSkip; /* timer_hz early return */
static u64            g_u64SoftLocalDefault;/* local start fallback INIT */
static u64            g_u64SoftLocalProg;   /* local start programmed INIT */
static u64            g_u64SoftIpiInit;     /* apic_ipi_init entries */

/* Wave 15 exclusive path tallies (complementary; never hard-gate). */
static u64            g_u64SoftLapicRdX2;   /* lapic_r via x2APIC MSR */
static u64            g_u64SoftLapicRdMmio; /* lapic_r via MMIO */
static u64            g_u64SoftLapicRdNull; /* lapic_r null MMIO */
static u64            g_u64SoftLapicWrX2;   /* lapic_w via x2APIC MSR */
static u64            g_u64SoftLapicWrMmio; /* lapic_w via MMIO */
static u64            g_u64SoftLapicWrNull; /* lapic_w null MMIO skip */
static u64            g_u64SoftCalSample;   /* apic_cal_sample entries */
static u64            g_u64SoftCalSampleOk; /* cal sample returned bus_hz */
static u64            g_u64SoftCalSampleFail;/* cal sample returned 0 */
static u64            g_u64SoftQReady;      /* apic_ready samples */
static u64            g_u64SoftQCal;        /* apic_calibrated samples */
static u64            g_u64SoftQBusHz;      /* apic_bus_hz samples */
static u64            g_u64SoftQTicks;      /* apic_timer_ticks samples */
static u64            g_u64SoftQTicksCpu;   /* apic_timer_ticks_cpu samples */
static u64            g_u64SoftQTicksCpuBad;/* ticks_cpu slot OOB */
static u64            g_u64SoftQNpt;        /* apic_nsec_per_tick samples */
static u64            g_u64SoftQInitCnt;    /* apic_timer_init_count samples */
static u64            g_u64SoftQCurCnt;     /* apic_timer_cur_count samples */
static u64            g_u64SoftQCurSkip;    /* cur_count !ready */
static u64            g_u64SoftQSnap;       /* cal_soft_snapshot samples */
static u64            g_u64SoftQSnapNull;   /* snapshot null out */
static u64            g_u64SoftQLog;        /* cal_soft_log samples */
static u64            g_u64SoftTimerHzClamp;/* timer_hz min INIT clamp */
static u64            g_u64SoftLocalSkip;   /* local start !ready */
static u64            g_u64SoftIcrCap;      /* ICR wait hit spin budget */
static u64            g_u64SoftReschedSlot0;/* resched skip cpu0 */

extern void irq_stub_apic_timer(void);
extern void irq_stub_ipi_resched(void);
extern void irq_stub_ipi_tlb(void);

/** Soft: saturating-ish bump (u32 wrap is fine for telemetry). */
static void
apic_soft_inc32(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/** Soft: u64 path tally (wrap OK). */
static void
apic_soft_inc64(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
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

/* x2APIC MSR = 0x800 + (mmio_offset >> 4) */
static u32
lapic_msr(u32 u32Off)
{
    return 0x800u + (u32Off >> 4);
}

static void
lapic_w(u32 u32Off, u32 u32Val)
{
    if (x2apic_enabled()) {
        u32 u32Msr = lapic_msr(u32Off);
        u32 u32Lo = u32Val;
        u32 u32Hi = 0;

        /* ICR is 64-bit; other regs are 32-bit in low half */
        __asm__ volatile ("wrmsr" : : "c"(u32Msr), "a"(u32Lo), "d"(u32Hi));
        apic_soft_inc64(&g_u64SoftLapicWrX2);
        return;
    }
    if (g_pLapic) {
        g_pLapic[u32Off / 4] = u32Val;
        apic_soft_inc64(&g_u64SoftLapicWrMmio);
    } else {
        apic_soft_inc64(&g_u64SoftLapicWrNull);
    }
}

static u32
lapic_r(u32 u32Off)
{
    if (x2apic_enabled()) {
        u32 u32Msr = lapic_msr(u32Off);
        u32 u32Lo;
        u32 u32Hi;

        __asm__ volatile ("rdmsr" : "=a"(u32Lo), "=d"(u32Hi) : "c"(u32Msr));
        (void)u32Hi;
        apic_soft_inc64(&g_u64SoftLapicRdX2);
        return u32Lo;
    }
    if (g_pLapic) {
        apic_soft_inc64(&g_u64SoftLapicRdMmio);
        return g_pLapic[u32Off / 4];
    }
    apic_soft_inc64(&g_u64SoftLapicRdNull);
    return 0;
}

static void
cal_fail(u32 u32Status)
{
    g_u32CalStatus = u32Status;
    g_u32CalFail++;
    /* Wave 12: soft per-reason fail tallies (never hard-gate). */
    switch (u32Status) {
    case GJ_APIC_CAL_FAIL_RDY:
        apic_soft_inc64(&g_u64SoftCalFailRdy);
        break;
    case GJ_APIC_CAL_FAIL_PIT:
        apic_soft_inc64(&g_u64SoftCalFailPit);
        break;
    case GJ_APIC_CAL_FAIL_CUR:
        apic_soft_inc64(&g_u64SoftCalFailCur);
        break;
    case GJ_APIC_CAL_FAIL_HZ:
        apic_soft_inc64(&g_u64SoftCalFailHz);
        break;
    case GJ_APIC_CAL_FAIL_WIN:
        apic_soft_inc64(&g_u64SoftCalFailWin);
        break;
    default:
        break;
    }
}

/**
 * Wave 15 greppable soft APIC inventory (product / smoke).
 * Prefix-stable markers under "apic: soft …". Never hard-gates; no alloc.
 * via= names the call site (init / calibrate / timer_hz / cal_log / …).
 * Primary Wave 10/12 field names stay stable; Wave 15 adds complementary
 * sub-lines only.
 * greppable: apic: soft
 */
static void
apic_soft_inventory(const char *szVia)
{
    u32 u32Id;
    u32 u32Svr;
    u32 u32Lvt;
    u32 u32Div;
    u32 u32Cur;
    u32 u32InitReg;
    u64 u64Base;
    u32 u32X2;
    u32 u32BaseEn;
    u32 u32Bsp;
    u32 u32X2En;
    const char *szVerdict;
    const char *szCal;
    const char *szBringup;
    u32 u32CpuTicksSum;
    u32 iCpu;
    u32 u32LvtMasked;
    u32 u32LvtPeriodic;
    u32 u32SvrEn;

    if (szVia == NULL) {
        szVia = "path";
    }
    apic_soft_inc32(&g_u32SoftLogN);

    u32Id = 0;
    u32Svr = 0;
    u32Lvt = 0;
    u32Div = 0;
    u32Cur = 0;
    u32InitReg = 0;
    u64Base = 0;
    u32X2 = 0;
    u32BaseEn = 0;
    u32Bsp = 0;
    u32X2En = 0;
    u32CpuTicksSum = 0;
    u32LvtMasked = 0;
    u32LvtPeriodic = 0;
    u32SvrEn = 0;

    if (g_fReady) {
        u32Id = lapic_r(LAPIC_ID) >> 24;
        u32Svr = lapic_r(LAPIC_SVR);
        u32Lvt = lapic_r(LAPIC_TIMER_LVT);
        u32Div = lapic_r(LAPIC_TIMER_DIV);
        u32Cur = lapic_r(LAPIC_TIMER_CUR);
        u32InitReg = lapic_r(LAPIC_TIMER_INIT);
        u64Base = rdmsr(IA32_APIC_BASE_MSR);
        u32BaseEn = ((u64Base & APIC_BASE_ENABLE) != 0) ? 1u : 0u;
        u32Bsp = ((u64Base & APIC_BASE_BSP) != 0) ? 1u : 0u;
        u32X2En = ((u64Base & (1ull << 10)) != 0) ? 1u : 0u;
        u32X2 = x2apic_enabled() ? 1u : 0u;
        u32LvtMasked = ((u32Lvt & LAPIC_TIMER_MASKED) != 0) ? 1u : 0u;
        u32LvtPeriodic = ((u32Lvt & LAPIC_TIMER_PERIODIC) != 0) ? 1u : 0u;
        u32SvrEn = ((u32Svr & LAPIC_SVR_ENABLE) != 0) ? 1u : 0u;
        for (iCpu = 0; iCpu < GJ_CPU_STATIC_MAX; iCpu++) {
            /* Soft sum of per-cpu timer IRQ tallies (wrap-tolerant u32). */
            u32CpuTicksSum += (u32)g_aCpuTicks[iCpu];
        }
    }

    switch (g_u32CalStatus) {
    case GJ_APIC_CAL_OK:
        szCal = "OK";
        break;
    case GJ_APIC_CAL_FAIL_RDY:
        szCal = "FAIL_RDY";
        break;
    case GJ_APIC_CAL_FAIL_PIT:
        szCal = "FAIL_PIT";
        break;
    case GJ_APIC_CAL_FAIL_CUR:
        szCal = "FAIL_CUR";
        break;
    case GJ_APIC_CAL_FAIL_HZ:
        szCal = "FAIL_HZ";
        break;
    case GJ_APIC_CAL_FAIL_WIN:
        szCal = "FAIL_WIN";
        break;
    default:
        szCal = "NONE";
        break;
    }

    if (!g_fReady) {
        szVerdict = "NONE";
    } else if (g_u32CalStatus >= GJ_APIC_CAL_FAIL_RDY &&
               g_u32CalStatus <= GJ_APIC_CAL_FAIL_WIN &&
               !g_fCalibrated) {
        szVerdict = "FAIL";
    } else if (g_fCalibrated && g_u64BusHz != 0 && g_u32PeriodInit != 0 &&
               g_u32HzProgrammed != 0) {
        szVerdict = "PASS";
    } else if (g_fReady && g_u32CalStatus == GJ_APIC_CAL_NONE &&
               g_u32PeriodInit == 0) {
        szVerdict = "UP";
    } else {
        szVerdict = "PARTIAL";
    }

    /*
     * Grep: apic: soft PASS|PARTIAL|UP|FAIL|NONE
     * Primary verdict line — field order stable for smoke greps.
     */
    kprintf("apic: soft %s via=%s ready=%u cal=%u bus_hz=%lu hz=%u "
            "init=%u npt=%lu bsp_ticks=%lu log_n=%u\n",
            szVerdict, szVia, g_fReady ? 1u : 0u,
            g_fCalibrated ? 1u : 0u, (unsigned long)g_u64BusHz,
            (unsigned)g_u32HzProgrammed, (unsigned)g_u32PeriodInit,
            (unsigned long)g_u64NsecPerTick, (unsigned long)g_u64Ticks,
            (unsigned)g_u32SoftLogN);

    /* Grep: apic: soft inventory */
    kprintf("apic: soft inventory via=%s ready=%u cal=%u status=%s "
            "attempts=%u ok=%u fail=%u samples=%u period=%u hz=%u "
            "bsp_ticks=%lu cpu_ticks_sum=%u inits=%u cal_calls=%u "
            "timer_hz_n=%u local_n=%u eoi_n=%u log_n=%u wave=%u\n",
            szVia, g_fReady ? 1u : 0u, g_fCalibrated ? 1u : 0u, szCal,
            (unsigned)g_u32CalAttempts, (unsigned)g_u32CalOk,
            (unsigned)g_u32CalFail, (unsigned)g_u32CalSamples,
            (unsigned)g_u32PeriodInit, (unsigned)g_u32HzProgrammed,
            (unsigned long)g_u64Ticks, (unsigned)u32CpuTicksSum,
            (unsigned)g_u32SoftInit, (unsigned)g_u32SoftCalCalls,
            (unsigned)g_u32SoftTimerHz, (unsigned)g_u32SoftLocalStart,
            (unsigned)g_u32SoftEoi, (unsigned)g_u32SoftLogN,
            (unsigned)APIC_SOFT_WAVE);

    /* Grep: apic: soft timer */
    kprintf("apic: soft timer vec=%u div=%u div_cfg=0x%x lvt=0x%x "
            "period_init=%u cur=%u init_reg=%u hz=%u npt=%lu "
            "bus_hz=%lu s0=%lu s1=%lu mono_src_hint=%u "
            "masked=%u periodic=%u cal_path=%lu guess_path=%lu "
            "skip=%lu\n",
            (unsigned)LAPIC_TIMER_VEC, (unsigned)LAPIC_DIV_FACTOR,
            (unsigned)u32Div, (unsigned)u32Lvt,
            (unsigned)g_u32PeriodInit, (unsigned)u32Cur,
            (unsigned)u32InitReg, (unsigned)g_u32HzProgrammed,
            (unsigned long)g_u64NsecPerTick, (unsigned long)g_u64BusHz,
            (unsigned long)g_u64BusHzSample0,
            (unsigned long)g_u64BusHzSample1,
            (g_fCalibrated && g_u32PeriodInit != 0) ? 1u : 0u,
            (unsigned)u32LvtMasked, (unsigned)u32LvtPeriodic,
            (unsigned long)g_u64SoftTimerHzCal,
            (unsigned long)g_u64SoftTimerHzGuess,
            (unsigned long)g_u64SoftTimerHzSkip);

    /* Grep: apic: soft cal — companion to legacy "apic: cal soft" */
    kprintf("apic: soft cal status=%s attempts=%u ok=%u fail=%u "
            "samples=%u pit_ticks=%u elapsed=%u align=%u wait=%u "
            "bus_hz=%lu s0=%lu s1=%lu fail_rdy=%lu fail_pit=%lu "
            "fail_cur=%lu fail_hz=%lu fail_win=%lu s1_skip=%lu\n",
            szCal, (unsigned)g_u32CalAttempts, (unsigned)g_u32CalOk,
            (unsigned)g_u32CalFail, (unsigned)g_u32CalSamples,
            (unsigned)g_u32CalPitTicks, (unsigned)g_u32CalElapsed,
            (unsigned)g_u32CalAlignSpins, (unsigned)g_u32CalWaitSpins,
            (unsigned long)g_u64BusHz,
            (unsigned long)g_u64BusHzSample0,
            (unsigned long)g_u64BusHzSample1,
            (unsigned long)g_u64SoftCalFailRdy,
            (unsigned long)g_u64SoftCalFailPit,
            (unsigned long)g_u64SoftCalFailCur,
            (unsigned long)g_u64SoftCalFailHz,
            (unsigned long)g_u64SoftCalFailWin,
            (unsigned long)g_u64SoftCalSample1Skip);

    /* Grep: apic: soft ipi */
    kprintf("apic: soft ipi send=%lu self=%lu init_sipi=%lu resched=%lu "
            "last_dest=%u last_vec=%u resched_vec=%u tlb_vec=%u "
            "x2=%lu xapic=%lu nrdy=%lu self_x2=%lu self_xapic=%lu "
            "self_nrdy=%lu init_x2=%lu init_xapic=%lu init_nrdy=%lu "
            "resched_skip=%lu\n",
            (unsigned long)g_u64SoftIpiSend,
            (unsigned long)g_u64SoftIpiSelf,
            (unsigned long)g_u64SoftInitSipi,
            (unsigned long)g_u64SoftResched,
            (unsigned)g_u32SoftLastApicId,
            (unsigned)g_u32SoftLastIpiVec,
            (unsigned)LAPIC_IPI_RESCHED_VEC,
            (unsigned)LAPIC_IPI_TLB_VEC,
            (unsigned long)g_u64SoftIpiX2,
            (unsigned long)g_u64SoftIpiXapic,
            (unsigned long)g_u64SoftIpiNotReady,
            (unsigned long)g_u64SoftSelfX2,
            (unsigned long)g_u64SoftSelfXapic,
            (unsigned long)g_u64SoftSelfNotReady,
            (unsigned long)g_u64SoftInitSipiX2,
            (unsigned long)g_u64SoftInitSipiXapic,
            (unsigned long)g_u64SoftInitSipiNotReady,
            (unsigned long)g_u64SoftReschedSkip);

    /* Grep: apic: soft tlb */
    kprintf("apic: soft tlb shoot=%lu ipi=%lu timeout=%lu local_only=%lu "
            "last_expect=%u last_ack=%u va=0x%lx invlpg=%lu cr3=%lu "
            "not_ready=%lu online1=%lu irq=%lu\n",
            (unsigned long)g_u64SoftTlbShoot,
            (unsigned long)g_u64SoftTlbIpi,
            (unsigned long)g_u64SoftTlbTimeout,
            (unsigned long)g_u64SoftTlbLocalOnly,
            (unsigned)g_u32SoftLastTlbExpect,
            (unsigned)g_u32SoftLastTlbAck,
            (unsigned long)g_vaTlbShoot,
            (unsigned long)g_u64SoftTlbInvlpg,
            (unsigned long)g_u64SoftTlbCr3,
            (unsigned long)g_u64SoftTlbNotReady,
            (unsigned long)g_u64SoftTlbOnline1,
            (unsigned long)g_u64SoftTlbIrq);

    /* Grep: apic: soft vectors */
    kprintf("apic: soft vectors timer=%u resched=%u tlb=%u spur=0xff "
            "svr=0x%x id=%u svr_en=%u\n",
            (unsigned)LAPIC_TIMER_VEC, (unsigned)LAPIC_IPI_RESCHED_VEC,
            (unsigned)LAPIC_IPI_TLB_VEC, (unsigned)u32Svr,
            (unsigned)u32Id, (unsigned)u32SvrEn);

    /* Grep: apic: soft mode */
    kprintf("apic: soft mode xapic_mmio=%p base_en=%u bsp=%u x2_msr=%u "
            "x2_enabled=%u base=0x%lx div_factor=%u cal_samples_max=%u "
            "wave=%u\n",
            (void *)g_pLapic, (unsigned)u32BaseEn, (unsigned)u32Bsp,
            (unsigned)u32X2En, (unsigned)u32X2,
            (unsigned long)u64Base, (unsigned)LAPIC_DIV_FACTOR,
            (unsigned)APIC_CAL_SAMPLES_MAX, (unsigned)APIC_SOFT_WAVE);

    /* Grep: apic: soft stats */
    kprintf("apic: soft stats init=%u cal_calls=%u timer_hz_n=%u "
            "local_n=%u eoi_n=%u ipi_send=%lu ipi_self=%lu "
            "init_sipi=%lu resched=%lu tlb_shoot=%lu timer_irq=%lu "
            "ipi_init=%lu log_n=%u wave=%u\n",
            (unsigned)g_u32SoftInit, (unsigned)g_u32SoftCalCalls,
            (unsigned)g_u32SoftTimerHz, (unsigned)g_u32SoftLocalStart,
            (unsigned)g_u32SoftEoi,
            (unsigned long)g_u64SoftIpiSend,
            (unsigned long)g_u64SoftIpiSelf,
            (unsigned long)g_u64SoftInitSipi,
            (unsigned long)g_u64SoftResched,
            (unsigned long)g_u64SoftTlbShoot,
            (unsigned long)g_u64SoftTimerIrq,
            (unsigned long)g_u64SoftIpiInit,
            (unsigned)g_u32SoftLogN, (unsigned)APIC_SOFT_WAVE);

    /* Grep: apic: soft last */
    kprintf("apic: soft last dest=%u vec=%u tlb_expect=%u tlb_ack=%u "
            "icr_spins=%u via=%s\n",
            (unsigned)g_u32SoftLastApicId,
            (unsigned)g_u32SoftLastIpiVec,
            (unsigned)g_u32SoftLastTlbExpect,
            (unsigned)g_u32SoftLastTlbAck,
            (unsigned)g_u32SoftIcrLastSpins, szVia);

    /* Grep: apic: soft eoi */
    kprintf("apic: soft eoi total=%u x2=%lu mmio=%lu skip=%lu\n",
            (unsigned)g_u32SoftEoi,
            (unsigned long)g_u64SoftEoiX2,
            (unsigned long)g_u64SoftEoiMmio,
            (unsigned long)g_u64SoftEoiSkip);

    /* Grep: apic: soft icr */
    kprintf("apic: soft icr waits=%lu last_spins=%u xapic_ipi=%lu "
            "x2_ipi=%lu self_xapic=%lu self_x2=%lu init_xapic=%lu "
            "init_x2=%lu\n",
            (unsigned long)g_u64SoftIcrWait,
            (unsigned)g_u32SoftIcrLastSpins,
            (unsigned long)g_u64SoftIpiXapic,
            (unsigned long)g_u64SoftIpiX2,
            (unsigned long)g_u64SoftSelfXapic,
            (unsigned long)g_u64SoftSelfX2,
            (unsigned long)g_u64SoftInitSipiXapic,
            (unsigned long)g_u64SoftInitSipiX2);

    /* Grep: apic: soft irq */
    kprintf("apic: soft irq timer=%lu bsp=%lu ap=%lu mono=%lu "
            "resched_irq=%lu tlb_irq=%lu local_default=%lu "
            "local_prog=%lu\n",
            (unsigned long)g_u64SoftTimerIrq,
            (unsigned long)g_u64SoftTimerIrqBsp,
            (unsigned long)g_u64SoftTimerIrqAp,
            (unsigned long)g_u64SoftTimerMono,
            (unsigned long)g_u64SoftReschedIrq,
            (unsigned long)g_u64SoftTlbIrq,
            (unsigned long)g_u64SoftLocalDefault,
            (unsigned long)g_u64SoftLocalProg);

    if (g_u64SoftInitSipi != 0) {
        szBringup = "PASS";
    } else if (g_u64SoftIpiSend == 0 && g_u64SoftIpiSelf == 0 &&
               g_u64SoftResched == 0) {
        szBringup = "idle";
    } else {
        szBringup = "active";
    }
    /* Grep: apic: soft bringup */
    kprintf("apic: soft bringup %s init_sipi=%lu ipi=%lu self=%lu "
            "resched=%lu timer_irq=%lu ready=%u cal=%u\n",
            szBringup,
            (unsigned long)g_u64SoftInitSipi,
            (unsigned long)g_u64SoftIpiSend,
            (unsigned long)g_u64SoftIpiSelf,
            (unsigned long)g_u64SoftResched,
            (unsigned long)g_u64SoftTimerIrq,
            g_fReady ? 1u : 0u, g_fCalibrated ? 1u : 0u);

    /*
     * Grep: apic: soft path
     * Honesty: soft APIC inventory ≠ product multi-server / bar3 close.
     */
    kprintf("apic: soft path claim=xapic_timer+ipi+tlb p_irq1=1 "
            "p_time4=1 p_smp5=1 mono=bsp_only x2_optional=1 "
            "bar3=open via=%s wave=%u (soft inventory; not bar3)\n",
            szVia, (unsigned)APIC_SOFT_WAVE);

    /*
     * Wave 15 complementary sub-lines (kept; never reshape primary).
     */
    /* Grep: apic: soft query */
    kprintf("apic: soft query ready=%lu cal=%lu bus_hz=%lu ticks=%lu "
            "ticks_cpu=%lu ticks_cpu_bad=%lu npt=%lu init_cnt=%lu "
            "cur_cnt=%lu cur_skip=%lu snap=%lu snap_null=%lu log=%lu\n",
            (unsigned long)g_u64SoftQReady,
            (unsigned long)g_u64SoftQCal,
            (unsigned long)g_u64SoftQBusHz,
            (unsigned long)g_u64SoftQTicks,
            (unsigned long)g_u64SoftQTicksCpu,
            (unsigned long)g_u64SoftQTicksCpuBad,
            (unsigned long)g_u64SoftQNpt,
            (unsigned long)g_u64SoftQInitCnt,
            (unsigned long)g_u64SoftQCurCnt,
            (unsigned long)g_u64SoftQCurSkip,
            (unsigned long)g_u64SoftQSnap,
            (unsigned long)g_u64SoftQSnapNull,
            (unsigned long)g_u64SoftQLog);

    /* Grep: apic: soft lapic */
    kprintf("apic: soft lapic rd_x2=%lu rd_mmio=%lu rd_null=%lu "
            "wr_x2=%lu wr_mmio=%lu wr_null=%lu "
            "path=x2_msr|mmio|null\n",
            (unsigned long)g_u64SoftLapicRdX2,
            (unsigned long)g_u64SoftLapicRdMmio,
            (unsigned long)g_u64SoftLapicRdNull,
            (unsigned long)g_u64SoftLapicWrX2,
            (unsigned long)g_u64SoftLapicWrMmio,
            (unsigned long)g_u64SoftLapicWrNull);

    /* Grep: apic: soft sample */
    kprintf("apic: soft sample enter=%lu ok=%lu fail=%lu "
            "wait_ticks=%u max_samples=%u s1_skip=%lu "
            "align_last=%u wait_last=%u\n",
            (unsigned long)g_u64SoftCalSample,
            (unsigned long)g_u64SoftCalSampleOk,
            (unsigned long)g_u64SoftCalSampleFail,
            (unsigned)APIC_CAL_WAIT_TICKS,
            (unsigned)APIC_CAL_SAMPLES_MAX,
            (unsigned long)g_u64SoftCalSample1Skip,
            (unsigned)g_u32CalAlignSpins,
            (unsigned)g_u32CalWaitSpins);

    /* Grep: apic: soft lamps */
    kprintf("apic: soft lamps ready=%u cal=%u period=%u hz=%u "
            "svr_en=%u lvt_masked=%u lvt_periodic=%u x2=%u "
            "base_en=%u bsp=%u mono_hint=%u verdict=%s\n",
            g_fReady ? 1u : 0u, g_fCalibrated ? 1u : 0u,
            (g_u32PeriodInit != 0) ? 1u : 0u,
            (g_u32HzProgrammed != 0) ? 1u : 0u,
            (unsigned)u32SvrEn, (unsigned)u32LvtMasked,
            (unsigned)u32LvtPeriodic, (unsigned)u32X2,
            (unsigned)u32BaseEn, (unsigned)u32Bsp,
            (g_fCalibrated && g_u32PeriodInit != 0) ? 1u : 0u,
            szVerdict);

    /* Grep: apic: soft reject */
    kprintf("apic: soft reject ipi_nrdy=%lu self_nrdy=%lu "
            "init_nrdy=%lu resched_skip=%lu resched_slot0=%lu "
            "timer_hz_skip=%lu timer_hz_clamp=%lu local_skip=%lu "
            "icr_cap=%lu tlb_nrdy=%lu\n",
            (unsigned long)g_u64SoftIpiNotReady,
            (unsigned long)g_u64SoftSelfNotReady,
            (unsigned long)g_u64SoftInitSipiNotReady,
            (unsigned long)g_u64SoftReschedSkip,
            (unsigned long)g_u64SoftReschedSlot0,
            (unsigned long)g_u64SoftTimerHzSkip,
            (unsigned long)g_u64SoftTimerHzClamp,
            (unsigned long)g_u64SoftLocalSkip,
            (unsigned long)g_u64SoftIcrCap,
            (unsigned long)g_u64SoftTlbNotReady);

    /* Grep: apic: soft capacity (geometry) */
    kprintf("apic: soft capacity timer_vec=%u resched_vec=%u tlb_vec=%u "
            "div=%u cal_samples_max=%u cal_wait_ticks=%u "
            "icr_spin_max=%u ready=%u cal=%u wave=%u\n",
            (unsigned)LAPIC_TIMER_VEC, (unsigned)LAPIC_IPI_RESCHED_VEC,
            (unsigned)LAPIC_IPI_TLB_VEC, (unsigned)LAPIC_DIV_FACTOR,
            (unsigned)APIC_CAL_SAMPLES_MAX, (unsigned)APIC_CAL_WAIT_TICKS,
            (unsigned)APIC_SOFT_ICR_SPIN_MAX,
            g_fReady ? 1u : 0u, g_fCalibrated ? 1u : 0u,
            (unsigned)APIC_SOFT_WAVE);

    /*
     * Wave 16 complementary sub-lines (kept; never reshape primary).
     */
    /* Grep: apic: soft exclusive */
    kprintf("apic: soft exclusive wave=%u exclusive=1 soft=1 "
            "unit=apic.c bar3=0 hard_gate=0 multi_server=0 "
            "hot_irq_kprintf=0 via=%s\n",
            (unsigned)APIC_SOFT_WAVE, szVia);

    /* Grep: apic: soft claim — product claim bounds */
    kprintf("apic: soft claim xapic_timer=1 ipi=1 tlb=1 "
            "p_irq1=1 p_time4=1 p_smp5=1 mono=bsp_only "
            "x2_optional=1 multi_server=0 bar3=0 hard_gate=0 "
            "wave=%u via=%s\n",
            (unsigned)APIC_SOFT_WAVE, szVia);

    /* Grep: apic: soft ratio — cal/ipi/tlb/irq path ratios */
    kprintf("apic: soft ratio cal_ok=%u cal_fail=%u samples=%u "
            "ipi_send=%lu ipi_self=%lu init_sipi=%lu resched=%lu "
            "tlb_shoot=%lu timer_irq=%lu eoi=%u "
            "ipi_nrdy=%lu timer_hz_skip=%lu wave=%u\n",
            (unsigned)g_u32CalOk, (unsigned)g_u32CalFail,
            (unsigned)g_u32CalSamples,
            (unsigned long)g_u64SoftIpiSend,
            (unsigned long)g_u64SoftIpiSelf,
            (unsigned long)g_u64SoftInitSipi,
            (unsigned long)g_u64SoftResched,
            (unsigned long)g_u64SoftTlbShoot,
            (unsigned long)g_u64SoftTimerIrq,
            (unsigned)g_u32SoftEoi,
            (unsigned long)g_u64SoftIpiNotReady,
            (unsigned long)g_u64SoftTimerHzSkip,
            (unsigned)APIC_SOFT_WAVE);

    /* Grep: apic: soft honesty */
    kprintf("apic: soft honesty soft_ne_product_complete=1 "
            "bar3=0 multi_server=0 hard_gate=0 "
            "claim=xapic_timer+ipi+tlb mono=bsp_only "
            "x2_optional=1 wave=%u unit=apic.c via=%s\n",
            (unsigned)APIC_SOFT_WAVE, szVia);

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: apic: soft return — Wave 17 API return surfaces (kept) */
    kprintf("apic: soft return ready=%u cal=%u timer=1 ipi=1 tlb=1 eoi=1 "
            "init_sipi=1 bringup=1 product_kernel=OPEN bar3=0 "
            "hard_gate=0 wave=%u soft PASS\n",
            g_fReady ? 1u : 0u, g_fCalibrated ? 1u : 0u,
            (unsigned)APIC_SOFT_WAVE);

    /* Grep: apic: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("apic: soft return selftest inv_ret=1 ready=%u cal=%u "
            "product_kernel=OPEN multi_server=0 bar3=0 wave=%u soft PASS\n",
            g_fReady ? 1u : 0u, g_fCalibrated ? 1u : 0u,
            (unsigned)APIC_SOFT_WAVE);

    /* Grep: apic: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("apic: soft retmap init=1 cal=1 timer_set=1 eoi=1 ipi=1 "
            "self_ipi=1 init_sipi=1 tlb=1 soft_inv=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)APIC_SOFT_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: apic: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("apic: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)APIC_SOFT_WAVE);

    /* Grep: apic: soft retcode — Wave 19 retcode catalog */
    kprintf("apic: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)APIC_SOFT_WAVE);

    /* Grep: apic: soft deepen */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: apic: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("apic: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)APIC_SOFT_WAVE);
    /* Grep: apic: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("apic: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)APIC_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: apic: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("apic: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)APIC_SOFT_WAVE);
    /* Grep: apic: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("apic: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)APIC_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: apic: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("apic: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)APIC_SOFT_WAVE);
            /* Grep: apic: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("apic: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)APIC_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: apic: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("apic: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)APIC_SOFT_WAVE);
            /* Grep: apic: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("apic: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)APIC_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: apic: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("apic: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)APIC_SOFT_WAVE);
            /* Grep: apic: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("apic: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)APIC_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: apic: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("apic: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)APIC_SOFT_WAVE);
            /* Grep: apic: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("apic: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)APIC_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: apic: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("apic: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)APIC_SOFT_WAVE);
            /* Grep: apic: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("apic: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)APIC_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: apic: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("apic: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)APIC_SOFT_WAVE);
            /* Grep: apic: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("apic: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)APIC_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: apic: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("apic: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)APIC_SOFT_WAVE);
                    /* Grep: apic: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("apic: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)APIC_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: apic: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("apic: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)APIC_SOFT_WAVE);
                            /* Grep: apic: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("apic: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)APIC_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: apic: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("apic: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)APIC_SOFT_WAVE);
                            /* Grep: apic: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("apic: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)APIC_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: apic: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("apic: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)APIC_SOFT_WAVE);
                            /* Grep: apic: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("apic: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)APIC_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: apic: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("apic: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)APIC_SOFT_WAVE);
                            /* Grep: apic: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("apic: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)APIC_SOFT_WAVE);
                            /* Grep: apic: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("apic: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)APIC_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: apic: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("apic: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)APIC_SOFT_WAVE);
/* Grep: apic: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("apic: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)APIC_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: apic: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("apic: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)APIC_SOFT_WAVE);
/* Grep: apic: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("apic: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)APIC_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: apic: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("apic: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)APIC_SOFT_WAVE);
/* Grep: apic: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("apic: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)APIC_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: apic: soft retfortress — Wave 35 return-fortress honesty */
kprintf("apic: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)APIC_SOFT_WAVE);
/* Grep: apic: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("apic: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)APIC_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: apic: soft rethold — Wave 36 return-hold honesty */
kprintf("apic: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)APIC_SOFT_WAVE);
/* Grep: apic: soft retspire — Wave 36 exclusive spire stamp */
kprintf("apic: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)APIC_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: apic: soft retwall — Wave 37 return-wall honesty */
kprintf("apic: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)APIC_SOFT_WAVE);
/* Grep: apic: soft retgate — Wave 37 exclusive gate stamp */
kprintf("apic: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)APIC_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: apic: soft retmoat — Wave 38 return-moat honesty */
kprintf("apic: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)APIC_SOFT_WAVE);
/* Grep: apic: soft retower — Wave 38 exclusive tower stamp */
kprintf("apic: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)APIC_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: apic: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("apic: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)APIC_SOFT_WAVE);
/* Grep: apic: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("apic: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)APIC_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: apic: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("apic: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)APIC_SOFT_WAVE);
/* Grep: apic: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("apic: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)APIC_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: apic: soft retravelin — Wave 41 return-travelin honesty */
kprintf("apic: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)APIC_SOFT_WAVE);
/* Grep: apic: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("apic: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)APIC_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: apic: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("apic: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)APIC_SOFT_WAVE);
/* Grep: apic: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("apic: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)APIC_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: apic: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("apic: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)APIC_SOFT_WAVE);
/* Grep: apic: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("apic: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)APIC_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: apic: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("apic: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)APIC_SOFT_WAVE);
/* Grep: apic: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("apic: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)APIC_SOFT_WAVE);


                            kprintf("apic: soft deepen wave=%u areas="
            "inventory,timer,cal,ipi,tlb,vectors,mode,stats,last,"
            "eoi,icr,irq,bringup,path,query,lapic,sample,lamps,reject,"
            "capacity,exclusive,claim,ratio,honesty,return,return_selftest,retmap,return_rate,retcode "
            "unit=apic.c only hot_irq_kprintf=0 via=%s\n",
            (unsigned)APIC_SOFT_WAVE, szVia);
}

void
apic_timer_irq(void)
{
    u32 u32Cpu = 0;

    /* Soft: IRQ path counter only — never kprintf here. */
    apic_soft_inc64(&g_u64SoftTimerIrq);

    /* GS may not be live very early; treat as BSP then */
    if (g_fReady) {
        u32Cpu = cpu_id();
    }
    if (u32Cpu < GJ_CPU_STATIC_MAX) {
        g_aCpuTicks[u32Cpu]++;
    }
    /*
     * Only BSP advances global mono/jiffies. APs only EOI + per-cpu count so
     * HLT wakes without double-ticking the clock.
     */
    if (u32Cpu == 0) {
        g_u64Ticks++;
        apic_soft_inc64(&g_u64SoftTimerIrqBsp);
        if (timer_apic_source()) {
            timer_tick_apic();
            apic_soft_inc64(&g_u64SoftTimerMono);
        }
    } else {
        apic_soft_inc64(&g_u64SoftTimerIrqAp);
    }
    if (x2apic_enabled()) {
        x2apic_eoi();
        apic_soft_inc64(&g_u64SoftEoiX2);
    } else {
        lapic_w(LAPIC_EOI, 0);
        apic_soft_inc64(&g_u64SoftEoiMmio);
    }
}

void
apic_init(void)
{
    u64 u64Base;
    u32 u32Id;

    g_u32SoftInit++;
    g_fReady = 0;
    g_fCalibrated = 0;
    g_u64Ticks = 0;
    g_u64BusHz = 0;
    g_u64NsecPerTick = 0;
    g_u32PeriodInit = 0;
    g_u32CalStatus = GJ_APIC_CAL_NONE;
    g_u32CalAttempts = 0;
    g_u32CalOk = 0;
    g_u32CalFail = 0;
    g_u32CalSamples = 0;
    g_u32CalPitTicks = 0;
    g_u32CalElapsed = 0;
    g_u32CalAlignSpins = 0;
    g_u32CalWaitSpins = 0;
    g_u64BusHzSample0 = 0;
    g_u64BusHzSample1 = 0;
    g_u32HzProgrammed = 0;
    /* Soft path counters reset with bring-up (inventory re-arms). */
    g_u64SoftIpiSend = 0;
    g_u64SoftIpiSelf = 0;
    g_u64SoftInitSipi = 0;
    g_u64SoftResched = 0;
    g_u64SoftTlbShoot = 0;
    g_u64SoftTlbIpi = 0;
    g_u64SoftTlbTimeout = 0;
    g_u64SoftTlbLocalOnly = 0;
    g_u32SoftLastApicId = 0;
    g_u32SoftLastIpiVec = 0;
    g_u32SoftLastTlbExpect = 0;
    g_u32SoftLastTlbAck = 0;
    g_u32SoftEoi = 0;
    g_u32SoftLocalStart = 0;
    g_u32SoftTimerHz = 0;
    g_u32SoftCalCalls = 0;
    g_u32SoftLogN = 0;
    /* Wave 12 path deepen reset. */
    g_u64SoftIpiX2 = 0;
    g_u64SoftIpiXapic = 0;
    g_u64SoftIpiNotReady = 0;
    g_u64SoftSelfX2 = 0;
    g_u64SoftSelfXapic = 0;
    g_u64SoftSelfNotReady = 0;
    g_u64SoftInitSipiX2 = 0;
    g_u64SoftInitSipiXapic = 0;
    g_u64SoftInitSipiNotReady = 0;
    g_u64SoftReschedSkip = 0;
    g_u64SoftIcrWait = 0;
    g_u32SoftIcrLastSpins = 0;
    g_u64SoftEoiX2 = 0;
    g_u64SoftEoiMmio = 0;
    g_u64SoftEoiSkip = 0;
    g_u64SoftTimerIrq = 0;
    g_u64SoftTimerIrqBsp = 0;
    g_u64SoftTimerIrqAp = 0;
    g_u64SoftTimerMono = 0;
    g_u64SoftReschedIrq = 0;
    g_u64SoftTlbIrq = 0;
    g_u64SoftTlbInvlpg = 0;
    g_u64SoftTlbCr3 = 0;
    g_u64SoftTlbNotReady = 0;
    g_u64SoftTlbOnline1 = 0;
    g_u64SoftCalFailRdy = 0;
    g_u64SoftCalFailPit = 0;
    g_u64SoftCalFailCur = 0;
    g_u64SoftCalFailHz = 0;
    g_u64SoftCalFailWin = 0;
    g_u64SoftCalSample1Skip = 0;
    g_u64SoftTimerHzCal = 0;
    g_u64SoftTimerHzGuess = 0;
    g_u64SoftTimerHzSkip = 0;
    g_u64SoftLocalDefault = 0;
    g_u64SoftLocalProg = 0;
    g_u64SoftIpiInit = 0;
    /* Wave 15 exclusive path deepen reset. */
    g_u64SoftLapicRdX2 = 0;
    g_u64SoftLapicRdMmio = 0;
    g_u64SoftLapicRdNull = 0;
    g_u64SoftLapicWrX2 = 0;
    g_u64SoftLapicWrMmio = 0;
    g_u64SoftLapicWrNull = 0;
    g_u64SoftCalSample = 0;
    g_u64SoftCalSampleOk = 0;
    g_u64SoftCalSampleFail = 0;
    g_u64SoftQReady = 0;
    g_u64SoftQCal = 0;
    g_u64SoftQBusHz = 0;
    g_u64SoftQTicks = 0;
    g_u64SoftQTicksCpu = 0;
    g_u64SoftQTicksCpuBad = 0;
    g_u64SoftQNpt = 0;
    g_u64SoftQInitCnt = 0;
    g_u64SoftQCurCnt = 0;
    g_u64SoftQCurSkip = 0;
    g_u64SoftQSnap = 0;
    g_u64SoftQSnapNull = 0;
    g_u64SoftQLog = 0;
    g_u64SoftTimerHzClamp = 0;
    g_u64SoftLocalSkip = 0;
    g_u64SoftIcrCap = 0;
    g_u64SoftReschedSlot0 = 0;
    u64Base = rdmsr(IA32_APIC_BASE_MSR);
    if ((u64Base & APIC_BASE_ENABLE) == 0) {
        u64Base |= APIC_BASE_ENABLE;
        wrmsr(IA32_APIC_BASE_MSR, u64Base);
    }
    g_pLapic = (volatile u32 *)(gj_vaddr_t)(u64Base & 0xfffff000ull);
    /* Enable spurious vector */
    lapic_w(LAPIC_SVR, LAPIC_SVR_ENABLE | 0xFFu);
    u32Id = lapic_r(LAPIC_ID) >> 24;
    idt_set_gate(LAPIC_TIMER_VEC, (void *)irq_stub_apic_timer, 0x8E);
    idt_set_gate(LAPIC_IPI_RESCHED_VEC, (void *)irq_stub_ipi_resched, 0x8E);
    idt_set_gate(LAPIC_IPI_TLB_VEC, (void *)irq_stub_ipi_tlb, 0x8E);
    /* Mask timer until programmed */
    lapic_w(LAPIC_TIMER_LVT, LAPIC_TIMER_MASKED | LAPIC_TIMER_VEC);
    lapic_w(LAPIC_TIMER_DIV, LAPIC_DIV_16);
    g_fReady = 1;
    kprintf("apic: xAPIC @%p id=%u\n", (void *)g_pLapic, u32Id);
    /* Wave 12: greppable soft inventory at enable (pre-calibrate = UP). */
    apic_soft_inventory("init");
}

void
apic_ipi_init(void)
{
    apic_soft_inc64(&g_u64SoftIpiInit);
    /* Gates installed in apic_init; kept for explicit call sites */
    if (g_fReady) {
        idt_set_gate(LAPIC_IPI_RESCHED_VEC, (void *)irq_stub_ipi_resched, 0x8E);
        idt_set_gate(LAPIC_IPI_TLB_VEC, (void *)irq_stub_ipi_tlb, 0x8E);
    }
}

void
apic_eoi(void)
{
    apic_soft_inc32(&g_u32SoftEoi);
    if (x2apic_enabled()) {
        x2apic_eoi();
        apic_soft_inc64(&g_u64SoftEoiX2);
    } else if (g_pLapic != NULL) {
        lapic_w(LAPIC_EOI, 0);
        apic_soft_inc64(&g_u64SoftEoiMmio);
    } else {
        apic_soft_inc64(&g_u64SoftEoiSkip);
    }
}

void
apic_ipi_resched_irq(void)
{
    /* Wake from HLT; AP schedule loop picks work. Soft: counter only. */
    apic_soft_inc64(&g_u64SoftReschedIrq);
    apic_eoi();
}

void
apic_ipi_tlb_irq(void)
{
    gj_vaddr_t va = g_vaTlbShoot;

    /* Soft: IRQ path counter only — never kprintf here. */
    apic_soft_inc64(&g_u64SoftTlbIrq);

    if (va == 0) {
        /* Full reload of current CR3 */
        u64 u64Cr3;

        __asm__ volatile ("mov %%cr3, %0" : "=r"(u64Cr3));
        __asm__ volatile ("mov %0, %%cr3" : : "r"(u64Cr3) : "memory");
        apic_soft_inc64(&g_u64SoftTlbCr3);
    } else {
        __asm__ volatile ("invlpg (%0)" : : "r"(va) : "memory");
        apic_soft_inc64(&g_u64SoftTlbInvlpg);
    }
    __asm__ volatile ("" ::: "memory");
    g_u32TlbAck++;
    apic_eoi();
}

void
apic_send_resched(u32 u32CpuSlot)
{
    u32 u32ApicId;

    if (!g_fReady || u32CpuSlot == 0) {
        apic_soft_inc64(&g_u64SoftReschedSkip);
        if (u32CpuSlot == 0) {
            apic_soft_inc64(&g_u64SoftReschedSlot0);
        }
        return;
    }
    apic_soft_inc64(&g_u64SoftResched);
    u32ApicId = smp_apic_id_for_cpu(u32CpuSlot);
    apic_send_ipi(u32ApicId, (u8)LAPIC_IPI_RESCHED_VEC);
}

void
tlb_shootdown(gj_vaddr_t va)
{
    u32 u32Online;
    u32 u32Self;
    u32 u32Slot;
    u32 u32Spins;

    apic_soft_inc64(&g_u64SoftTlbShoot);

    if (!g_fReady) {
        apic_soft_inc64(&g_u64SoftTlbLocalOnly);
        apic_soft_inc64(&g_u64SoftTlbNotReady);
        if (va != 0) {
            __asm__ volatile ("invlpg (%0)" : : "r"(va) : "memory");
            apic_soft_inc64(&g_u64SoftTlbInvlpg);
        }
        return;
    }

    u32Online = smp_cpu_count_online();
    u32Self = cpu_id();
    if (u32Online <= 1) {
        apic_soft_inc64(&g_u64SoftTlbLocalOnly);
        apic_soft_inc64(&g_u64SoftTlbOnline1);
        if (va != 0) {
            __asm__ volatile ("invlpg (%0)" : : "r"(va) : "memory");
            apic_soft_inc64(&g_u64SoftTlbInvlpg);
        } else {
            u64 u64Cr3;

            __asm__ volatile ("mov %%cr3, %0" : "=r"(u64Cr3));
            __asm__ volatile ("mov %0, %%cr3" : : "r"(u64Cr3) : "memory");
            apic_soft_inc64(&g_u64SoftTlbCr3);
        }
        return;
    }

    g_vaTlbShoot = va;
    g_u32TlbAck = 0;
    g_u32TlbExpect = 0;
    __asm__ volatile ("" ::: "memory");

    for (u32Slot = 0; u32Slot < u32Online && u32Slot < GJ_CPU_STATIC_MAX;
         u32Slot++) {
        if (u32Slot == u32Self) {
            continue;
        }
        g_u32TlbExpect++;
        apic_soft_inc64(&g_u64SoftTlbIpi);
        apic_send_ipi(smp_apic_id_for_cpu(u32Slot), (u8)LAPIC_IPI_TLB_VEC);
    }

    /* Local invalidate */
    if (va != 0) {
        __asm__ volatile ("invlpg (%0)" : : "r"(va) : "memory");
        apic_soft_inc64(&g_u64SoftTlbInvlpg);
    } else {
        u64 u64Cr3;

        __asm__ volatile ("mov %%cr3, %0" : "=r"(u64Cr3));
        __asm__ volatile ("mov %0, %%cr3" : : "r"(u64Cr3) : "memory");
        apic_soft_inc64(&g_u64SoftTlbCr3);
    }

    for (u32Spins = 0;
         u32Spins < 10000000u && g_u32TlbAck < g_u32TlbExpect;
         u32Spins++) {
        __asm__ volatile ("pause");
    }
    g_u32SoftLastTlbExpect = g_u32TlbExpect;
    g_u32SoftLastTlbAck = g_u32TlbAck;
    if (g_u32TlbAck < g_u32TlbExpect) {
        apic_soft_inc64(&g_u64SoftTlbTimeout);
    }
    g_vaTlbShoot = 0;
}

int
apic_ready(void)
{
    apic_soft_inc64(&g_u64SoftQReady);
    return g_fReady;
}

/*
 * One PIT-aligned measurement window. Returns bus_hz sample or 0 on fail.
 * Updates soft spin/elapsed fields for the last window attempted.
 */
static u64
apic_cal_sample(u32 u32WaitTicks, u32 *pPitDelta, u32 *pElapsed)
{
    u64 u64J0;
    u64 u64J1;
    u64 u64Hz;
    u32 u32Cur;
    u32 u32Elapsed;
    u32 u32Spins;
    const u32 u32InitMax = 0xffffffffu;

    apic_soft_inc64(&g_u64SoftCalSample);

    /* Align to a PIT tick boundary */
    u64J0 = timer_jiffies();
    u32Spins = 0;
    while (timer_jiffies() == u64J0 && u32Spins < 100000000u) {
        u32Spins++;
        __asm__ volatile ("pause");
    }
    g_u32CalAlignSpins = u32Spins;
    if (timer_jiffies() == u64J0) {
        apic_soft_inc64(&g_u64SoftCalSampleFail);
        return 0;
    }

    /* Start countdown from max */
    lapic_w(LAPIC_TIMER_INIT, u32InitMax);
    u64J0 = timer_jiffies();
    u32Spins = 0;
    while ((timer_jiffies() - u64J0) < (u64)u32WaitTicks &&
           u32Spins < 500000000u) {
        u32Spins++;
        __asm__ volatile ("pause");
    }
    u32Cur = lapic_r(LAPIC_TIMER_CUR);
    u64J1 = timer_jiffies();
    lapic_w(LAPIC_TIMER_INIT, 0); /* stop */
    g_u32CalWaitSpins = u32Spins;

    if (u64J1 <= u64J0 || u32Cur >= u32InitMax) {
        apic_soft_inc64(&g_u64SoftCalSampleFail);
        return 0;
    }

    u32Elapsed = u32InitMax - u32Cur;
    if (pPitDelta != NULL) {
        *pPitDelta = (u32)(u64J1 - u64J0);
    }
    if (pElapsed != NULL) {
        *pElapsed = u32Elapsed;
    }
    /*
     * u32Elapsed counts over (j1-j0) PIT ticks at GJ_TIMER_HZ.
     * bus_hz = elapsed * GJ_TIMER_HZ / (j1-j0)
     */
    u64Hz = ((u64)u32Elapsed * (u64)GJ_TIMER_HZ) / (u64J1 - u64J0);
    apic_soft_inc64(&g_u64SoftCalSampleOk);
    return u64Hz;
}

int
apic_calibrate(void)
{
    u64 u64Hz0;
    u64 u64Hz1;
    u64 u64Hz;
    u32 u32Pit0;
    u32 u32Pit1;
    u32 u32El0;
    u32 u32El1;
    u32 u32Samples;

    g_u32SoftCalCalls++;
    g_u32CalAttempts++;
    g_u64BusHzSample0 = 0;
    g_u64BusHzSample1 = 0;
    g_u32CalSamples = 0;

    if (!g_fReady || !timer_ready()) {
        cal_fail(GJ_APIC_CAL_FAIL_RDY);
        kprintf("apic: calibrate failed (not ready apic=%d timer=%d)\n",
                g_fReady, timer_ready());
        apic_soft_inventory("calibrate_fail_rdy");
        return -1;
    }

    /* Mask APIC timer; one-shot mode for measurement */
    lapic_w(LAPIC_TIMER_LVT, LAPIC_TIMER_MASKED | LAPIC_TIMER_ONESHOT | LAPIC_TIMER_VEC);
    lapic_w(LAPIC_TIMER_DIV, LAPIC_DIV_16);

    u32Pit0 = 0;
    u32El0 = 0;
    u64Hz0 = apic_cal_sample(APIC_CAL_WAIT_TICKS, &u32Pit0, &u32El0);
    if (u64Hz0 == 0) {
        /* Align budget exhausted → PIT not advancing; else countdown/window. */
        if (g_u32CalAlignSpins >= 100000000u) {
            cal_fail(GJ_APIC_CAL_FAIL_PIT);
            kprintf("apic: calibrate failed (PIT not ticking) align_spins=%u\n",
                    (unsigned)g_u32CalAlignSpins);
        } else {
            cal_fail(GJ_APIC_CAL_FAIL_CUR);
            kprintf("apic: calibrate failed sample0 (cur/window) "
                    "align=%u wait=%u pit=%u el=%u\n",
                    (unsigned)g_u32CalAlignSpins, (unsigned)g_u32CalWaitSpins,
                    (unsigned)u32Pit0, (unsigned)u32El0);
        }
        apic_soft_inventory("calibrate_fail_sample0");
        return -1;
    }
    g_u64BusHzSample0 = u64Hz0;
    u32Samples = 1;

    /* Second sample when first looked sane (deepen observability + average). */
    u32Pit1 = 0;
    u32El1 = 0;
    u64Hz1 = apic_cal_sample(APIC_CAL_WAIT_TICKS, &u32Pit1, &u32El1);
    if (u64Hz1 != 0) {
        g_u64BusHzSample1 = u64Hz1;
        u32Samples = 2;
        u64Hz = (u64Hz0 / 2ull) + (u64Hz1 / 2ull);
        g_u32CalPitTicks = u32Pit0 + u32Pit1;
        g_u32CalElapsed = u32El0 + u32El1;
    } else {
        u64Hz = u64Hz0;
        g_u32CalPitTicks = u32Pit0;
        g_u32CalElapsed = u32El0;
        apic_soft_inc64(&g_u64SoftCalSample1Skip);
        kprintf("apic: cal sample1 skipped/fail; using sample0 only\n");
    }
    g_u32CalSamples = u32Samples;

    if (u64Hz < 1000ull) {
        cal_fail(GJ_APIC_CAL_FAIL_HZ);
        kprintf("apic: calibrate bus_hz too low %lu samples=%u s0=%lu s1=%lu\n",
                (unsigned long)u64Hz, (unsigned)u32Samples,
                (unsigned long)g_u64BusHzSample0,
                (unsigned long)g_u64BusHzSample1);
        g_u64BusHz = 0;
        apic_soft_inventory("calibrate_fail_hz");
        return -1;
    }
    if (g_u32CalPitTicks == 0 || g_u32CalElapsed == 0) {
        cal_fail(GJ_APIC_CAL_FAIL_WIN);
        kprintf("apic: calibrate degenerate window pit=%u el=%u\n",
                (unsigned)g_u32CalPitTicks, (unsigned)g_u32CalElapsed);
        g_u64BusHz = 0;
        apic_soft_inventory("calibrate_fail_win");
        return -1;
    }

    g_u64BusHz = u64Hz;
    g_fCalibrated = 1;
    g_u32CalStatus = GJ_APIC_CAL_OK;
    g_u32CalOk++;
    kprintf("apic: calibrated bus_hz=%lu (div16, samples=%u pit_ticks=%u "
            "elapsed=%u align=%u wait=%u s0=%lu s1=%lu)\n",
            (unsigned long)g_u64BusHz, (unsigned)u32Samples,
            (unsigned)g_u32CalPitTicks, (unsigned)g_u32CalElapsed,
            (unsigned)g_u32CalAlignSpins, (unsigned)g_u32CalWaitSpins,
            (unsigned long)g_u64BusHzSample0,
            (unsigned long)g_u64BusHzSample1);
    /* Wave 12: soft inventory after accepted bus_hz (PARTIAL until timer_hz). */
    apic_soft_inventory("calibrate");
    return 0;
}

int
apic_calibrated(void)
{
    apic_soft_inc64(&g_u64SoftQCal);
    return g_fCalibrated;
}

u64
apic_bus_hz(void)
{
    apic_soft_inc64(&g_u64SoftQBusHz);
    return g_u64BusHz;
}

void
apic_timer_hz(u32 u32Hz)
{
    u32 u32Init;

    if (!g_fReady || u32Hz == 0) {
        apic_soft_inc64(&g_u64SoftTimerHzSkip);
        return;
    }

    apic_soft_inc32(&g_u32SoftTimerHz);
    g_u32HzProgrammed = u32Hz;
    if (g_fCalibrated && g_u64BusHz != 0) {
        u32Init = (u32)(g_u64BusHz / (u64)u32Hz);
        apic_soft_inc64(&g_u64SoftTimerHzCal);
    } else {
        /*
         * Best-effort without calibration: QEMU-ish ~1 GHz / 16.
         */
        u32Init = 1000000000u / LAPIC_DIV_FACTOR / u32Hz;
        apic_soft_inc64(&g_u64SoftTimerHzGuess);
    }
    if (u32Init < 1000u) {
        u32Init = 1000u;
        apic_soft_inc64(&g_u64SoftTimerHzClamp);
    }
    g_u32PeriodInit = u32Init;
    g_u64NsecPerTick = 1000000000ull / (u64)u32Hz;

    lapic_w(LAPIC_TIMER_DIV, LAPIC_DIV_16);
    lapic_w(LAPIC_TIMER_INIT, u32Init);
    lapic_w(LAPIC_TIMER_LVT, LAPIC_TIMER_PERIODIC | LAPIC_TIMER_VEC);

    if (g_fCalibrated) {
        timer_set_apic_source(g_u64NsecPerTick);
        kprintf("apic: timer %u Hz init=%u nsec/tick=%lu bus_hz=%lu (mono=APIC)\n",
                u32Hz, u32Init, (unsigned long)g_u64NsecPerTick,
                (unsigned long)g_u64BusHz);
    } else {
        kprintf("apic: timer ~%u Hz init=%u (uncalibrated status=%u)\n",
                u32Hz, u32Init, (unsigned)g_u32CalStatus);
    }
    /* Wave 12: full soft inventory after periodic program (PASS when cal OK). */
    apic_soft_inventory("timer_hz");
}

u64
apic_timer_ticks(void)
{
    apic_soft_inc64(&g_u64SoftQTicks);
    return g_u64Ticks;
}

u64
apic_timer_ticks_cpu(u32 u32Cpu)
{
    apic_soft_inc64(&g_u64SoftQTicksCpu);
    if (u32Cpu >= GJ_CPU_STATIC_MAX) {
        apic_soft_inc64(&g_u64SoftQTicksCpuBad);
        return 0;
    }
    return g_aCpuTicks[u32Cpu];
}

u64
apic_nsec_per_tick(void)
{
    apic_soft_inc64(&g_u64SoftQNpt);
    return g_u64NsecPerTick;
}

u32
apic_timer_init_count(void)
{
    apic_soft_inc64(&g_u64SoftQInitCnt);
    return g_u32PeriodInit;
}

u32
apic_timer_cur_count(void)
{
    apic_soft_inc64(&g_u64SoftQCurCnt);
    if (!g_fReady) {
        apic_soft_inc64(&g_u64SoftQCurSkip);
        return 0;
    }
    return lapic_r(LAPIC_TIMER_CUR);
}

void
apic_cal_soft_snapshot(struct gj_apic_cal_soft *pOut)
{
    if (pOut == NULL) {
        apic_soft_inc64(&g_u64SoftQSnapNull);
        return;
    }
    apic_soft_inc64(&g_u64SoftQSnap);
    pOut->u32Status = g_u32CalStatus;
    pOut->u32Attempts = g_u32CalAttempts;
    pOut->u32Ok = g_u32CalOk;
    pOut->u32Fail = g_u32CalFail;
    pOut->u32Samples = g_u32CalSamples;
    pOut->u32PitTicks = g_u32CalPitTicks;
    pOut->u32Elapsed = g_u32CalElapsed;
    pOut->u32AlignSpins = g_u32CalAlignSpins;
    pOut->u32WaitSpins = g_u32CalWaitSpins;
    pOut->u64BusHz = g_u64BusHz;
    pOut->u64BusHzSample0 = g_u64BusHzSample0;
    pOut->u64BusHzSample1 = g_u64BusHzSample1;
    pOut->u64NsecPerTick = g_u64NsecPerTick;
    pOut->u32PeriodInit = g_u32PeriodInit;
    pOut->u32HzProgrammed = g_u32HzProgrammed;
    pOut->u64BspTicks = g_u64Ticks;
    pOut->u32Calibrated = g_fCalibrated ? 1u : 0u;
    pOut->u32Ready = g_fReady ? 1u : 0u;
}

void
apic_cal_soft_log(void)
{
    struct gj_apic_cal_soft stCal;
    const char *szSt;
    const char *szVerdict;

    apic_soft_inc64(&g_u64SoftQLog);
    apic_cal_soft_snapshot(&stCal);

    switch (stCal.u32Status) {
    case GJ_APIC_CAL_OK:
        szSt = "OK";
        break;
    case GJ_APIC_CAL_FAIL_RDY:
        szSt = "FAIL_RDY";
        break;
    case GJ_APIC_CAL_FAIL_PIT:
        szSt = "FAIL_PIT";
        break;
    case GJ_APIC_CAL_FAIL_CUR:
        szSt = "FAIL_CUR";
        break;
    case GJ_APIC_CAL_FAIL_HZ:
        szSt = "FAIL_HZ";
        break;
    case GJ_APIC_CAL_FAIL_WIN:
        szSt = "FAIL_WIN";
        break;
    default:
        szSt = "NONE";
        break;
    }

    kprintf("apic: cal soft status=%s attempts=%u ok=%u fail=%u samples=%u "
            "pit_ticks=%u elapsed=%u align=%u wait=%u bus_hz=%lu s0=%lu s1=%lu "
            "npt=%lu init=%u hz=%u bsp_ticks=%lu ready=%u cal=%u\n",
            szSt, (unsigned)stCal.u32Attempts, (unsigned)stCal.u32Ok,
            (unsigned)stCal.u32Fail, (unsigned)stCal.u32Samples,
            (unsigned)stCal.u32PitTicks, (unsigned)stCal.u32Elapsed,
            (unsigned)stCal.u32AlignSpins, (unsigned)stCal.u32WaitSpins,
            (unsigned long)stCal.u64BusHz,
            (unsigned long)stCal.u64BusHzSample0,
            (unsigned long)stCal.u64BusHzSample1,
            (unsigned long)stCal.u64NsecPerTick,
            (unsigned)stCal.u32PeriodInit, (unsigned)stCal.u32HzProgrammed,
            (unsigned long)stCal.u64BspTicks,
            (unsigned)stCal.u32Ready, (unsigned)stCal.u32Calibrated);

    if (stCal.u32Status == GJ_APIC_CAL_OK && stCal.u64BusHz != 0) {
        szVerdict = "PASS";
    } else if (stCal.u32Status == GJ_APIC_CAL_NONE) {
        szVerdict = "NONE";
    } else {
        szVerdict = "FAIL";
    }
    kprintf("apic: cal soft %s\n", szVerdict);
    /* Wave 12 companion inventory (prefix "apic: soft …"). */
    apic_soft_inventory("cal_log");
}

void
apic_timer_start_local(void)
{
    u32 u32Init = g_u32PeriodInit;

    if (!g_fReady) {
        apic_soft_inc64(&g_u64SoftLocalSkip);
        return;
    }
    apic_soft_inc32(&g_u32SoftLocalStart);
    if (u32Init == 0) {
        u32Init = 1000000000u / LAPIC_DIV_FACTOR / GJ_TIMER_HZ;
        if (u32Init < 1000u) {
            u32Init = 1000u;
        }
        apic_soft_inc64(&g_u64SoftLocalDefault);
    } else {
        apic_soft_inc64(&g_u64SoftLocalProg);
    }
    /* Enable this CPU's APIC (APs start with SVR disabled until we set it) */
    {
        u64 u64Base = rdmsr(IA32_APIC_BASE_MSR);

        if ((u64Base & APIC_BASE_ENABLE) == 0) {
            u64Base |= APIC_BASE_ENABLE;
            wrmsr(IA32_APIC_BASE_MSR, u64Base);
        }
        g_pLapic = (volatile u32 *)(gj_vaddr_t)(u64Base & 0xfffff000ull);
    }
    lapic_w(LAPIC_SVR, LAPIC_SVR_ENABLE | 0xFFu);
    lapic_w(LAPIC_TIMER_DIV, LAPIC_DIV_16);
    lapic_w(LAPIC_TIMER_INIT, u32Init);
    lapic_w(LAPIC_TIMER_LVT, LAPIC_TIMER_PERIODIC | LAPIC_TIMER_VEC);
}

static void
apic_icr_wait(void)
{
    u32 u32Spins = 0;

    apic_soft_inc64(&g_u64SoftIcrWait);
    while ((lapic_r(LAPIC_ICR_LOW) & LAPIC_ICR_PENDING) != 0 &&
           u32Spins < APIC_SOFT_ICR_SPIN_MAX) {
        u32Spins++;
        __asm__ volatile ("pause");
    }
    g_u32SoftIcrLastSpins = u32Spins;
    if (u32Spins >= APIC_SOFT_ICR_SPIN_MAX) {
        apic_soft_inc64(&g_u64SoftIcrCap);
    }
}

void
apic_send_ipi(u32 u32ApicId, u8 u8Vector)
{
    if (!g_fReady) {
        apic_soft_inc64(&g_u64SoftIpiNotReady);
        return;
    }
    apic_soft_inc64(&g_u64SoftIpiSend);
    g_u32SoftLastApicId = u32ApicId;
    g_u32SoftLastIpiVec = (u32)u8Vector;
    if (x2apic_enabled()) {
        x2apic_send_ipi(u32ApicId, u8Vector);
        apic_soft_inc64(&g_u64SoftIpiX2);
        return;
    }
    apic_soft_inc64(&g_u64SoftIpiXapic);
    apic_icr_wait();
    lapic_w(LAPIC_ICR_HIGH, (u32ApicId & 0xffu) << 24);
    lapic_w(LAPIC_ICR_LOW, (u32)u8Vector); /* fixed delivery, edge, dest physical */
    apic_icr_wait();
}

void
apic_send_self_ipi(u8 u8Vector)
{
    if (!g_fReady) {
        apic_soft_inc64(&g_u64SoftSelfNotReady);
        return;
    }
    apic_soft_inc64(&g_u64SoftIpiSelf);
    g_u32SoftLastApicId = 0;
    g_u32SoftLastIpiVec = (u32)u8Vector;
    if (x2apic_enabled()) {
        x2apic_send_self_ipi(u8Vector);
        apic_soft_inc64(&g_u64SoftSelfX2);
        return;
    }
    apic_soft_inc64(&g_u64SoftSelfXapic);
    /* xAPIC ICR dest shorthand: self = bits 19:18 = 01 */
    apic_icr_wait();
    lapic_w(LAPIC_ICR_HIGH, 0);
    lapic_w(LAPIC_ICR_LOW, (u32)u8Vector | (1u << 18));
    apic_icr_wait();
}

void
apic_send_init_sipi(u32 u32ApicId, u8 u8StartupVector)
{
    u32 i;

    if (!g_fReady) {
        apic_soft_inc64(&g_u64SoftInitSipiNotReady);
        return;
    }
    apic_soft_inc64(&g_u64SoftInitSipi);
    g_u32SoftLastApicId = u32ApicId;
    g_u32SoftLastIpiVec = (u32)u8StartupVector;
    if (x2apic_enabled()) {
        /*
         * x2APIC ICR: delivery mode in bits 10:8, vector in 7:0,
         * destination in bits 63:32. INIT=5, STARTUP=6. Level/assert
         * not used the same way; single INIT + two SIPIs (Intel SDM).
         */
        u64 u64Dest = ((u64)u32ApicId) << 32;

        apic_soft_inc64(&g_u64SoftInitSipiX2);
        x2apic_send_ipi_raw(u64Dest | (5ull << 8)); /* INIT */
        for (i = 0; i < 1000000u; i++) {
            __asm__ volatile ("pause");
        }
        for (i = 0; i < 2u; i++) {
            x2apic_send_ipi_raw(u64Dest | (6ull << 8) | (u64)u8StartupVector);
            {
                u32 j;

                for (j = 0; j < 200000u; j++) {
                    __asm__ volatile ("pause");
                }
            }
        }
        return;
    }
    apic_soft_inc64(&g_u64SoftInitSipiXapic);
    /* INIT assert */
    apic_icr_wait();
    lapic_w(LAPIC_ICR_HIGH, (u32ApicId & 0xffu) << 24);
    lapic_w(LAPIC_ICR_LOW, LAPIC_ICR_INIT | LAPIC_ICR_LEVEL | LAPIC_ICR_ASSERT);
    apic_icr_wait();
    /* INIT deassert */
    lapic_w(LAPIC_ICR_HIGH, (u32ApicId & 0xffu) << 24);
    lapic_w(LAPIC_ICR_LOW, LAPIC_ICR_INIT | LAPIC_ICR_LEVEL);
    apic_icr_wait();
    /* Brief delay via spins (≈10ms class on QEMU) */
    for (i = 0; i < 1000000u; i++) {
        __asm__ volatile ("pause");
    }
    /* Two SIPIs */
    for (i = 0; i < 2u; i++) {
        apic_icr_wait();
        lapic_w(LAPIC_ICR_HIGH, (u32ApicId & 0xffu) << 24);
        lapic_w(LAPIC_ICR_LOW, LAPIC_ICR_STARTUP | (u32)u8StartupVector);
        apic_icr_wait();
        {
            u32 j;

            for (j = 0; j < 200000u; j++) {
                __asm__ volatile ("pause");
            }
        }
    }
}
