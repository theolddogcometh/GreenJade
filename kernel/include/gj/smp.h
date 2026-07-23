/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * SMP discovery (MADT) + AP bring-up + one-shot AP work handoff.
 *
 * Online inventory: MADT fills aApicId[]; smp_start_aps publishes slots
 * 1..N into the static bring-up pool (percpu + park stacks). Soft helpers
 * below are for boot telemetry and affinity probes — not hot-path locks.
 */
#pragma once

#include <gj/types.h>

#define GJ_SMP_MAX_APICS 64

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
 * Safe on UP: always reports at least BSP.
 */
void smp_init_from_mb2(u32 paMb2Info);

/**
 * MADT discovery from a known RSDP physical address (UEFI config table).
 * Falls back to BIOS scan if u64Rsdp is 0 or invalid.
 */
void smp_init_from_rsdp(u64 u64Rsdp);

const struct gj_smp_info *smp_info(void);
u32 smp_cpu_count_detected(void);
u32 smp_cpu_count_online(void);

/**
 * Soft bring-up ceiling for AP slots (static percpu + park-stack pools).
 * Always >= 1 (BSP). Full GJ_MAX_CPUS requires scaled stacks/idle arrays.
 */
u32 smp_bringup_cap(void);

/** Soft: 1 if u32CpuSlot is within published online inventory (incl. BSP). */
int smp_cpu_online(u32 u32CpuSlot);

/** Bring up APs (INIT-SIPI + trampoline). */
void smp_start_aps(void);

/**
 * Run pfn(pArg) once on AP slot u32CpuSlot (1..online-1).
 * BSP waits until done (or timeout). Returns 0 on success, -1 if no AP / busy.
 * Posts work into the AP schedule path (affinity thread or one-shot slot).
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
 */
void smp_bringup_soft_log(void);
