/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch202: ChaCha20 single-block keystream helper.
 *
 * chacha20_poly1305_* one-shot AEAD already lives in graph_batch70.c
 * (RFC 8439; uses batch42 chacha20_* / poly1305_auth). This TU does NOT
 * redefine those symbols.
 *
 * Surface (unique symbols only):
 *   void gj_chacha20_block_counter(unsigned char out[64],
 *                                  const unsigned char key[32],
 *                                  const unsigned char nonce[12],
 *                                  uint32_t counter);
 *     — one 64-byte ChaCha20 keystream block with out-first arg order.
 *       Thin adapter over batch42 chacha20_block(key, nonce, counter, out).
 *       NULL key/nonce/out → no-op (void API; no return code).
 *   __gj_chacha20_block_counter  (alias)
 *   __libcgj_batch202_marker = "libcgj-batch202"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No third-party source copied. No malloc, no errno.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch202_marker[] = "libcgj-batch202";

/* batch42 — do not redefine; resolved at link time */
extern int chacha20_block(const unsigned char aKey[32],
                          const unsigned char aNonce[12], uint32_t uCounter,
                          unsigned char aOut[64]);

/*
 * gj_chacha20_block_counter — emit one 64-byte ChaCha20 block.
 *
 * Argument order is (out, key, nonce, counter) for call-site convenience
 * when the destination buffer is the primary operand. Internally forwards
 * to chacha20_block(key, nonce, counter, out).
 *
 * If key, nonce, or out is NULL the call is a no-op (chacha20_block would
 * return -1; this void wrapper swallows that path without writing).
 */
void
gj_chacha20_block_counter(unsigned char out[64], const unsigned char key[32],
                          const unsigned char nonce[12], uint32_t counter)
{
	if (out == NULL || key == NULL || nonce == NULL) {
		return;
	}
	(void)chacha20_block(key, nonce, counter, out);
}

void __gj_chacha20_block_counter(unsigned char out[64],
                                 const unsigned char key[32],
                                 const unsigned char nonce[12],
                                 uint32_t counter)
    __attribute__((alias("gj_chacha20_block_counter")));
