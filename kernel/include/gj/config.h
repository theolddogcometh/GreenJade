/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Compile-time product knobs (IMPLEMENTATION §15 / P-MEM / P-SMP).
 * Pure C11 freestanding - macros only (no runtime state).
 *
 * greppable: GJ_CONFIG_PRODUCT GJ_PAGE_SIZE GJ_MAX_CPUS GJ_CPU_STATIC_MAX
 * greppable: GJ_HHDM_BASE GJ_DEVICE_MMIO GJ_SPIN_CONFIG GJ_CACHELINE
 *
 * Assurance lite (docs/ASSURANCE_LITE.md - process honesty, not DO-178C):
 *   GJ_IMAGE_VERSION is the deliverable object-code identity (test what you fly).
 *   Soft!=product · G-AC-1 · Dual DoD open until host USB path (A) and
 *   interactive SSH login (B) on stamped media. L3 ARP/ping/banner != B close.
 * greppable: assurance: lite | test what you fly | Soft!=product | G-AC-1
 *
 * Geometry / SMP / klog knobs live here. Continuum / wave deepen is leftover
 * (not product). Soft!=product. G-AC-1.
 */
#pragma once

/* ------------------------------------------------------------------ */
/* Image identity (STATUS static title + boot log)                     */
/* ------------------------------------------------------------------ */

/*
 * Bump when shipping a new flashable image so operators can confirm the
 * DUT is running the expected build (STATUS title: "STATUS (static) v...").
 *
 * Semver product series (not date stamps):
 *   0.1.N  — foundation + Dual DoD eng (M0 OK; NIC/USB digs). N = fly cut.
 *   0.2.0  — reserved. Do not ship.
 * Panel shows v${GJ_IMAGE_VERSION} e.g. STATUS (static) v0.1.184
 *
 * Assurance (C1 Dual DoD): host probes must target this stamp after flash
 * (L3 object code). Tree residual after pack is not media until re-image.
 * greppable: GJ_IMAGE_VERSION | test what you fly | v0.1. | 0.2.0
 */
#ifndef GJ_IMAGE_VERSION
/* Fly cut: A park RS-off; B exec TX drain after 183 Sending command (login OPEN). */
#define GJ_IMAGE_VERSION  "0.1.184"
#endif

/*
 * Freestanding xHCI MSC BOT probe (lab G752 a12f).
 * Default 0: SKIP freestanding USB class driver (not product). Soft USB /
 * Linux-shaped userspace UDX path is product direction (G-AC-1).
 * Set to 1 only for targeted freestanding MSC residual experiments.
 * greppable: GJ_XHCI_MSC_PROBE Soft!=product freestanding USB SKIP
 */
#ifndef GJ_XHCI_MSC_PROBE
#define GJ_XHCI_MSC_PROBE  0
#endif

/*
 * Freestanding in-kernel rtl8168 probe (G752 10ec:8168).
 * Default 0: SKIP freestanding NIC class driver - same product policy as
 * freestanding USB (GJ_XHCI_MSC_PROBE=0). Operator 2026-08: stop freestanding
 * rtl rabbit hole; product net = virtio T0 + Linux-shaped userspace UDX/DDI
 * over hot+cold ABI (not in-kernel r8169.ko; G-AC-1). Soft r8169 load/ksym
 * eng residual remains separate (GJ_SOFT_R8169_LOAD).
 * Set to 1 only for deliberate freestanding rtl residual experiments.
 * greppable: GJ_RTL8168_PROBE Soft!=product freestanding rtl SKIP
 */
#ifndef GJ_RTL8168_PROBE
#define GJ_RTL8168_PROBE  0
#endif

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

/*
 * Soft MMIO ownership handoff experiments (lab only; rarely used).
 * Default 0. Freestanding rtl is SKIP by default (GJ_RTL8168_PROBE=0);
 * product path is userspace UDX, not freestanding->.ko handoff.
 * Soft!=product; G-AC-1; see docs/R8169_MMIO_HANDOFF.md.
 * greppable: GJ_SOFT_R8169_MMIO_HANDOFF
 */
#ifndef GJ_SOFT_R8169_MMIO_HANDOFF
#define GJ_SOFT_R8169_MMIO_HANDOFF  0
#endif

/*
 * Soft boot load of embedded/media r8169.ko (lab/eng only).
 * Default 0: abandoned path not linked (G-AC-1). Soft load residual only if set.
 * Not product wire owner. Product NIC = userspace UDX + ABI.
 * Soft!=product. greppable: GJ_SOFT_R8169_LOAD
 * See docs/R8169_MMIO_HANDOFF.md · docs/LINUX_MODULE_PATH.md · docs/ABI_FIRST_PIVOT.md.
 */
#ifndef GJ_SOFT_R8169_LOAD
#define GJ_SOFT_R8169_LOAD  0
#endif

/*
 * Phase-3 Option B: call .ko ndo_open after sole-owner (RISKY lab only).
 * Default 0: phase-3 try_open does soft open only (carrier/queue; no .ko).
 * Requires GJ_SOFT_R8169_MMIO_HANDOFF=1 as well. Soft!=product; G-AC-1.
 * greppable: GJ_SOFT_R8169_KO_NDO_OPEN
 * See docs/R8169_MMIO_HANDOFF.md phase 3 Option B.
 */
#ifndef GJ_SOFT_R8169_KO_NDO_OPEN
#define GJ_SOFT_R8169_KO_NDO_OPEN  0
#endif

/*
 * Soft-originated L2 TX smoke at bridge enable (lab only).
 * Default 0: API + ksym only - never auto-inject frames on live NIC.
 * Set 1 only for deliberate reverse-path smoke (builds a soft skb and
 * calls linux_netdev_soft_l2_tx_from_skb once). Soft!=product; G752 has
 * real wire - do not ship default 1.
 * greppable: GJ_SOFT_L2_TX_SMOKE
 */
#ifndef GJ_SOFT_L2_TX_SMOKE
#define GJ_SOFT_L2_TX_SMOKE  0
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
_Static_assert(GJ_SOFT_R8169_MMIO_HANDOFF == 0 ||
               GJ_SOFT_R8169_MMIO_HANDOFF == 1,
               "GJ_SOFT_R8169_MMIO_HANDOFF must be 0 or 1");
_Static_assert(GJ_SOFT_R8169_KO_NDO_OPEN == 0 ||
               GJ_SOFT_R8169_KO_NDO_OPEN == 1,
               "GJ_SOFT_R8169_KO_NDO_OPEN must be 0 or 1");
/* Option B requires handoff gate; both off or both on is fine; B alone is not. */
_Static_assert(GJ_SOFT_R8169_KO_NDO_OPEN == 0 ||
               GJ_SOFT_R8169_MMIO_HANDOFF == 1,
               "GJ_SOFT_R8169_KO_NDO_OPEN=1 requires GJ_SOFT_R8169_MMIO_HANDOFF=1");
_Static_assert(GJ_SOFT_L2_TX_SMOKE == 0 || GJ_SOFT_L2_TX_SMOKE == 1,
               "GJ_SOFT_L2_TX_SMOKE must be 0 or 1");
