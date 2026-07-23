/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8152: strtok colon separator constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtok_sep_colon_8152(void);
 *     - Return soft strtok colon separator constant (':').
 *   uint32_t __gj_strtok_sep_colon_8152  (alias)
 *   __libcgj_batch8152_marker = "libcgj-batch8152"
 *
 * Exclusive continuum CREATE-ONLY (8151-8160: strtok sep stubs —
 * sep_comma, sep_colon, sep_space, sep_tab, sep_nl, is_sep, is_ws,
 * is_csv, sep_ok, batch_id_8160). Unique gj_strtok_sep_colon_8152
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8152_marker[] = "libcgj-batch8152";

/* Soft strtok path/list-style field separator: colon. */
#define B8152_SEP_COLON  ((uint32_t)':')

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8152_sep_colon(void)
{
	return B8152_SEP_COLON;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtok_sep_colon_8152 - soft strtok colon separator constant.
 *
 * Always returns ':' (0x3a). Catalog constant only; does not tokenize.
 * No parent wires.
 */
uint32_t
gj_strtok_sep_colon_8152(void)
{
	(void)NULL;
	return b8152_sep_colon();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtok_sep_colon_8152(void)
    __attribute__((alias("gj_strtok_sep_colon_8152")));
