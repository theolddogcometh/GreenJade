/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4561: base64 encode output length (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_b64_enc_len_u(size_t n);
 *     - Encoded length for n raw octets: 4 * ((n + 2) / 3) including
 *       canonical '=' padding capacity (no NUL).
 *   size_t __gj_b64_enc_len_u  (alias)
 *   __libcgj_batch4561_marker = "libcgj-batch4561"
 *
 * Exclusive continuum CREATE-ONLY (4561-4570: b64_enc_len_u, b64_dec_len_u,
 * b64_is_char_u, b64_char_val_u, b64_enc_sextet_u, b64url_enc_sextet_u,
 * b64_pad_count_u, b64_group_count_u, b64_is_pad_u, batch_id_4570).
 * Distinct from streaming gj_b64_enc_* (batch206) — length helper only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4561_marker[] = "libcgj-batch4561";

/* ---- freestanding helpers ---------------------------------------------- */

/* 4 * ceil(n / 3) via (n + 2) / 3; n==0 → 0. */
static size_t
b4561_enc_len(size_t cbN)
{
	return 4u * ((cbN + 2u) / 3u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_enc_len_u - RFC 4648 §4 encode output size for n input octets.
 *
 * n: raw input byte count
 *
 * Returns 4 * ((n + 2) / 3). Covers full groups plus pad quartets.
 * Self-contained; no parent wires.
 */
size_t
gj_b64_enc_len_u(size_t n)
{
	(void)NULL;
	return b4561_enc_len(n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_b64_enc_len_u(size_t n)
    __attribute__((alias("gj_b64_enc_len_u")));
