/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8155: strtok newline separator constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtok_sep_nl_8155(void);
 *     - Return soft strtok newline separator constant ('\\n').
 *   uint32_t __gj_strtok_sep_nl_8155  (alias)
 *   __libcgj_batch8155_marker = "libcgj-batch8155"
 *
 * Exclusive continuum CREATE-ONLY (8151-8160: strtok sep stubs —
 * sep_comma, sep_colon, sep_space, sep_tab, sep_nl, is_sep, is_ws,
 * is_csv, sep_ok, batch_id_8160). Unique gj_strtok_sep_nl_8155
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8155_marker[] = "libcgj-batch8155";

/* Soft strtok line-break field separator: newline. */
#define B8155_SEP_NL  ((uint32_t)'\n')

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8155_sep_nl(void)
{
	return B8155_SEP_NL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtok_sep_nl_8155 - soft strtok newline separator constant.
 *
 * Always returns '\\n' (0x0a). Catalog constant only; does not tokenize.
 * No parent wires.
 */
uint32_t
gj_strtok_sep_nl_8155(void)
{
	(void)NULL;
	return b8155_sep_nl();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtok_sep_nl_8155(void)
    __attribute__((alias("gj_strtok_sep_nl_8155")));
