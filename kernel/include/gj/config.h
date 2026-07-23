/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 */
#pragma once

#define GJ_PAGE_SIZE     4096u
#define GJ_KERNEL_STACK  (16u * 1024u)
#define GJ_SERIAL_PORT   0x3F8u
/*
 * Product design ceiling for multi-CPU workstations (docs / arrays that scale).
 * Static per-CPU BSS uses GJ_CPU_STATIC_MAX to keep kernel below user maps.
 */
#define GJ_MAX_CPUS        256u
#define GJ_CPU_STATIC_MAX  16u
/* Design headroom for multi-TiB machines (see PMM rewrite). */
#define GJ_PMM_MAX_PHYS_TIB 4u
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
#define GJ_DEBUG         1
#define GJ_KLOG_LEVEL    3
