/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4562: base64 decode max output length (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_b64_dec_len_u(size_t n);
 *     - Approximate max decoded octet count for n alphabet/pad chars:
 *       3 * n / 4 (integer). Upper bound for buffer sizing; ignores
 *       exact pad stripping (true payload may be 0..2 bytes shorter).
 *   size_t __gj_b64_dec_len_u  (alias)
 *   __libcgj_batch4562_marker = "libcgj-batch4562"
 *
 * Exclusive continuum CREATE-ONLY (4561-4570). Distinct from streaming
 * gj_b64_dec_* (batch258) and gj_b64_enc_len_u (batch4561) — decode
 * length helper only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4562_marker[] = "libcgj-batch4562";

/* ---- freestanding helpers ---------------------------------------------- */

/* 3 * floor(n / 4) rewritten as 3*(n/4) to avoid 3*n overflow. */
static size_t
b4562_dec_len(size_t cbN)
{
	return 3u * (cbN / 4u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_dec_len_u - approximate max decode output for n input chars.
 *
 * n: encoded character count (alphabet and/or pads)
 *
 * Returns 3 * (n / 4). Self-contained; no parent wires.
 */
size_t
gj_b64_dec_len_u(size_t n)
{
	(void)NULL;
	return b4562_dec_len(n);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_b64_dec_len_u(size_t n)
    __attribute__((alias("gj_b64_dec_len_u")));
