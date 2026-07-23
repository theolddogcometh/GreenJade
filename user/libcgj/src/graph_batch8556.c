/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8556: regex start-anchor catalog id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_re_anchor_start_id_8556(void);
 *     - Return soft catalog id for the '^' start-of-string / start-of-
 *       line anchor (0x5E).
 *   uint32_t __gj_re_anchor_start_id_8556  (alias)
 *   __libcgj_batch8556_marker = "libcgj-batch8556"
 *
 * Exclusive continuum CREATE-ONLY (8551-8560: regex/class id stubs —
 * empty_ok, dot_id, star_id, plus_id, qmark_id, anchor_start_id,
 * anchor_end_id, class_ok, escape_ok, batch_id_8560). Unique
 * gj_re_anchor_start_id_8556 surface only; no multi-def. No parent
 * wires. No real regex engine. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8556_marker[] = "libcgj-batch8556";

/* Soft '^' start-anchor catalog id (ASCII circumflex). */
#define B8556_RE_ANCHOR_START_ID ((uint32_t)0x5Eu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8556_anchor_start_id(void)
{
	return B8556_RE_ANCHOR_START_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_re_anchor_start_id_8556 - soft catalog id for regex '^' anchor.
 *
 * Always returns 0x5E. Catalog constant only; does not anchor or match.
 * No parent wires.
 */
uint32_t
gj_re_anchor_start_id_8556(void)
{
	(void)NULL;
	return b8556_anchor_start_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_re_anchor_start_id_8556(void)
    __attribute__((alias("gj_re_anchor_start_id_8556")));
