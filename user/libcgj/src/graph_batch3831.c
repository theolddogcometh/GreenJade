/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3831: host uint16_t → little-endian wire value.
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_to_le_u(uint16_t v);
 *     - Convert host-order v to a little-endian wire encoding expressed
 *       as a host integer (htole16 shape). On LE hosts identity; on BE
 *       hosts byte-swap. Pure integer; no buffer.
 *   uint16_t __gj_u16_to_le_u  (alias)
 *   __libcgj_batch3831_marker = "libcgj-batch3831"
 *
 * Exclusive continuum CREATE-ONLY (3831-3840: u16_to_le_u, u32_to_be_u,
 * u32_to_le_u, u64_to_be_u, u64_to_le_u, checksum_sum16_u,
 * checksum_sum32_u, checksum_xor16_u, checksum_xor32_u, batch_id_3840).
 * Distinct from gj_u32_to_bytes_le_u (batch2789) and htons-family —
 * unique value-returning _u surface; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3831_marker[] = "libcgj-batch3831";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3831_host_is_le(void)
{
	const uint16_t u = 1u;

	return ((const unsigned char *)&u)[0] == 1u;
}

static uint16_t
b3831_bswap16(uint16_t uV)
{
	return (uint16_t)(((uV & 0x00ffu) << 8) | ((uV & 0xff00u) >> 8));
}

static uint16_t
b3831_to_le(uint16_t uV)
{
	if (b3831_host_is_le()) {
		return uV;
	}
	return b3831_bswap16(uV);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_to_le_u — host uint16_t to little-endian wire value.
 *
 * v: host-order input
 *
 * Returns LE-encoded value as host integer. No parent wires.
 */
uint16_t
gj_u16_to_le_u(uint16_t uV)
{
	(void)NULL;
	return b3831_to_le(uV);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_to_le_u(uint16_t uV)
    __attribute__((alias("gj_u16_to_le_u")));
