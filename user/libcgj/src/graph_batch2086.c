/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2086: unaligned little-endian uint64 store (_u).
 *
 * Surface (unique symbols):
 *   void gj_store_le64_u(uint8_t *p, uint64_t v);
 *     - Store v as 64-bit little-endian into p[0..7]. NULL p is a no-op.
 *       Unaligned-safe octet walk only.
 *   void __gj_store_le64_u  (alias)
 *   __libcgj_batch2086_marker = "libcgj-batch2086"
 *
 * Endian store exclusive wave (2081-2090). Distinct from gj_store_le64
 * (batch395) - unique _u name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2086_marker[] = "libcgj-batch2086";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b2086_store_le64(uint8_t *pDst, uint64_t u64V)
{
	pDst[0] = (uint8_t)u64V;
	pDst[1] = (uint8_t)(u64V >> 8);
	pDst[2] = (uint8_t)(u64V >> 16);
	pDst[3] = (uint8_t)(u64V >> 24);
	pDst[4] = (uint8_t)(u64V >> 32);
	pDst[5] = (uint8_t)(u64V >> 40);
	pDst[6] = (uint8_t)(u64V >> 48);
	pDst[7] = (uint8_t)(u64V >> 56);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_store_le64_u - little-endian uint64 store (unaligned-safe).
 *
 * p: pointer to at least 8 writable octets, or NULL (no-op)
 * v: value to store; p[0] receives the least-significant octet
 */
void
gj_store_le64_u(uint8_t *p, uint64_t v)
{
	(void)NULL;
	if (p == NULL) {
		return;
	}
	b2086_store_le64(p, v);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_store_le64_u(uint8_t *p, uint64_t v)
    __attribute__((alias("gj_store_le64_u")));
