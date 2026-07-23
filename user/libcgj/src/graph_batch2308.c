/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2308: freestanding byte-buffer peek one u8
 * (post-2300 buffer exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_buf_peek_u8(const uint8_t *p, size_t len, uint8_t *pout);
 *     - Read the last used byte p[len - 1] into *pout without changing
 *       len. Returns 1 on success, 0 when empty or bad args.
 *       NULL p, len == 0, or NULL pout -> 0.
 *   int __gj_buf_peek_u8  (alias)
 *   __libcgj_batch2308_marker = "libcgj-batch2308"
 *
 * Post-2300 buffer exclusive wave (2301-2310). Distinct from
 * gj_buf_pop_u8 (batch2307, mutates len) - unique gj_buf_peek_u8
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2308_marker[] = "libcgj-batch2308";

/* ---- freestanding helpers ---------------------------------------------- */

/* Peek last used byte into *pOut; 1 on success. */
static int
b2308_peek(const uint8_t *pBuf, size_t cbLen, uint8_t *pOut)
{
	if (pBuf == NULL || pOut == NULL || cbLen == 0u) {
		return 0;
	}
	*pOut = pBuf[cbLen - 1u];
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buf_peek_u8 - read the last used byte without mutating length.
 *
 * p:    buffer base (NULL -> fail)
 * len:  used length in bytes (0 -> fail)
 * pout: destination for the peeked byte (NULL -> fail)
 *
 * Returns 1 on success (*pout written), else 0. Does not call libc.
 */
int
gj_buf_peek_u8(const uint8_t *pBuf, size_t cbLen, uint8_t *pOut)
{
	(void)NULL;
	return b2308_peek(pBuf, cbLen, pOut);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_buf_peek_u8(const uint8_t *pBuf, size_t cbLen, uint8_t *pOut)
    __attribute__((alias("gj_buf_peek_u8")));
