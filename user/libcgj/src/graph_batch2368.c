/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2368: game title length soft-ok predicate
 * (post-2360 product path exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_game_title_len_ok(size_t len);
 *     - Return 1 if len is a usable game title length in the closed
 *       product range 1..256, else 0. Soft pure-data bound only; does
 *       not inspect title text.
 *   int __gj_game_title_len_ok  (alias)
 *   __libcgj_batch2368_marker = "libcgj-batch2368"
 *
 * Post-2360 product path exclusive wave (2361-2370). Unique
 * game_title_len_ok surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2368_marker[] = "libcgj-batch2368";

/* Inclusive max game title length for product launch metadata. */
#define B2368_TITLE_MAX  256u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if len is in [1, B2368_TITLE_MAX]. */
static int
b2368_len_ok(size_t nLen)
{
	if (nLen == 0u) {
		return 0;
	}
	if (nLen > (size_t)B2368_TITLE_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_game_title_len_ok - soft check for a game title length.
 *
 * len: title length in characters/bytes (caller-defined unit)
 *
 * Returns 1 when 1 <= len <= 256, else 0. Does not call libc.
 */
int
gj_game_title_len_ok(size_t nLen)
{
	(void)NULL;
	return b2368_len_ok(nLen);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_game_title_len_ok(size_t nLen)
    __attribute__((alias("gj_game_title_len_ok")));
