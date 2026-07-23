/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8151: strtok comma separator constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtok_sep_comma_8151(void);
 *     - Return soft strtok comma separator constant (',').
 *   uint32_t __gj_strtok_sep_comma_8151  (alias)
 *   __libcgj_batch8151_marker = "libcgj-batch8151"
 *
 * Exclusive continuum CREATE-ONLY (8151-8160: strtok sep stubs —
 * sep_comma, sep_colon, sep_space, sep_tab, sep_nl, is_sep, is_ws,
 * is_csv, sep_ok, batch_id_8160). Unique gj_strtok_sep_comma_8151
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8151_marker[] = "libcgj-batch8151";

/* Soft strtok CSV-style field separator: comma. */
#define B8151_SEP_COMMA  ((uint32_t)',')

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8151_sep_comma(void)
{
	return B8151_SEP_COMMA;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtok_sep_comma_8151 - soft strtok comma separator constant.
 *
 * Always returns ',' (0x2c). Catalog constant only; does not tokenize.
 * No parent wires.
 */
uint32_t
gj_strtok_sep_comma_8151(void)
{
	(void)NULL;
	return b8151_sep_comma();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtok_sep_comma_8151(void)
    __attribute__((alias("gj_strtok_sep_comma_8151")));
