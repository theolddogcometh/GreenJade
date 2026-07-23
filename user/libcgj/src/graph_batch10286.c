/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10286: bar3 GPU checklist soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_bar3_gpu_check_u_10286(void);
 *     - Soft product GPU checklist lamp for bar #3 (always 0 =
 *       bar3 still open). Compile-time product status only.
 *   uint32_t __gj_bar3_gpu_check_u_10286  (alias)
 *   __libcgj_batch10286_marker = "libcgj-batch10286"
 *
 * Bar3 checklist soft product stubs CREATE-ONLY (10281-10290). Unique
 * gj_bar3_gpu_check_u_10286 surface only; no multi-def. Distinct from
 * other GPU / bar3 product lamps. Bar3 remains open at this wave.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10286_marker[] = "libcgj-batch10286";

/* Bar3 GPU check soft status (0 = still open). */
#define B10286_GPU_CHECK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10286_check(void)
{
	return B10286_GPU_CHECK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bar3_gpu_check_u_10286 - soft bar3 GPU checklist lamp.
 *
 * Always returns 0 (bar #3 still open). Soft pure-data only; does not
 * probe GPU hardware. Does not call libc. No parent wires.
 */
uint32_t
gj_bar3_gpu_check_u_10286(void)
{
	(void)NULL;
	return b10286_check();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_bar3_gpu_check_u_10286(void)
    __attribute__((alias("gj_bar3_gpu_check_u_10286")));
