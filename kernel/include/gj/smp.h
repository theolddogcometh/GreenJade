/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SMP discovery (ACPI MADT) + AP bring-up + one-shot AP work handoff.
 * Pure C11 freestanding; dual-licensed MIT OR Apache-2.0.
 *
 * -------------------------------------------------------------------------
 * Scope
 * -------------------------------------------------------------------------
 * Product multi-CPU path for x86_64 (P-SMP-1..6):
 *   1. Discover local APICs via ACPI MADT (RSDP from UEFI or Multiboot scan).
 *   2. Bring up APs with INIT-SIPI + low trampoline (identity-mapped).
 *   3. Publish online inventory for affinity, IPI resched, TLB shootdown.
 *   4. Optional one-shot smp_ap_run work drained on the AP schedule loop.
 *
 * Soft helpers below are boot telemetry and affinity probes — not hot-path
 * locks. Soft never hard-gates product; wrap-OK counters only.
 *
 * Bring-up ceiling
 * ----------------
 * Online inventory: MADT fills aApicId[]; smp_start_aps publishes slots
 * 1..N into the static bring-up pool (percpu + park stacks). smp_bringup_cap()
 * is the soft/static ceiling (always >= 1 for BSP). Full GJ_MAX_CPUS needs
 * scaled park stacks / idle arrays; PMM percpu growth in cpu_init_ap is
 * ready for higher ids once those pools scale.
 *
 * AP lifecycle (soft phase ladder, sticky max)
 * --------------------------------------------
 *   ENTRY → PERCPU → X2APIC → TIMER → IDLE → SCHED
 * See GJ_SMP_AP_PHASE_* and smp_bringup_soft_phase().
 *
 * Cross-module: gj/cpu.h (percpu), gj/apic.h / gj/x2apic.h (IPI), gj/gdt.h
 * + gj/idt.h (AP must load GDT then IDT before unmasking IRQs).
 *
 * greppable: smp: bringup soft
 * greppable: GJ_SMP_SOFT_SLOT_ GJ_SMP_AP_PHASE_
 * greppable: smp_start_aps smp_ap_run smp_bringup_soft_log
 * greppable: MADT INIT-SIPI P-SMP
 */
#pragma once

#include <gj/types.h>

/** Soft cap on MADT local-APIC ids stored in gj_smp_info::aApicId[]. */
#define GJ_SMP_MAX_APICS 64

/**
 * MADT-derived topology snapshot (discovery view, not live online count alone).
 * fHasMadt=0 still yields a valid BSP-only inventory after smp_init_*.
 */
struct gj_smp_info {
    u32 u32NLocalApic;     /* processors listed in MADT */
    u32 u32NOnline;        /* currently running (BSP inventory view) */
    u32 u32BspApicId;
    u32 aApicId[GJ_SMP_MAX_APICS];
    u8  aEnabled[GJ_SMP_MAX_APICS];
    int fHasMadt;
    int fX2ApicIds;        /* 1 if entries used x2APIC type */
};

/* Soft per-slot bring-up outcome (sticky after smp_start_aps). */
#define GJ_SMP_SOFT_SLOT_NONE      0u /* never attempted / OOB */
#define GJ_SMP_SOFT_SLOT_ONLINE    1u /* INIT-SIPI + ready handshake ok */
#define GJ_SMP_SOFT_SLOT_TIMEOUT   2u /* no ready within spin budget */
#define GJ_SMP_SOFT_SLOT_FAIL      3u /* AP entered but park (percpu/idle) */

/*
 * Soft AP lifecycle phase (sticky max reached on that slot).
 * Deepens bring-up observability past binary online/timeout.
 */
#define GJ_SMP_AP_PHASE_NONE       0u
#define GJ_SMP_AP_PHASE_ENTRY      1u /* smp_ap_c_entry entered */
#define GJ_SMP_AP_PHASE_PERCPU     2u /* cpu_init_ap + slot online */
#define GJ_SMP_AP_PHASE_X2APIC     3u /* optional x2APIC enable attempted */
#define GJ_SMP_AP_PHASE_TIMER      4u /* local APIC timer started */
#define GJ_SMP_AP_PHASE_IDLE       5u /* per-CPU idle thread created */
#define GJ_SMP_AP_PHASE_SCHED      6u /* scheduler_run_ap about to run */

/**
 * Soft snapshot of last start_aps / ap_run telemetry.
 * Filled by smp_bringup_soft_snapshot(); all fields zero before start_aps.
 */
struct gj_smp_bringup_soft {
    u32 u32Tried;        /* INIT-SIPI attempts */
    u32 u32Ok;           /* ready handshake success */
    u32 u32Timeout;      /* ready never set */
    u32 u32Skipped;      /* MADT disabled / BSP / over cap (not tried) */
    u32 u32Online;       /* g_Smp.u32NOnline after start_aps */
    u32 u32Cap;          /* smp_bringup_cap() at snapshot */
    u32 u32SchedReady;   /* AP slots with g_aApSchedReady */
    u32 u32ApRunOk;      /* cumulative smp_ap_run success */
    u32 u32ApRunFail;    /* cumulative smp_ap_run hard fail (bad args/busy) */
    u32 u32ApRunTimeout; /* cumulative smp_ap_run wait timeout */
    u32 u32LastSpins;    /* ready-wait spins on last attempted AP */
    u32 u32LastApicId;   /* APIC id of last attempted AP */
    u32 u32LastSlot;     /* CPU slot of last attempted AP */
};

/**
 * Parse Multiboot2 tags for ACPI RSDP → MADT local APICs.
 * Safe on UP: always reports at least BSP. Dev path (P-BOOT-2); product
 * prefers smp_init_from_rsdp with the UEFI config-table RSDP.
 */
void smp_init_from_mb2(u32 paMb2Info);

/**
 * MADT discovery from a known RSDP physical address (UEFI config table).
 * Falls back to BIOS scan if u64Rsdp is 0 or invalid.
 * Product path: P-BOOT-4 / P-SMP-2.
 */
void smp_init_from_rsdp(u64 u64Rsdp);

/** Pointer to static MADT/online inventory (never NULL after any smp_init_*). */
const struct gj_smp_info *smp_info(void);

/** MADT local-APIC count (enabled+disabled listings); at least 1 (BSP). */
u32 smp_cpu_count_detected(void);

/** Currently running CPUs in the bring-up inventory (BSP + successful APs). */
u32 smp_cpu_count_online(void);

/**
 * Soft bring-up ceiling for AP slots (static percpu + park-stack pools).
 * Always >= 1 (BSP). Full GJ_MAX_CPUS requires scaled stacks/idle arrays.
 */
u32 smp_bringup_cap(void);

/** Soft: 1 if u32CpuSlot is within published online inventory (incl. BSP). */
int smp_cpu_online(u32 u32CpuSlot);

/**
 * Bring up APs (INIT-SIPI + trampoline).
 * For each MADT-enabled AP under smp_bringup_cap: send INIT/SIPI, wait for
 * ready handshake, soft-tag ONLINE|TIMEOUT|FAIL. Idempotent soft notes if
 * re-entered. Call after apic_init / idt / gdt product paths are ready.
 */
void smp_start_aps(void);

/**
 * Run pfn(pArg) once on AP slot u32CpuSlot (1..online-1).
 * BSP waits until done (or timeout). Returns 0 on success, -1 if no AP / busy.
 * Posts work into the AP schedule path (affinity thread or one-shot slot).
 * Not a general remote call — one outstanding work item at a time.
 */
int  smp_ap_run(u32 u32CpuSlot, void (*pfn)(void *), void *pArg);

/** True after AP has entered scheduler_run_ap (per-CPU RQ live). */
int  smp_ap_sched_ready(u32 u32CpuSlot);

/** Drain one-shot smp_ap_run work on this AP (called from AP schedule loop). */
void smp_ap_poll_work(void);

/** APIC id for online CPU slot (0=BSP). Soft 0 if slot unpublished. */
u32  smp_apic_id_for_cpu(u32 u32CpuSlot);

/* ------------------------------------------------------------------ */
/* Soft AP bring-up observability (boot telemetry — not hot-path)       */
/* ------------------------------------------------------------------ */

/** Soft snapshot of bring-up / ap_run counters into *pOut (NULL-safe). */
void smp_bringup_soft_snapshot(struct gj_smp_bringup_soft *pOut);

/** Soft: per-slot bring-up outcome (GJ_SMP_SOFT_SLOT_*). BSP → ONLINE. */
u32  smp_bringup_soft_slot(u32 u32CpuSlot);

/** Soft: sticky max AP phase reached (GJ_SMP_AP_PHASE_*). BSP → SCHED. */
u32  smp_bringup_soft_phase(u32 u32CpuSlot);

/** Soft: last ready-wait spin count for slot (0 if never attempted). */
u32  smp_bringup_soft_spins(u32 u32CpuSlot);

/**
 * Greppable bring-up soft summary (safe after smp_start_aps; also called
 * from start_aps itself). Emits:
 *   smp: bringup soft PASS|PARTIAL|UP|NONE …
 *   smp: bringup soft slot=… status=… phase=… spins=… apic=…
 *   + x2apic_icr_soft_log() when x2APIC ICR soft counters are armed.
 * greppable: smp: bringup soft
 */
void smp_bringup_soft_log(void);
