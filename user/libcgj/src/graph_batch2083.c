/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2083: unaligned big-endian uint64 store (_u).
 *
 * Surface (unique symbols):
 *   void gj_store_be64_u(uint8_t *p, uint64_t v);
 *     - Store v as 64-bit big-endian into p[0..7]. NULL p is a no-op.
 *       Unaligned-safe octet walk only.
 *   void __gj_store_be64_u  (alias)
 *   __libcgj_batch2083_marker = "libcgj-batch2083"
 *
 * Endian store exclusive wave (2081-2090). Distinct from gj_store_be64
 * (batch394) - unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2083_marker[] = "libcgj-batch2083";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2083_store_be64(uint8_t *pDst, uint64_t u64V)
{
	pDst[0] = (uint8_t)(u64V >> 56);
	pDst[1] = (uint8_t)(u64V >> 48);
	pDst[2] = (uint8_t)(u64V >> 40);
	pDst[3] = (uint8_t)(u64V >> 32);
	pDst[4] = (uint8_t)(u64V >> 24);
	pDst[5] = (uint8_t)(u64V >> 16);
	pDst[6] = (uint8_t)(u64V >> 8);
	pDst[7] = (uint8_t)u64V;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_store_be64_u - big-endian uint64 store (unaligned-safe).
 *
 * p: pointer to at least 8 writable octets, or NULL (no-op)
 * v: value to store; p[0] receives the most-significant octet
 */
void
gj_store_be64_u(uint8_t *p, uint64_t v)
{
	(void)NULL;
	if (p == NULL) {
		return;
	}
	b2083_store_be64(p, v);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_store_be64_u(uint8_t *p, uint64_t v)
    __attribute__((alias("gj_store_be64_u")));
