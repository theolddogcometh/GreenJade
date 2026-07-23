/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1648: Adobe ASCII85 encode length upper bound.
 *
 * Surface (unique symbols):
 *   size_t gj_ascii85_encode_len_hint(size_t n);
 *     — Worst-case character count for Adobe ASCII85 with optional 'z'
 *       zero-block compression. 'z' only shortens, so the upper bound
 *       equals pure base85: full groups *5 + remainder r → r+1 chars.
 *       Excludes stream delimiters <~ ~> and trailing NUL. n == 0 → 0.
 *   size_t __gj_ascii85_encode_len_hint  (alias)
 *   __libcgj_batch1648_marker = "libcgj-batch1648"
 *
 * Does NOT redefine ascii85_encode (graph_batch68) — length-hint only.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1648_marker[] = "libcgj-batch1648";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Worst case = pure base85 length (no zero 4-byte groups to collapse to
 * 'z'). Matches graph_batch68 pure path: nFull*5 + (nRem ? nRem+1 : 0).
 */
static size_t
b1648_enc_worst(size_t cbIn)
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
 * gj_ascii85_encode_len_hint — worst-case ASCII85 chars for n raw bytes.
 */
size_t
gj_ascii85_encode_len_hint(size_t cbIn)
{
	return b1648_enc_worst(cbIn);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ascii85_encode_len_hint(size_t cbIn)
    __attribute__((alias("gj_ascii85_encode_len_hint")));
