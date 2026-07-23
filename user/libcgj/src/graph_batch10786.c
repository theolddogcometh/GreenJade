/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10786: bar3 gpu checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_gpu_check_u_10786(void);
 *     - Returns 0 (soft stub: gpu checklist slot is soft/unprobed;
 *       not a runtime GPU capability probe).
 *   uint32_t __gj_bar3_gpu_check_u_10786  (alias)
 *   __libcgj_batch10786_marker = "libcgj-batch10786"
 *
 * Exclusive continuum CREATE-ONLY (10781-10790: bar3 checklist soft
 * stubs — all→0). Unique gj_bar3_gpu_check_u_10786 surface only; no
 * multi-def. Distinct from gj_bar3_gpu_check_u_10586,
 * gj_bar3_gpu_check_u_10386, and sibling check_u soft stubs in this
 * wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10786_marker[] = "libcgj-batch10786";

/* Soft-stub gpu checklist value (unprobed). */
#define B10786_GPU_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10786_gpu_check(void)
{
	return B10786_GPU_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_gpu_check_u_10786 - soft bar3 gpu checklist stub.
 *
 * Always returns 0 (soft/unprobed gpu slot). Does not call libc or
 * probe GPU capabilities. No parent wires.
 */
uint32_t
gj_bar3_gpu_check_u_10786(void)
{
	(void)NULL;
	return b10786_gpu_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_gpu_check_u_10786(void)
    __attribute__((alias("gj_bar3_gpu_check_u_10786")));
