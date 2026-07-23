/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1507: bit writer flush (partial final byte).
 *
 * Surface (unique symbols):
 *   typedef struct {
 *       uint64_t bitbuf;
 *       unsigned nfill;
 *       uint8_t *out;
 *       size_t out_cap;
 *       size_t out_pos;
 *   } gj_bitwriter_t;      — local typedef; layout matches batch1506
 *
 *   int gj_bitwriter_flush(gj_bitwriter_t *bw);
 *     — Emit any residual bits in bitbuf as a final partial byte
 *       (high bits zero-padded). Clears bitbuf and nfill on success.
 *       No-op (returns 0) when nfill == 0.
 *       Returns 0 on success, -1 on error (NULL bw/out, insufficient cap).
 *   int __gj_bitwriter_flush  (alias)
 *   __libcgj_batch1507_marker = "libcgj-batch1507"
 *
 * Pair with gj_bitwriter_put (batch1506). Local typedef — no shared header.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1507_marker[] = "libcgj-batch1507";

/* ---- local state (plain struct typedef; layout = batch1506) ------------ */

typedef struct {
	uint64_t bitbuf;
	unsigned nfill;
	uint8_t *out;
	size_t out_cap;
	size_t out_pos;
} gj_bitwriter_t;

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitwriter_flush — write residual 1..7 bits as one padded byte.
 *
 * After put() has emitted all full bytes, nfill is in 0..7. A non-zero
 * residual is written as (bitbuf & 0xFF) with unused high bits zero.
 */
int
gj_bitwriter_flush(gj_bitwriter_t *pBw)
{
	if (pBw == NULL || pBw->out == NULL) {
		return -1;
	}
	if (pBw->nfill == 0u) {
		return 0;
	}
	/* Sanity: put() never leaves nfill >= 8 without emitting. */
	if (pBw->nfill > 7u) {
		return -1;
	}
	if (pBw->out_pos >= pBw->out_cap) {
		return -1;
	}

	pBw->out[pBw->out_pos] = (uint8_t)(pBw->bitbuf & 0xFFu);
	pBw->out_pos++;
	pBw->bitbuf = 0u;
	pBw->nfill = 0u;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitwriter_flush(gj_bitwriter_t *pBw)
    __attribute__((alias("gj_bitwriter_flush")));
