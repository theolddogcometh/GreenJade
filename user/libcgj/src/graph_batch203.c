/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch203: AES-CTR stream helper.
 * Clean-room freestanding pure C (NIST SP 800-38A §6.5 CTR shape).
 * Block cipher primitives from batch45 (extern only — do not reimplement):
 *   AES_set_encrypt_key / AES_encrypt
 * Local AES_KEY layout matches OpenSSL aes.h / batch45.
 * Builds under userspace -ffreestanding -msse2. No third-party CTR
 * source was copied.
 *
 * Surface (unique symbols):
 *   void gj_aes_ctr_xor(const unsigned char *key, int bits,
 *                       unsigned char counter[16],
 *                       unsigned char *buf, size_t n);
 *     Expand key (bits = 128/192/256), then XOR AES-CTR keystream over
 *     buf[0..n) in place. counter is the full 16-byte counter block and
 *     is advanced (big-endian 128-bit) once per full keystream block.
 *     Encrypt and decrypt are identical (XOR). No-op on NULL key/counter/
 *     buf, n == 0, or key-schedule failure.
 *   __gj_aes_ctr_xor  (alias)
 *   __libcgj_batch203_marker = "libcgj-batch203"
 *
 * Existing AES-CTR surface NOT redefined (avoid multi-def):
 *   AES_ctr128_encrypt / aes_ctr_xor  → graph_batch74.c
 *     (those take a pre-scheduled AES_KEY; this helper takes raw key)
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch203_marker[] = "libcgj-batch203";

/* OpenSSL aes.h layout (AES_MAXNR == 14) — match batch45/50/74. */
#define AES_MAXNR 14
#define B203_BLOCK 16u

typedef struct aes_key_st {
	unsigned int rd_key[4 * (AES_MAXNR + 1)];
	int rounds;
} AES_KEY;

/* batch45 AES primitives (no local AES block implementation). */
int AES_set_encrypt_key(const unsigned char *userKey, const int bits,
                        AES_KEY *key);
void AES_encrypt(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key);

/* ========================================================================
 * Local helpers (no libc string; freestanding / -fno-builtin safe)
 * ======================================================================== */

/*
 * Big-endian 128-bit counter increment (OpenSSL CRYPTO_ctr128_inc shape).
 * Used for NIST SP 800-38A CTR with a full-block counter.
 */
static void
b203_ctr_inc(unsigned char aCtr[16])
{
	unsigned int uCarry = 1u;
	int i;

	for (i = 15; i >= 0 && uCarry != 0u; i--) {
		unsigned int u = (unsigned int)aCtr[i] + uCarry;

		aCtr[i] = (unsigned char)u;
		uCarry = u >> 8;
	}
}

/* ========================================================================
 * gj_aes_ctr_xor
 * ======================================================================== */

/*
 * One-shot AES-CTR over buf[0..n): expand raw key, then for each byte
 * generate keystream via AES_encrypt(counter) and XOR in place.
 * Counter advance matches batch74 AES_ctr128_encrypt / OpenSSL:
 * encrypt counter → big-endian 128-bit inc → consume keystream bytes.
 * Partial final blocks leave counter already advanced past that block;
 * residual mid-block offset is not retained across calls (always starts
 * at keystream offset 0).
 */
void
gj_aes_ctr_xor(const unsigned char *key, int bits,
               unsigned char counter[16], unsigned char *buf, size_t n)
{
	AES_KEY stKey;
	unsigned char aKs[B203_BLOCK];
	unsigned int uOff;
	size_t i;
	int nRc;

	if (key == NULL || counter == NULL || buf == NULL || n == 0u) {
		return;
	}

	nRc = AES_set_encrypt_key(key, bits, &stKey);
	if (nRc != 0) {
		return;
	}

	uOff = 0u;
	for (i = 0u; i < n; i++) {
		if (uOff == 0u) {
			AES_encrypt(counter, aKs, &stKey);
			b203_ctr_inc(counter);
		}
		buf[i] = (unsigned char)(buf[i] ^ aKs[uOff]);
		uOff++;
		if (uOff == B203_BLOCK) {
			uOff = 0u;
		}
	}
}

void __gj_aes_ctr_xor(const unsigned char *key, int bits,
                      unsigned char counter[16], unsigned char *buf,
                      size_t n)
    __attribute__((alias("gj_aes_ctr_xor")));
