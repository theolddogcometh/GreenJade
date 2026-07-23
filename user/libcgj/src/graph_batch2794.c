/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2794: uint64_t → big-endian bytes (exclusive _u).
 *
 * Surface (unique symbols):
 *   void gj_u64_to_bytes_be_u(unsigned char *p, uint64_t v);
 *     - Store v as eight big-endian octets at p (unaligned-safe).
 *       NULL → no-op.
 *   void __gj_u64_to_bytes_be_u  (alias)
 *   __libcgj_batch2794_marker = "libcgj-batch2794"
 *
 * Milestone 2800 exclusive byte/hex helpers (2791-2800). Unique
 * gj_u64_to_bytes_be_u surface only; no multi-def. Distinct from
 * gj_store_be64 (batch394), gj_bytes_to_u64_be_u (batch2792 inverse),
 * and gj_u64_to_bytes_le_u (batch2795 LE counterpart).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2794_marker[] = "libcgj-batch2794";

/* ---- freestanding helpers ---------------------------------------------- */

/* Store one BE u64 into eight octets. NULL → no-op. */
static void
b2794_store(unsigned char *pB, uint64_t uV)
{
	if (pB == NULL) {
		return;
	}
	pB[0] = (unsigned char)(uV >> 56);
	pB[1] = (unsigned char)(uV >> 48);
	pB[2] = (unsigned char)(uV >> 40);
	pB[3] = (unsigned char)(uV >> 32);
	pB[4] = (unsigned char)(uV >> 24);
	pB[5] = (unsigned char)(uV >> 16);
	pB[6] = (unsigned char)(uV >> 8);
	pB[7] = (unsigned char)uV;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_to_bytes_be_u - big-endian uint64 store into eight octets.
 *
 * p: pointer to at least 8 writable bytes (NULL → no-op)
 * v: value to store (MSB first)
 *
 * Unaligned-safe pure byte split. Does not call libc.
 */
void
gj_u64_to_bytes_be_u(unsigned char *pB, uint64_t uV)
{
	(void)NULL;
	b2794_store(pB, uV);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u64_to_bytes_be_u(unsigned char *pB, uint64_t uV)
    __attribute__((alias("gj_u64_to_bytes_be_u")));
