/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2793: uint32_t → big-endian bytes (exclusive _u).
 *
 * Surface (unique symbols):
 *   void gj_u32_to_bytes_be_u(unsigned char *p, uint32_t v);
 *     - Store v as four big-endian octets at p (unaligned-safe).
 *       NULL → no-op.
 *   void __gj_u32_to_bytes_be_u  (alias)
 *   __libcgj_batch2793_marker = "libcgj-batch2793"
 *
 * Milestone 2800 exclusive byte/hex helpers (2791-2800). Unique
 * gj_u32_to_bytes_be_u surface only; no multi-def. Distinct from
 * gj_store_be32 (batch394) and gj_bytes_to_u32_be_u (batch2791 inverse).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2793_marker[] = "libcgj-batch2793";

/* ---- freestanding helpers ---------------------------------------------- */

/* Store one BE u32 into four octets. NULL → no-op. */
static void
b2793_store(unsigned char *pB, uint32_t uV)
{
	if (pB == NULL) {
		return;
	}
	pB[0] = (unsigned char)(uV >> 24);
	pB[1] = (unsigned char)(uV >> 16);
	pB[2] = (unsigned char)(uV >> 8);
	pB[3] = (unsigned char)uV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_to_bytes_be_u - big-endian uint32 store into four octets.
 *
 * p: pointer to at least 4 writable bytes (NULL → no-op)
 * v: value to store (MSB first)
 *
 * Unaligned-safe pure byte split. Does not call libc.
 */
void
gj_u32_to_bytes_be_u(unsigned char *pB, uint32_t uV)
{
	(void)NULL;
	b2793_store(pB, uV);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_to_bytes_be_u(unsigned char *pB, uint32_t uV)
    __attribute__((alias("gj_u32_to_bytes_be_u")));
