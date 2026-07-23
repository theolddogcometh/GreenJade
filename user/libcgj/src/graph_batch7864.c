/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7864: nl_langinfo CODESET item match stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nl_is_codeset_u_7864(uint32_t item);
 *     - Return 1 if item equals soft CODESET (1), else 0.
 *   uint32_t __gj_nl_is_codeset_u_7864  (alias)
 *   __libcgj_batch7864_marker = "libcgj-batch7864"
 *
 * Exclusive continuum CREATE-ONLY (7861-7870: nl_langinfo item stubs —
 * codeset_id, d_t_fmt_id, radixchar_id, is_codeset, is_d_t_fmt,
 * is_radixchar, item_ok, item_pack, item_errorish, batch_id_7870).
 * Unique gj_nl_is_codeset_u_7864 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7864_marker[] = "libcgj-batch7864";

/* Soft nl_item catalog: CODESET. */
#define B7864_NL_CODESET ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7864_nl_is_codeset(uint32_t u32Item)
{
	return (u32Item == B7864_NL_CODESET) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nl_is_codeset_u_7864 - 1 if item is soft CODESET.
 *
 * item: soft nl_item catalog value
 *
 * Returns 1 when item equals 1 (CODESET), else 0. Soft equality only;
 * does not call nl_langinfo(3). No parent wires.
 */
uint32_t
gj_nl_is_codeset_u_7864(uint32_t u32Item)
{
	(void)NULL;
	return b7864_nl_is_codeset(u32Item);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nl_is_codeset_u_7864(uint32_t u32Item)
    __attribute__((alias("gj_nl_is_codeset_u_7864")));
