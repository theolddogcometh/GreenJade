/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7861: nl_langinfo CODESET item id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nl_codeset_id_7861(void);
 *     - Return soft CODESET nl_item constant (1).
 *   uint32_t __gj_nl_codeset_id_7861  (alias)
 *   __libcgj_batch7861_marker = "libcgj-batch7861"
 *
 * Exclusive continuum CREATE-ONLY (7861-7870: nl_langinfo item stubs —
 * codeset_id, d_t_fmt_id, radixchar_id, is_codeset, is_d_t_fmt,
 * is_radixchar, item_ok, item_pack, item_errorish, batch_id_7870).
 * Unique gj_nl_codeset_id_7861 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7861_marker[] = "libcgj-batch7861";

/* Soft nl_item catalog: CODESET. */
#define B7861_NL_CODESET ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7861_nl_codeset_id(void)
{
	return B7861_NL_CODESET;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nl_codeset_id_7861 - soft CODESET nl_item constant.
 *
 * Always returns 1 (soft CODESET catalog id). Catalog id only; does not
 * call nl_langinfo(3). No parent wires.
 */
uint32_t
gj_nl_codeset_id_7861(void)
{
	(void)NULL;
	return b7861_nl_codeset_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nl_codeset_id_7861(void)
    __attribute__((alias("gj_nl_codeset_id_7861")));
