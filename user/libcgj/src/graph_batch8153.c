/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8153: strtok space separator constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_strtok_sep_space_8153(void);
 *     - Return soft strtok space separator constant (' ').
 *   uint32_t __gj_strtok_sep_space_8153  (alias)
 *   __libcgj_batch8153_marker = "libcgj-batch8153"
 *
 * Exclusive continuum CREATE-ONLY (8151-8160: strtok sep stubs —
 * sep_comma, sep_colon, sep_space, sep_tab, sep_nl, is_sep, is_ws,
 * is_csv, sep_ok, batch_id_8160). Unique gj_strtok_sep_space_8153
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8153_marker[] = "libcgj-batch8153";

/* Soft strtok whitespace field separator: space. */
#define B8153_SEP_SPACE  ((uint32_t)' ')

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8153_sep_space(void)
{
	return B8153_SEP_SPACE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_strtok_sep_space_8153 - soft strtok space separator constant.
 *
 * Always returns ' ' (0x20). Catalog constant only; does not tokenize.
 * No parent wires.
 */
uint32_t
gj_strtok_sep_space_8153(void)
{
	(void)NULL;
	return b8153_sep_space();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_strtok_sep_space_8153(void)
    __attribute__((alias("gj_strtok_sep_space_8153")));
