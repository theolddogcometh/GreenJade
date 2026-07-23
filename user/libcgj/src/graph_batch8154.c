/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8154: strtok tab separator constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtok_sep_tab_8154(void);
 *     - Return soft strtok tab separator constant ('\\t').
 *   uint32_t __gj_strtok_sep_tab_8154  (alias)
 *   __libcgj_batch8154_marker = "libcgj-batch8154"
 *
 * Exclusive continuum CREATE-ONLY (8151-8160: strtok sep stubs —
 * sep_comma, sep_colon, sep_space, sep_tab, sep_nl, is_sep, is_ws,
 * is_csv, sep_ok, batch_id_8160). Unique gj_strtok_sep_tab_8154
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8154_marker[] = "libcgj-batch8154";

/* Soft strtok whitespace field separator: horizontal tab. */
#define B8154_SEP_TAB  ((uint32_t)'\t')

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8154_sep_tab(void)
{
	return B8154_SEP_TAB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtok_sep_tab_8154 - soft strtok tab separator constant.
 *
 * Always returns '\\t' (0x09). Catalog constant only; does not tokenize.
 * No parent wires.
 */
uint32_t
gj_strtok_sep_tab_8154(void)
{
	(void)NULL;
	return b8154_sep_tab();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtok_sep_tab_8154(void)
    __attribute__((alias("gj_strtok_sep_tab_8154")));
