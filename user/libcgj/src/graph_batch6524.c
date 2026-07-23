/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6524: Top50 title name length soft-ok gate.
 *
 * Surface (unique symbols):
 *   int gj_top50_title_name_len_ok_6524(size_t len);
 *     - Return 1 if len is a usable Top50 title name length in the
 *       closed range 1..256, else 0. Soft pure-data bound only; does
 *       not inspect title text.
 *   int __gj_top50_title_name_len_ok_6524  (alias)
 *   __libcgj_batch6524_marker = "libcgj-batch6524"
 *
 * Exclusive continuum CREATE-ONLY (6521-6530: post-6500 top50 title
 * deepen). Unique gj_top50_title_name_len_ok_6524 surface only; no
 * multi-def. Distinct from gj_game_title_len_ok (batch2368). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6524_marker[] = "libcgj-batch6524";

/* Inclusive max title name length for matrix row metadata. */
#define B6524_NAME_MAX  256u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6524_name_len_ok(size_t nLen)
{
	if (nLen == 0u) {
		return 0;
	}
	if (nLen > (size_t)B6524_NAME_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_top50_title_name_len_ok_6524 - soft check Top50 title name length.
 *
 * len: title name length in bytes/characters (caller-defined unit).
 * Returns 1 when 1 <= len <= 256, else 0.
 * Soft pure-data only; does not call libc. No parent wires.
 */
int
gj_top50_title_name_len_ok_6524(size_t nLen)
{
	(void)NULL;
	return b6524_name_len_ok(nLen);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_top50_title_name_len_ok_6524(size_t nLen)
    __attribute__((alias("gj_top50_title_name_len_ok_6524")));
