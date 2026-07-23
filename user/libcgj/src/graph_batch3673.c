/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3673: freestanding BER definite-length field
 * size probe (_u surface).
 *
 * Surface (unique symbols):
 *   unsigned gj_ber_len_need_u(uint32_t len);
 *     - Return the number of octets needed to encode a definite BER
 *       length field for value length len (X.690 short/long form):
 *         len <= 127          → 1  (short form)
 *         128..255            → 2  (0x81 + 1 content byte)
 *         256..65535          → 3  (0x82 + 2)
 *         65536..16777215     → 4  (0x83 + 3)
 *         16777216..UINT32_MAX→ 5  (0x84 + 4)
 *       Indefinite form (0x80) is not used. Length-probe only — does
 *       not write an encoding.
 *   unsigned __gj_ber_len_need_u  (alias)
 *   __libcgj_batch3673_marker = "libcgj-batch3673"
 *
 * CREATE-ONLY exclusive continuum wave (3671-3680). Unique
 * gj_ber_len_need_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3673_marker[] = "libcgj-batch3673";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Definite BER length-field octet count for value length u32Len.
 * Short form when u32Len < 128; else 1 + minimal big-endian width.
 */
static unsigned
b3673_need(uint32_t u32Len)
{
	if (u32Len < 128u) {
		return 1u;
	}
	if (u32Len <= 0xffu) {
		return 2u;
	}
	if (u32Len <= 0xffffu) {
		return 3u;
	}
	if (u32Len <= 0xffffffu) {
		return 4u;
	}
	return 5u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ber_len_need_u - octets required for a definite BER length of len.
 *
 * len: value length in octets to be length-encoded
 *
 * Result is always in 1..5 for any uint32_t. No parent wires.
 */
unsigned
gj_ber_len_need_u(uint32_t len)
{
	(void)NULL;
	return b3673_need(len);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_ber_len_need_u(uint32_t len)
    __attribute__((alias("gj_ber_len_need_u")));
