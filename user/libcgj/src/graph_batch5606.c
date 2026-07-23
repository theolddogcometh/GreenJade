/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5606: GPU clock min/max (core constants).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_clock_min_mhz_5606(void);
 *     - Returns 200 (soft GPU clock floor in MHz for Deck APU path).
 *   uint32_t gj_gpu_clock_max_mhz_5606(void);
 *     - Returns 1600 (soft GPU clock ceiling in MHz).
 *   uint32_t gj_gpu_clock_default_mhz_5606(void);
 *     - Returns 1000 (soft default GPU clock target in MHz).
 *   uint32_t gj_gpu_clock_score_5606(void);
 *     - Returns 5606 (wave-tagged GPU clock score).
 *   uint32_t __gj_gpu_clock_min_mhz_5606  (alias)
 *   uint32_t __gj_gpu_clock_max_mhz_5606  (alias)
 *   uint32_t __gj_gpu_clock_default_mhz_5606  (alias)
 *   uint32_t __gj_gpu_clock_score_5606  (alias)
 *   __libcgj_batch5606_marker = "libcgj-batch5606"
 *
 * Exclusive continuum CREATE-ONLY (5601-5610). Soft GPU clock min/max
 * constants for Deck APU product path; no DRM/sysfs probe. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5606_marker[] = "libcgj-batch5606";

#define B5606_MIN_MHZ      200u
#define B5606_MAX_MHZ      1600u
#define B5606_DEFAULT_MHZ  1000u
#define B5606_SCORE        5606u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5606_min(void)
{
	return B5606_MIN_MHZ;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_clock_min_mhz_5606 - soft GPU clock floor in MHz.
 *
 * Always returns 200. Product-path constant only.
 */
uint32_t
gj_gpu_clock_min_mhz_5606(void)
{
	(void)NULL;
	return b5606_min();
}

/*
 * gj_gpu_clock_max_mhz_5606 - soft GPU clock ceiling in MHz.
 *
 * Always returns 1600.
 */
uint32_t
gj_gpu_clock_max_mhz_5606(void)
{
	return B5606_MAX_MHZ;
}

/*
 * gj_gpu_clock_default_mhz_5606 - soft default GPU clock target.
 *
 * Always returns 1000.
 */
uint32_t
gj_gpu_clock_default_mhz_5606(void)
{
	return B5606_DEFAULT_MHZ;
}

/*
 * gj_gpu_clock_score_5606 - wave-tagged GPU clock score.
 *
 * Always returns 5606.
 */
uint32_t
gj_gpu_clock_score_5606(void)
{
	return B5606_SCORE;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_gpu_clock_min_mhz_5606(void)
    __attribute__((alias("gj_gpu_clock_min_mhz_5606")));

uint32_t __gj_gpu_clock_max_mhz_5606(void)
    __attribute__((alias("gj_gpu_clock_max_mhz_5606")));

uint32_t __gj_gpu_clock_default_mhz_5606(void)
    __attribute__((alias("gj_gpu_clock_default_mhz_5606")));

uint32_t __gj_gpu_clock_score_5606(void)
    __attribute__((alias("gj_gpu_clock_score_5606")));
