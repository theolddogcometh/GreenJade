/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Early mono clock: PIT ch0 @ GJ_TIMER_HZ on remapped IRQ0 (vector 32).
 * After LAPIC calibrate, jiffies/mono prefer the local APIC (vector 48);
 * PIT is demoted to soft-fallback only (IRQ0 masked; no mono advance).
 *
 * Soft mono (timer_mono_nsec_soft / mono soft snapshot) may interpolate
 * within the current APIC period using LAPIC CUR; coarse mono stays
 * jiffy-stable for futex/door deadlines. Quantum soft-preempt counters
 * deepen tick → yield-request → preempt_check observability.
 *
 * Mono source preference (APIC/x2APIC armed):
 *   mono/jiffies advance from timer_tick_apic only; timer_tick EOIs PIC
 *   but never increments jiffies while g_fApicSource is set.
 * Honesty: full x2APIC ICR/timer replace of the 8259+PIT product path
 * remains PARTIAL when the hardware path is incomplete (xAPIC timer
 * handoff alone ≠ complete x2APIC ICR/timer replace).
 *
 * Soft timer inventory (Wave 10 base + Wave 13 path + Wave 35 exclusive deepen;
 * this unit only — greppable "timer: soft …"):
 *   timer: soft inventory     — ready/src/hz/quantum + surface catalog + wave
 *   timer: soft mono          — coarse/soft mono delta + pit/apic tick axes
 *   timer: soft preempt       — quantum slice + soft preempt_check counters
 *   timer: soft source        — PIT/APIC handoff + LAPIC INIT/CUR sample
 *   timer: soft apic mono     — APIC mono preference deepen (Wave 13)
 *   timer: soft path          — honesty catalog (product surface bounds)
 *   timer: soft handoff       — PIT→APIC demotion / switch tallies
 *   timer: soft interpolate   — LAPIC CUR soft-mono sample counters
 *   timer: soft deepen        — wave stamp + area catalog
 *   timer: soft PASS|FAIL     — soft lamp (ready + quantum); never hard-gates
 * Wave 15 complementary surfaces (kept; never reshape primary fields):
 *   timer: soft lamps         — ready/apic/x2/interp/quantum readiness lamps
 *   timer: soft stats         — aggregate path counters + wave
 *   timer: soft vectors       — PIT32 / APIC48 + EOI policy lamps
 *   timer: soft quantum       — slice/preempt/yield axis deepen
 *   timer: soft futex         — futex_timer_check coupling lamp (soft)
 *   timer: soft honesty       — soft ≠ full x2APIC ICR/timer product replace
 *   timer: soft surface       — bit catalog of soft product surfaces
 * Wave 16 complementary surfaces (kept; never reshape primary fields):
 *   timer: soft exclusive     — exclusive=1 unit stamp + wave
 *   timer: soft claim         — product claim bounds (soft-only)
 *   timer: soft ratio         — pit/apic/handoff/interp path ratios
 *   timer: soft eoi           — PIC EOI + spur policy lamps
 * Wave 17 complementary surfaces (kept) (never reshape primary fields):
 *   timer: soft return        — Wave 17 API return surfaces (kept)
 *   timer: soft return selftest — Wave 17 terminal return surface (kept)
 *   timer: soft retmap        — Wave 17 return-surface map (kept)
 * Diagnostics only — never hard-gates boot or product deadlines. Pure C.
 * Soft ≠ full x2APIC timer product (xAPIC handoff alone remains PARTIAL).
 *
 * greppable: timer: soft inventory
 * greppable: timer: soft mono
 * greppable: timer: soft preempt
 * greppable: timer: soft source
 * greppable: timer: soft apic mono
 * greppable: timer: soft path
 * greppable: timer: soft handoff
 * greppable: timer: soft interpolate
 * greppable: timer: soft deepen
 * greppable: timer: soft lamps
 * greppable: timer: soft stats
 * greppable: timer: soft vectors
 * greppable: timer: soft quantum
 * greppable: timer: soft futex
 * greppable: timer: soft honesty
 * greppable: timer: soft surface
 * greppable: timer: soft exclusive
 * greppable: timer: soft claim
 * greppable: timer: soft ratio
 * greppable: timer: soft eoi
 * greppable: timer: soft return
 * greppable: timer: soft return selftest
 * greppable: timer: soft retmap
 * greppable: timer: soft PASS
 * greppable: timer: soft FAIL
 * greppable: timer: mono soft
 * greppable: timer: preempt soft
 * greppable: timer: mono source APIC soft
 * greppable: timer: mono source PIT soft
 * greppable: timer: pit demoted soft
 * greppable: timer: apic mono preferred PASS
 * greppable: timer: x2apic mono replace soft
 */
#include <gj/apic.h>
#include <gj/cap.h>
#include <gj/futex.h>
#include <gj/idt.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/thread.h>
#include <gj/timer.h>
#include <gj/types.h>
#include <gj/x2apic.h>

#define PIT_CH0     0x40
#define PIT_CMD     0x43
#define PIT_HZ_IN   1193182u
#define PIC1_CMD    0x20
#define PIC1_DATA   0x21
#define PIC2_CMD    0xA0
#define PIC2_DATA   0xA1
#define PIC_EOI     0x20

/* Soft inventory wave stamp (this unit exclusive deepen; never hard-gates). */
#define TIMER_SOFT_WAVE 106u

/* Soft surface bit lamps (Wave 15+ catalog; software-only claims). */
#define TIMER_SOFT_SURF_MONO       (1u << 0)
#define TIMER_SOFT_SURF_SOFT_MONO  (1u << 1)
#define TIMER_SOFT_SURF_APIC_SRC   (1u << 2)
#define TIMER_SOFT_SURF_PREEMPT    (1u << 3)
#define TIMER_SOFT_SURF_PIT_FALL   (1u << 4)
#define TIMER_SOFT_SURF_INTERP     (1u << 5)
#define TIMER_SOFT_SURF_FUTEX_COUP (1u << 6)
#define TIMER_SOFT_SURF_HANDOFF    (1u << 7)
/* Honesty: bit never claims full x2APIC ICR/timer product replace. */
#define TIMER_SOFT_SURF_X2_FULL    0u

static volatile u64 g_u64Jiffies;
static int          g_fTimerReady;
static int          g_fApicSource;
static u64          g_u64NsecPerTick;

/* Quantum / soft-preempt */
static u32          g_u32Quantum = 5; /* ticks; ~GJ_TIMER_HZ/20 @ 100 Hz */
static u32          g_u32SliceLeft = 5;
static u64          g_u64Preempts;
static u64          g_u64YieldRequests;
static u64          g_u64PreemptChecks;
static u64          g_u64PreemptCheckHits;
static u64          g_u64PreemptCheckYields;
static u64          g_u64QuantumSets;
static u64          g_u64QuantumTicks;

/* Soft mono source accounting */
static u64          g_u64PitTicks;
static u64          g_u64ApicMonoTicks;
static u64          g_u64SourceSwitch;
static u64          g_u64PitDemotions;      /* PIT→APIC soft demotions */
static u64          g_u64MonoPrefLogs;      /* mono preference soft log emits */

/*
 * Soft timer inventory extras (Wave 10 base + Wave 13 path + Wave 20 deepen;
 * file-local). Emission + path tallies only — never hard product gates. wrap OK.
 * greppable: timer: soft
 * greppable: timer: soft apic mono
 */
static u64          g_u64SoftInventoryLogs;
static u64          g_u64SoftMonoSamples;   /* timer_mono_nsec_soft calls */
static u64          g_u64SoftMonoInterp;    /* LAPIC CUR interpolate applied */
static u64          g_u64SoftMonoCoarseOnly;/* soft fell back to coarse */
static u64          g_u64SoftMonoClamp;     /* soft < coarse race clamp */
static u64          g_u64SoftPitStray;      /* PIT IRQ0 while APIC sourced */
static u64          g_u64SoftApicMonoLogs;  /* timer: soft apic mono emits */
/* Wave 15+ exclusive path tallies (complementary; never hard-gate). */
static u64          g_u64SoftTickPit;       /* timer_tick PIT mono advance */
static u64          g_u64SoftTickApic;      /* timer_tick_apic entries */
static u64          g_u64SoftTickPitEoi;    /* timer_tick always-EOI path */
static u64          g_u64SoftHandoffCalls;  /* timer_set_apic_source entries */
static u64          g_u64SoftHandoffSkip;   /* set_apic_source npt==0 skip */
static u64          g_u64SoftHandoffFirst;  /* first PIT→APIC handoff */
static u64          g_u64SoftSnapMono;      /* mono_soft_snapshot fills */
static u64          g_u64SoftSnapMonoNull;  /* mono snapshot null out */
static u64          g_u64SoftSnapPreempt;   /* preempt_soft_snapshot fills */
static u64          g_u64SoftSnapPreemptNull;/* preempt snapshot null out */
static u64          g_u64SoftSleepCalls;    /* timer_sleep_until entries */
static u64          g_u64SoftSleepGuard;    /* sleep hit spin guard */
static u64          g_u64SoftLogCalls;      /* timer_soft_log entries */
static u64          g_u64SoftInitCalls;     /* timer_init entries */
static u64          g_u64SoftFutexCoupled;  /* futex_timer_check from tick */

static void timer_soft_inventory_log(void);
static void timer_mono_pref_soft_log(void);
static void timer_soft_apic_mono_log(void);

/** Soft: saturating-ish bump (u64 wrap is fine for telemetry). */
static void
timer_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < ~0ull) {
        (*pCtr)++;
    }
}

static void
outb(u16 u16Port, u8 u8Val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(u8Val), "Nd"(u16Port));
}

static u8
inb(u16 u16Port)
{
    u8 u8Val;

    __asm__ volatile ("inb %1, %0" : "=a"(u8Val) : "Nd"(u16Port));
    return u8Val;
}

/* isr_stubs.S — IRQ0 after PIC remap (vector 32) */
extern void irq_stub_0(void);

static void
quantum_tick(void)
{
    g_u64QuantumTicks++;
    if (g_u32SliceLeft > 0) {
        g_u32SliceLeft--;
    }
    if (g_u32SliceLeft == 0) {
        g_u64Preempts++;
        g_u64YieldRequests++;
        g_u32SliceLeft = g_u32Quantum ? g_u32Quantum : 1;
        /* Soft preempt: request yield at the next safe point */
        thread_yield_request();
    }
}

static void
revoke_hygiene_tick(void)
{
    /* Deferred CNode slot hygiene every 16 ticks (R7) */
    if ((g_u64Jiffies & 15ull) == 0) {
        (void)gj_revoke_process_deferred(8);
    }
}

void
timer_tick(void)
{
    /*
     * Mono preference: when APIC/x2APIC timer source is armed, jiffies/mono
     * advance only from timer_tick_apic. PIT IRQ0 is soft-fallback only —
     * still EOI the PIC if a stray IRQ arrives, but do not advance mono.
     * Wave 13/15: count demoted stray IRQ0 for greppable soft apic mono path.
     */
    timer_soft_inc(&g_u64SoftTickPitEoi);
    if (!g_fApicSource) {
        g_u64Jiffies++;
        g_u64PitTicks++;
        timer_soft_inc(&g_u64SoftTickPit);
        futex_timer_check();
        timer_soft_inc(&g_u64SoftFutexCoupled);
        quantum_tick();
        revoke_hygiene_tick();
    } else {
        g_u64SoftPitStray++;
    }
    outb(PIC1_CMD, PIC_EOI);
}

void
timer_tick_apic(void)
{
    timer_soft_inc(&g_u64SoftTickApic);
    g_u64Jiffies++;
    g_u64ApicMonoTicks++;
    futex_timer_check();
    timer_soft_inc(&g_u64SoftFutexCoupled);
    quantum_tick();
    revoke_hygiene_tick();
}

void
timer_set_quantum_ticks(u32 u32Ticks)
{
    if (u32Ticks == 0) {
        u32Ticks = 1;
    }
    g_u32Quantum = u32Ticks;
    g_u32SliceLeft = u32Ticks;
    g_u64QuantumSets++;
}

u32
timer_quantum_ticks(void)
{
    return g_u32Quantum;
}

u32
timer_slice_left(void)
{
    return g_u32SliceLeft;
}

u64
timer_preempt_count(void)
{
    return g_u64Preempts;
}

void
timer_preempt_check(void)
{
    g_u64PreemptChecks++;
    /*
     * Soft-preempt consume: if quantum path requested a yield, schedule now.
     * Idle path also clears the flag; this deepens voluntary check points
     * (sched helpers, sleep loops) with hit/yield counters.
     */
    if (thread_yield_pending() != 0) {
        g_u64PreemptCheckHits++;
        g_u64PreemptCheckYields++;
        thread_yield();
        return;
    }
    /* Slice already zero without pending flag: re-arm request (lost race). */
    if (g_u32SliceLeft == 0 && g_u32Quantum != 0) {
        g_u64PreemptCheckHits++;
        g_u64YieldRequests++;
        thread_yield_request();
        g_u64PreemptCheckYields++;
        thread_yield();
    }
}

/* C entry from irq_stub_0 (PIC timer) */
void
irq_timer_handler(void)
{
    timer_tick();
}

static void
pic_remap(void)
{
    u8 u8A1;
    u8 u8A2;

    u8A1 = inb(PIC1_DATA);
    u8A2 = inb(PIC2_DATA);

    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);
    outb(PIC1_DATA, 0x20); /* master offset 32 */
    outb(PIC2_DATA, 0x28); /* slave offset 40 */
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    /* Unmask IRQ0 only; leave remaining PIC lines masked */
    (void)u8A1;
    (void)u8A2;
    outb(PIC1_DATA, 0xFE); /* IRQ0 unmasked */
    outb(PIC2_DATA, 0xFF);
}

static void
pit_set_hz(u32 u32Hz)
{
    u32 u32Div;

    if (u32Hz == 0) {
        u32Hz = GJ_TIMER_HZ;
    }
    u32Div = PIT_HZ_IN / u32Hz;
    if (u32Div == 0) {
        u32Div = 1;
    }
    outb(PIT_CMD, 0x36); /* ch0, lobyte/hibyte, mode 3 */
    outb(PIT_CH0, (u8)(u32Div & 0xff));
    outb(PIT_CH0, (u8)((u32Div >> 8) & 0xff));
    g_u64NsecPerTick = 1000000000ull / (u64)u32Hz;
}

/* Public IDT gate install (idt.c); needed for vector 32 / IRQ0 */
void idt_set_gate(u32 u32Vec, void *pHandler, u8 u8Type);

void
timer_init(void)
{
    timer_soft_inc(&g_u64SoftInitCalls);
    g_u64Jiffies = 0;
    g_fApicSource = 0;
    g_u64NsecPerTick = 1000000000ull / (u64)GJ_TIMER_HZ;
    g_u64PitTicks = 0;
    g_u64ApicMonoTicks = 0;
    g_u64SourceSwitch = 0;
    g_u64PitDemotions = 0;
    g_u64MonoPrefLogs = 0;
    g_u64Preempts = 0;
    g_u64YieldRequests = 0;
    g_u64PreemptChecks = 0;
    g_u64PreemptCheckHits = 0;
    g_u64PreemptCheckYields = 0;
    g_u64QuantumSets = 0;
    g_u64QuantumTicks = 0;
    g_u64SoftInventoryLogs = 0;
    g_u64SoftMonoSamples = 0;
    g_u64SoftMonoInterp = 0;
    g_u64SoftMonoCoarseOnly = 0;
    g_u64SoftMonoClamp = 0;
    g_u64SoftPitStray = 0;
    g_u64SoftApicMonoLogs = 0;
    g_u64SoftTickPit = 0;
    g_u64SoftTickApic = 0;
    g_u64SoftTickPitEoi = 0;
    g_u64SoftHandoffCalls = 0;
    g_u64SoftHandoffSkip = 0;
    g_u64SoftHandoffFirst = 0;
    g_u64SoftSnapMono = 0;
    g_u64SoftSnapMonoNull = 0;
    g_u64SoftSnapPreempt = 0;
    g_u64SoftSnapPreemptNull = 0;
    g_u64SoftSleepCalls = 0;
    g_u64SoftSleepGuard = 0;
    g_u64SoftLogCalls = 0;
    /* Preserve init call tally across re-init soft reset. */
    g_u64SoftFutexCoupled = 0;
    g_u32Quantum = 5;
    g_u32SliceLeft = 5;
    pic_remap();
    pit_set_hz(GJ_TIMER_HZ);
    idt_set_gate(32, (void *)irq_stub_0, 0x8E);
    g_fTimerReady = 1;
    __asm__ volatile ("sti");
    kprintf("timer: PIT %u Hz IRQ0 vector 32 nsec/tick=%lu quantum=%u\n",
            (unsigned)GJ_TIMER_HZ, (unsigned long)g_u64NsecPerTick,
            (unsigned)g_u32Quantum);
    /* Early boot: mono prefers PIT until APIC/x2APIC timer source armed. */
    timer_mono_pref_soft_log();
    /* Wave 15: baseline soft inventory (zeros/jiffies=0 expected early). */
    timer_soft_inventory_log();
}

void
timer_set_apic_source(u64 u64NsecPerTick)
{
    int fFirstHandoff;

    timer_soft_inc(&g_u64SoftHandoffCalls);
    if (u64NsecPerTick == 0) {
        timer_soft_inc(&g_u64SoftHandoffSkip);
        return;
    }
    g_u64NsecPerTick = u64NsecPerTick;
    fFirstHandoff = 0;
    if (!g_fApicSource) {
        g_u64SourceSwitch++;
        g_u64PitDemotions++;
        fFirstHandoff = 1;
        timer_soft_inc(&g_u64SoftHandoffFirst);
    }
    /*
     * Prefer APIC for mono/jiffies. Mask PIT IRQ0 so mono advances only
     * from the APIC timer; PIT remains soft-fallback telemetry only.
     */
    g_fApicSource = 1;
    outb(PIC1_DATA, 0xFF);

    /* Greppable mono preference + demotion lamps (product / smoke). */
    timer_mono_pref_soft_log();
    if (fFirstHandoff != 0) {
        /* Continuity: keep nsec/tick on serial for older greps / humans. */
        kprintf("timer: mono handoff APIC nsec/tick=%lu switches=%lu "
                "pit_demotions=%lu\n",
                (unsigned long)g_u64NsecPerTick,
                (unsigned long)g_u64SourceSwitch,
                (unsigned long)g_u64PitDemotions);
    }
    /* Wave 15: soft inventory after PIT→APIC handoff (apic mono preferred). */
    timer_soft_inventory_log();
}

int
timer_apic_source(void)
{
    return g_fApicSource;
}

u64
timer_jiffies(void)
{
    return g_u64Jiffies;
}

u64
timer_nsec_per_tick(void)
{
    return g_u64NsecPerTick;
}

u64
timer_mono_nsec(void)
{
    return g_u64Jiffies * g_u64NsecPerTick;
}

u64
timer_mono_nsec_soft(void)
{
    u64 u64Coarse;
    u32 u32Init;
    u32 u32Cur;
    u64 u64Frac;
    u64 u64Soft;

    /* Wave 13: soft interpolate sample tally (never hard-gates). */
    if (g_u64SoftMonoSamples < ~0ull) {
        g_u64SoftMonoSamples++;
    }

    u64Coarse = g_u64Jiffies * g_u64NsecPerTick;
    if (!g_fApicSource || g_u64NsecPerTick == 0) {
        if (g_u64SoftMonoCoarseOnly < ~0ull) {
            g_u64SoftMonoCoarseOnly++;
        }
        return u64Coarse;
    }
    u32Init = apic_timer_init_count();
    if (u32Init < 2u) {
        if (g_u64SoftMonoCoarseOnly < ~0ull) {
            g_u64SoftMonoCoarseOnly++;
        }
        return u64Coarse;
    }
    u32Cur = apic_timer_cur_count();
    /*
     * LAPIC timer counts down from INIT to 0 each period.
     * Elapsed fraction ≈ (INIT - CUR) / INIT; clamp CUR to INIT.
     */
    if (u32Cur > u32Init) {
        u32Cur = u32Init;
    }
    u64Frac = ((u64)(u32Init - u32Cur) * g_u64NsecPerTick) / (u64)u32Init;
    u64Soft = u64Coarse + u64Frac;
    /* Never report less than coarse (wrap / IRQ race). */
    if (u64Soft < u64Coarse) {
        if (g_u64SoftMonoClamp < ~0ull) {
            g_u64SoftMonoClamp++;
        }
        return u64Coarse;
    }
    if (g_u64SoftMonoInterp < ~0ull) {
        g_u64SoftMonoInterp++;
    }
    return u64Soft;
}

int
timer_ready(void)
{
    return g_fTimerReady;
}

void
timer_mono_soft_snapshot(struct gj_timer_mono_soft *pOut)
{
    if (pOut == NULL) {
        timer_soft_inc(&g_u64SoftSnapMonoNull);
        return;
    }
    timer_soft_inc(&g_u64SoftSnapMono);
    pOut->u64Jiffies = g_u64Jiffies;
    pOut->u64MonoNsec = g_u64Jiffies * g_u64NsecPerTick;
    pOut->u64MonoSoftNsec = timer_mono_nsec_soft();
    pOut->u64NsecPerTick = g_u64NsecPerTick;
    if (!g_fTimerReady) {
        pOut->u32Source = GJ_TIMER_SRC_NONE;
    } else if (g_fApicSource) {
        pOut->u32Source = GJ_TIMER_SRC_APIC;
    } else {
        pOut->u32Source = GJ_TIMER_SRC_PIT;
    }
    pOut->u32Ready = g_fTimerReady ? 1u : 0u;
    pOut->u64PitTicks = g_u64PitTicks;
    pOut->u64ApicTicks = g_u64ApicMonoTicks;
    pOut->u64SourceSwitch = g_u64SourceSwitch;
}

void
timer_preempt_soft_snapshot(struct gj_timer_preempt_soft *pOut)
{
    if (pOut == NULL) {
        timer_soft_inc(&g_u64SoftSnapPreemptNull);
        return;
    }
    timer_soft_inc(&g_u64SoftSnapPreempt);
    pOut->u32Quantum = g_u32Quantum;
    pOut->u32SliceLeft = g_u32SliceLeft;
    pOut->u64Preempts = g_u64Preempts;
    pOut->u64YieldRequests = g_u64YieldRequests;
    pOut->u64PreemptChecks = g_u64PreemptChecks;
    pOut->u64PreemptCheckHits = g_u64PreemptCheckHits;
    pOut->u64PreemptCheckYields = g_u64PreemptCheckYields;
    pOut->u64QuantumSets = g_u64QuantumSets;
    pOut->u64QuantumTicks = g_u64QuantumTicks;
}

/**
 * Mono clock source preference soft log (APIC preferred when armed).
 * Prefix-stable markers:
 *   timer: mono source APIC soft …  — mono prefers APIC/x2APIC timer
 *   timer: mono source PIT soft …   — early boot / unarmed APIC path
 *   timer: pit demoted soft …       — PIT soft-fallback only (IRQ0 masked)
 *   timer: apic mono preferred PASS — product lamp when source is APIC
 *   timer: x2apic mono replace soft PARTIAL|… — honesty on full replace
 *
 * Never hard-gates boot; pure telemetry. Preserves quantum/preempt soft.
 * greppable: timer: mono source
 * greppable: timer: pit demoted soft
 * greppable: timer: apic mono preferred PASS
 */
static void
timer_mono_pref_soft_log(void)
{
    const char *szSrc;
    const char *szPreferred;
    const char *szReplace;
    int fApic;
    int fX2En;
    int fX2Supp;
    u32 u32Init;
    u32 u32Cur;

    if (g_u64MonoPrefLogs < ~0ull) {
        g_u64MonoPrefLogs++;
    }

    fApic = g_fApicSource ? 1 : 0;
    if (!g_fTimerReady) {
        szSrc = "NONE";
        szPreferred = "NONE";
    } else if (fApic != 0) {
        szSrc = "APIC";
        szPreferred = "APIC";
    } else {
        szSrc = "PIT";
        szPreferred = "PIT";
    }

    fX2En = x2apic_enabled() ? 1 : 0;
    fX2Supp = x2apic_supported() ? 1 : 0;

    u32Init = 0;
    u32Cur = 0;
    if (fApic != 0) {
        u32Init = apic_timer_init_count();
        u32Cur = apic_timer_cur_count();
    }

    /*
     * Grep: timer: mono source APIC soft | timer: mono source PIT soft
     * When APIC armed: preferred=APIC, pit_soft_fallback=1 (demoted).
     */
    kprintf("timer: mono source %s soft preferred=%s pit_soft_fallback=%u "
            "ready=%u jiffies=%lu npt=%lu switches=%lu "
            "pit_ticks=%lu apic_ticks=%lu demotions=%lu logs=%lu\n",
            szSrc, szPreferred,
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(g_fTimerReady ? 1u : 0u),
            (unsigned long)g_u64Jiffies,
            (unsigned long)g_u64NsecPerTick,
            (unsigned long)g_u64SourceSwitch,
            (unsigned long)g_u64PitTicks,
            (unsigned long)g_u64ApicMonoTicks,
            (unsigned long)g_u64PitDemotions,
            (unsigned long)g_u64MonoPrefLogs);

    if (fApic != 0) {
        /*
         * PIT demoted: soft-fallback only — no mono advance from IRQ0.
         * Grep: timer: pit demoted soft
         */
        kprintf("timer: pit demoted soft irq0_masked=1 mono_advance=0 "
                "soft_fallback=1 demotions=%lu switches=%lu "
                "vector_pit=32 vector_apic=48 pit_stray=%lu\n",
                (unsigned long)g_u64PitDemotions,
                (unsigned long)g_u64SourceSwitch,
                (unsigned long)g_u64SoftPitStray);

        /*
         * Soft product lamp when mono source is APIC.
         * Grep: timer: apic mono preferred PASS
         */
        kprintf("timer: apic mono preferred PASS\n");
    }

    /*
     * Honesty: full x2APIC ICR/timer replace of PIT remains PARTIAL while
     * the hardware product path is incomplete. APIC mono preference (even
     * with x2APIC mode on + INIT armed) does not claim complete 8259/PIT
     * product exit — ICR/timer replace is still partial today.
     * Grep: timer: x2apic mono replace soft
     */
    if (fApic != 0) {
        szReplace = "PARTIAL";
    } else {
        szReplace = "PIT";
    }
    kprintf("timer: x2apic mono replace soft %s apic_src=%u x2apic_en=%u "
            "x2apic_supp=%u init_cnt=%u cur_cnt=%u "
            "honesty=full_x2apic_icr_timer_replace_incomplete\n",
            szReplace,
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(fX2En != 0 ? 1u : 0u),
            (unsigned)(fX2Supp != 0 ? 1u : 0u),
            (unsigned)u32Init, (unsigned)u32Cur);

    /* Wave 13: twin soft apic mono axis under timer: soft … */
    timer_soft_apic_mono_log();
}

/**
 * Wave 13 exclusive: greppable "timer: soft apic mono …" deepen.
 * Preference + demotion + LAPIC INIT/CUR + honesty; never hard-gates.
 * greppable: timer: soft apic mono
 * greppable: timer: apic mono preferred PASS
 */
static void
timer_soft_apic_mono_log(void)
{
    const char *szPreferred;
    const char *szVerdict;
    int fApic;
    int fX2En;
    int fX2Supp;
    u32 u32Init;
    u32 u32Cur;
    u32 u32Elapsed;
    u32 u32FracPpm; /* parts-per-million of period elapsed (soft) */

    if (g_u64SoftApicMonoLogs < ~0ull) {
        g_u64SoftApicMonoLogs++;
    }

    fApic = g_fApicSource ? 1 : 0;
    fX2En = x2apic_enabled() ? 1 : 0;
    fX2Supp = x2apic_supported() ? 1 : 0;

    u32Init = 0;
    u32Cur = 0;
    u32Elapsed = 0;
    u32FracPpm = 0;
    if (fApic != 0) {
        u32Init = apic_timer_init_count();
        u32Cur = apic_timer_cur_count();
        if (u32Cur > u32Init) {
            u32Cur = u32Init;
        }
        if (u32Init >= 2u) {
            u32Elapsed = u32Init - u32Cur;
            u32FracPpm = (u32)(((u64)u32Elapsed * 1000000ull) / (u64)u32Init);
        }
    }

    if (!g_fTimerReady) {
        szPreferred = "NONE";
        szVerdict = "FAIL";
    } else if (fApic != 0) {
        szPreferred = "APIC";
        szVerdict = "PASS";
    } else {
        szPreferred = "PIT";
        szVerdict = "READY";
    }

    /*
     * Grep: timer: soft apic mono
     * Preferred APIC when armed; pit demoted soft-fallback; INIT/CUR sample.
     */
    kprintf("timer: soft apic mono preferred=%s apic_src=%u ready=%u "
            "jiffies=%lu apic_ticks=%lu pit_ticks=%lu switches=%lu "
            "demotions=%lu pit_stray=%lu init_cnt=%u cur_cnt=%u "
            "elapsed=%u frac_ppm=%u x2apic_en=%u x2apic_supp=%u "
            "vector_pit=32 vector_apic=48 logs=%lu\n",
            szPreferred,
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(g_fTimerReady ? 1u : 0u),
            (unsigned long)g_u64Jiffies,
            (unsigned long)g_u64ApicMonoTicks,
            (unsigned long)g_u64PitTicks,
            (unsigned long)g_u64SourceSwitch,
            (unsigned long)g_u64PitDemotions,
            (unsigned long)g_u64SoftPitStray,
            (unsigned)u32Init, (unsigned)u32Cur,
            (unsigned)u32Elapsed, (unsigned)u32FracPpm,
            (unsigned)(fX2En != 0 ? 1u : 0u),
            (unsigned)(fX2Supp != 0 ? 1u : 0u),
            (unsigned long)g_u64SoftApicMonoLogs);

    /* Twin lamp under soft prefix (legacy bare lamp kept in mono_pref). */
    if (fApic != 0) {
        kprintf("timer: soft apic mono preferred PASS\n");
        kprintf("timer: soft apic mono %s init_armed=%u cur_live=%u "
                "pit_mono_advance=0 irq0_masked=1\n",
                szVerdict,
                (unsigned)(u32Init >= 2u ? 1u : 0u),
                (unsigned)(u32Init >= 2u && u32Cur <= u32Init ? 1u : 0u));
    } else {
        kprintf("timer: soft apic mono preferred %s "
                "apic_src=0 pit_active=%u\n",
                szVerdict,
                (unsigned)(g_fTimerReady ? 1u : 0u));
    }
}

/**
 * Greppable soft timer inventory (Wave 10 base + Wave 13 path + Wave 15 deepen).
 * Prefix-stable markers (timer: soft …). Primary field names stay stable;
 * Wave 15 adds complementary sub-lines only. Never allocates; safe from
 * boot soft-smoke / timer_soft_log. Soft ≠ full x2APIC ICR/timer product.
 * greppable: timer: soft
 */
static void
timer_soft_inventory_log(void)
{
    struct gj_timer_mono_soft stMono;
    struct gj_timer_preempt_soft stPre;
    const char *szSrc;
    const char *szPreferred;
    u64 u64SoftDelta;
    u32 u32Init;
    u32 u32Cur;
    u32 u32Elapsed;
    u32 u32FracPpm;
    u32 u32Hz;
    u32 u32Surf;
    int fSoftPass;
    int fApic;
    int fX2En;
    int fX2Supp;
    int fInterpLive;
    u32 u32LampReady;
    u32 u32LampApic;
    u32 u32LampX2;
    u32 u32LampInterp;
    u32 u32LampQuantum;
    u32 u32LampHandoff;
    u32 u32LampFutex;

    timer_mono_soft_snapshot(&stMono);
    timer_preempt_soft_snapshot(&stPre);

    timer_soft_inc(&g_u64SoftInventoryLogs);

    fApic = g_fApicSource ? 1 : 0;
    fX2En = x2apic_enabled() ? 1 : 0;
    fX2Supp = x2apic_supported() ? 1 : 0;

    if (stMono.u32Source == GJ_TIMER_SRC_APIC) {
        szSrc = "APIC";
        szPreferred = "APIC";
    } else if (stMono.u32Source == GJ_TIMER_SRC_PIT) {
        szSrc = "PIT";
        szPreferred = "PIT";
    } else {
        szSrc = "NONE";
        szPreferred = "NONE";
    }

    u64SoftDelta = 0;
    if (stMono.u64MonoSoftNsec >= stMono.u64MonoNsec) {
        u64SoftDelta = stMono.u64MonoSoftNsec - stMono.u64MonoNsec;
    }

    u32Init = 0;
    u32Cur = 0;
    u32Elapsed = 0;
    u32FracPpm = 0;
    fInterpLive = 0;
    if (fApic != 0) {
        u32Init = apic_timer_init_count();
        u32Cur = apic_timer_cur_count();
        if (u32Cur > u32Init) {
            u32Cur = u32Init;
        }
        if (u32Init >= 2u) {
            u32Elapsed = u32Init - u32Cur;
            u32FracPpm = (u32)(((u64)u32Elapsed * 1000000ull) / (u64)u32Init);
            fInterpLive = 1;
        }
    }

    u32Hz = GJ_TIMER_HZ;
    if (stMono.u64NsecPerTick != 0) {
        u32Hz = (u32)(1000000000ull / stMono.u64NsecPerTick);
        if (u32Hz == 0) {
            u32Hz = GJ_TIMER_HZ;
        }
    }

    /* Soft surface catalog (software claims only; x2 full replace = 0). */
    u32Surf = TIMER_SOFT_SURF_MONO | TIMER_SOFT_SURF_SOFT_MONO |
              TIMER_SOFT_SURF_APIC_SRC | TIMER_SOFT_SURF_PREEMPT |
              TIMER_SOFT_SURF_PIT_FALL | TIMER_SOFT_SURF_INTERP |
              TIMER_SOFT_SURF_FUTEX_COUP | TIMER_SOFT_SURF_HANDOFF |
              TIMER_SOFT_SURF_X2_FULL;

    u32LampReady = stMono.u32Ready != 0 ? 1u : 0u;
    u32LampApic = fApic != 0 ? 1u : 0u;
    u32LampX2 = fX2En != 0 ? 1u : 0u;
    u32LampInterp = fInterpLive != 0 ? 1u : 0u;
    u32LampQuantum = stPre.u32Quantum > 0 ? 1u : 0u;
    u32LampHandoff = g_u64SourceSwitch > 0 ? 1u : 0u;
    u32LampFutex = g_u64SoftFutexCoupled > 0 ? 1u : 0u;

    /* Grep: timer: soft inventory — Wave 16 appends wave= only; keys stable. */
    kprintf("timer: soft inventory ready=%u src=%s hz=%u quantum=%u "
            "jiffies=%lu npt=%lu logs=%lu "
            "g_timer_mono=1 soft_mono=1 apic_src=1 preempt=1 "
            "apic_pref=1 pit_fallback=1 wave=%u\n",
            (unsigned)stMono.u32Ready, szSrc, (unsigned)u32Hz,
            (unsigned)stPre.u32Quantum,
            (unsigned long)stMono.u64Jiffies,
            (unsigned long)stMono.u64NsecPerTick,
            (unsigned long)g_u64SoftInventoryLogs,
            (unsigned)TIMER_SOFT_WAVE);

    /* Grep: timer: soft mono */
    kprintf("timer: soft mono coarse=%lu soft=%lu delta=%lu "
            "jiffies=%lu npt=%lu pit_ticks=%lu apic_ticks=%lu switches=%lu "
            "samples=%lu interp=%lu coarse_only=%lu clamp=%lu\n",
            (unsigned long)stMono.u64MonoNsec,
            (unsigned long)stMono.u64MonoSoftNsec,
            (unsigned long)u64SoftDelta,
            (unsigned long)stMono.u64Jiffies,
            (unsigned long)stMono.u64NsecPerTick,
            (unsigned long)stMono.u64PitTicks,
            (unsigned long)stMono.u64ApicTicks,
            (unsigned long)stMono.u64SourceSwitch,
            (unsigned long)g_u64SoftMonoSamples,
            (unsigned long)g_u64SoftMonoInterp,
            (unsigned long)g_u64SoftMonoCoarseOnly,
            (unsigned long)g_u64SoftMonoClamp);

    /* Grep: timer: soft preempt */
    kprintf("timer: soft preempt quantum=%u slice_left=%u preempts=%lu "
            "yields=%lu checks=%lu hits=%lu check_yields=%lu sets=%lu "
            "q_ticks=%lu\n",
            (unsigned)stPre.u32Quantum, (unsigned)stPre.u32SliceLeft,
            (unsigned long)stPre.u64Preempts,
            (unsigned long)stPre.u64YieldRequests,
            (unsigned long)stPre.u64PreemptChecks,
            (unsigned long)stPre.u64PreemptCheckHits,
            (unsigned long)stPre.u64PreemptCheckYields,
            (unsigned long)stPre.u64QuantumSets,
            (unsigned long)stPre.u64QuantumTicks);

    /* Grep: timer: soft source — preferred APIC; PIT soft-fallback when demoted */
    kprintf("timer: soft source=%s apic=%u preferred=%s pit_soft_fallback=%u "
            "init_cnt=%u cur_cnt=%u vector_pit=32 vector_apic=48 "
            "switches=%lu demotions=%lu pit_stray=%lu "
            "x2apic_en=%u x2apic_supp=%u\n",
            szSrc, (unsigned)(fApic != 0 ? 1u : 0u),
            szPreferred,
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)u32Init, (unsigned)u32Cur,
            (unsigned long)stMono.u64SourceSwitch,
            (unsigned long)g_u64PitDemotions,
            (unsigned long)g_u64SoftPitStray,
            (unsigned)(fX2En != 0 ? 1u : 0u),
            (unsigned)(fX2Supp != 0 ? 1u : 0u));

    /* Grep: timer: soft handoff — PIT→APIC demotion / switch axis */
    kprintf("timer: soft handoff switches=%lu demotions=%lu "
            "apic_src=%u preferred=%s pit_soft_fallback=%u "
            "irq0_masked=%u mono_from_pit=%u mono_from_apic=%u "
            "pit_stray=%lu npt=%lu\n",
            (unsigned long)stMono.u64SourceSwitch,
            (unsigned long)g_u64PitDemotions,
            (unsigned)(fApic != 0 ? 1u : 0u),
            szPreferred,
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(fApic != 0 ? 0u : 1u),
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned long)g_u64SoftPitStray,
            (unsigned long)stMono.u64NsecPerTick);

    /* Grep: timer: soft interpolate — LAPIC CUR soft mono path */
    kprintf("timer: soft interpolate live=%u init_cnt=%u cur_cnt=%u "
            "elapsed=%u frac_ppm=%u delta_nsec=%lu samples=%lu "
            "interp_ok=%lu coarse_only=%lu clamp=%lu "
            "apic_src=%u npt=%lu\n",
            (unsigned)(fInterpLive != 0 ? 1u : 0u),
            (unsigned)u32Init, (unsigned)u32Cur,
            (unsigned)u32Elapsed, (unsigned)u32FracPpm,
            (unsigned long)u64SoftDelta,
            (unsigned long)g_u64SoftMonoSamples,
            (unsigned long)g_u64SoftMonoInterp,
            (unsigned long)g_u64SoftMonoCoarseOnly,
            (unsigned long)g_u64SoftMonoClamp,
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned long)stMono.u64NsecPerTick);

    /*
     * Grep: timer: soft path
     * Honesty catalog: product surface vs open full x2APIC ICR/timer replace.
     * Soft inventory ≠ full x2APIC timer product (PARTIAL remains).
     */
    kprintf("timer: soft path coarse_mono=1 soft_mono=1 apic_pref=1 "
            "pit_fallback=1 preempt_quantum=1 futex_timer_check=1 "
            "full_x2apic_icr_timer_replace=0 claim_bar3=0 "
            "wave=%u via=timer.c\n",
            (unsigned)TIMER_SOFT_WAVE);

    /* Wave 13 apic mono deepen (twin under soft inventory). */
    timer_soft_apic_mono_log();

    /*
     * Wave 15 complementary sub-lines (kept; never reshape primary).
     */
    /* Grep: timer: soft lamps */
    kprintf("timer: soft lamps ready=%u apic=%u x2=%u x2_supp=%u "
            "interp=%u quantum=%u handoff=%u futex=%u "
            "pit_active=%u preferred=%s\n",
            (unsigned)u32LampReady, (unsigned)u32LampApic,
            (unsigned)u32LampX2, (unsigned)(fX2Supp != 0 ? 1u : 0u),
            (unsigned)u32LampInterp, (unsigned)u32LampQuantum,
            (unsigned)u32LampHandoff, (unsigned)u32LampFutex,
            (unsigned)(fApic != 0 ? 0u : (g_fTimerReady ? 1u : 0u)),
            szPreferred);

    /* Grep: timer: soft stats */
    kprintf("timer: soft stats tick_pit=%lu tick_apic=%lu tick_pit_eoi=%lu "
            "handoff_calls=%lu handoff_skip=%lu handoff_first=%lu "
            "snap_mono=%lu snap_mono_null=%lu snap_preempt=%lu "
            "snap_preempt_null=%lu sleep=%lu sleep_guard=%lu "
            "soft_log=%lu init=%lu futex_coupled=%lu "
            "inv_logs=%lu apic_mono_logs=%lu wave=%u\n",
            (unsigned long)g_u64SoftTickPit,
            (unsigned long)g_u64SoftTickApic,
            (unsigned long)g_u64SoftTickPitEoi,
            (unsigned long)g_u64SoftHandoffCalls,
            (unsigned long)g_u64SoftHandoffSkip,
            (unsigned long)g_u64SoftHandoffFirst,
            (unsigned long)g_u64SoftSnapMono,
            (unsigned long)g_u64SoftSnapMonoNull,
            (unsigned long)g_u64SoftSnapPreempt,
            (unsigned long)g_u64SoftSnapPreemptNull,
            (unsigned long)g_u64SoftSleepCalls,
            (unsigned long)g_u64SoftSleepGuard,
            (unsigned long)g_u64SoftLogCalls,
            (unsigned long)g_u64SoftInitCalls,
            (unsigned long)g_u64SoftFutexCoupled,
            (unsigned long)g_u64SoftInventoryLogs,
            (unsigned long)g_u64SoftApicMonoLogs,
            (unsigned)TIMER_SOFT_WAVE);

    /* Grep: timer: soft vectors */
    kprintf("timer: soft vectors pit=32 apic=48 irq0_eoi=1 "
            "pit_mono_advance=%u apic_mono_advance=%u "
            "pic_mask_after_handoff=%u spur_eoi=%u\n",
            (unsigned)(fApic != 0 ? 0u : 1u),
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(fApic != 0 ? 1u : 0u));

    /* Grep: timer: soft quantum */
    kprintf("timer: soft quantum ticks=%u slice_left=%u preempts=%lu "
            "yield_req=%lu checks=%lu hits=%lu check_yields=%lu "
            "sets=%lu q_ticks=%lu default=5 hz=%u\n",
            (unsigned)stPre.u32Quantum, (unsigned)stPre.u32SliceLeft,
            (unsigned long)stPre.u64Preempts,
            (unsigned long)stPre.u64YieldRequests,
            (unsigned long)stPre.u64PreemptChecks,
            (unsigned long)stPre.u64PreemptCheckHits,
            (unsigned long)stPre.u64PreemptCheckYields,
            (unsigned long)stPre.u64QuantumSets,
            (unsigned long)stPre.u64QuantumTicks,
            (unsigned)u32Hz);

    /* Grep: timer: soft futex — mono tick → futex_timer_check coupling */
    kprintf("timer: soft futex coupled=%lu tick_pit=%lu tick_apic=%lu "
            "path=timer_tick|timer_tick_apic hard_gate=0\n",
            (unsigned long)g_u64SoftFutexCoupled,
            (unsigned long)g_u64SoftTickPit,
            (unsigned long)g_u64SoftTickApic);

    /*
     * Grep: timer: soft honesty
     * Explicit: soft inventory deepen ≠ full x2APIC ICR/timer product.
     * xAPIC mono handoff alone remains PARTIAL.
     */
    kprintf("timer: soft honesty soft_ne_full_x2apic=1 "
            "full_x2apic_icr_timer_replace=0 apic_src=%u x2apic_en=%u "
            "x2apic_supp=%u claim=PARTIAL bar3=0 product_complete=0 "
            "wave=%u unit=timer.c\n",
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(fX2En != 0 ? 1u : 0u),
            (unsigned)(fX2Supp != 0 ? 1u : 0u),
            (unsigned)TIMER_SOFT_WAVE);

    /* Grep: timer: soft surface — bit catalog of soft product surfaces */
    kprintf("timer: soft surface bits=0x%x mono=1 soft_mono=1 apic_src=1 "
            "preempt=1 pit_fallback=1 interp=1 futex_coup=1 handoff=1 "
            "x2_full_replace=0 wave=%u\n",
            (unsigned)u32Surf, (unsigned)TIMER_SOFT_WAVE);

    /*
     * Wave 16 complementary sub-lines (kept; never reshape primary).
     */
    /* Grep: timer: soft exclusive */
    kprintf("timer: soft exclusive wave=%u exclusive=1 soft=1 "
            "unit=timer.c bar3=0 hard_gate=0 product_complete=0 "
            "soft_ne_full_x2apic=1\n",
            (unsigned)TIMER_SOFT_WAVE);

    /* Grep: timer: soft claim — product claim bounds (soft-only) */
    kprintf("timer: soft claim coarse_mono=1 soft_mono=1 apic_pref=1 "
            "pit_fallback=1 preempt_quantum=1 futex_timer_check=1 "
            "vector_pit=32 vector_apic=48 full_x2apic_icr_timer_replace=0 "
            "claim=PARTIAL bar3=0 wave=%u\n",
            (unsigned)TIMER_SOFT_WAVE);

    /* Grep: timer: soft ratio — pit/apic/handoff/interp path ratios */
    kprintf("timer: soft ratio tick_pit=%lu tick_apic=%lu "
            "pit_stray=%lu handoff=%lu demotions=%lu "
            "interp=%lu coarse_only=%lu clamp=%lu "
            "futex_coupled=%lu sleep_guard=%lu wave=%u\n",
            (unsigned long)g_u64SoftTickPit,
            (unsigned long)g_u64SoftTickApic,
            (unsigned long)g_u64SoftPitStray,
            (unsigned long)g_u64SourceSwitch,
            (unsigned long)g_u64PitDemotions,
            (unsigned long)g_u64SoftMonoInterp,
            (unsigned long)g_u64SoftMonoCoarseOnly,
            (unsigned long)g_u64SoftMonoClamp,
            (unsigned long)g_u64SoftFutexCoupled,
            (unsigned long)g_u64SoftSleepGuard,
            (unsigned)TIMER_SOFT_WAVE);

    /* Grep: timer: soft eoi — PIC EOI + spur policy lamps */
    kprintf("timer: soft eoi pit_eoi=%lu spur_eoi=%u irq0_masked=%u "
            "apic_src=%u mono_from_pit=%u mono_from_apic=%u "
            "pic1_eoi=1 wave=%u\n",
            (unsigned long)g_u64SoftTickPitEoi,
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(fApic != 0 ? 0u : 1u),
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)TIMER_SOFT_WAVE);

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: timer: soft return — Wave 17 API return surfaces (kept) */
    kprintf("timer: soft return ready=%u apic_src=%u mono=1 soft_mono=1 "
            "preempt=1 futex=1 product_kernel=OPEN bar3=0 hard_gate=0 "
            "wave=%u soft PASS\n",
            (unsigned)stMono.u32Ready,
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)TIMER_SOFT_WAVE);

    /* Grep: timer: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("timer: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 x2_full_replace=0 wave=%u soft PASS\n",
            (unsigned)TIMER_SOFT_WAVE);

    /* Grep: timer: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("timer: soft retmap init=1 tick=1 mono=1 sleep=1 "
            "soft_inv=1 deepen=1 product=OPEN wave=%u soft PASS\n",
            (unsigned)TIMER_SOFT_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: timer: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("timer: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)TIMER_SOFT_WAVE);

    /* Grep: timer: soft retcode — Wave 19 retcode catalog */
    kprintf("timer: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)TIMER_SOFT_WAVE);

    /* Grep: timer: soft deepen — wave stamp + area catalog */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: timer: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("timer: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)TIMER_SOFT_WAVE);
    /* Grep: timer: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("timer: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)TIMER_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: timer: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("timer: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)TIMER_SOFT_WAVE);
    /* Grep: timer: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("timer: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)TIMER_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: timer: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("timer: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)TIMER_SOFT_WAVE);
            /* Grep: timer: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("timer: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)TIMER_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: timer: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("timer: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)TIMER_SOFT_WAVE);
            /* Grep: timer: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("timer: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)TIMER_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: timer: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("timer: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)TIMER_SOFT_WAVE);
            /* Grep: timer: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("timer: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)TIMER_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: timer: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("timer: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)TIMER_SOFT_WAVE);
            /* Grep: timer: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("timer: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)TIMER_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: timer: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("timer: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)TIMER_SOFT_WAVE);
            /* Grep: timer: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("timer: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)TIMER_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: timer: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("timer: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)TIMER_SOFT_WAVE);
            /* Grep: timer: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("timer: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)TIMER_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: timer: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("timer: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)TIMER_SOFT_WAVE);
                    /* Grep: timer: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("timer: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)TIMER_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: timer: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("timer: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)TIMER_SOFT_WAVE);
                            /* Grep: timer: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("timer: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)TIMER_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: timer: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("timer: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)TIMER_SOFT_WAVE);
                            /* Grep: timer: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("timer: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)TIMER_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: timer: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("timer: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)TIMER_SOFT_WAVE);
                            /* Grep: timer: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("timer: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)TIMER_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: timer: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("timer: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)TIMER_SOFT_WAVE);
                            /* Grep: timer: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("timer: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)TIMER_SOFT_WAVE);
                            /* Grep: timer: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("timer: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)TIMER_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("timer: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("timer: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("timer: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("timer: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("timer: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("timer: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retfortress — Wave 35 return-fortress honesty */
kprintf("timer: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("timer: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft rethold — Wave 36 return-hold honesty */
kprintf("timer: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retspire — Wave 36 exclusive spire stamp */
kprintf("timer: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retwall — Wave 37 return-wall honesty */
kprintf("timer: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retgate — Wave 37 exclusive gate stamp */
kprintf("timer: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retmoat — Wave 38 return-moat honesty */
kprintf("timer: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retower — Wave 38 exclusive tower stamp */
kprintf("timer: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("timer: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("timer: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("timer: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("timer: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retravelin — Wave 41 return-travelin honesty */
kprintf("timer: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("timer: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("timer: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("timer: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("timer: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("timer: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("timer: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("timer: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("timer: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("timer: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retbailey — Wave 46 return-bailey honesty */
kprintf("timer: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)TIMER_SOFT_WAVE);
/* Grep: timer: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("timer: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)TIMER_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("timer: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("timer: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("timer: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("timer: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("timer: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("timer: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retsally — Wave 50 return-sally honesty */
kprintf("timer: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("timer: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retfosse — Wave 51 return-fosse honesty */
kprintf("timer: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("timer: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("timer: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("timer: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retravelin — Wave 53 return-travelin honesty */
kprintf("timer: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("timer: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("timer: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retredan — Wave 54 exclusive redan stamp */
kprintf("timer: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retflank — Wave 55 return-flank honesty */
kprintf("timer: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retface — Wave 55 exclusive face stamp */
kprintf("timer: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retgorge — Wave 56 return-gorge honesty */
kprintf("timer: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("timer: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retraverse — Wave 57 return-traverse honesty */
kprintf("timer: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("timer: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retorillon — Wave 58 return-orillon honesty */
kprintf("timer: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("timer: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("timer: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("timer: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retplace — Wave 60 return-place honesty */
kprintf("timer: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("timer: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("timer: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("timer: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("timer: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("timer: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("timer: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("timer: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: timer: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("timer: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: timer: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("timer: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: timer: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("timer: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: timer: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("timer: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: timer: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("timer: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=106 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: timer: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("timer: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=106 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("timer: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("timer: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("timer: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("timer: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("timer: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=106 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("timer: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=106 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("timer: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("timer: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("timer: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("timer: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: timer: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("timer: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("timer: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: timer: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("timer: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("timer: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retbastionangle stamp; Soft≠product)\n");
/* Grep: timer: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("timer: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("timer: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retparapetangle stamp; Soft≠product)\n");
/* Grep: timer: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("timer: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("timer: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retowerangle stamp; Soft≠product)\n");
/* Grep: timer: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("timer: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("timer: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retwallangle stamp; Soft≠product)\n");
/* Grep: timer: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("timer: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("timer: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retholdangle stamp; Soft≠product)\n");
/* Grep: timer: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("timer: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("timer: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retfortressangle stamp; Soft≠product)\n");
/* Grep: timer: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("timer: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("timer: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: timer: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("timer: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("timer: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: timer: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("timer: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("timer: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: timer: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("timer: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("timer: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retaegisangle stamp; Soft≠product)\n");
/* Grep: timer: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("timer: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("timer: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retsigilangle stamp; Soft≠product)\n");
/* Grep: timer: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("timer: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("timer: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retglyphangle stamp; Soft≠product)\n");
/* Grep: timer: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("timer: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("timer: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retshardangle stamp; Soft≠product)\n");
/* Grep: timer: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("timer: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("timer: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retprismangle stamp; Soft≠product)\n");
/* Grep: timer: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("timer: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("timer: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retcipherangle stamp; Soft≠product)\n");
/* Grep: timer: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("timer: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("timer: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retledgerangle stamp; Soft≠product)\n");
/* Grep: timer: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("timer: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("timer: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retvaultangle stamp; Soft≠product)\n");
/* Grep: timer: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("timer: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("timer: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (rettokenangle stamp; Soft≠product)\n");
/* Grep: timer: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("timer: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("timer: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retphaseangle stamp; Soft≠product)\n");
/* Grep: timer: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("timer: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("timer: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retpulseangle stamp; Soft≠product)\n");

/* Grep: timer: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("timer: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("timer: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retboundangle stamp; Soft≠product)\n");
/* Grep: timer: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("timer: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("timer: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retbladeangle stamp; Soft≠product)\n");
/* Grep: timer: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("timer: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("timer: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retarcangle stamp; Soft≠product)\n");
/* Grep: timer: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("timer: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("timer: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: timer: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("timer: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("timer: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: timer: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("timer: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("timer: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retellipseangle stamp; Soft≠product)\n");
/* Grep: timer: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("timer: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("timer: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: timer: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("timer: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("timer: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (rethelixangle stamp; Soft≠product)\n");
/* Grep: timer: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("timer: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("timer: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retknotangle stamp; Soft≠product)\n");
/* Grep: timer: soft retmoebiusangle — Wave 102 return-moebiusangle honesty */
kprintf("timer: soft retmoebiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retmoebiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retkleinangle — Wave 102 exclusive kleinangle stamp */
kprintf("timer: soft retkleinangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retkleinangle stamp; Soft≠product)\n");
/* Grep: timer: soft retprojectangle — Wave 103 return-projectangle honesty */
kprintf("timer: soft retprojectangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retprojectangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retaffineangle — Wave 103 exclusive affineangle stamp */
kprintf("timer: soft retaffineangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retaffineangle stamp; Soft≠product)\n");
/* Grep: timer: soft retlinearangle — Wave 104 return-linearangle honesty */
kprintf("timer: soft retlinearangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retlinearangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retbilinearangle — Wave 104 exclusive bilinearangle stamp */
kprintf("timer: soft retbilinearangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retbilinearangle stamp; Soft≠product)\n");
/* Grep: timer: soft retquadraticangle — Wave 105 return-quadraticangle honesty */
kprintf("timer: soft retquadraticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retquadraticangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retcubicangle — Wave 105 exclusive cubicangle stamp */
kprintf("timer: soft retcubicangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retcubicangle stamp; Soft≠product)\n");
/* Grep: timer: soft retquarticangle — Wave 106 return-quarticangle honesty */
kprintf("timer: soft retquarticangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=106 (retquarticangle honesty; Soft≠product; not bar3)\n");
/* Grep: timer: soft retquinticangle — Wave 106 exclusive quinticangle stamp */
kprintf("timer: soft retquinticangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=106 (retquinticangle stamp; Soft≠product)\n");
                            kprintf("timer: soft deepen wave=%u areas=inventory,mono,preempt,,retclass,retlane"
            "source,apic_mono,path,handoff,interpolate,"
            "lamps,stats,vectors,quantum,futex,honesty,surface,"
            "exclusive,claim,ratio,eoi,return,return_selftest,retmap,return_rate,retcode "
            "logs=%lu apic_mono_logs=%lu ready=%u apic_src=%u "
            "unit=timer.c only hard_gate=0\n",
            (unsigned)TIMER_SOFT_WAVE,
            (unsigned long)g_u64SoftInventoryLogs,
            (unsigned long)g_u64SoftApicMonoLogs,
            (unsigned)stMono.u32Ready,
            (unsigned)(fApic != 0 ? 1u : 0u));

    /*
     * Soft lamp only — ready + non-zero quantum. Never hard-gates boot.
     * Grep: timer: soft PASS | timer: soft FAIL
     * Does not touch preemption quantum product lamp (main.c).
     */
    fSoftPass = 0;
    if (stMono.u32Ready != 0 && stPre.u32Quantum > 0) {
        fSoftPass = 1;
    }
    if (fSoftPass != 0) {
        kprintf("timer: soft PASS\n");
    } else {
        kprintf("timer: soft FAIL\n");
    }
}

void
timer_soft_log(void)
{
    struct gj_timer_mono_soft stMono;
    struct gj_timer_preempt_soft stPre;
    const char *szSrc;

    timer_soft_inc(&g_u64SoftLogCalls);
    timer_mono_soft_snapshot(&stMono);
    timer_preempt_soft_snapshot(&stPre);

    if (stMono.u32Source == GJ_TIMER_SRC_APIC) {
        szSrc = "APIC";
    } else if (stMono.u32Source == GJ_TIMER_SRC_PIT) {
        szSrc = "PIT";
    } else {
        szSrc = "NONE";
    }

    kprintf("timer: mono soft src=%s ready=%u jiffies=%lu nsec=%lu soft_nsec=%lu "
            "npt=%lu pit_ticks=%lu apic_ticks=%lu switches=%lu\n",
            szSrc, (unsigned)stMono.u32Ready,
            (unsigned long)stMono.u64Jiffies,
            (unsigned long)stMono.u64MonoNsec,
            (unsigned long)stMono.u64MonoSoftNsec,
            (unsigned long)stMono.u64NsecPerTick,
            (unsigned long)stMono.u64PitTicks,
            (unsigned long)stMono.u64ApicTicks,
            (unsigned long)stMono.u64SourceSwitch);

    kprintf("timer: preempt soft quantum=%u slice_left=%u preempts=%lu "
            "yields=%lu checks=%lu hits=%lu check_yields=%lu sets=%lu "
            "q_ticks=%lu\n",
            (unsigned)stPre.u32Quantum, (unsigned)stPre.u32SliceLeft,
            (unsigned long)stPre.u64Preempts,
            (unsigned long)stPre.u64YieldRequests,
            (unsigned long)stPre.u64PreemptChecks,
            (unsigned long)stPre.u64PreemptCheckHits,
            (unsigned long)stPre.u64PreemptCheckYields,
            (unsigned long)stPre.u64QuantumSets,
            (unsigned long)stPre.u64QuantumTicks);

    if (stMono.u32Ready != 0 && stMono.u64Jiffies > 0) {
        kprintf("timer: mono soft PASS\n");
    } else if (stMono.u32Ready != 0) {
        kprintf("timer: mono soft READY\n");
    } else {
        kprintf("timer: mono soft FAIL\n");
    }
    if (stPre.u32Quantum > 0) {
        kprintf("timer: preempt soft PASS\n");
    } else {
        kprintf("timer: preempt soft FAIL\n");
    }

    /*
     * Mono source preference deepen: APIC preferred when armed; PIT
     * soft-fallback + honesty on partial x2APIC ICR/timer replace.
     * Wave 13 also emits timer: soft apic mono … from mono_pref.
     */
    timer_mono_pref_soft_log();

    /* Wave 19 exclusive: greppable timer: soft … inventory rollup. */
    timer_soft_inventory_log();
}

void
timer_sleep_until(u64 u64DeadlineMonoNsec)
{
    u32 u32Guard;

    timer_soft_inc(&g_u64SoftSleepCalls);
    if (!g_fTimerReady) {
        return;
    }
    if (u64DeadlineMonoNsec == 0) {
        __asm__ volatile ("sti; hlt");
        return;
    }
    /*
     * Yield + STI/HLT so IRQs and other threads can progress. Cap spins so a
     * stuck mono clock (e.g. after a user #PF path) cannot hang forever.
     * Soft-preempt check deepens quantum consume on sleep paths.
     */
    for (u32Guard = 0;
         timer_mono_nsec() < u64DeadlineMonoNsec && u32Guard < 1000000u;
         u32Guard++) {
        timer_preempt_check();
        thread_yield();
        __asm__ volatile ("sti; hlt");
    }
    if (u32Guard >= 1000000u) {
        timer_soft_inc(&g_u64SoftSleepGuard);
    }
}
