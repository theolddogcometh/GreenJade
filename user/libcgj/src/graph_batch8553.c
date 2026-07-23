/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8553: regex star quantifier catalog id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_re_star_id_8553(void);
 *     - Return soft catalog id for the '*' zero-or-more quantifier
 *       (0x2A).
 *   uint32_t __gj_re_star_id_8553  (alias)
 *   __libcgj_batch8553_marker = "libcgj-batch8553"
 *
 * Exclusive continuum CREATE-ONLY (8551-8560: regex/class id stubs —
 * empty_ok, dot_id, star_id, plus_id, qmark_id, anchor_start_id,
 * anchor_end_id, class_ok, escape_ok, batch_id_8560). Unique
 * gj_re_star_id_8553 surface only; no multi-def. No parent wires.
 * No real regex engine. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8553_marker[] = "libcgj-batch8553";

/* Soft '*' quantifier catalog id (ASCII asterisk / zero-or-more). */
#define B8553_RE_STAR_ID ((uint32_t)0x2Au)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8553_star_id(void)
{
	return B8553_RE_STAR_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_re_star_id_8553 - soft catalog id for regex '*' quantifier.
 *
 * Always returns 0x2A. Catalog constant only; does not quantify or
 * match. No parent wires.
 */
uint32_t
gj_re_star_id_8553(void)
{
	(void)NULL;
	return b8553_star_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_re_star_id_8553(void)
    __attribute__((alias("gj_re_star_id_8553")));
