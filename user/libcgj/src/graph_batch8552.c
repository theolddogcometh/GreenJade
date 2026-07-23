/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8552: regex dot metachar catalog id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_re_dot_id_8552(void);
 *     - Return soft catalog id for the '.' any-byte metachar (0x2E).
 *   uint32_t __gj_re_dot_id_8552  (alias)
 *   __libcgj_batch8552_marker = "libcgj-batch8552"
 *
 * Exclusive continuum CREATE-ONLY (8551-8560: regex/class id stubs —
 * empty_ok, dot_id, star_id, plus_id, qmark_id, anchor_start_id,
 * anchor_end_id, class_ok, escape_ok, batch_id_8560). Unique
 * gj_re_dot_id_8552 surface only; no multi-def. No parent wires.
 * No real regex engine. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8552_marker[] = "libcgj-batch8552";

/* Soft '.' metachar catalog id (ASCII full stop / any-byte). */
#define B8552_RE_DOT_ID ((uint32_t)0x2Eu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8552_dot_id(void)
{
	return B8552_RE_DOT_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_re_dot_id_8552 - soft catalog id for regex '.' metachar.
 *
 * Always returns 0x2E. Catalog constant only; does not match input.
 * No parent wires.
 */
uint32_t
gj_re_dot_id_8552(void)
{
	(void)NULL;
	return b8552_dot_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_re_dot_id_8552(void)
    __attribute__((alias("gj_re_dot_id_8552")));
