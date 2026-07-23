/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4553: encode one byte as two lowercase hex
 * digits (_u).
 *
 * Surface (unique symbols):
 *   void gj_hex_encode_byte_u(uint8_t b, char out[2]);
 *     - Write two lowercase hex ASCII digits for b into out[0]/out[1]
 *       (no trailing NUL). NULL out is a no-op return (ok).
 *   void __gj_hex_encode_byte_u  (alias)
 *   __libcgj_batch4553_marker = "libcgj-batch4553"
 *
 * Exclusive continuum CREATE-ONLY (4551-4560: hex_digit_u,
 * hex_digit_upper_u, hex_encode_byte_u, hex_decode_nibble_u,
 * hex_decode_byte_u, hex_encode_len_u, hex_decode_len_u, is_hex_char_u,
 * hex_nybble_pair_u, batch_id_4560). Distinct from
 * gj_u8_to_hex_pair_u (batch3375, returns -1 on NULL) and full-buffer
 * gj_hex_encode (batch298) — unique gj_hex_encode_byte_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4553_marker[] = "libcgj-batch4553";

/* ---- freestanding helpers ---------------------------------------------- */

/* Low nibble → lowercase hex ASCII. */
static char
b4553_digit(unsigned uNib)
{
	uNib &= 15u;
	if (uNib < 10u) {
		return (char)('0' + (char)uNib);
	}
	return (char)('a' + (char)(uNib - 10u));
}

/* Write two hex digits for one octet; NULL destination is a no-op. */
static void
b4553_encode_byte(uint8_t u8B, char *pOut)
{
	if (pOut == NULL) {
		return;
	}
	pOut[0] = b4553_digit(((unsigned)u8B >> 4) & 0xfu);
	pOut[1] = b4553_digit((unsigned)u8B & 0xfu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hex_encode_byte_u - encode one byte as two lowercase hex digits.
 *
 * b:   octet to encode (0..255)
 * out: destination of at least 2 chars; not NUL-terminated; NULL → return
 *
 * Writes high nibble then low nibble as '0'-'9'/'a'-'f'. No return value.
 * Self-contained; no parent wires.
 */
void
gj_hex_encode_byte_u(uint8_t u8B, char *pOut)
{
	(void)NULL;
	b4553_encode_byte(u8B, pOut);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_hex_encode_byte_u(uint8_t u8B, char *pOut)
    __attribute__((alias("gj_hex_encode_byte_u")));
