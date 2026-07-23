/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8455: hex encode buffer need stub.
 *
 * Surface (unique symbols):
 *   size_t gj_hex_encode_need_u_8455(size_t n);
 *     - Buffer octets needed to hex-encode n source bytes as a
 *       NUL-terminated C string: 2 * n + 1. Returns 0 if 2*n+1 would
 *       overflow size_t.
 *   size_t __gj_hex_encode_need_u_8455  (alias)
 *   __libcgj_batch8455_marker = "libcgj-batch8455"
 *
 * Exclusive continuum CREATE-ONLY (8451-8460: hex encode/decode stubs).
 * Distinct from gj_hex_enc_len (batch822; no NUL), gj_base16_encode_len_u
 * (batch2581) — unique gj_hex_encode_need_u_8455 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8455_marker[] = "libcgj-batch8455";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Digits (2 per octet) plus trailing NUL. Overflow → 0 so callers can
 * treat zero as "cannot represent need".
 */
static size_t
b8455_encode_need(size_t cbIn)
{
	if (cbIn > ((~(size_t)0u) - 1u) / 2u) {
		return 0u;
	}
	return (cbIn * 2u) + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_encode_need_u_8455 - buffer size for hex-encoding n octets + NUL.
 *
 * n: number of source octets.
 *
 * Returns 2*n + 1 on success, or 0 if the product would overflow size_t.
 * Pure length math. No parent wires.
 */
size_t
gj_hex_encode_need_u_8455(size_t cbIn)
{
	(void)NULL;
	return b8455_encode_need(cbIn);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_hex_encode_need_u_8455(size_t cbIn)
    __attribute__((alias("gj_hex_encode_need_u_8455")));
