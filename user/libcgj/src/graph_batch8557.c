/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8557: regex end-anchor catalog id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_re_anchor_end_id_8557(void);
 *     - Return soft catalog id for the '$' end-of-string / end-of-line
 *       anchor (0x24).
 *   uint32_t __gj_re_anchor_end_id_8557  (alias)
 *   __libcgj_batch8557_marker = "libcgj-batch8557"
 *
 * Exclusive continuum CREATE-ONLY (8551-8560: regex/class id stubs —
 * empty_ok, dot_id, star_id, plus_id, qmark_id, anchor_start_id,
 * anchor_end_id, class_ok, escape_ok, batch_id_8560). Unique
 * gj_re_anchor_end_id_8557 surface only; no multi-def. No parent
 * wires. No real regex engine. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8557_marker[] = "libcgj-batch8557";

/* Soft '$' end-anchor catalog id (ASCII dollar sign). */
#define B8557_RE_ANCHOR_END_ID ((uint32_t)0x24u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8557_anchor_end_id(void)
{
	return B8557_RE_ANCHOR_END_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_re_anchor_end_id_8557 - soft catalog id for regex '$' anchor.
 *
 * Always returns 0x24. Catalog constant only; does not anchor or match.
 * No parent wires.
 */
uint32_t
gj_re_anchor_end_id_8557(void)
{
	(void)NULL;
	return b8557_anchor_end_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_re_anchor_end_id_8557(void)
    __attribute__((alias("gj_re_anchor_end_id_8557")));
