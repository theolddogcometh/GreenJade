/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch206: streaming RFC 4648 base64 encoder state.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No third-party source copied. No malloc, no errno.
 *
 * Existing one-shot base64 is NOT redefined:
 *   base64_encode / base64_decode           → graph_batch40.c
 *   b64_ntop / b64_pton                     → graph_batch22.c
 *   base64url_encode / base64url_decode     → graph_batch93.c
 *   gj_b64url_encode_pad / decode_pad       → graph_batch186.c
 *
 * This TU adds only unique streaming encoder symbols:
 *   void   gj_b64_enc_init(void *ctx);
 *   size_t gj_b64_enc_update(void *ctx, const void *in, size_t inlen,
 *                            char *out, size_t outcap);
 *   size_t gj_b64_enc_final(void *ctx, char *out, size_t outcap);
 *   __gj_b64_enc_init / __gj_b64_enc_update / __gj_b64_enc_final  (aliases)
 *   __libcgj_batch206_marker = "libcgj-batch206"
 *
 * Context buffer:
 *   Caller-owned opaque buffer; size MUST be >= 32 bytes (GJ_B64_ENC_CTX_SIZE).
 *   Layout is private; treat as opaque. After gj_b64_enc_final the context is
 *   finalized (further update/final is undefined); re-init to reuse.
 *
 * Streaming behaviour (standard alphabet + padding):
 *   update() absorbs binary octets and emits only complete 4-char groups
 *   (no '=' until final). final() flushes 0..2 pending input bytes with
 *   canonical '=' padding (0 or 4 output chars). Chunks are not
 *   NUL-terminated; concatenate update outputs then final output.
 *
 * Return codes (update/final):
 *   success → number of chars written to out (multiple of 4, possibly 0)
 *   error   → (size_t)-1  (NULL ctx, ENOSPC-style short out, use-after-final)
 *   update with in==NULL && inlen!=0 → (size_t)-1
 *   update with out==NULL && needed>0 → (size_t)-1 (no size-query mode)
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch206_marker[] = "libcgj-batch206";

/* Documented minimum context size (bytes). Callers must allocate at least this. */
enum { GJ_B64_ENC_CTX_SIZE = 32 };

/* RFC 4648 §4 standard base64 alphabet */
static const char s_b206_b64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/*
 * Internal streaming encoder state. sizeof is well under 32; the public
 * contract still requires a 32-byte buffer for ABI headroom.
 */
struct b206_b64_enc {
	unsigned char rem[2];	/* 0..2 leftover input octets */
	unsigned rem_n;		/* bytes valid in rem[] */
	unsigned done;		/* non-zero after final() */
	unsigned reserved;	/* pad / future */
};

/* Compile-time guard: internal state must fit in the documented buffer. */
typedef char b206_ctx_size_ok[
    (sizeof(struct b206_b64_enc) <= (size_t)GJ_B64_ENC_CTX_SIZE) ? 1 : -1];

/* ---- helpers ----------------------------------------------------------- */

/* Encode one full 3-byte triple → 4 alphabet chars (no padding). */
static void
b206_enc_triple(unsigned u0, unsigned u1, unsigned u2, char *pOut)
{
	unsigned uTrip = (u0 << 16) | (u1 << 8) | u2;

	pOut[0] = s_b206_b64[(uTrip >> 18) & 63u];
	pOut[1] = s_b206_b64[(uTrip >> 12) & 63u];
	pOut[2] = s_b206_b64[(uTrip >> 6) & 63u];
	pOut[3] = s_b206_b64[uTrip & 63u];
}

/* ---- gj_b64_enc_init --------------------------------------------------- */

/*
 * Initialize streaming base64 encoder state in ctx.
 * ctx must point to at least GJ_B64_ENC_CTX_SIZE (32) bytes of writable storage.
 * NULL ctx is a no-op.
 */
void
gj_b64_enc_init(void *ctx)
{
	struct b206_b64_enc *pSt;

	if (ctx == NULL) {
		return;
	}

	pSt = (struct b206_b64_enc *)ctx;
	pSt->rem[0] = 0u;
	pSt->rem[1] = 0u;
	pSt->rem_n = 0u;
	pSt->done = 0u;
	pSt->reserved = 0u;

	/* Silence unused-typedef under some pedantic modes. */
	(void)sizeof(b206_ctx_size_ok);
}

void __gj_b64_enc_init(void *ctx) __attribute__((alias("gj_b64_enc_init")));

/* ---- gj_b64_enc_update ------------------------------------------------- */

/*
 * Absorb inlen octets from in; emit complete 4-char base64 groups to out.
 * Pending 0..2 octets stay in ctx until more input or final().
 *
 * Returns chars written (0, 4, 8, ...) or (size_t)-1 on error.
 * outcap must be large enough for all complete groups this call produces:
 *   4 * ((rem_n + inlen) / 3)
 */
size_t
gj_b64_enc_update(void *ctx, const void *in, size_t inlen, char *out,
                  size_t outcap)
{
	struct b206_b64_enc *pSt;
	const unsigned char *p;
	size_t nLeft;
	size_t nWrote;
	size_t nNeed;
	size_t nTotalIn;

	if (ctx == NULL) {
		return (size_t)-1;
	}
	pSt = (struct b206_b64_enc *)ctx;
	if (pSt->done != 0u) {
		return (size_t)-1;
	}
	if (inlen == 0u) {
		return 0u;
	}
	if (in == NULL) {
		return (size_t)-1;
	}

	nTotalIn = (size_t)pSt->rem_n + inlen;
	nNeed = 4u * (nTotalIn / 3u);
	if (nNeed > 0u && out == NULL) {
		return (size_t)-1;
	}
	if (nNeed > outcap) {
		return (size_t)-1;
	}

	p = (const unsigned char *)in;
	nLeft = inlen;
	nWrote = 0u;

	/* Drain pending remainder first, filling to a full triple. */
	if (pSt->rem_n != 0u) {
		while (nLeft > 0u && pSt->rem_n < 3u) {
			/* rem only holds 2; third byte comes from stream. */
			if (pSt->rem_n < 2u) {
				pSt->rem[pSt->rem_n] = *p;
				pSt->rem_n++;
				p++;
				nLeft--;
			} else {
				/* rem has 2; take one more → full triple */
				unsigned u0 = pSt->rem[0];
				unsigned u1 = pSt->rem[1];
				unsigned u2 = *p;

				b206_enc_triple(u0, u1, u2, out + nWrote);
				nWrote += 4u;
				pSt->rem_n = 0u;
				pSt->rem[0] = 0u;
				pSt->rem[1] = 0u;
				p++;
				nLeft--;
			}
		}
		/* If we still have rem and no more input, stop (partial). */
	}

	/* Full triples from the stream. */
	while (nLeft >= 3u) {
		b206_enc_triple(p[0], p[1], p[2], out + nWrote);
		nWrote += 4u;
		p += 3u;
		nLeft -= 3u;
	}

	/* Stash 0..2 remainder. */
	while (nLeft > 0u) {
		pSt->rem[pSt->rem_n] = *p;
		pSt->rem_n++;
		p++;
		nLeft--;
	}

	return nWrote;
}

size_t __gj_b64_enc_update(void *ctx, const void *in, size_t inlen, char *out,
                           size_t outcap)
    __attribute__((alias("gj_b64_enc_update")));

/* ---- gj_b64_enc_final -------------------------------------------------- */

/*
 * Flush pending 0..2 input octets with canonical '=' padding.
 *   rem_n == 0 → write 0 chars
 *   rem_n == 1 → write 4 chars  (xx==)
 *   rem_n == 2 → write 4 chars  (xxx=)
 *
 * Marks ctx finalized. Returns chars written or (size_t)-1 on error.
 * Does not write a trailing NUL.
 */
size_t
gj_b64_enc_final(void *ctx, char *out, size_t outcap)
{
	struct b206_b64_enc *pSt;
	unsigned u0;
	unsigned u1;
	unsigned uTrip;

	if (ctx == NULL) {
		return (size_t)-1;
	}
	pSt = (struct b206_b64_enc *)ctx;
	if (pSt->done != 0u) {
		return (size_t)-1;
	}

	if (pSt->rem_n == 0u) {
		pSt->done = 1u;
		return 0u;
	}

	if (out == NULL || outcap < 4u) {
		return (size_t)-1;
	}

	u0 = pSt->rem[0];
	u1 = (pSt->rem_n > 1u) ? pSt->rem[1] : 0u;
	uTrip = (u0 << 16) | (u1 << 8);

	out[0] = s_b206_b64[(uTrip >> 18) & 63u];
	out[1] = s_b206_b64[(uTrip >> 12) & 63u];
	if (pSt->rem_n > 1u) {
		out[2] = s_b206_b64[(uTrip >> 6) & 63u];
		out[3] = '=';
	} else {
		out[2] = '=';
		out[3] = '=';
	}

	pSt->rem[0] = 0u;
	pSt->rem[1] = 0u;
	pSt->rem_n = 0u;
	pSt->done = 1u;
	return 4u;
}

size_t __gj_b64_enc_final(void *ctx, char *out, size_t outcap)
    __attribute__((alias("gj_b64_enc_final")));
