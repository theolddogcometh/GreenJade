/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11386: bar3 gpu checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_gpu_check_u_11386(void);
 *     - Returns 0 (soft stub: gpu checklist slot is soft/unprobed;
 *       not a runtime GPU/driver probe).
 *   uint32_t __gj_bar3_gpu_check_u_11386  (alias)
 *   __libcgj_batch11386_marker = "libcgj-batch11386"
 *
 * Exclusive continuum CREATE-ONLY (11381-11390: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_gpu_check_u_11386 surface only; no
 * multi-def. Distinct from gj_bar3_gpu_check_u_11186 and sibling
 * check_u soft stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11386_marker[] = "libcgj-batch11386";

/* Soft-stub gpu checklist value (unprobed). */
#define B11386_GPU_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11386_gpu_check(void)
{
	return B11386_GPU_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_gpu_check_u_11386 - soft bar3 gpu checklist stub.
 *
 * Always returns 0 (soft/unprobed gpu slot). Does not call libc or
 * probe GPUs. No parent wires.
 */
uint32_t
gj_bar3_gpu_check_u_11386(void)
{
	(void)NULL;
	return b11386_gpu_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_gpu_check_u_11386(void)
    __attribute__((alias("gj_bar3_gpu_check_u_11386")));
