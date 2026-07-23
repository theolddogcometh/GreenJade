/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7862: nl_langinfo D_T_FMT item id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nl_d_t_fmt_id_7862(void);
 *     - Return soft D_T_FMT nl_item constant (2).
 *   uint32_t __gj_nl_d_t_fmt_id_7862  (alias)
 *   __libcgj_batch7862_marker = "libcgj-batch7862"
 *
 * Exclusive continuum CREATE-ONLY (7861-7870: nl_langinfo item stubs —
 * codeset_id, d_t_fmt_id, radixchar_id, is_codeset, is_d_t_fmt,
 * is_radixchar, item_ok, item_pack, item_errorish, batch_id_7870).
 * Unique gj_nl_d_t_fmt_id_7862 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7862_marker[] = "libcgj-batch7862";

/* Soft nl_item catalog: D_T_FMT. */
#define B7862_NL_D_T_FMT ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7862_nl_d_t_fmt_id(void)
{
	return B7862_NL_D_T_FMT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nl_d_t_fmt_id_7862 - soft D_T_FMT nl_item constant.
 *
 * Always returns 2 (soft D_T_FMT catalog id). Catalog id only; does not
 * call nl_langinfo(3). No parent wires.
 */
uint32_t
gj_nl_d_t_fmt_id_7862(void)
{
	(void)NULL;
	return b7862_nl_d_t_fmt_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nl_d_t_fmt_id_7862(void)
    __attribute__((alias("gj_nl_d_t_fmt_id_7862")));
