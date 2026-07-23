/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch201: streaming SipHash-2-4 context API.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party SipHash source was copied.
 *
 * Existing one-shot SipHash lives elsewhere and is NOT redefined:
 *   siphash24 / siphash24_buf / __siphash24           → graph_batch36.c
 *   siphash24_with_key / siphash13_with_key / halfsiphash24 → graph_batch75.c
 *
 * This TU adds only unique streaming symbols:
 *   void     gj_sip24_init(void *ctx, const unsigned char key[16]);
 *   void     gj_sip24_update(void *ctx, const void *data, size_t n);
 *   uint64_t gj_sip24_final(void *ctx);
 *   __gj_sip24_init / __gj_sip24_update / __gj_sip24_final  (aliases)
 *   __libcgj_batch201_marker = "libcgj-batch201"
 *
 * Context buffer:
 *   Caller-owned opaque buffer; size MUST be >= 64 bytes (GJ_SIP24_CTX_SIZE).
 *   Layout is private; treat as opaque. After gj_sip24_final the context is
 *   finalized (further update/final is undefined); re-init to reuse.
 *
 * Algorithm: SipHash-2-4 (Aumasson / Bernstein) — 2 compression rounds per
 * 8-byte block, 4 finalization rounds after the length-encoded last block.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch201_marker[] = "libcgj-batch201";

/* Documented minimum context size (bytes). Callers must allocate at least this. */
enum { GJ_SIP24_CTX_SIZE = 64 };

/*
 * Internal streaming state. sizeof(struct b201_sip24) is well under 64;
 * the public contract still requires a 64-byte buffer for ABI headroom.
 */
struct b201_sip24 {
	uint64_t v0;
	uint64_t v1;
	uint64_t v2;
	uint64_t v3;
	uint64_t total;		/* total message bytes absorbed so far */
	unsigned char buf[8];	/* partial (0..7) block pending compression */
	unsigned buf_n;		/* bytes valid in buf[] */
	unsigned reserved;	/* pad / future */
};

/* Compile-time guard: internal state must fit in the documented buffer. */
typedef char b201_ctx_size_ok[(sizeof(struct b201_sip24) <= (size_t)GJ_SIP24_CTX_SIZE) ? 1 : -1];

/* ---- helpers ----------------------------------------------------------- */

static uint64_t
b201_rol64(uint64_t uX, unsigned uN)
{
	return (uX << uN) | (uX >> (64u - uN));
}

static uint64_t
b201_load_le64(const unsigned char *p, size_t cb)
{
	uint64_t u = 0;
	size_t i;

	for (i = 0; i < cb && i < 8u; i++) {
		u |= (uint64_t)p[i] << (8u * i);
	}
	return u;
}

#define B201_SIPROUND(v0, v1, v2, v3)                                         \
	do {                                                                  \
		(v0) += (v1);                                                 \
		(v1) = b201_rol64((v1), 13);                                  \
		(v1) ^= (v0);                                                 \
		(v0) = b201_rol64((v0), 32);                                  \
		(v2) += (v3);                                                 \
		(v3) = b201_rol64((v3), 16);                                  \
		(v3) ^= (v2);                                                 \
		(v0) += (v3);                                                 \
		(v3) = b201_rol64((v3), 21);                                  \
		(v3) ^= (v0);                                                 \
		(v2) += (v1);                                                 \
		(v1) = b201_rol64((v1), 17);                                  \
		(v1) ^= (v2);                                                 \
		(v2) = b201_rol64((v2), 32);                                  \
	} while (0)

/* Compress one full little-endian 64-bit message word (c=2 rounds). */
static void
b201_compress(struct b201_sip24 *pSt, uint64_t uM)
{
	pSt->v3 ^= uM;
	B201_SIPROUND(pSt->v0, pSt->v1, pSt->v2, pSt->v3);
	B201_SIPROUND(pSt->v0, pSt->v1, pSt->v2, pSt->v3);
	pSt->v0 ^= uM;
}

/* ---- gj_sip24_init ----------------------------------------------------- */

/*
 * Initialize streaming SipHash-2-4 state in ctx.
 * ctx must point to at least 64 bytes of writable storage.
 * key may be NULL (treated as 16 zero bytes).
 */
void
gj_sip24_init(void *ctx, const unsigned char key[16])
{
	struct b201_sip24 *pSt;
	static const unsigned char aZero[16];
	const unsigned char *pKey;
	uint64_t uK0;
	uint64_t uK1;
	unsigned i;

	if (ctx == NULL) {
		return;
	}

	pSt = (struct b201_sip24 *)ctx;
	pKey = (key != NULL) ? key : aZero;
	uK0 = b201_load_le64(pKey, 8u);
	uK1 = b201_load_le64(pKey + 8, 8u);

	pSt->v0 = 0x736f6d6570736575ull ^ uK0;
	pSt->v1 = 0x646f72616e646f6dull ^ uK1;
	pSt->v2 = 0x6c7967656e657261ull ^ uK0;
	pSt->v3 = 0x7465646279746573ull ^ uK1;
	pSt->total = 0ull;
	pSt->buf_n = 0u;
	pSt->reserved = 0u;
	for (i = 0; i < 8u; i++) {
		pSt->buf[i] = 0u;
	}

	/* Silence unused-typedef under some pedantic modes. */
	(void)sizeof(b201_ctx_size_ok);
}

void __gj_sip24_init(void *ctx, const unsigned char key[16])
    __attribute__((alias("gj_sip24_init")));

/* ---- gj_sip24_update --------------------------------------------------- */

/*
 * Absorb n bytes from data into the streaming context.
 * NULL data with n != 0 is a no-op. n == 0 is a no-op.
 */
void
gj_sip24_update(void *ctx, const void *data, size_t n)
{
	struct b201_sip24 *pSt;
	const unsigned char *p;
	size_t nLeft;

	if (ctx == NULL || n == 0u) {
		return;
	}
	if (data == NULL) {
		return;
	}

	pSt = (struct b201_sip24 *)ctx;
	p = (const unsigned char *)data;
	nLeft = n;

	/* Fill any pending partial block first. */
	if (pSt->buf_n != 0u) {
		while (nLeft > 0u && pSt->buf_n < 8u) {
			pSt->buf[pSt->buf_n] = *p;
			pSt->buf_n++;
			p++;
			nLeft--;
		}
		if (pSt->buf_n == 8u) {
			b201_compress(pSt, b201_load_le64(pSt->buf, 8u));
			pSt->buf_n = 0u;
		}
	}

	/* Full 8-byte blocks. */
	while (nLeft >= 8u) {
		b201_compress(pSt, b201_load_le64(p, 8u));
		p += 8u;
		nLeft -= 8u;
	}

	/* Stash remainder. */
	while (nLeft > 0u) {
		pSt->buf[pSt->buf_n] = *p;
		pSt->buf_n++;
		p++;
		nLeft--;
	}

	pSt->total += (uint64_t)n;
}

void __gj_sip24_update(void *ctx, const void *data, size_t n)
    __attribute__((alias("gj_sip24_update")));

/* ---- gj_sip24_final ---------------------------------------------------- */

/*
 * Finalize the stream: length-encoded last block (c=2), then d=4 rounds
 * after v2 ^= 0xff. Returns the 64-bit tag. ctx may be NULL → 0.
 * After this call the context is spent; call gj_sip24_init to reuse.
 */
uint64_t
gj_sip24_final(void *ctx)
{
	struct b201_sip24 *pSt;
	uint64_t uB;
	unsigned i;

	if (ctx == NULL) {
		return 0ull;
	}

	pSt = (struct b201_sip24 *)ctx;

	/*
	 * Last block: remaining 0..7 message bytes in the low 56 bits,
	 * total length (mod 256) in the high byte — SipHash-2-4 padding.
	 * buf_n is always 0..7 so partial bytes always fit under the length.
	 */
	uB = (pSt->total & 0xffull) << 56;
	for (i = 0; i < pSt->buf_n && i < 7u; i++) {
		uB |= (uint64_t)pSt->buf[i] << (8u * i);
	}

	b201_compress(pSt, uB);

	pSt->v2 ^= 0xffull;
	B201_SIPROUND(pSt->v0, pSt->v1, pSt->v2, pSt->v3);
	B201_SIPROUND(pSt->v0, pSt->v1, pSt->v2, pSt->v3);
	B201_SIPROUND(pSt->v0, pSt->v1, pSt->v2, pSt->v3);
	B201_SIPROUND(pSt->v0, pSt->v1, pSt->v2, pSt->v3);

	return pSt->v0 ^ pSt->v1 ^ pSt->v2 ^ pSt->v3;
}

uint64_t __gj_sip24_final(void *ctx)
    __attribute__((alias("gj_sip24_final")));

#undef B201_SIPROUND
