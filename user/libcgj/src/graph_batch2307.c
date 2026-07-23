/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2307: freestanding byte-buffer pop one u8
 * (post-2300 buffer exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_buf_pop_u8(uint8_t *p, size_t *plen, uint8_t *pout);
 *     - Remove the last used byte p[*plen - 1], store it in *pout when
 *       pout is non-NULL, and decrement *plen. Returns 1 on success,
 *       0 when empty or bad args. NULL p or NULL plen or *plen == 0 -> 0.
 *   int __gj_buf_pop_u8  (alias)
 *   __libcgj_batch2307_marker = "libcgj-batch2307"
 *
 * Post-2300 buffer exclusive wave (2301-2310). Distinct from
 * gj_buf_push_u8 (batch2306) - unique gj_buf_pop_u8 surface only;
 * no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2307_marker[] = "libcgj-batch2307";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pop last used byte; optional store to *pOut. 1 on success. */
static int
b2307_pop(uint8_t *pBuf, size_t *pcbLen, uint8_t *pOut)
{
	size_t cbUsed;
	uint8_t u8V;

	if (pBuf == NULL || pcbLen == NULL) {
		return 0;
	}
	cbUsed = *pcbLen;
	if (cbUsed == 0u) {
		return 0;
	}
	u8V = pBuf[cbUsed - 1u];
	*pcbLen = cbUsed - 1u;
	if (pOut != NULL) {
		*pOut = u8V;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_pop_u8 - remove the last used byte from a buffer.
 *
 * p:    buffer base (NULL -> fail)
 * plen: in/out used length (NULL -> fail); must be > 0 to succeed
 * pout: optional destination for the popped byte (may be NULL)
 *
 * Returns 1 on success (*plen decremented), else 0. Does not call libc.
 */
int
gj_buf_pop_u8(uint8_t *pBuf, size_t *pcbLen, uint8_t *pOut)
{
	(void)NULL;
	return b2307_pop(pBuf, pcbLen, pOut);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_buf_pop_u8(uint8_t *pBuf, size_t *pcbLen, uint8_t *pOut)
    __attribute__((alias("gj_buf_pop_u8")));
