/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8447: base64 encode output need (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_b64_encode_need_u_8447(size_t n);
 *     - Encoded length for n raw octets: 4 * ((n + 2) / 3) including
 *       canonical '=' padding capacity (no NUL).
 *   size_t __gj_b64_encode_need_u_8447  (alias)
 *   __libcgj_batch8447_marker = "libcgj-batch8447"
 *
 * Base64 alphabet stubs wave (8441-8450). Distinct from
 * gj_b64_enc_len_u (batch4561) — unique _8447 surface; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8447_marker[] = "libcgj-batch8447";

/* ---- freestanding helpers ---------------------------------------------- */

/* 4 * ceil(n / 3) via (n + 2) / 3; n==0 → 0. */
static size_t
b8447_encode_need(size_t cbN)
{
	return 4u * ((cbN + 2u) / 3u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_encode_need_u_8447 - RFC 4648 §4 encode output size for n octets.
 *
 * n: raw input byte count
 *
 * Returns 4 * ((n + 2) / 3). Covers full groups plus pad quartets.
 * Self-contained; no parent wires.
 */
size_t
gj_b64_encode_need_u_8447(size_t n)
{
	(void)NULL;
	return b8447_encode_need(n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_b64_encode_need_u_8447(size_t n)
    __attribute__((alias("gj_b64_encode_need_u_8447")));
