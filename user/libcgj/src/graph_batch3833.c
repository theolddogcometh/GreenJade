/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3833: host uint32_t → little-endian wire value.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_to_le_u(uint32_t v);
 *     - Convert host-order v to a little-endian wire encoding expressed
 *       as a host integer (htole32 shape). On LE hosts identity; on BE
 *       hosts byte-swap. Pure integer; no buffer.
 *   uint32_t __gj_u32_to_le_u  (alias)
 *   __libcgj_batch3833_marker = "libcgj-batch3833"
 *
 * Exclusive continuum CREATE-ONLY (3831-3840). Distinct from
 * gj_u32_to_bytes_le_u (batch2789) and gj_u32_to_be_u (batch3832) —
 * unique value-returning LE _u surface; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3833_marker[] = "libcgj-batch3833";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3833_host_is_le(void)
{
	const uint32_t u = 1u;

	return ((const unsigned char *)&u)[0] == 1u;
}

static uint32_t
b3833_bswap32(uint32_t uV)
{
	return ((uV & 0x000000ffu) << 24) |
	       ((uV & 0x0000ff00u) << 8) |
	       ((uV & 0x00ff0000u) >> 8) |
	       ((uV & 0xff000000u) >> 24);
}

static uint32_t
b3833_to_le(uint32_t uV)
{
	if (b3833_host_is_le()) {
		return uV;
	}
	return b3833_bswap32(uV);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_to_le_u — host uint32_t to little-endian wire value.
 *
 * v: host-order input
 *
 * Returns LE-encoded value as host integer. No parent wires.
 */
uint32_t
gj_u32_to_le_u(uint32_t uV)
{
	(void)NULL;
	return b3833_to_le(uV);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_to_le_u(uint32_t uV)
    __attribute__((alias("gj_u32_to_le_u")));
