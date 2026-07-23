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
 * Soft timer inventory (Wave 10 exclusive): greppable "timer: soft …"
 * rollup over mono / preempt / source axes. Diagnostics only — never
 * hard-gates boot or product deadlines. Pure C; this file only.
 *
 * greppable: timer: soft inventory
 * greppable: timer: soft mono
 * greppable: timer: soft preempt
 * greppable: timer: soft source
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
 * Soft timer inventory extras (Wave 10 exclusive; file-local).
 * Emission count only — never hard product gates. wrap OK.
 * greppable: timer: soft
 */
static u64          g_u64SoftInventoryLogs;

static void timer_soft_inventory_log(void);
static void timer_mono_pref_soft_log(void);

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
     */
    if (!g_fApicSource) {
        g_u64Jiffies++;
        g_u64PitTicks++;
        futex_timer_check();
        quantum_tick();
        revoke_hygiene_tick();
    }
    outb(PIC1_CMD, PIC_EOI);
}

void
timer_tick_apic(void)
{
    g_u64Jiffies++;
    g_u64ApicMonoTicks++;
    futex_timer_check();
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
    /* Wave 10: baseline soft inventory (zeros/jiffies=0 expected early). */
    timer_soft_inventory_log();
}

void
timer_set_apic_source(u64 u64NsecPerTick)
{
    int fFirstHandoff;

    if (u64NsecPerTick == 0) {
        return;
    }
    g_u64NsecPerTick = u64NsecPerTick;
    fFirstHandoff = 0;
    if (!g_fApicSource) {
        g_u64SourceSwitch++;
        g_u64PitDemotions++;
        fFirstHandoff = 1;
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
    /* Wave 10: soft inventory after PIT→APIC handoff. */
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

    u64Coarse = g_u64Jiffies * g_u64NsecPerTick;
    if (!g_fApicSource || g_u64NsecPerTick == 0) {
        return u64Coarse;
    }
    u32Init = apic_timer_init_count();
    if (u32Init < 2u) {
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
        return u64Coarse;
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
        return;
    }
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
        return;
    }
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
                "vector_pit=32 vector_apic=48\n",
                (unsigned long)g_u64PitDemotions,
                (unsigned long)g_u64SourceSwitch);

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
}

/**
 * Greppable soft timer inventory (Wave 10 exclusive; product / smoke).
 * Prefix-stable markers (timer: soft …):
 *   timer: soft inventory  — ready/src/hz/quantum + surface catalog + logs
 *   timer: soft mono       — coarse/soft mono delta + pit/apic tick axes
 *   timer: soft preempt    — quantum slice + soft preempt_check counters
 *   timer: soft source     — PIT/APIC handoff + LAPIC INIT/CUR sample
 *   timer: soft PASS|FAIL  — soft lamp (ready + quantum); never hard-gates
 *
 * Never allocates; safe from boot soft-smoke / timer_soft_log.
 * greppable: timer: soft
 */
static void
timer_soft_inventory_log(void)
{
    struct gj_timer_mono_soft stMono;
    struct gj_timer_preempt_soft stPre;
    const char *szSrc;
    u64 u64SoftDelta;
    u32 u32Init;
    u32 u32Cur;
    u32 u32Hz;
    int fSoftPass;

    timer_mono_soft_snapshot(&stMono);
    timer_preempt_soft_snapshot(&stPre);

    if (g_u64SoftInventoryLogs < ~0ull) {
        g_u64SoftInventoryLogs++;
    }

    if (stMono.u32Source == GJ_TIMER_SRC_APIC) {
        szSrc = "APIC";
    } else if (stMono.u32Source == GJ_TIMER_SRC_PIT) {
        szSrc = "PIT";
    } else {
        szSrc = "NONE";
    }

    u64SoftDelta = 0;
    if (stMono.u64MonoSoftNsec >= stMono.u64MonoNsec) {
        u64SoftDelta = stMono.u64MonoSoftNsec - stMono.u64MonoNsec;
    }

    u32Init = 0;
    u32Cur = 0;
    if (g_fApicSource != 0) {
        u32Init = apic_timer_init_count();
        u32Cur = apic_timer_cur_count();
    }

    u32Hz = GJ_TIMER_HZ;
    if (stMono.u64NsecPerTick != 0) {
        u32Hz = (u32)(1000000000ull / stMono.u64NsecPerTick);
        if (u32Hz == 0) {
            u32Hz = GJ_TIMER_HZ;
        }
    }

    /* Grep: timer: soft inventory */
    kprintf("timer: soft inventory ready=%u src=%s hz=%u quantum=%u "
            "jiffies=%lu npt=%lu logs=%lu "
            "g_timer_mono=1 soft_mono=1 apic_src=1 preempt=1 "
            "apic_pref=1 pit_fallback=1\n",
            (unsigned)stMono.u32Ready, szSrc, (unsigned)u32Hz,
            (unsigned)stPre.u32Quantum,
            (unsigned long)stMono.u64Jiffies,
            (unsigned long)stMono.u64NsecPerTick,
            (unsigned long)g_u64SoftInventoryLogs);

    /* Grep: timer: soft mono */
    kprintf("timer: soft mono coarse=%lu soft=%lu delta=%lu "
            "jiffies=%lu npt=%lu pit_ticks=%lu apic_ticks=%lu switches=%lu\n",
            (unsigned long)stMono.u64MonoNsec,
            (unsigned long)stMono.u64MonoSoftNsec,
            (unsigned long)u64SoftDelta,
            (unsigned long)stMono.u64Jiffies,
            (unsigned long)stMono.u64NsecPerTick,
            (unsigned long)stMono.u64PitTicks,
            (unsigned long)stMono.u64ApicTicks,
            (unsigned long)stMono.u64SourceSwitch);

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
            "switches=%lu demotions=%lu\n",
            szSrc, (unsigned)(g_fApicSource ? 1u : 0u),
            g_fApicSource ? "APIC" : "PIT",
            (unsigned)(g_fApicSource ? 1u : 0u),
            (unsigned)u32Init, (unsigned)u32Cur,
            (unsigned long)stMono.u64SourceSwitch,
            (unsigned long)g_u64PitDemotions);

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
     */
    timer_mono_pref_soft_log();

    /* Wave 10 exclusive: greppable timer: soft … inventory rollup. */
    timer_soft_inventory_log();
}

void
timer_sleep_until(u64 u64DeadlineMonoNsec)
{
    u32 u32Guard;

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
}
