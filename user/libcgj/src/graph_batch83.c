/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch83: AES-NI accelerated single-block encrypt with
 * CPUID dispatch and pure-C fallback to batch45 AES_encrypt.
 * Userspace may use SSE2 / AES-NI (CGJ_CFLAGS has -msse2). Kernel stays no-SSE.
 * Clean-room freestanding public ABI.
 *
 * Surface:
 *   int  gj_cpu_has_aesni(void);   // CPUID.1 ECX bit 25
 *   int  AES_encrypt_aesni(in, out, key);
 *        Uses AES-NI when available, else AES_encrypt. Returns 0 / -1.
 *   void AES_encrypt_auto(in, out, key);
 *        Always-succeeds dispatch (AES-NI or software).
 *   int  AES_decrypt_aesni(in, out, key);
 *        AES-NI decrypt when available, else AES_decrypt. Returns 0 / -1.
 *   void AES_decrypt_auto(in, out, key);
 *        Always-succeeds decrypt dispatch.
 *   __gj_cpu_has_aesni / __AES_encrypt_aesni / __AES_encrypt_auto
 *   __AES_decrypt_aesni / __AES_decrypt_auto  (aliases)
 *   __libcgj_batch83_marker = "libcgj-batch83"
 *
 * AES block primitives from batch45 (extern only; do not redefine):
 *   AES_encrypt / AES_decrypt
 * Local AES_KEY layout matches OpenSSL aes.h / batch45.
 *
 * Soft deepen: decrypt NI path (AESDEC/AESDECLAST), mirror auto dispatch,
 * null/round reject paths match encrypt.
 */

#include <immintrin.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch83_marker[] = "libcgj-batch83";

/* OpenSSL aes.h layout (AES_MAXNR == 14) — match batch45/50/74. */
#define AES_MAXNR 14

typedef struct aes_key_st {
	unsigned int rd_key[4 * (AES_MAXNR + 1)];
	int rounds;
} AES_KEY;

/* batch45 software block ops (no local AES_set_* multi-def). */
void AES_encrypt(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key);
void AES_decrypt(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key);

/* Cached CPUID.1 ECX.AES (bit 25): -1 unknown, 0 absent, 1 present. */
static int s_b83_aesni = -1;

/*
 * CPUID leaf 1 → ECX bit 25 (AES-NI). Pure asm; freestanding-safe.
 */
static void
b83_cpuid1(unsigned int *pEax, unsigned int *pEbx, unsigned int *pEcx,
           unsigned int *pEdx)
{
	unsigned int a, b, c, d;

	__asm__ volatile("cpuid"
	                 : "=a"(a), "=b"(b), "=c"(c), "=d"(d)
	                 : "a"(1u), "c"(0u));
	if (pEax != NULL) {
		*pEax = a;
	}
	if (pEbx != NULL) {
		*pEbx = b;
	}
	if (pEcx != NULL) {
		*pEcx = c;
	}
	if (pEdx != NULL) {
		*pEdx = d;
	}
}

int
gj_cpu_has_aesni(void)
{
	unsigned int uEcx;

	if (s_b83_aesni >= 0) {
		return s_b83_aesni;
	}
	b83_cpuid1(NULL, NULL, &uEcx, NULL);
	/* CPUID.1:ECX[25] = AES-NI */
	s_b83_aesni = ((uEcx & (1u << 25)) != 0u) ? 1 : 0;
	return s_b83_aesni;
}

/*
 * Convert one OpenSSL/batch45 round-key block (4 BE-valued uint32 words) into
 * an AES-NI xmm register (memory / little-endian byte order matching plaintext).
 * SSE2-only construction (no SSSE3 pshufb).
 */
static __attribute__((target("aes,sse2"))) __m128i
b83_load_rk(const unsigned int *pRk)
{
	unsigned char a[16];
	unsigned i;

	for (i = 0u; i < 4u; i++) {
		unsigned int w = pRk[i];

		a[i * 4u + 0u] = (unsigned char)(w >> 24);
		a[i * 4u + 1u] = (unsigned char)(w >> 16);
		a[i * 4u + 2u] = (unsigned char)(w >> 8);
		a[i * 4u + 3u] = (unsigned char)w;
	}
	return _mm_loadu_si128((const __m128i *)(const void *)a);
}

/*
 * AES-NI single-block encrypt. key->rounds must be 10, 12, or 14.
 * Marked target("aes,sse2") so AESENC/AESENCLAST are legal without -maes.
 */
static __attribute__((target("aes,sse2"))) void
b83_aesni_encrypt_block(const unsigned char *in, unsigned char *out,
                        const AES_KEY *key)
{
	const unsigned int *rk = key->rd_key;
	int nr = key->rounds;
	__m128i s;
	int r;

	s = _mm_loadu_si128((const __m128i *)(const void *)in);
	s = _mm_xor_si128(s, b83_load_rk(rk));

	/* Rounds 1 .. Nr-1: AESENC */
	for (r = 1; r < nr; r++) {
		s = _mm_aesenc_si128(s, b83_load_rk(rk + (unsigned)r * 4u));
	}
	/* Final round: AESENCLAST */
	s = _mm_aesenclast_si128(s, b83_load_rk(rk + (unsigned)nr * 4u));

	_mm_storeu_si128((__m128i *)(void *)out, s);
}

/*
 * AES-NI single-block decrypt. key must be a decrypt schedule
 * (AES_set_decrypt_key shape): rd_key[0] first AddRoundKey, then AESDEC
 * for rounds 1..Nr-1, AESDECLAST for Nr. Soft deepen twin of encrypt.
 */
static __attribute__((target("aes,sse2"))) void
b83_aesni_decrypt_block(const unsigned char *in, unsigned char *out,
                        const AES_KEY *key)
{
	const unsigned int *rk = key->rd_key;
	int nr = key->rounds;
	__m128i s;
	int r;

	s = _mm_loadu_si128((const __m128i *)(const void *)in);
	s = _mm_xor_si128(s, b83_load_rk(rk));

	for (r = 1; r < nr; r++) {
		s = _mm_aesdec_si128(s, b83_load_rk(rk + (unsigned)r * 4u));
	}
	s = _mm_aesdeclast_si128(s, b83_load_rk(rk + (unsigned)nr * 4u));

	_mm_storeu_si128((__m128i *)(void *)out, s);
}

int
AES_encrypt_aesni(const unsigned char *in, unsigned char *out,
                  const AES_KEY *key)
{
	int nr;

	if (in == NULL || out == NULL || key == NULL) {
		return -1;
	}
	nr = key->rounds;
	if (nr != 10 && nr != 12 && nr != 14) {
		return -1;
	}

	if (gj_cpu_has_aesni() != 0) {
		b83_aesni_encrypt_block(in, out, key);
		return 0;
	}

	/* Pure-C fallback — batch45 table AES. */
	AES_encrypt(in, out, key);
	return 0;
}

void
AES_encrypt_auto(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key)
{
	int nr;

	if (in == NULL || out == NULL || key == NULL) {
		return;
	}
	nr = key->rounds;
	if (nr != 10 && nr != 12 && nr != 14) {
		/* Still attempt software path if schedule is present. */
		AES_encrypt(in, out, key);
		return;
	}

	if (gj_cpu_has_aesni() != 0) {
		b83_aesni_encrypt_block(in, out, key);
		return;
	}
	AES_encrypt(in, out, key);
}

int
AES_decrypt_aesni(const unsigned char *in, unsigned char *out,
                  const AES_KEY *key)
{
	int nr;

	if (in == NULL || out == NULL || key == NULL) {
		return -1;
	}
	nr = key->rounds;
	if (nr != 10 && nr != 12 && nr != 14) {
		return -1;
	}

	if (gj_cpu_has_aesni() != 0) {
		b83_aesni_decrypt_block(in, out, key);
		return 0;
	}

	AES_decrypt(in, out, key);
	return 0;
}

void
AES_decrypt_auto(const unsigned char *in, unsigned char *out,
                 const AES_KEY *key)
{
	int nr;

	if (in == NULL || out == NULL || key == NULL) {
		return;
	}
	nr = key->rounds;
	if (nr != 10 && nr != 12 && nr != 14) {
		AES_decrypt(in, out, key);
		return;
	}

	if (gj_cpu_has_aesni() != 0) {
		b83_aesni_decrypt_block(in, out, key);
		return;
	}
	AES_decrypt(in, out, key);
}

int __gj_cpu_has_aesni(void)
    __attribute__((alias("gj_cpu_has_aesni")));
int __AES_encrypt_aesni(const unsigned char *in, unsigned char *out,
                        const AES_KEY *key)
    __attribute__((alias("AES_encrypt_aesni")));
void __AES_encrypt_auto(const unsigned char *in, unsigned char *out,
                        const AES_KEY *key)
    __attribute__((alias("AES_encrypt_auto")));
int __AES_decrypt_aesni(const unsigned char *in, unsigned char *out,
                        const AES_KEY *key)
    __attribute__((alias("AES_decrypt_aesni")));
void __AES_decrypt_auto(const unsigned char *in, unsigned char *out,
                        const AES_KEY *key)
    __attribute__((alias("AES_decrypt_auto")));
