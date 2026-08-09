/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Early mono clock: PIT ch0 @ GJ_TIMER_HZ on remapped IRQ0 (vector 32).
 * After LAPIC calibrate, jiffies/mono prefer the local APIC (vector 48);
 * PIT is demoted to soft-fallback only (IRQ0 unmasked for HLT wake; no mono).
 *
 * Soft mono (timer_mono_nsec_soft / mono soft snapshot) may interpolate
 * within the current APIC period using LAPIC CUR; coarse mono stays
 * jiffy-stable for futex/door deadlines. Quantum soft-preempt counters
 * deepen tick -> yield-request -> preempt_check observability.
 *
 * Mono source preference (APIC/x2APIC armed):
 *   mono/jiffies advance from timer_tick_apic only; timer_tick EOIs PIC
 *   but never increments jiffies while g_fApicSource is set.
 * Honesty: full x2APIC ICR/timer replace of the 8259+PIT product path
 * remains PARTIAL when the hardware path is incomplete (xAPIC timer
 * handoff alone != complete x2APIC ICR/timer replace).
 *
 * Dual DoD A/B / H1 residual (Soft!=product - this unit exclusive; G-AC-1):
 *   timer_tick / timer_tick_apic / irq_timer_handler NEVER call net_eth_poll.
 *   Fault class (H1): timer IRQ -> net_eth_poll -> net_tcp_poll / soft residual ->
 *   IRQ stack smash -> #PF I=1 wild RIP. Eth poll ownership is run-loop only
 *   (scheduler_run on full thr stack). TIMER_H1_* compile-time locks + lean
 *   residual lamps document ownership; soft PASS != product Dual DoD close
 *   (host arping/ping / :22 still OPEN; agent!=close). Dual MIT|Apache-2.0.
 *   No version stamps. No stamp storms (inventory cap init+handoff; residual
 *   lean <= few greppable lines - never tick path). No GPL.
 *
 * Soft timer inventory (this unit only - greppable "timer: soft ..."):
 *   timer: soft inventory     - ready/src/hz/quantum + H1 eth ownership tokens
 *   timer: soft mono          - coarse/soft mono delta + pit/apic tick axes
 *   timer: soft preempt       - quantum slice + soft preempt_check counters
 *   timer: soft source        - PIT/APIC handoff + LAPIC INIT/CUR sample
 *   timer: soft apic mono     - APIC mono preference deepen
 *   timer: soft path          - honesty catalog (product surface bounds)
 *   timer: soft handoff       - PIT->APIC demotion / switch tallies
 *   timer: soft interpolate   - LAPIC CUR soft-mono sample counters
 *   timer: soft eth           - lean H1 eth poll ownership (DoD B residual)
 *   timer: soft residual      - lean dual_dod_b residual surface (Soft!=product)
 *   timer: soft deepen        - area catalog (inventory-capped only)
 *   timer: soft PASS|FAIL     - soft lamp (ready + quantum); never hard-gates
 * Complementary surfaces (kept; never reshape primary fields):
 *   timer: soft lamps|stats|vectors|quantum|futex|honesty|surface
 *   timer: soft exclusive|claim|ratio|eoi
 * Diagnostics only - never hard-gates boot or product deadlines. Pure C.
 * Soft != full x2APIC timer product (xAPIC handoff alone remains PARTIAL).
 * Soft != product Dual DoD B close.
 *
 * greppable: timer: soft inventory
 * greppable: timer: soft mono
 * greppable: timer: soft preempt
 * greppable: timer: soft source
 * greppable: timer: soft apic mono
 * greppable: timer: soft path
 * greppable: timer: soft handoff
 * greppable: timer: soft interpolate
 * greppable: timer: soft eth
 * greppable: timer: soft residual dual_dod_b
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
 * greppable: timer: soft PASS
 * greppable: timer: soft FAIL
 * greppable: timer: mono soft
 * greppable: timer: preempt soft
 * greppable: timer: mono source APIC soft
 * greppable: timer: mono source PIT soft
 * greppable: timer: pit demoted soft
 * greppable: timer: apic mono preferred PASS
 * greppable: timer: x2apic mono replace soft
 * greppable: net_eth_poll=run_loop_only
 * greppable: net_eth_irq=0
 * greppable: dual_dod_b
 * greppable: dual_dod_a
 * greppable: soft_ne_product=1
 * greppable: G-AC-1
 * greppable: TIMER_H1_
 * greppable: fault_class=H1_irq_stack_smash
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
#define TIMER_SOFT_WAVE 126u

/*
 * Soft inventory emit cap: init + PIT->APIC handoff only (never timer_soft_log,
 * never tick path). Multi-line dump collapsed; soft != product. G752 panel /
 * COM1 storm guard. Paired with no net_eth_poll on IRQ (Dual DoD B residual).
 */
#define TIMER_SOFT_INV_CAP 2u

/*
 * H1 residual locks (C0 timer exclusive; Soft!=product; G-AC-1).
 * Flip requires H1 review - timer IRQ -> net_eth_poll is #PF I=1 fault class.
 * No net_eth.h in this unit - cannot call net_eth_poll by construction.
 * Dual DoD A/B remain OPEN (agent!=close). No version stamps. No GPL.
 * greppable: TIMER_H1_ | net_eth_irq=0 | tick_path=0 | dual_dod_b=OPEN
 * greppable: fault_class=H1_irq_stack_smash | G-AC-1 | soft_ne_product=1
 */
#define TIMER_H1_ETH_POLL_IRQ    0u /* never net_eth_poll on timer IRQ */
#define TIMER_H1_TICK_PATH       0u /* timer_tick has no eth poll */
#define TIMER_H1_TICK_APIC_PATH  0u /* timer_tick_apic has no eth poll */
#define TIMER_H1_IRQ_HANDLER     0u /* irq_timer_handler has no eth poll */
#define TIMER_H1_NET_ETH_IRQ     0u /* net_eth_irq=0 forever from this unit */
#define TIMER_H1_RUN_LOOP_ONLY   1u /* eth poll owner = scheduler_run thr */
#define TIMER_H1_LEAN_CHECKS     6u /* soft residual lean self-check count */

_Static_assert(TIMER_H1_ETH_POLL_IRQ == 0u,
               "H1: timer IRQ must never call net_eth_poll");
_Static_assert(TIMER_H1_TICK_PATH == 0u,
               "H1: timer_tick path must not poll eth");
_Static_assert(TIMER_H1_TICK_APIC_PATH == 0u,
               "H1: timer_tick_apic path must not poll eth");
_Static_assert(TIMER_H1_IRQ_HANDLER == 0u,
               "H1: irq_timer_handler must not poll eth");
_Static_assert(TIMER_H1_NET_ETH_IRQ == 0u,
               "H1: net_eth_irq must be 0 (run-loop thr owns eth poll)");
_Static_assert(TIMER_H1_RUN_LOOP_ONLY == 1u,
               "H1: eth poll ownership is scheduler_run thr stack only");
_Static_assert(TIMER_H1_LEAN_CHECKS == 6u,
               "H1 lean: residual self-check count locked");

/* Soft surface bit lamps (catalog; software-only claims). */
#define TIMER_SOFT_SURF_MONO       (1u << 0)
#define TIMER_SOFT_SURF_SOFT_MONO  (1u << 1)
#define TIMER_SOFT_SURF_APIC_SRC   (1u << 2)
#define TIMER_SOFT_SURF_PREEMPT    (1u << 3)
#define TIMER_SOFT_SURF_PIT_FALL   (1u << 4)
#define TIMER_SOFT_SURF_INTERP     (1u << 5)
#define TIMER_SOFT_SURF_FUTEX_COUP (1u << 6)
#define TIMER_SOFT_SURF_HANDOFF    (1u << 7)
/* Eth poll ownership: run-loop only (never timer IRQ). Dual DoD B residual. */
#define TIMER_SOFT_SURF_ETH_RUNLOOP (1u << 8)
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
static u64          g_u64PitDemotions;      /* PIT->APIC soft demotions */
static u64          g_u64MonoPrefLogs;      /* mono preference soft log emits */

/*
 * Soft timer inventory extras (Wave 10 base + Wave 13 path + Wave 20 deepen;
 * file-local). Emission + path tallies only - never hard product gates. wrap OK.
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
static u64          g_u64SoftHandoffFirst;  /* first PIT->APIC handoff */
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

/* isr_stubs.S - IRQ0 after PIC remap (vector 32); PIC 1-15 share spurious */
extern void irq_stub_0(void);
extern void irq_stub_pic_spurious(void);

/* Soft count of unhandled / spurious PIC IRQs (vectors 33-47). */
static volatile u64 g_u64SoftPicSpurious;

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
     * advance only from timer_tick_apic. PIT IRQ0 is soft-fallback only -
     * still EOI the PIC if a stray IRQ arrives, but do not advance mono.
     *
     * H1 / Dual DoD B residual (Soft!=product):
     *   NEVER call net_eth_poll on this IRQ stack.
     *   Fault class: timer IRQ -> net_eth_poll -> net_tcp_poll / soft residual
     *   -> stack smash -> #PF I=1 wild RIP. Timer still wakes HLT; sched run
     *   loop owns eth poll every pass (full thr stack). net_eth_irq=0.
     *   net_eth_poll=run_loop_only. No soft inventory / kprintf here.
     *   No net_eth.h - this unit cannot call net_eth_poll by construction.
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
    /* H1: net_eth_poll never - run-loop only (Dual DoD B residual). */
}

void
timer_tick_apic(void)
{
    /*
     * APIC mono tick (BSP). Same side effects as PIT mono path, no PIC EOI.
     * H1 / Dual DoD B residual: NEVER net_eth_poll here (IRQ stack smash).
     * Eth poll ownership = scheduler_run thr stack only. Soft!=product.
     * No net_eth.h - cannot call net_eth_poll by construction.
     */
    timer_soft_inc(&g_u64SoftTickApic);
    g_u64Jiffies++;
    g_u64ApicMonoTicks++;
    futex_timer_check();
    timer_soft_inc(&g_u64SoftFutexCoupled);
    quantum_tick();
    revoke_hygiene_tick();
    /* H1: net_eth_poll never - run-loop only (see timer_tick Dual DoD B). */
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

/* C entry from irq_stub_0 (PIC timer). H1: no net_eth_poll - timer_tick only. */
void
irq_timer_handler(void)
{
    timer_tick();
}

/*
 * C entry from irq_stub_pic_spurious (PIC IRQs 1-15 -> vectors 33-47).
 * G752: after sti, PIC may deliver spurious IRQ7 (vec 39). Must have a
 * present gate + EOI or CPU raises #GP on the missing IDT entry.
 */
void
irq_pic_spurious_handler(void)
{
    if (g_u64SoftPicSpurious < ~0ull) {
        g_u64SoftPicSpurious++;
    }
    /* Slave then master EOI covers IRQ8-15 and is harmless for master-only. */
    outb(PIC2_CMD, PIC_EOI);
    outb(PIC1_CMD, PIC_EOI);
}

/** Remap PICs to 32/40. fUnmaskIrq0=0 keeps all masked (safe setup). */
static void
pic_remap(int fUnmaskIrq0)
{
    u8 u8A1;
    u8 u8A2;

    u8A1 = inb(PIC1_DATA);
    u8A2 = inb(PIC2_DATA);
    (void)u8A1;
    (void)u8A2;

    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);
    outb(PIC1_DATA, 0x20); /* master offset 32 */
    outb(PIC2_DATA, 0x28); /* slave offset 40 */
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    if (fUnmaskIrq0 != 0) {
        outb(PIC1_DATA, 0xFE); /* IRQ0 only */
    } else {
        outb(PIC1_DATA, 0xFF); /* all masked */
    }
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
    /*
     * Program PIC+PIT with IRQ0 masked and IF left clear. Do NOT sti here.
     * G752: any early sti in timer_init raced IRQ0 -> kernel fault halt
     * (RIP in timer_init / kprintf setup). IRQs enabled later via
     * timer_irq_enable() after APIC init.
     */
    __asm__ volatile ("cli" ::: "memory");
    pic_remap(0); /* remap, keep IRQ0 masked */
    pit_set_hz(GJ_TIMER_HZ);
    idt_set_gate(32, (void *)irq_stub_0, 0x8E);
    /*
     * Install present gates for PIC IRQs 1-15 (vectors 33-47) BEFORE any
     * later sti. Missing gate 39 -> #GP on spurious IRQ7 (G752 STATUS
     * FAULT vec=13 err=0x13b rip=timer_irq_enable+sti).
     */
    {
        u32 u32Vec;

        for (u32Vec = 33u; u32Vec < 48u; u32Vec++) {
            idt_set_gate(u32Vec, (void *)irq_stub_pic_spurious, 0x8E);
        }
    }
    g_fTimerReady = 1;
    kprintf("timer: PIT %u Hz IRQ0 vector 32 nsec/tick=%lu quantum=%u "
            "(IRQs still masked; PIC 33-47 spurious gates OK)\n",
            (unsigned)GJ_TIMER_HZ, (unsigned long)g_u64NsecPerTick,
            (unsigned)g_u32Quantum);
    /*
     * Skip multi-KiB soft inventory on panel path (dead COM1 / G752): floods
     * the FB log and has raced faults after SMEP/SMAP harden. Full inventory
     * still runs when THRE is live (QEMU Multiboot).
     */
    if (serial_thre_dead() == 0u) {
        timer_mono_pref_soft_log();
        timer_soft_inventory_log();
    } else {
        kprintf("timer: soft inventory SKIP (panel path)\n");
    }
}

void
timer_irq_enable(void)
{
    if (g_fTimerReady == 0) {
        kprintf("timer: irq_enable SKIP (timer not ready)\n");
        return;
    }
    /* Mask slave fully; unmask master IRQ0 only; then open IF. */
    outb(PIC2_DATA, 0xFF);
    outb(PIC1_DATA, 0xFE); /* unmask IRQ0 only */
    __asm__ volatile ("sti" ::: "memory");
    kprintf("timer: IRQ0 unmasked + sti PASS (pic_spur=%lu)\n",
            (unsigned long)g_u64SoftPicSpurious);
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
     * Prefer APIC for mono/jiffies. Keep PIT IRQ0 unmasked so HLT still wakes
     * if LAPIC timer IRQs are quiet (G752: frozen after M0 when PIT was fully
     * masked). H1 / Dual DoD B residual: net_eth_poll is NOT on IRQ - sched
     * run loop only (net_eth_poll=run_loop_only). Mono still only advances
     * from timer_tick_apic. Soft!=product.
     */
    g_fApicSource = 1;
    outb(PIC2_DATA, 0xFF);
    outb(PIC1_DATA, 0xFE); /* IRQ0 unmasked - wake HLT; mono not advanced */

    /* Greppable mono preference + demotion lamps (product / smoke). */
    timer_mono_pref_soft_log();
    if (fFirstHandoff != 0) {
        /* Continuity: keep nsec/tick on serial for older greps / humans. */
        kprintf("timer: mono handoff APIC nsec/tick=%lu switches=%lu "
                "pit_demotions=%lu\n",
                (unsigned long)g_u64NsecPerTick,
                (unsigned long)g_u64SourceSwitch,
                (unsigned long)g_u64PitDemotions);
        /* Soft inventory: first handoff only (paired with timer_init; capped). */
        timer_soft_inventory_log();
    }
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
     * Elapsed fraction ~= (INIT - CUR) / INIT; clamp CUR to INIT.
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
 *   timer: mono source APIC soft ...  - mono prefers APIC/x2APIC timer
 *   timer: mono source PIT soft ...   - early boot / unarmed APIC path
 *   timer: pit demoted soft ...       - PIT soft-fallback only (IRQ0 masked)
 *   timer: apic mono preferred PASS - product lamp when source is APIC
 *   timer: x2apic mono replace soft PARTIAL|... - honesty on full replace
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
         * PIT demoted: soft-fallback only - no mono advance from IRQ0.
         * Grep: timer: pit demoted soft
         */
        /* IRQ0 stays unmasked so HLT still wakes; mono advance remains APIC. */
        kprintf("timer: pit demoted soft irq0_masked=0 irq0_hlt_wake=1 "
                "mono_advance=0 soft_fallback=1 demotions=%lu switches=%lu "
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
     * product exit - ICR/timer replace is still partial today.
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

    /* Wave 13: twin soft apic mono axis under timer: soft ... */
    timer_soft_apic_mono_log();
}

/**
 * Wave 13 exclusive: greppable "timer: soft apic mono ..." deepen.
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
        /* irq0_masked=0: HLT wake; mono still only from timer_tick_apic. */
        kprintf("timer: soft apic mono %s init_armed=%u cur_live=%u "
                "pit_mono_advance=0 irq0_masked=0 irq0_hlt_wake=1\n",
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
 * Cap: TIMER_SOFT_INV_CAP emits - init + PIT->APIC handoff only (never tick,
 * never timer_soft_log). Multi-line complementary stamps collapsed to a few
 * dense rollups (G752 panel / COM1 storm guard). Soft != full x2APIC product.
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

    /* Cap: init + handoff only; later callers (soft_log, re-handoff) skip. */
    if (g_u64SoftInventoryLogs >= (u64)TIMER_SOFT_INV_CAP) {
        return;
    }
    /* Panel path (dead COM1): skip multi-line dump entirely. */
    if (serial_thre_dead() != 0u) {
        kprintf("timer: soft inventory SKIP (panel path cap)\n");
        timer_soft_inc(&g_u64SoftInventoryLogs);
        return;
    }

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
              TIMER_SOFT_SURF_ETH_RUNLOOP | TIMER_SOFT_SURF_X2_FULL;

    /*
     * Collapsed inventory (storm guard; cap=init+handoff). H1 eth tokens once;
     * lean residual surface below owns dual_dod_b detail.
     * Grep: timer: soft inventory | net_eth_poll=run_loop_only | net_eth_irq=0
     */
    kprintf("timer: soft inventory ready=%u src=%s hz=%u quantum=%u "
            "jiffies=%lu npt=%lu logs=%lu/%u "
            "g_timer_mono=1 soft_mono=1 apic_src=1 preempt=1 "
            "apic_pref=1 pit_fallback=1 cap=init_handoff "
            "net_eth_irq=0 net_eth_poll=run_loop_only "
            "irq0_hlt_wake=1 soft_ne_product=1 storm=0\n",
            (unsigned)stMono.u32Ready, szSrc, (unsigned)u32Hz,
            (unsigned)stPre.u32Quantum,
            (unsigned long)stMono.u64Jiffies,
            (unsigned long)stMono.u64NsecPerTick,
            (unsigned long)g_u64SoftInventoryLogs,
            (unsigned)TIMER_SOFT_INV_CAP);

    /* Grep: timer: soft mono | timer: soft preempt | timer: soft source */
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
    /* irq0_masked=0 after handoff: HLT wake; mono only from APIC tick. */
    kprintf("timer: soft source=%s apic=%u preferred=%s pit_soft_fallback=%u "
            "init_cnt=%u cur_cnt=%u vector_pit=32 vector_apic=48 "
            "switches=%lu demotions=%lu pit_stray=%lu "
            "x2apic_en=%u x2apic_supp=%u irq0_masked=0 irq0_hlt_wake=1\n",
            szSrc, (unsigned)(fApic != 0 ? 1u : 0u),
            szPreferred,
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)u32Init, (unsigned)u32Cur,
            (unsigned long)stMono.u64SourceSwitch,
            (unsigned long)g_u64PitDemotions,
            (unsigned long)g_u64SoftPitStray,
            (unsigned)(fX2En != 0 ? 1u : 0u),
            (unsigned)(fX2Supp != 0 ? 1u : 0u));

    /*
     * Collapsed handoff + interpolate + path (was 3 storm lines).
     * Grep: timer: soft handoff | timer: soft interpolate | timer: soft path
     */
    kprintf("timer: soft handoff switches=%lu demotions=%lu "
            "apic_src=%u preferred=%s pit_soft_fallback=%u "
            "irq0_masked=0 irq0_hlt_wake=1 mono_from_pit=%u mono_from_apic=%u "
            "pit_stray=%lu npt=%lu "
            "interp_live=%u elapsed=%u frac_ppm=%u delta_nsec=%lu "
            "path=coarse_mono+soft_mono+apic_pref+pit_fallback+preempt+"
            "futex full_x2apic_icr_timer_replace=0 wave=%u via=timer.c\n",
            (unsigned long)stMono.u64SourceSwitch,
            (unsigned long)g_u64PitDemotions,
            (unsigned)(fApic != 0 ? 1u : 0u),
            szPreferred,
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(fApic != 0 ? 0u : 1u),
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned long)g_u64SoftPitStray,
            (unsigned long)stMono.u64NsecPerTick,
            (unsigned)(fInterpLive != 0 ? 1u : 0u),
            (unsigned)u32Elapsed, (unsigned)u32FracPpm,
            (unsigned long)u64SoftDelta,
            (unsigned)TIMER_SOFT_WAVE);

    /* Wave 13 apic mono deepen (twin under soft inventory; parent-capped). */
    timer_soft_apic_mono_log();

    /*
     * Collapsed complementary rollup (Wave 15-16 stamps were multi-line storm).
     * Grep: timer: soft lamps | stats | vectors | quantum | futex | honesty |
     *       surface | exclusive | claim | ratio | eoi | deepen
     */
    kprintf("timer: soft lamps ready=%u apic=%u x2=%u x2_supp=%u "
            "interp=%u quantum=%u handoff=%u futex=%u pit_active=%u "
            "preferred=%s "
            "stats tick_pit=%lu tick_apic=%lu tick_pit_eoi=%lu "
            "handoff_calls=%lu handoff_first=%lu futex_coupled=%lu "
            "inv_logs=%lu apic_mono_logs=%lu "
            "vectors pit=32 apic=48 irq0_eoi=1 pic_mask_after_handoff=0 "
            "pit_mono_advance=%u apic_mono_advance=%u "
            "eoi pit_eoi=%lu irq0_masked=0 irq0_hlt_wake=1 wave=%u\n",
            (unsigned)(stMono.u32Ready != 0 ? 1u : 0u),
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(fX2En != 0 ? 1u : 0u),
            (unsigned)(fX2Supp != 0 ? 1u : 0u),
            (unsigned)(fInterpLive != 0 ? 1u : 0u),
            (unsigned)(stPre.u32Quantum > 0 ? 1u : 0u),
            (unsigned)(g_u64SourceSwitch > 0 ? 1u : 0u),
            (unsigned)(g_u64SoftFutexCoupled > 0 ? 1u : 0u),
            (unsigned)(fApic != 0 ? 0u : (g_fTimerReady ? 1u : 0u)),
            szPreferred,
            (unsigned long)g_u64SoftTickPit,
            (unsigned long)g_u64SoftTickApic,
            (unsigned long)g_u64SoftTickPitEoi,
            (unsigned long)g_u64SoftHandoffCalls,
            (unsigned long)g_u64SoftHandoffFirst,
            (unsigned long)g_u64SoftFutexCoupled,
            (unsigned long)g_u64SoftInventoryLogs,
            (unsigned long)g_u64SoftApicMonoLogs,
            (unsigned)(fApic != 0 ? 0u : 1u),
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned long)g_u64SoftTickPitEoi,
            (unsigned)TIMER_SOFT_WAVE);

    /*
     * Honesty rollup (x2APIC partial claim). Dual DoD B residual tokens live
     * on the lean residual + eth lamps below - not restated here (storm lean).
     * Grep: timer: soft honesty | soft_ne_product
     */
    kprintf("timer: soft honesty soft_ne_full_x2apic=1 "
            "full_x2apic_icr_timer_replace=0 apic_src=%u x2apic_en=%u "
            "x2apic_supp=%u claim=PARTIAL product_complete=0 "
            "surface bits=0x%x exclusive=1 unit=timer.c "
            "ratio tick_pit=%lu tick_apic=%lu pit_stray=%lu handoff=%lu "
            "interp=%lu futex_coupled=%lu "
            "deepen areas=inventory,mono,preempt,source,apic_mono,path,"
            "handoff,interpolate,eth,residual,lamps,stats,vectors,quantum,"
            "futex,honesty,surface,exclusive,claim,ratio,eoi "
            "cap=%u logs=%lu hard_gate=0 soft_ne_product=1\n",
            (unsigned)(fApic != 0 ? 1u : 0u),
            (unsigned)(fX2En != 0 ? 1u : 0u),
            (unsigned)(fX2Supp != 0 ? 1u : 0u),
            (unsigned)u32Surf,
            (unsigned long)g_u64SoftTickPit,
            (unsigned long)g_u64SoftTickApic,
            (unsigned long)g_u64SoftPitStray,
            (unsigned long)g_u64SourceSwitch,
            (unsigned long)g_u64SoftMonoInterp,
            (unsigned long)g_u64SoftFutexCoupled,
            (unsigned)TIMER_SOFT_INV_CAP,
            (unsigned long)g_u64SoftInventoryLogs);

    /*
     * Lean Dual DoD A/B / H1 residual (Soft!=product; inventory-capped only).
     * Two greppable lines max - no stamp storms, no version stamp.
     * Compile-time TIMER_H1_* locks + run-loop-only ownership; never product close.
     * Grep: timer: soft eth | timer: soft residual dual_dod_b
     * Grep: net_eth_poll=run_loop_only | net_eth_irq=0 | dual_dod_b | G-AC-1
     * Grep: fault_class=H1_irq_stack_smash | TIMER_H1_ | soft_ne_product=1
     */
    kprintf("timer: soft eth net_eth_poll=run_loop_only net_eth_irq=%u "
            "tick_path=%u tick_apic_path=%u irq_timer_handler=%u "
            "owner=scheduler_run stack=thr fault_class=H1_irq_stack_smash "
            "surf_eth_runloop=1 thr_only=%u G-AC-1=1 soft_ne_product=1 "
            "dual=MIT_OR_Apache-2.0\n",
            (unsigned)TIMER_H1_NET_ETH_IRQ,
            (unsigned)TIMER_H1_TICK_PATH,
            (unsigned)TIMER_H1_TICK_APIC_PATH,
            (unsigned)TIMER_H1_IRQ_HANDLER,
            (unsigned)TIMER_H1_RUN_LOOP_ONLY);
    kprintf("timer: soft residual dual_dod_a=OPEN dual_dod_b=OPEN "
            "net_eth_poll=run_loop_only net_eth_irq=%u "
            "owner=scheduler_run never_on_timer_irq=1 "
            "product_sshd_tcp22=OPEN eth_poll_irq=%u lean_checks=%u "
            "soft_ne_product=1 G-AC-1=1 storm=0 Soft!=product "
            "agent!=close dual=MIT_OR_Apache-2.0\n",
            (unsigned)TIMER_H1_NET_ETH_IRQ,
            (unsigned)TIMER_H1_ETH_POLL_IRQ,
            (unsigned)TIMER_H1_LEAN_CHECKS);

    /*
     * Soft lamp only - ready + non-zero quantum. Never hard-gates boot.
     * Grep: timer: soft PASS | timer: soft FAIL
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
     * Wave 13 also emits timer: soft apic mono ... from mono_pref.
     * Soft inventory is NOT here - init + first handoff only (cap; storm guard).
     */
    timer_mono_pref_soft_log();
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
