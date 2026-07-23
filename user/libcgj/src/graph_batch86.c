/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch86: SHA-NI–aware SHA-256 block transform.
 *
 * Already present elsewhere (NOT redefined here — avoid multi-def):
 *   SHA256_Init / SHA256_Update / SHA256_Final / SHA256_End / SHA256_Data
 *     → graph_batch31.c / graph_batch32.c
 *
 * This TU adds only unique symbols:
 *   int gj_cpu_has_sha_ni(void);
 *     — CPUID leaf 7 subleaf 0, EBX bit 29 (SHA). Cached after first call.
 *   void SHA256_Transform_shani(uint32_t state[8],
 *                               const unsigned char block[64]);
 *     — one 64-byte block transform: SHA-NI when present, else software
 *       (FIPS 180-4). Always correct; HW is an acceleration only.
 *   void SHA256_Update_auto(void *pCtx, const void *pData, size_t cb);
 *     — streaming Update over the batch31 SHA-256 context layout; full
 *       blocks go through SHA256_Transform_shani (auto dispatch).
 *   __gj_cpu_has_sha_ni / __SHA256_Transform_shani / __SHA256_Update_auto
 *   __libcgj_batch86_marker = "libcgj-batch86"
 *
 * SHA-NI path uses __attribute__((target("sha,ssse3,sse4.1"))) so the
 * TU still builds with CGJ -msse2 freestanding flags. Clean-room FIPS
 * 180-4 + Intel SHA extension usage; no third-party source was copied.
 */

#include <immintrin.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch86_marker[] = "libcgj-batch86";

/* Context layout must match graph_batch31 struct b31_sha256 (112 bytes). */
struct b86_sha256 {
	uint32_t aState[8];
	uint32_t aCount[2];
	unsigned char aBuf[64];
	unsigned char aPad[8];
};

/* ---- freestanding helpers ---------------------------------------------- */

static void
b86_bcopy(void *pDst, const void *pSrc, size_t cb)
{
	unsigned char *pD = (unsigned char *)pDst;
	const unsigned char *pS = (const unsigned char *)pSrc;
	size_t i;

	for (i = 0; i < cb; i++) {
		pD[i] = pS[i];
	}
}

static uint32_t
b86_ror32(uint32_t uX, unsigned uN)
{
	return (uX >> uN) | (uX << (32u - uN));
}

static uint32_t
b86_get_be32(const unsigned char *p)
{
	return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
	       ((uint32_t)p[2] << 8) | (uint32_t)p[3];
}

/* ---- CPUID / SHA-NI detect --------------------------------------------- */

/* -1 unknown, 0 absent, 1 present */
static int s_b86_shani = -1;

static void
b86_cpuid(unsigned uLeaf, unsigned uSub, unsigned *pEax, unsigned *pEbx,
    unsigned *pEcx, unsigned *pEdx)
{
	unsigned uEax = 0u;
	unsigned uEbx = 0u;
	unsigned uEcx = 0u;
	unsigned uEdx = 0u;

#if defined(__x86_64__) || defined(__i386__)
	__asm__ volatile("cpuid"
	    : "=a"(uEax), "=b"(uEbx), "=c"(uEcx), "=d"(uEdx)
	    : "a"(uLeaf), "c"(uSub)
	    : "memory");
#else
	(void)uLeaf;
	(void)uSub;
#endif
	if (pEax != NULL) {
		*pEax = uEax;
	}
	if (pEbx != NULL) {
		*pEbx = uEbx;
	}
	if (pEcx != NULL) {
		*pEcx = uEcx;
	}
	if (pEdx != NULL) {
		*pEdx = uEdx;
	}
}

/*
 * SHA-NI is CPUID.(EAX=7,ECX=0):EBX[29].
 * Returns 1 if present, 0 otherwise. Result is cached.
 */
int
gj_cpu_has_sha_ni(void)
{
	unsigned uEax;
	unsigned uEbx;

	if (s_b86_shani >= 0) {
		return s_b86_shani;
	}
#if !(defined(__x86_64__) || defined(__i386__))
	s_b86_shani = 0;
	return 0;
#else
	/* Need extended feature leaf 7. */
	b86_cpuid(0u, 0u, &uEax, NULL, NULL, NULL);
	if (uEax < 7u) {
		s_b86_shani = 0;
		return 0;
	}
	b86_cpuid(7u, 0u, NULL, &uEbx, NULL, NULL);
	/* EBX bit 29 = SHA */
	s_b86_shani = ((uEbx & (1u << 29)) != 0u) ? 1 : 0;
	return s_b86_shani;
#endif
}

/* ---- software transform (FIPS 180-4) ----------------------------------- */

static const uint32_t s_b86_k256[64] = {
    0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u, 0x3956c25bu,
    0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u, 0xd807aa98u, 0x12835b01u,
    0x243185beu, 0x550c7dc3u, 0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u,
    0xc19bf174u, 0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
    0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau, 0x983e5152u,
    0xa831c66du, 0xb00327c8u, 0xbf597fc7u, 0xc6e00bf3u, 0xd5a79147u,
    0x06ca6351u, 0x14292967u, 0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu,
    0x53380d13u, 0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
    0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u, 0xd192e819u,
    0xd6990624u, 0xf40e3585u, 0x106aa070u, 0x19a4c116u, 0x1e376c08u,
    0x2748774cu, 0x34b0bcb5u, 0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu,
    0x682e6ff3u, 0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
    0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};

static void
b86_sha256_transform_sw(uint32_t aState[8], const unsigned char aBlk[64])
{
	uint32_t aW[64];
	uint32_t uA, uB, uC, uD, uE, uF, uG, uH;
	unsigned i;

	for (i = 0; i < 16u; i++) {
		aW[i] = b86_get_be32(aBlk + i * 4u);
	}
	for (i = 16; i < 64u; i++) {
		uint32_t uS0 = b86_ror32(aW[i - 15u], 7u) ^
		    b86_ror32(aW[i - 15u], 18u) ^ (aW[i - 15u] >> 3);
		uint32_t uS1 = b86_ror32(aW[i - 2u], 17u) ^
		    b86_ror32(aW[i - 2u], 19u) ^ (aW[i - 2u] >> 10);

		aW[i] = aW[i - 16u] + uS0 + aW[i - 7u] + uS1;
	}

	uA = aState[0];
	uB = aState[1];
	uC = aState[2];
	uD = aState[3];
	uE = aState[4];
	uF = aState[5];
	uG = aState[6];
	uH = aState[7];

	for (i = 0; i < 64u; i++) {
		uint32_t uS1 =
		    b86_ror32(uE, 6u) ^ b86_ror32(uE, 11u) ^ b86_ror32(uE, 25u);
		uint32_t uCh = (uE & uF) ^ ((~uE) & uG);
		uint32_t uT1 = uH + uS1 + uCh + s_b86_k256[i] + aW[i];
		uint32_t uS0 =
		    b86_ror32(uA, 2u) ^ b86_ror32(uA, 13u) ^ b86_ror32(uA, 22u);
		uint32_t uMaj = (uA & uB) ^ (uA & uC) ^ (uB & uC);
		uint32_t uT2 = uS0 + uMaj;

		uH = uG;
		uG = uF;
		uF = uE;
		uE = uD + uT1;
		uD = uC;
		uC = uB;
		uB = uA;
		uA = uT1 + uT2;
	}

	aState[0] += uA;
	aState[1] += uB;
	aState[2] += uC;
	aState[3] += uD;
	aState[4] += uE;
	aState[5] += uF;
	aState[6] += uG;
	aState[7] += uH;
}

/* ---- SHA-NI hardware transform (target-specialized) -------------------- */

#if defined(__x86_64__) || defined(__i386__)

/*
 * One-block SHA-256 via Intel SHA extensions (SHA256RNDS2/MSG1/MSG2).
 * State layout after shuffle: ABEF / CDGH (Intel SHA programming model).
 * Compiled only into this function's ISA via target attributes.
 */
__attribute__((target("sha,ssse3,sse4.1"))) static void
b86_sha256_transform_hw(uint32_t aState[8], const unsigned char aBlk[64])
{
	__m128i vState0, vState1, vMsg, vTmp;
	__m128i vMsg0, vMsg1, vMsg2, vMsg3;
	__m128i vAbefSave, vCdghSave;
	const __m128i vMask = _mm_set_epi64x(0x0c0d0e0f08090a0bull,
	    0x0405060700010203ull);

	/* Load initial hash value. */
	vTmp = _mm_loadu_si128((const __m128i *)(const void *)&aState[0]);
	vState1 = _mm_loadu_si128((const __m128i *)(const void *)&aState[4]);

	vTmp = _mm_shuffle_epi32(vTmp, 0xb1);	  /* CDAB */
	vState1 = _mm_shuffle_epi32(vState1, 0x1b); /* EFGH */
	vState0 = _mm_alignr_epi8(vTmp, vState1, 8); /* ABEF */
	vState1 = _mm_blend_epi16(vState1, vTmp, 0xf0); /* CDGH */

	vAbefSave = vState0;
	vCdghSave = vState1;

	/* Rounds 0-3 */
	vMsg = _mm_loadu_si128((const __m128i *)(const void *)(aBlk + 0));
	vMsg0 = _mm_shuffle_epi8(vMsg, vMask);
	vMsg = _mm_add_epi32(vMsg0,
	    _mm_set_epi64x(0xe9b5dba5b5c0fbcfull, 0x71374491428a2f98ull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);

	/* Rounds 4-7 */
	vMsg1 = _mm_loadu_si128((const __m128i *)(const void *)(aBlk + 16));
	vMsg1 = _mm_shuffle_epi8(vMsg1, vMask);
	vMsg = _mm_add_epi32(vMsg1,
	    _mm_set_epi64x(0xab1c5ed5923f82a4ull, 0x59f111f13956c25bull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);
	vMsg0 = _mm_sha256msg1_epu32(vMsg0, vMsg1);

	/* Rounds 8-11 */
	vMsg2 = _mm_loadu_si128((const __m128i *)(const void *)(aBlk + 32));
	vMsg2 = _mm_shuffle_epi8(vMsg2, vMask);
	vMsg = _mm_add_epi32(vMsg2,
	    _mm_set_epi64x(0x550c7dc3243185beull, 0x12835b01d807aa98ull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);
	vMsg1 = _mm_sha256msg1_epu32(vMsg1, vMsg2);

	/* Rounds 12-15 */
	vMsg3 = _mm_loadu_si128((const __m128i *)(const void *)(aBlk + 48));
	vMsg3 = _mm_shuffle_epi8(vMsg3, vMask);
	vMsg = _mm_add_epi32(vMsg3,
	    _mm_set_epi64x(0xc19bf1749bdc06a7ull, 0x80deb1fe72be5d74ull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vTmp = _mm_alignr_epi8(vMsg3, vMsg2, 4);
	vMsg0 = _mm_add_epi32(vMsg0, vTmp);
	vMsg0 = _mm_sha256msg2_epu32(vMsg0, vMsg3);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);
	vMsg2 = _mm_sha256msg1_epu32(vMsg2, vMsg3);

	/* Rounds 16-19 */
	vMsg = _mm_add_epi32(vMsg0,
	    _mm_set_epi64x(0x240ca1cc0fc19dc6ull, 0xefbe4786e49b69c1ull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vTmp = _mm_alignr_epi8(vMsg0, vMsg3, 4);
	vMsg1 = _mm_add_epi32(vMsg1, vTmp);
	vMsg1 = _mm_sha256msg2_epu32(vMsg1, vMsg0);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);
	vMsg3 = _mm_sha256msg1_epu32(vMsg3, vMsg0);

	/* Rounds 20-23 */
	vMsg = _mm_add_epi32(vMsg1,
	    _mm_set_epi64x(0x76f988da5cb0a9dcull, 0x4a7484aa2de92c6full));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vTmp = _mm_alignr_epi8(vMsg1, vMsg0, 4);
	vMsg2 = _mm_add_epi32(vMsg2, vTmp);
	vMsg2 = _mm_sha256msg2_epu32(vMsg2, vMsg1);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);
	vMsg0 = _mm_sha256msg1_epu32(vMsg0, vMsg1);

	/* Rounds 24-27 */
	vMsg = _mm_add_epi32(vMsg2,
	    _mm_set_epi64x(0xbf597fc7b00327c8ull, 0xa831c66d983e5152ull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vTmp = _mm_alignr_epi8(vMsg2, vMsg1, 4);
	vMsg3 = _mm_add_epi32(vMsg3, vTmp);
	vMsg3 = _mm_sha256msg2_epu32(vMsg3, vMsg2);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);
	vMsg1 = _mm_sha256msg1_epu32(vMsg1, vMsg2);

	/* Rounds 28-31 */
	vMsg = _mm_add_epi32(vMsg3,
	    _mm_set_epi64x(0x1429296706ca6351ull, 0xd5a79147c6e00bf3ull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vTmp = _mm_alignr_epi8(vMsg3, vMsg2, 4);
	vMsg0 = _mm_add_epi32(vMsg0, vTmp);
	vMsg0 = _mm_sha256msg2_epu32(vMsg0, vMsg3);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);
	vMsg2 = _mm_sha256msg1_epu32(vMsg2, vMsg3);

	/* Rounds 32-35 */
	vMsg = _mm_add_epi32(vMsg0,
	    _mm_set_epi64x(0x53380d134d2c6dfcull, 0x2e1b213827b70a85ull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vTmp = _mm_alignr_epi8(vMsg0, vMsg3, 4);
	vMsg1 = _mm_add_epi32(vMsg1, vTmp);
	vMsg1 = _mm_sha256msg2_epu32(vMsg1, vMsg0);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);
	vMsg3 = _mm_sha256msg1_epu32(vMsg3, vMsg0);

	/* Rounds 36-39 */
	vMsg = _mm_add_epi32(vMsg1,
	    _mm_set_epi64x(0x92722c8581c2c92eull, 0x766a0abb650a7354ull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vTmp = _mm_alignr_epi8(vMsg1, vMsg0, 4);
	vMsg2 = _mm_add_epi32(vMsg2, vTmp);
	vMsg2 = _mm_sha256msg2_epu32(vMsg2, vMsg1);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);
	vMsg0 = _mm_sha256msg1_epu32(vMsg0, vMsg1);

	/* Rounds 40-43 */
	vMsg = _mm_add_epi32(vMsg2,
	    _mm_set_epi64x(0xc76c51a3c24b8b70ull, 0xa81a664ba2bfe8a1ull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vTmp = _mm_alignr_epi8(vMsg2, vMsg1, 4);
	vMsg3 = _mm_add_epi32(vMsg3, vTmp);
	vMsg3 = _mm_sha256msg2_epu32(vMsg3, vMsg2);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);
	vMsg1 = _mm_sha256msg1_epu32(vMsg1, vMsg2);

	/* Rounds 44-47 */
	vMsg = _mm_add_epi32(vMsg3,
	    _mm_set_epi64x(0x106aa070f40e3585ull, 0xd6990624d192e819ull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vTmp = _mm_alignr_epi8(vMsg3, vMsg2, 4);
	vMsg0 = _mm_add_epi32(vMsg0, vTmp);
	vMsg0 = _mm_sha256msg2_epu32(vMsg0, vMsg3);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);
	vMsg2 = _mm_sha256msg1_epu32(vMsg2, vMsg3);

	/* Rounds 48-51 */
	vMsg = _mm_add_epi32(vMsg0,
	    _mm_set_epi64x(0x34b0bcb52748774cull, 0x1e376c0819a4c116ull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vTmp = _mm_alignr_epi8(vMsg0, vMsg3, 4);
	vMsg1 = _mm_add_epi32(vMsg1, vTmp);
	vMsg1 = _mm_sha256msg2_epu32(vMsg1, vMsg0);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);
	vMsg3 = _mm_sha256msg1_epu32(vMsg3, vMsg0);

	/* Rounds 52-55 */
	vMsg = _mm_add_epi32(vMsg1,
	    _mm_set_epi64x(0x682e6ff35b9cca4full, 0x4ed8aa4a391c0cb3ull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vTmp = _mm_alignr_epi8(vMsg1, vMsg0, 4);
	vMsg2 = _mm_add_epi32(vMsg2, vTmp);
	vMsg2 = _mm_sha256msg2_epu32(vMsg2, vMsg1);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);

	/* Rounds 56-59 */
	vMsg = _mm_add_epi32(vMsg2,
	    _mm_set_epi64x(0x8cc7020884c87814ull, 0x78a5636f748f82eeull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vTmp = _mm_alignr_epi8(vMsg2, vMsg1, 4);
	vMsg3 = _mm_add_epi32(vMsg3, vTmp);
	vMsg3 = _mm_sha256msg2_epu32(vMsg3, vMsg2);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);

	/* Rounds 60-63 */
	vMsg = _mm_add_epi32(vMsg3,
	    _mm_set_epi64x(0xc67178f2bef9a3f7ull, 0xa4506ceb90befffaull));
	vState1 = _mm_sha256rnds2_epu32(vState1, vState0, vMsg);
	vMsg = _mm_shuffle_epi32(vMsg, 0x0e);
	vState0 = _mm_sha256rnds2_epu32(vState0, vState1, vMsg);

	/* Combine with saved mid-state. */
	vState0 = _mm_add_epi32(vState0, vAbefSave);
	vState1 = _mm_add_epi32(vState1, vCdghSave);

	/* Write back in ABCD / EFGH order. */
	vTmp = _mm_shuffle_epi32(vState0, 0x1b);	/* FEBA */
	vState1 = _mm_shuffle_epi32(vState1, 0xb1);	/* DCHG */
	vState0 = _mm_blend_epi16(vTmp, vState1, 0xf0); /* DCBA */
	vState1 = _mm_alignr_epi8(vState1, vTmp, 8);	/* ABEF → EFGH */

	_mm_storeu_si128((__m128i *)(void *)&aState[0], vState0);
	_mm_storeu_si128((__m128i *)(void *)&aState[4], vState1);
}

#else /* !x86 */

static void
b86_sha256_transform_hw(uint32_t aState[8], const unsigned char aBlk[64])
{
	/* No SHA-NI off x86 — software only. */
	b86_sha256_transform_sw(aState, aBlk);
}

#endif

/* ---- public: SHA256_Transform_shani ------------------------------------ */

/*
 * One 64-byte block. Dispatches to SHA-NI when CPUID reports support;
 * otherwise uses the portable software transform. Both update state[8]
 * in place (FIPS 180-4 compression function).
 */
void
SHA256_Transform_shani(uint32_t aState[8], const unsigned char aBlk[64])
{
	if (aState == NULL || aBlk == NULL) {
		return;
	}
	if (gj_cpu_has_sha_ni() != 0) {
		b86_sha256_transform_hw(aState, aBlk);
	} else {
		b86_sha256_transform_sw(aState, aBlk);
	}
}

/* ---- public: SHA256_Update_auto ---------------------------------------- */

static void
b86_sha256_ensure(struct b86_sha256 *p)
{
	if (p->aState[0] == 0u && p->aState[1] == 0u && p->aState[2] == 0u &&
	    p->aState[3] == 0u && p->aState[4] == 0u && p->aState[5] == 0u &&
	    p->aState[6] == 0u && p->aState[7] == 0u && p->aCount[0] == 0u &&
	    p->aCount[1] == 0u) {
		p->aState[0] = 0x6a09e667u;
		p->aState[1] = 0xbb67ae85u;
		p->aState[2] = 0x3c6ef372u;
		p->aState[3] = 0xa54ff53au;
		p->aState[4] = 0x510e527fu;
		p->aState[5] = 0x9b05688cu;
		p->aState[6] = 0x1f83d9abu;
		p->aState[7] = 0x5be0cd19u;
	}
}

/*
 * Streaming update over batch31 SHA-256 context. Full 64-byte blocks use
 * SHA256_Transform_shani (SHA-NI or software). Does not redefine
 * SHA256_Update — call this explicitly for auto-dispatched acceleration.
 */
void
SHA256_Update_auto(void *pCtx, const void *pData, size_t cb)
{
	struct b86_sha256 *p = (struct b86_sha256 *)pCtx;
	const unsigned char *pQ = (const unsigned char *)pData;
	uint32_t uIndex;
	size_t cbPart;

	if (p == NULL || (pData == NULL && cb != 0u)) {
		return;
	}
	b86_sha256_ensure(p);
	uIndex = (p->aCount[0] >> 3) & 0x3fu;
	p->aCount[0] += (uint32_t)(cb << 3);
	if (p->aCount[0] < (uint32_t)(cb << 3)) {
		p->aCount[1]++;
	}
	p->aCount[1] += (uint32_t)(cb >> 29);
	cbPart = 64u - (size_t)uIndex;
	if (cb >= cbPart) {
		b86_bcopy(p->aBuf + uIndex, pQ, cbPart);
		SHA256_Transform_shani(p->aState, p->aBuf);
		pQ += cbPart;
		cb -= cbPart;
		while (cb >= 64u) {
			SHA256_Transform_shani(p->aState, pQ);
			pQ += 64u;
			cb -= 64u;
		}
		uIndex = 0;
	}
	if (cb > 0u) {
		b86_bcopy(p->aBuf + uIndex, pQ, cb);
	}
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_cpu_has_sha_ni(void) __attribute__((alias("gj_cpu_has_sha_ni")));

void __SHA256_Transform_shani(uint32_t aState[8],
    const unsigned char aBlk[64])
    __attribute__((alias("SHA256_Transform_shani")));

void __SHA256_Update_auto(void *pCtx, const void *pData, size_t cb)
    __attribute__((alias("SHA256_Update_auto")));
