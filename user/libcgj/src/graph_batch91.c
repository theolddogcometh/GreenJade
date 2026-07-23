/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch91: Poly1305 one-shot MAC with SSE2-tuned load path.
 * Clean-room freestanding C. No third-party Poly1305 source was copied.
 *
 * Surface (unique — do not redefine batch42 poly1305_auth):
 *   void poly1305_auth_sse2(unsigned char mac[16],
 *                           const unsigned char *m, size_t mlen,
 *                           const unsigned char key[32]);
 *   __poly1305_auth_sse2  (alias)
 *   __libcgj_batch91_marker = "libcgj-batch91"
 *
 * Notes:
 *   - Argument order differs from batch42 poly1305_auth (mac first, key last).
 *   - Field arithmetic uses classic 5×26-bit integer limbs (SSE does not speed
 *     the mul/reduce chain much on scalar messages). SSE2 is used to load each
 *     16-byte block into an XMM and extract LE 32-bit words for limb decoding.
 *   - Must match poly1305_auth on shared vectors. Reference:
 *       extern void poly1305_auth(const unsigned char aKey[32],
 *                                const unsigned char *pMsg, size_t cbMsg,
 *                                unsigned char aOut[16]);
 *     KAT (RFC 8439 §2.5.2) — both APIs must produce the same tag:
 *       key  = 85d6be7857556d337f4452fe42d506a80103808afb0db2fd4abff6af4149f51b
 *       msg  = "Cryptographic Forum Research Group" (34 bytes)
 *       tag  = a8061dc1305136c6c22b8baf0c0127a9
 *     Empty message (mlen=0) is valid: process no blocks, finish with s only.
 *
 * Soft deepen: null mac/key reject, mlen=0 tag path, partial final block
 * pad 0x01, SSE2 16B load for full blocks, clamp r bits per RFC 8439.
 */

#include <emmintrin.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch91_marker[] = "libcgj-batch91";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b91_load_le32(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

static void
b91_store_le32(unsigned char *p, uint32_t x)
{
	p[0] = (unsigned char)x;
	p[1] = (unsigned char)(x >> 8);
	p[2] = (unsigned char)(x >> 16);
	p[3] = (unsigned char)(x >> 24);
}

/*
 * Load 16 message bytes via SSE2 unaligned load; extract four LE u32 words.
 * Fall back path for partial final block uses scalar loads after pad.
 */
static void
b91_load_block_sse2(const unsigned char *p, uint32_t t[4])
{
	__m128i v = _mm_loadu_si128((const __m128i *)(const void *)p);

	t[0] = (uint32_t)_mm_cvtsi128_si32(v);
	t[1] = (uint32_t)_mm_cvtsi128_si32(_mm_srli_si128(v, 4));
	t[2] = (uint32_t)_mm_cvtsi128_si32(_mm_srli_si128(v, 8));
	t[3] = (uint32_t)_mm_cvtsi128_si32(_mm_srli_si128(v, 12));
}

/* One Poly1305 block: h = (h + m) * r mod (2^130 - 5). hibit = 1<<24 for full. */
static void
b91_poly_blocks(uint32_t *h0, uint32_t *h1, uint32_t *h2, uint32_t *h3,
                uint32_t *h4, uint32_t r0, uint32_t r1, uint32_t r2, uint32_t r3,
                uint32_t r4, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4,
                uint32_t t0, uint32_t t1, uint32_t t2, uint32_t t3,
                uint32_t hibit)
{
	uint64_t d0, d1, d2, d3, d4;
	uint64_t c;
	uint32_t a0 = *h0;
	uint32_t a1 = *h1;
	uint32_t a2 = *h2;
	uint32_t a3 = *h3;
	uint32_t a4 = *h4;

	a0 += t0 & 0x3ffffffu;
	a1 += ((t0 >> 26) | (t1 << 6)) & 0x3ffffffu;
	a2 += ((t1 >> 20) | (t2 << 12)) & 0x3ffffffu;
	a3 += ((t2 >> 14) | (t3 << 18)) & 0x3ffffffu;
	a4 += (t3 >> 8) | hibit;

	d0 = (uint64_t)a0 * r0 + (uint64_t)a1 * s4 + (uint64_t)a2 * s3 +
	     (uint64_t)a3 * s2 + (uint64_t)a4 * s1;
	d1 = (uint64_t)a0 * r1 + (uint64_t)a1 * r0 + (uint64_t)a2 * s4 +
	     (uint64_t)a3 * s3 + (uint64_t)a4 * s2;
	d2 = (uint64_t)a0 * r2 + (uint64_t)a1 * r1 + (uint64_t)a2 * r0 +
	     (uint64_t)a3 * s4 + (uint64_t)a4 * s3;
	d3 = (uint64_t)a0 * r3 + (uint64_t)a1 * r2 + (uint64_t)a2 * r1 +
	     (uint64_t)a3 * r0 + (uint64_t)a4 * s4;
	d4 = (uint64_t)a0 * r4 + (uint64_t)a1 * r3 + (uint64_t)a2 * r2 +
	     (uint64_t)a3 * r1 + (uint64_t)a4 * r0;

	c = d0 >> 26;
	a0 = (uint32_t)d0 & 0x3ffffffu;
	d1 += c;
	c = d1 >> 26;
	a1 = (uint32_t)d1 & 0x3ffffffu;
	d2 += c;
	c = d2 >> 26;
	a2 = (uint32_t)d2 & 0x3ffffffu;
	d3 += c;
	c = d3 >> 26;
	a3 = (uint32_t)d3 & 0x3ffffffu;
	d4 += c;
	c = d4 >> 26;
	a4 = (uint32_t)d4 & 0x3ffffffu;
	a0 += (uint32_t)c * 5u;
	c = a0 >> 26;
	a0 &= 0x3ffffffu;
	a1 += (uint32_t)c;

	*h0 = a0;
	*h1 = a1;
	*h2 = a2;
	*h3 = a3;
	*h4 = a4;
}

/*
 * poly1305_auth_sse2 — RFC 8439 §2.5 one-shot MAC.
 * key is r||s (32 bytes, r clamped). Writes 16-byte little-endian tag to mac.
 * Matches batch42 poly1305_auth on the same (key, m, mlen).
 */
void
poly1305_auth_sse2(unsigned char mac[16], const unsigned char *m, size_t mlen,
                   const unsigned char key[32])
{
	uint32_t r0, r1, r2, r3, r4;
	uint32_t s1, s2, s3, s4;
	uint32_t h0, h1, h2, h3, h4;
	uint64_t c;
	const unsigned char *p;
	size_t cbLeft;

	if (mac == NULL || key == NULL) {
		return;
	}
	if (m == NULL && mlen != 0u) {
		return;
	}

	/* load r and clamp (RFC 8439 §2.5) */
	{
		uint32_t t0 = b91_load_le32(key + 0);
		uint32_t t1 = b91_load_le32(key + 4);
		uint32_t t2 = b91_load_le32(key + 8);
		uint32_t t3 = b91_load_le32(key + 12);

		r0 = t0 & 0x3ffffffu;
		r1 = ((t0 >> 26) | (t1 << 6)) & 0x3ffff03u;
		r2 = ((t1 >> 20) | (t2 << 12)) & 0x3ffc0ffu;
		r3 = ((t2 >> 14) | (t3 << 18)) & 0x3f03fffu;
		r4 = (t3 >> 8) & 0x00fffffu;
	}
	s1 = r1 * 5u;
	s2 = r2 * 5u;
	s3 = r3 * 5u;
	s4 = r4 * 5u;

	h0 = h1 = h2 = h3 = h4 = 0;

	p = m;
	cbLeft = mlen;
	while (cbLeft >= 16u) {
		uint32_t t[4];

		b91_load_block_sse2(p, t);
		b91_poly_blocks(&h0, &h1, &h2, &h3, &h4, r0, r1, r2, r3, r4, s1,
		                s2, s3, s4, t[0], t[1], t[2], t[3], 1u << 24);
		p += 16;
		cbLeft -= 16u;
	}

	if (cbLeft > 0u) {
		unsigned char aBlock[16];
		size_t i;
		uint32_t t0, t1, t2, t3;

		for (i = 0; i < 16u; i++) {
			aBlock[i] = 0;
		}
		for (i = 0; i < cbLeft; i++) {
			aBlock[i] = p[i];
		}
		aBlock[cbLeft] = 1u;

		t0 = b91_load_le32(aBlock + 0);
		t1 = b91_load_le32(aBlock + 4);
		t2 = b91_load_le32(aBlock + 8);
		t3 = b91_load_le32(aBlock + 12);

		/* partial block: hibit already encoded by the trailing 0x01 pad */
		b91_poly_blocks(&h0, &h1, &h2, &h3, &h4, r0, r1, r2, r3, r4, s1,
		                s2, s3, s4, t0, t1, t2, t3, 0);
	}

	/* final reduction */
	c = h1 >> 26;
	h1 &= 0x3ffffffu;
	h2 += (uint32_t)c;
	c = h2 >> 26;
	h2 &= 0x3ffffffu;
	h3 += (uint32_t)c;
	c = h3 >> 26;
	h3 &= 0x3ffffffu;
	h4 += (uint32_t)c;
	c = h4 >> 26;
	h4 &= 0x3ffffffu;
	h0 += (uint32_t)c * 5u;
	c = h0 >> 26;
	h0 &= 0x3ffffffu;
	h1 += (uint32_t)c;

	/* h = h < p ? h : h - p  (p = 2^130 - 5) */
	{
		uint32_t g0, g1, g2, g3, g4;
		uint32_t mask;

		g0 = h0 + 5u;
		c = g0 >> 26;
		g0 &= 0x3ffffffu;
		g1 = h1 + (uint32_t)c;
		c = g1 >> 26;
		g1 &= 0x3ffffffu;
		g2 = h2 + (uint32_t)c;
		c = g2 >> 26;
		g2 &= 0x3ffffffu;
		g3 = h3 + (uint32_t)c;
		c = g3 >> 26;
		g3 &= 0x3ffffffu;
		g4 = h4 + (uint32_t)c - (1u << 26);

		mask = (g4 >> 31) - 1u;
		g0 &= mask;
		g1 &= mask;
		g2 &= mask;
		g3 &= mask;
		g4 &= mask;
		mask = ~mask;
		h0 = (h0 & mask) | g0;
		h1 = (h1 & mask) | g1;
		h2 = (h2 & mask) | g2;
		h3 = (h3 & mask) | g3;
		h4 = (h4 & mask) | g4;
	}

	/* pack 128-bit LE + add s */
	{
		uint64_t f0, f1, f2, f3;
		uint32_t s0 = b91_load_le32(key + 16);
		uint32_t s1x = b91_load_le32(key + 20);
		uint32_t s2x = b91_load_le32(key + 24);
		uint32_t s3x = b91_load_le32(key + 28);

		f0 = ((uint64_t)h0) | ((uint64_t)h1 << 26);
		f1 = ((uint64_t)(h1 >> 6)) | ((uint64_t)h2 << 20);
		f2 = ((uint64_t)(h2 >> 12)) | ((uint64_t)h3 << 14);
		f3 = ((uint64_t)(h3 >> 18)) | ((uint64_t)h4 << 8);

		f0 = (f0 & 0xffffffffull) + s0;
		f1 = (f1 & 0xffffffffull) + s1x + (f0 >> 32);
		f2 = (f2 & 0xffffffffull) + s2x + (f1 >> 32);
		f3 = (f3 & 0xffffffffull) + s3x + (f2 >> 32);

		b91_store_le32(mac + 0, (uint32_t)f0);
		b91_store_le32(mac + 4, (uint32_t)f1);
		b91_store_le32(mac + 8, (uint32_t)f2);
		b91_store_le32(mac + 12, (uint32_t)f3);
	}
}

void __poly1305_auth_sse2(unsigned char mac[16], const unsigned char *m,
                          size_t mlen, const unsigned char key[32])
    __attribute__((alias("poly1305_auth_sse2")));
