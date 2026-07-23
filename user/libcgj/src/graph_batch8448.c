/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8448: base64 decode output need (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_b64_decode_need_u_8448(size_t n);
 *     - Approximate max decoded octet count for n alphabet/pad chars:
 *       3 * (n / 4) (integer). Upper bound for buffer sizing; ignores
 *       exact pad stripping (true payload may be 0..2 bytes shorter).
 *   size_t __gj_b64_decode_need_u_8448  (alias)
 *   __libcgj_batch8448_marker = "libcgj-batch8448"
 *
 * Base64 alphabet stubs wave (8441-8450). Distinct from
 * gj_b64_dec_len_u (batch4562) — unique _8448 surface; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8448_marker[] = "libcgj-batch8448";

/* ---- freestanding helpers ---------------------------------------------- */

/* 3 * floor(n / 4) rewritten as 3*(n/4) to avoid 3*n overflow. */
static size_t
b8448_decode_need(size_t cbN)
{
	return 3u * (cbN / 4u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_decode_need_u_8448 - approximate max decode output for n chars.
 *
 * n: encoded character count (alphabet and/or pads)
 *
 * Returns 3 * (n / 4). Self-contained; no parent wires.
 */
size_t
gj_b64_decode_need_u_8448(size_t n)
{
	(void)NULL;
	return b8448_decode_need(n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_b64_decode_need_u_8448(size_t n)
    __attribute__((alias("gj_b64_decode_need_u_8448")));
