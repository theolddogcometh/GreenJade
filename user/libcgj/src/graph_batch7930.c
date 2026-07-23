/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7930: exclusive continuum batch identity for
 * the scanf conversion stubs wave (7921-7930).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7930(void);
 *     - Returns the compile-time graph batch number for this TU (7930).
 *   uint32_t gj_scanf_conv_continuum_ready_7930(void);
 *     - Returns 1 (scanf conversion continuum wave 7921-7930 ready).
 *   uint32_t __gj_batch_id_7930  (alias)
 *   uint32_t __gj_scanf_conv_continuum_ready_7930  (alias)
 *   __libcgj_batch7930_marker = "libcgj-batch7930"
 *
 * Exclusive continuum CREATE-ONLY (7921-7930: scanf conversion stubs —
 * digit_value, is_digit, is_space, sign_of, base_norm, width_clamp,
 * suppress_id, length_mod_id, accum_digit, batch_id_7930).
 * Unique gj_*_7930 surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / prior batch_id_* symbols. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7930_marker[] = "libcgj-batch7930";

#define B7930_ID     7930u
#define B7930_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7930_id(void)
{
	return B7930_ID;
}

static uint32_t
b7930_ready(void)
{
	return B7930_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7930 - report this TU's graph batch number.
 *
 * Always returns 7930.
 */
uint32_t
gj_batch_id_7930(void)
{
	(void)NULL;
	return b7930_id();
}

/*
 * gj_scanf_conv_continuum_ready_7930 - soft wave-ready lamp.
 *
 * Always returns 1 (scanf conversion continuum 7921-7930 complete).
 * Soft compile-time product tag; not a live scanf smoke probe.
 */
uint32_t
gj_scanf_conv_continuum_ready_7930(void)
{
	return b7930_ready();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_7930(void)
    __attribute__((alias("gj_batch_id_7930")));

uint32_t __gj_scanf_conv_continuum_ready_7930(void)
    __attribute__((alias("gj_scanf_conv_continuum_ready_7930")));
