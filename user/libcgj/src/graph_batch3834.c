/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3834: host uint64_t → big-endian wire value.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_to_be_u(uint64_t v);
 *     - Convert host-order v to a big-endian wire encoding expressed as
 *       a host integer (htobe64 shape). On BE hosts identity; on LE
 *       hosts byte-swap. Pure integer; no buffer. No __int128.
 *   uint64_t __gj_u64_to_be_u  (alias)
 *   __libcgj_batch3834_marker = "libcgj-batch3834"
 *
 * Exclusive continuum CREATE-ONLY (3831-3840). Distinct from
 * gj_u64_to_bytes_be_u (batch2794) and gj_bswap64 (batch417) — unique
 * value-returning BE _u surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3834_marker[] = "libcgj-batch3834";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3834_host_is_le(void)
{
	const uint32_t u = 1u;

	return ((const unsigned char *)&u)[0] == 1u;
}

static uint64_t
b3834_bswap64(uint64_t uV)
{
	return ((uV & 0x00000000000000ffull) << 56) |
	       ((uV & 0x000000000000ff00ull) << 40) |
	       ((uV & 0x0000000000ff0000ull) << 24) |
	       ((uV & 0x00000000ff000000ull) << 8) |
	       ((uV & 0x000000ff00000000ull) >> 8) |
	       ((uV & 0x0000ff0000000000ull) >> 24) |
	       ((uV & 0x00ff000000000000ull) >> 40) |
	       ((uV & 0xff00000000000000ull) >> 56);
}

static uint64_t
b3834_to_be(uint64_t uV)
{
	if (b3834_host_is_le()) {
		return b3834_bswap64(uV);
	}
	return uV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_to_be_u — host uint64_t to big-endian wire value.
 *
 * v: host-order input
 *
 * Returns BE-encoded value as host integer. No parent wires.
 */
uint64_t
gj_u64_to_be_u(uint64_t uV)
{
	(void)NULL;
	return b3834_to_be(uV);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_to_be_u(uint64_t uV)
    __attribute__((alias("gj_u64_to_be_u")));
