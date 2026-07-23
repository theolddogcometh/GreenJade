/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * DESIGN_SPEC_COMPLETE G-MAP-* · X86_64_INTEL_PLATFORM P-MEM-6
 * Supervisor-only kernel maps, SMEP, SMAP (with user_copy STAC/CLAC).
 */
#pragma once

/** Set CR4.SMEP — ring-3 cannot execute supervisor pages. */
void cpu_enable_smep(void);

/**
 * Set CR4.SMAP + CLAC; notifies user_copy to bracket accesses with STAC/CLAC.
 */
void cpu_enable_smap(void);

/**
 * Clear U on identity maps outside [GJ_USER_VA_BASE, GJ_USER_VA_END).
 * Call on kernel CR3 before enabling SMEP for product ring-3.
 */
void vmm_harden_kernel_maps(void);
