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
