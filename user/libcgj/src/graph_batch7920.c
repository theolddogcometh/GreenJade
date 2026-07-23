/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7920: printf format flag stubs continuum lamp +
 * exclusive batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_printf_fmt_flag_continuum_ok_7920(void);
 *     - Returns 1 (soft lamp: 7911-7920 printf format flag stubs
 *       continuum complete / ready).
 *   uint32_t gj_batch_id_7920(void);
 *     - Returns the compile-time graph batch number for this TU (7920).
 *   uint32_t __gj_printf_fmt_flag_continuum_ok_7920  (alias)
 *   uint32_t __gj_batch_id_7920  (alias)
 *   __libcgj_batch7920_marker = "libcgj-batch7920"
 *
 * Exclusive continuum CREATE-ONLY (7911-7920: printf format flag stubs —
 * left_bit, plus_bit, space_bit, alt_bit, zero_bit, is_flag_char,
 * flag_from_char, resolve_zero, space_vs_plus, continuum + batch_id_7920).
 * Unique surfaces only; no multi-def. Does NOT redefine bare
 * gj_batch_id. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7920_marker[] = "libcgj-batch7920";

#define B7920_CONTINUUM_OK  1u
#define B7920_BATCH_ID      7920u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7920_continuum(void)
{
	return B7920_CONTINUUM_OK;
}

static uint32_t
b7920_id(void)
{
	return B7920_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_printf_fmt_flag_continuum_ok_7920 - continuum-ready tag for 7911-7920.
 *
 * Always returns 1. Soft pure-data product tag that the printf format
 * flag stubs exclusive wave is present. Does not call libc.
 */
uint32_t
gj_printf_fmt_flag_continuum_ok_7920(void)
{
	(void)NULL;
	return b7920_continuum();
}

/*
 * gj_batch_id_7920 - report this TU's graph batch number.
 *
 * Always returns 7920.
 */
uint32_t
gj_batch_id_7920(void)
{
	return b7920_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_printf_fmt_flag_continuum_ok_7920(void)
    __attribute__((alias("gj_printf_fmt_flag_continuum_ok_7920")));

uint32_t __gj_batch_id_7920(void)
    __attribute__((alias("gj_batch_id_7920")));
