/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2935: freestanding little-endian uint64 unpack
 * (_u continuum).
 *
 * Surface (unique symbols):
 *   uint64_t gj_unpack_u64_le_u(const unsigned char *in);
 *     - Read a 64-bit little-endian unsigned integer from in[0..7].
 *       NULL in -> 0. Unaligned-safe (byte loads only).
 *   uint64_t __gj_unpack_u64_le_u  (alias)
 *   __libcgj_batch2935_marker = "libcgj-batch2935"
 *
 * Pack/mod exclusive wave (2931-2940). Distinct from gj_unpack_u64le
 * (batch522) and gj_bytes_to_u64_le_u (batch2788) — unique
 * gj_unpack_u64_le_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2935_marker[] = "libcgj-batch2935";

/* ---- freestanding helpers ---------------------------------------------- */

/* Load one LE u64 from eight octets (may be unaligned). pB non-NULL. */
static uint64_t
b2935_load(const unsigned char *pB)
{
	return (uint64_t)pB[0]
	    | ((uint64_t)pB[1] << 8)
	    | ((uint64_t)pB[2] << 16)
	    | ((uint64_t)pB[3] << 24)
	    | ((uint64_t)pB[4] << 32)
	    | ((uint64_t)pB[5] << 40)
	    | ((uint64_t)pB[6] << 48)
	    | ((uint64_t)pB[7] << 56);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpack_u64_le_u - load one little-endian uint64 from eight bytes.
 *
 * in: pointer to at least eight readable bytes (NULL -> 0)
 *
 * Wire layout: in[0] is the least-significant byte, in[7] is the MSB.
 * Returns the host-order uint64_t value, or 0 if in is NULL.
 */
uint64_t
gj_unpack_u64_le_u(const unsigned char *pIn)
{
	(void)NULL;
	if (pIn == NULL) {
		return 0u;
	}
	return b2935_load(pIn);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_unpack_u64_le_u(const unsigned char *pIn)
    __attribute__((alias("gj_unpack_u64_le_u")));
