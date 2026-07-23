/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2934: freestanding big-endian uint64 unpack
 * (_u continuum).
 *
 * Surface (unique symbols):
 *   uint64_t gj_unpack_u64_be_u(const unsigned char *in);
 *     - Read 8 big-endian bytes from in[0..7] into a uint64_t.
 *       NULL in -> 0. Unaligned-safe (byte loads only).
 *   uint64_t __gj_unpack_u64_be_u  (alias)
 *   __libcgj_batch2934_marker = "libcgj-batch2934"
 *
 * Pack/mod exclusive wave (2931-2940). Distinct from gj_unpack_u64be
 * (batch516) — unique gj_unpack_u64_be_u surface only; no multi-def.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2934_marker[] = "libcgj-batch2934";

/* ---- freestanding helpers ---------------------------------------------- */

/* Load one BE u64 from eight octets (may be unaligned). pB non-NULL. */
static uint64_t
b2934_load(const unsigned char *pB)
{
	return ((uint64_t)pB[0] << 56)
	    | ((uint64_t)pB[1] << 48)
	    | ((uint64_t)pB[2] << 40)
	    | ((uint64_t)pB[3] << 32)
	    | ((uint64_t)pB[4] << 24)
	    | ((uint64_t)pB[5] << 16)
	    | ((uint64_t)pB[6] << 8)
	    | (uint64_t)pB[7];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unpack_u64_be_u - load uint64_t from network / big-endian byte order.
 *
 * in: source of at least 8 bytes, or NULL (returns 0).
 * Returns the reconstructed host-order uint64_t; high byte at in[0],
 * low byte at in[7].
 */
uint64_t
gj_unpack_u64_be_u(const unsigned char *pIn)
{
	(void)NULL;
	if (pIn == NULL) {
		return 0u;
	}
	return b2934_load(pIn);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_unpack_u64_be_u(const unsigned char *pIn)
    __attribute__((alias("gj_unpack_u64_be_u")));
