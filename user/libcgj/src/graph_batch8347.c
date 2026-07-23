/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8347: CLOCK_MONOTONIC identity tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clock_mono_id_8347(void);
 *     - Returns 1: soft CLOCK_MONOTONIC catalog id for this exclusive
 *       time/clock continuum (POSIX CLOCK_MONOTONIC == 1).
 *   uint32_t __gj_clock_mono_id_8347  (alias)
 *   __libcgj_batch8347_marker = "libcgj-batch8347"
 *
 * Exclusive continuum CREATE-ONLY (8341-8350: time/clock unit stubs).
 * Distinct from gj_tfd_clock_monotonic_p_6633 (batch6633 predicate).
 * Unique gj_clock_mono_id_8347 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8347_marker[] = "libcgj-batch8347";

/* Soft CLOCK_MONOTONIC identity (POSIX clockid value 1). */
#define B8347_CLOCK_MONO_ID  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8347_mono_id(void)
{
	return B8347_CLOCK_MONO_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clock_mono_id_8347 - CLOCK_MONOTONIC identity tag.
 *
 * Always returns 1. Soft pure data product tag for the continuum;
 * does not call clock_gettime. No parent wires.
 */
uint32_t
gj_clock_mono_id_8347(void)
{
	(void)NULL;
	return b8347_mono_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clock_mono_id_8347(void)
    __attribute__((alias("gj_clock_mono_id_8347")));
