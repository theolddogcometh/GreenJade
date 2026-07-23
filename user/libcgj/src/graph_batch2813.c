/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2813: hex encode length (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_hex_enc_len_u(uint32_t n);
 *     - Output character count for hex-encoding n octets: 2 * n.
 *       Does not include a trailing NUL. Saturates to 0xffffffffu
 *       when 2 * n would overflow uint32_t.
 *   uint32_t __gj_hex_enc_len_u  (alias)
 *   __libcgj_batch2813_marker = "libcgj-batch2813"
 *
 * Encoding length exclusive wave (2811-2820). Distinct from
 * gj_hex_enc_len (size_t, batch822) — unique _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2813_marker[] = "libcgj-batch2813";

/* ---- freestanding helpers ---------------------------------------------- */

/* Hex encode length: 2 * n, saturating. */
static uint32_t
b2813_enc_len(uint32_t uN)
{
	if (uN > (0xffffffffu / 2u)) {
		return 0xffffffffu;
	}
	return uN * 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_enc_len_u - hex encode output length for n source bytes.
 *
 * Each input octet becomes two hex digits. n == 0 → 0.
 * Overflow of 2*n → 0xffffffffu.
 */
uint32_t
gj_hex_enc_len_u(uint32_t uN)
{
	(void)NULL;
	return b2813_enc_len(uN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_hex_enc_len_u(uint32_t uN)
    __attribute__((alias("gj_hex_enc_len_u")));
