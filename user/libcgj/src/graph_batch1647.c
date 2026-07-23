/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1647: pure base85 encode length (no 'z').
 *
 * Surface (unique symbols):
 *   size_t gj_base85_encode_len_hint(size_t n);
 *     — Exact character count for pure Adobe-style base85 (alphabet
 *       '!'..'u') without the 'z' zero-block shortcut: full 4-byte groups
 *       → 5 chars each; remainder r (1..3) → r+1 chars. Excludes NUL.
 *       n == 0 → 0.
 *   size_t __gj_base85_encode_len_hint  (alias)
 *   __libcgj_batch1647_marker = "libcgj-batch1647"
 *
 * Does NOT redefine base85_encode (graph_batch68) — length-hint only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1647_marker[] = "libcgj-batch1647";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Pure base85 (no 'z'): same formula as graph_batch68 b68_enc_need minus
 * the trailing NUL: nFull*5 + (nRem ? nRem+1 : 0).
 */
static size_t
b1647_enc_chars(size_t cbIn)
{
	size_t cFull;
	size_t cRem;
	size_t cb;

	cFull = cbIn / 4u;
	cRem = cbIn % 4u;
	cb = cFull * 5u;
	if (cRem != 0u) {
		cb += cRem + 1u;
	}
	return cb;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_base85_encode_len_hint — pure base85 output character count for n bytes.
 */
size_t
gj_base85_encode_len_hint(size_t cbIn)
{
	return b1647_enc_chars(cbIn);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_base85_encode_len_hint(size_t cbIn)
    __attribute__((alias("gj_base85_encode_len_hint")));
