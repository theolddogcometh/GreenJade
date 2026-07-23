/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Compile-time product knobs (IMPLEMENTATION §15 / P-MEM / P-SMP).
 * Pure C11 freestanding — macros only (no runtime state).
 *
 * greppable: GJ_CONFIG_PRODUCT GJ_PAGE_SIZE GJ_MAX_CPUS GJ_CPU_STATIC_MAX
 * greppable: GJ_HHDM_BASE GJ_DEVICE_MMIO GJ_SPIN_CONFIG GJ_CACHELINE
 *
 * Soft stamp (Wave 14 exclusive — header comment only):
 *   Compile-time geometry / SMP / klog knobs. Not a soft inventory helper
 *   (no runtime tallies). Does not light product lamps, close multi-server
 *   product, or claim Steam bar3. Continuum high-water toward 15600 is
 *   documented in companion scripts/docs (CREATE-ONLY soft graph only).
 * greppable: GJ_CONFIG_SOFT_STAMP_WAVE14
 */
#pragma once

/* ------------------------------------------------------------------ */
/* Memory geometry                                                     */
/* ------------------------------------------------------------------ */

/** Hardware / product page size (4 KiB; matches PMM core and wow64). */
#define GJ_PAGE_SIZE     4096u
#define GJ_PAGE_SHIFT    12u
#define GJ_PAGE_MASK     (GJ_PAGE_SIZE - 1u)

/** Kernel / IRQ thread stack floor (bytes) for early BSP paths. */
#define GJ_KERNEL_STACK  (16u * 1024u)

/** Early 16550 COM1 I/O port (x86_64 bring-up; ignored on aarch64). */
#define GJ_SERIAL_PORT   0x3F8u

/*
 * Product design ceiling for multi-CPU workstations (docs / arrays that scale).
 * Static per-CPU BSS uses GJ_CPU_STATIC_MAX to keep kernel below user maps;
 * slots [STATIC_MAX, GJ_MAX_CPUS) grow via PMM (cpu.c soft dyn pool).
 * P-SMP-6: ceiling must stay multi-CPU capable (not frozen at 1).
 */
#define GJ_MAX_CPUS        256u
#define GJ_CPU_STATIC_MAX  16u

/* Design headroom for multi-TiB machines (see PMM rewrite / P-MEM). */
#define GJ_PMM_MAX_PHYS_TIB   4u
/** Derived byte span for design soft probes (TiB << 40). */
#define GJ_PMM_MAX_PHYS_BYTES (GJ_PMM_MAX_PHYS_TIB * 1024ull * 1024ull * 1024ull * 1024ull)

/*
 * Higher-half direct map (P-MEM-5): phys 0 maps to this VA.
 * Canonical 4-level: PML4[256] covers 0xffff_8000_0000_0000.
 */
#define GJ_HHDM_BASE     0xffff800000000000ull

/*
 * Device MMIO window (UC): PA maps at GJ_DEVICE_MMIO_BASE + PA for the first
 * 512 GiB of physical MMIO. Avoids identity-map collisions with low kernel.
 */
#define GJ_DEVICE_MMIO_BASE 0xffff810000000000ull
#define GJ_DEVICE_MMIO_SPAN (512ull << 30)

/* ------------------------------------------------------------------ */
/* Cache / SMP soft knobs                                              */
/* ------------------------------------------------------------------ */

/*
 * False-sharing pad for hot shared data and aligned spinlocks.
 * x86_64 and common aarch64 cores use 64 B lines; override at -D if needed.
 * greppable: GJ_CACHELINE_SIZE
 */
#ifndef GJ_CACHELINE_SIZE
#define GJ_CACHELINE_SIZE 64u
#endif

/*
 * Spinlock contention: max cpu_relax() iterations in the inner read-spin
 * before re-attempting the CAS (ticket-free TAS path in gj/spinlock.h).
 * 0 = single relax per outer loop iteration (still correct).
 * greppable: GJ_SPIN_RELAX_ITERS
 */
#ifndef GJ_SPIN_RELAX_ITERS
#define GJ_SPIN_RELAX_ITERS 64u
#endif

/* ------------------------------------------------------------------ */
/* Debug / klog                                                        */
/* ------------------------------------------------------------------ */

/** 1 = verbose / debug-only product paths allowed; 0 = sealed product. */
#ifndef GJ_DEBUG
#define GJ_DEBUG         1
#endif

/*
 * Klog severity ladder. Messages with level > GJ_KLOG_LEVEL are suppressed
 * by callers that honor the gate (STYLE: GJ_DEBUG gates verbose logs).
 * greppable: GJ_KLOG_LEVEL
 */
#define GJ_KLOG_ERR    0
#define GJ_KLOG_WARN   1
#define GJ_KLOG_INFO   2
#define GJ_KLOG_DBG    3
#define GJ_KLOG_TRACE  4

#ifndef GJ_KLOG_LEVEL
#define GJ_KLOG_LEVEL  3
#endif

/* ------------------------------------------------------------------ */
/* Derived helpers (pure macros)                                       */
/* ------------------------------------------------------------------ */

/** Round byte count up to a whole page. */
#define GJ_BYTES_TO_PAGES(cb) \
    (((unsigned long long)(cb) + (unsigned long long)GJ_PAGE_SIZE - 1ull) \
     / (unsigned long long)GJ_PAGE_SIZE)

/** True if address/size is page-aligned (compile- or run-time expression). */
#define GJ_IS_PAGE_ALIGNED(x) \
    (((unsigned long long)(x) & (unsigned long long)GJ_PAGE_MASK) == 0ull)

/** HHDM VA for a physical address (caller must ensure PA is mapped). */
#define GJ_HHDM_VA(pa) ((unsigned long long)(pa) + GJ_HHDM_BASE)

/** Device-MMIO VA for a physical MMIO address within GJ_DEVICE_MMIO_SPAN. */
#define GJ_DEVICE_MMIO_VA(pa) ((unsigned long long)(pa) + GJ_DEVICE_MMIO_BASE)

/* ------------------------------------------------------------------ */
/* Compile-time integrity (freestanding C11)                           */
/* ------------------------------------------------------------------ */

_Static_assert(GJ_PAGE_SIZE == (1u << GJ_PAGE_SHIFT),
               "GJ_PAGE_SIZE must match GJ_PAGE_SHIFT");
_Static_assert((GJ_PAGE_SIZE & (GJ_PAGE_SIZE - 1u)) == 0u,
               "GJ_PAGE_SIZE must be power of two");
_Static_assert(GJ_PAGE_MASK == (GJ_PAGE_SIZE - 1u),
               "GJ_PAGE_MASK");
_Static_assert(GJ_CPU_STATIC_MAX >= 1u,
               "GJ_CPU_STATIC_MAX needs BSP slot");
_Static_assert(GJ_CPU_STATIC_MAX <= GJ_MAX_CPUS,
               "GJ_CPU_STATIC_MAX must not exceed GJ_MAX_CPUS");
_Static_assert(GJ_MAX_CPUS >= 2u,
               "P-SMP-6: product ceiling must not freeze at 1 CPU");
_Static_assert(GJ_CACHELINE_SIZE >= 16u &&
               (GJ_CACHELINE_SIZE & (GJ_CACHELINE_SIZE - 1u)) == 0u,
               "GJ_CACHELINE_SIZE must be power-of-two >= 16");
_Static_assert(GJ_PMM_MAX_PHYS_TIB >= 1u,
               "GJ_PMM_MAX_PHYS_TIB design headroom");
_Static_assert(GJ_KLOG_LEVEL <= GJ_KLOG_TRACE,
               "GJ_KLOG_LEVEL out of ladder");
