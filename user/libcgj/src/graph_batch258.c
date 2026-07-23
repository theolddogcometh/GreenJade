/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch258: streaming RFC 4648 base64 decoder state.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No third-party source copied. No malloc, no errno.
 *
 * Existing one-shot base64 is NOT redefined:
 *   base64_encode / base64_decode           → graph_batch40.c
 *   b64_ntop / b64_pton                     → graph_batch22.c
 *   base64url_encode / base64url_decode     → graph_batch93.c
 *   gj_b64url_encode_pad / decode_pad       → graph_batch186.c
 *   gj_b64_enc_init / update / final        → graph_batch206.c
 *
 * This TU adds only unique streaming decoder symbols:
 *   void   gj_b64_dec_init(void *state);
 *   size_t gj_b64_dec_update(void *state, const char *in, size_t nin,
 *                            unsigned char *out, size_t out_cap);
 *   size_t gj_b64_dec_final(void *state, unsigned char *out, size_t out_cap);
 *   __gj_b64_dec_init / __gj_b64_dec_update / __gj_b64_dec_final  (aliases)
 *   __libcgj_batch258_marker = "libcgj-batch258"
 *
 * State buffer:
 *   Caller-owned opaque buffer; size MUST be >= 16 bytes (GJ_B64_DEC_STATE_SIZE).
 *   Layout is private; treat as opaque. After gj_b64_dec_final the state is
 *   finalized (further update/final is undefined); re-init to reuse.
 *
 * Streaming behaviour (RFC 4648 §4 alphabet + optional whitespace):
 *   update() absorbs base64 text (A-Za-z0-9+/ and '='), skips SP/HT/LF/CR,
 *   and emits complete decoded octets for finished quartets only.
 *   final() flushes a trailing unpadded 2- or 3-char remainder (1 or 2
 *   octets), or returns 0 if the stream already ended on a complete
 *   quartet (with or without '='). A single leftover alphabet char is
 *   invalid. After a padded quartet, only whitespace may follow.
 *
 * Return codes (update/final):
 *   success → number of octets written to out (possibly 0)
 *   error   → (size_t)-1  (NULL state, bad alphabet, ENOSPC, use-after-final,
 *              data after padding, incomplete single-char remainder, etc.)
 *   update with in==NULL && nin!=0 → (size_t)-1
 *   update with out==NULL && needed>0 → (size_t)-1 (no size-query mode)
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch258_marker[] = "libcgj-batch258";

/* Documented minimum state size (bytes). Callers must allocate at least this. */
enum { GJ_B64_DEC_STATE_SIZE = 16 };

/*
 * Internal streaming decoder state. sizeof is well under 16; the public
 * contract still requires a 16-byte buffer for ABI headroom.
 *
 * acc[] holds 0..3 alphabet sextet values (0..63) for the open quartet.
 * n_pad is pads seen in the open quartet (0..2). n_slot = n_acc + n_pad.
 * When n_slot hits 4, a quartet is complete and is decoded.
 * f_ended is set after a padded quartet; only whitespace may follow.
 */
struct b258_b64_dec {
	unsigned char acc[3];	/* alphabet sextets in open quartet (max 3) */
	unsigned char n_acc;	/* 0..3 */
	unsigned char n_pad;	/* 0..2 pads in open quartet */
	unsigned char f_ended;	/* non-zero after padded quartet completed */
	unsigned char f_done;	/* non-zero after final() */
	unsigned char reserved;	/* pad / future */
};

/* Compile-time guard: internal state must fit in the documented buffer. */
typedef char b258_state_size_ok[
    (sizeof(struct b258_b64_dec) <= (size_t)GJ_B64_DEC_STATE_SIZE) ? 1 : -1];

/* ---- helpers ----------------------------------------------------------- */

/* Map one RFC 4648 §4 alphabet character to 0..63, or -1 if invalid. */
static int
b258_b64_val(int ch)
{
	if (ch >= 'A' && ch <= 'Z') {
		return ch - 'A';
	}
	if (ch >= 'a' && ch <= 'z') {
		return ch - 'a' + 26;
	}
	if (ch >= '0' && ch <= '9') {
		return ch - '0' + 52;
	}
	if (ch == '+') {
		return 62;
	}
	if (ch == '/') {
		return 63;
	}
	return -1;
}

static int
b258_is_ws(int ch)
{
	return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
}

/*
 * Decode one complete quartet described by n_acc alphabet sextets in pAcc
 * and n_pad padding marks. Writes 1..3 octets to pOut.
 * Returns octets written, or -1 if the pad/alphabet combination is illegal.
 *
 * Legal shapes (RFC 4648):
 *   n_acc==4, n_pad==0 → 3 octets
 *   n_acc==3, n_pad==1 → 2 octets
 *   n_acc==2, n_pad==2 → 1 octet
 *   anything else      → invalid
 * (n_acc + n_pad must be 4; caller guarantees that.)
 */
static int
b258_dec_quartet(const unsigned char *pAcc, unsigned n_acc, unsigned n_pad,
                 unsigned char *pOut)
{
	unsigned u0;
	unsigned u1;
	unsigned u2;
	unsigned uTrip;

	if (n_acc + n_pad != 4u) {
		return -1;
	}
	if (n_acc < 2u || n_acc > 4u) {
		return -1;
	}
	if (n_pad > 2u) {
		return -1;
	}
	/* Pads only at the end: n_pad==1 ⇒ n_acc==3; n_pad==2 ⇒ n_acc==2. */
	if (n_pad == 1u && n_acc != 3u) {
		return -1;
	}
	if (n_pad == 2u && n_acc != 2u) {
		return -1;
	}

	u0 = pAcc[0];
	u1 = pAcc[1];
	u2 = (n_acc > 2u) ? pAcc[2] : 0u;

	if (n_acc == 4u) {
		/* Full quartet: pAcc[0..3] are all sextets (caller buffer). */
		uTrip = (u0 << 18) | (u1 << 12) | (u2 << 6) | (unsigned)pAcc[3];
		pOut[0] = (unsigned char)(uTrip >> 16);
		pOut[1] = (unsigned char)(uTrip >> 8);
		pOut[2] = (unsigned char)uTrip;
		return 3;
	}

	uTrip = (u0 << 18) | (u1 << 12) | (u2 << 6);

	if (n_pad == 2u) {
		/* AB== → 1 octet; low 4 bits of second sextet should be 0
		 * for canonical encoding — accept non-canonical bits. */
		pOut[0] = (unsigned char)(uTrip >> 16);
		return 1;
	}

	/* n_pad == 1: ABC= → 2 octets */
	pOut[0] = (unsigned char)(uTrip >> 16);
	pOut[1] = (unsigned char)(uTrip >> 8);
	return 2;
}

/*
 * Emit a complete open quartet from state into out, advancing *pnWrote.
 * On success clears open-quartet fields. Returns 0 or -1.
 * n_full_acc is 2..4; when 4, pFull[0..3] holds all sextets (state.acc holds 3).
 */
static int
b258_flush_quartet(struct b258_b64_dec *pSt, const unsigned char *pFull,
                   unsigned n_acc, unsigned n_pad, unsigned char *out,
                   size_t out_cap, size_t *pnWrote)
{
	unsigned char aTmp[4];
	int nOut;
	size_t nNeed;
	unsigned i;

	if (n_acc == 4u) {
		for (i = 0u; i < 4u; i++) {
			aTmp[i] = pFull[i];
		}
	} else {
		for (i = 0u; i < n_acc; i++) {
			aTmp[i] = pSt->acc[i];
		}
	}

	/* Probe length without writing when out is tight — decode to stack. */
	{
		unsigned char aProbe[3];

		nOut = b258_dec_quartet(aTmp, n_acc, n_pad, aProbe);
		if (nOut < 0) {
			return -1;
		}
		nNeed = (size_t)nOut;
		if (nNeed > 0u && out == NULL) {
			return -1;
		}
		if (*pnWrote + nNeed > out_cap) {
			return -1;
		}
		for (i = 0u; i < nNeed; i++) {
			out[*pnWrote + i] = aProbe[i];
		}
		*pnWrote += nNeed;
	}

	pSt->n_acc = 0u;
	pSt->n_pad = 0u;
	pSt->acc[0] = 0u;
	pSt->acc[1] = 0u;
	pSt->acc[2] = 0u;
	if (n_pad != 0u) {
		pSt->f_ended = 1u;
	}
	return 0;
}

/* ---- gj_b64_dec_init --------------------------------------------------- */

/*
 * Initialize streaming base64 decoder state.
 * state must point to at least GJ_B64_DEC_STATE_SIZE (16) bytes.
 * NULL state is a no-op.
 */
void
gj_b64_dec_init(void *state)
{
	struct b258_b64_dec *pSt;

	if (state == NULL) {
		return;
	}

	pSt = (struct b258_b64_dec *)state;
	pSt->acc[0] = 0u;
	pSt->acc[1] = 0u;
	pSt->acc[2] = 0u;
	pSt->n_acc = 0u;
	pSt->n_pad = 0u;
	pSt->f_ended = 0u;
	pSt->f_done = 0u;
	pSt->reserved = 0u;

	(void)sizeof(b258_state_size_ok);
}

void __gj_b64_dec_init(void *state) __attribute__((alias("gj_b64_dec_init")));

/* ---- gj_b64_dec_update ------------------------------------------------- */

/*
 * Absorb nin chars from in; emit complete decoded quartets to out.
 * Pending 0..3 sextets / pads stay in state until more input or final().
 *
 * Returns octets written or (size_t)-1 on error.
 * out_cap must cover all complete groups this call produces (at most
 * 3 octets per finished quartet). Capacity is enforced per quartet.
 */
size_t
gj_b64_dec_update(void *state, const char *in, size_t nin, unsigned char *out,
                  size_t out_cap)
{
	struct b258_b64_dec *pSt;
	const char *p;
	size_t nLeft;
	size_t nWrote;
	unsigned char aFull[4];

	if (state == NULL) {
		return (size_t)-1;
	}
	pSt = (struct b258_b64_dec *)state;
	if (pSt->f_done != 0u) {
		return (size_t)-1;
	}
	if (nin == 0u) {
		return 0u;
	}
	if (in == NULL) {
		return (size_t)-1;
	}

	/*
	 * Capacity is checked per completed quartet in b258_flush_quartet.
	 * A single pre-pass upper bound is wrong when input is whitespace-
	 * heavy (would spuriously ENOSPC). out==NULL with actual output
	 * still fails inside flush.
	 */

	p = in;
	nLeft = nin;
	nWrote = 0u;

	while (nLeft > 0u) {
		int ch = (unsigned char)*p;
		int nVal;

		p++;
		nLeft--;

		if (b258_is_ws(ch)) {
			continue;
		}

		if (pSt->f_ended != 0u) {
			/* Non-whitespace after a padded quartet. */
			return (size_t)-1;
		}

		if (ch == '=') {
			/* Padding only after at least 2 alphabet chars, and
			 * only while the open quartet is unfinished. */
			if (pSt->n_acc < 2u) {
				return (size_t)-1;
			}
			if (pSt->n_pad >= 2u) {
				return (size_t)-1;
			}
			/* Once padding starts, no more alphabet in this quartet. */
			pSt->n_pad++;
			if ((unsigned)pSt->n_acc + (unsigned)pSt->n_pad == 4u) {
				if (b258_flush_quartet(pSt, NULL, pSt->n_acc,
				                       pSt->n_pad, out, out_cap,
				                       &nWrote) != 0) {
					return (size_t)-1;
				}
			}
			continue;
		}

		/* Alphabet char — illegal if padding already started. */
		if (pSt->n_pad != 0u) {
			return (size_t)-1;
		}

		nVal = b258_b64_val(ch);
		if (nVal < 0) {
			return (size_t)-1;
		}

		if (pSt->n_acc < 3u) {
			pSt->acc[pSt->n_acc] = (unsigned char)nVal;
			pSt->n_acc++;
		} else {
			/* 4th alphabet sextet completes an unpadded quartet. */
			aFull[0] = pSt->acc[0];
			aFull[1] = pSt->acc[1];
			aFull[2] = pSt->acc[2];
			aFull[3] = (unsigned char)nVal;
			/* n_acc becomes 4 for the flush; clear via flush. */
			pSt->n_acc = 3u; /* still 3 in acc; pass 4 via aFull */
			if (b258_flush_quartet(pSt, aFull, 4u, 0u, out, out_cap,
			                       &nWrote) != 0) {
				return (size_t)-1;
			}
		}
	}

	return nWrote;
}

size_t __gj_b64_dec_update(void *state, const char *in, size_t nin,
                           unsigned char *out, size_t out_cap)
    __attribute__((alias("gj_b64_dec_update")));

/* ---- gj_b64_dec_final -------------------------------------------------- */

/*
 * Flush trailing unpadded remainder:
 *   n_acc==0, n_pad==0 → 0 octets (clean end, or already ended via pad)
 *   n_acc==2, n_pad==0 → 1 octet  (implicit AB==)
 *   n_acc==3, n_pad==0 → 2 octets (implicit ABC=)
 *   n_acc==1           → error
 *   n_pad!=0 (incomplete pad quartet) → error
 *
 * Marks state finalized. Returns octets written or (size_t)-1 on error.
 */
size_t
gj_b64_dec_final(void *state, unsigned char *out, size_t out_cap)
{
	struct b258_b64_dec *pSt;
	size_t nWrote;
	unsigned char aProbe[3];
	int nOut;
	unsigned i;

	if (state == NULL) {
		return (size_t)-1;
	}
	pSt = (struct b258_b64_dec *)state;
	if (pSt->f_done != 0u) {
		return (size_t)-1;
	}

	/* Incomplete padding in the open quartet is not allowed. */
	if (pSt->n_pad != 0u) {
		return (size_t)-1;
	}

	if (pSt->n_acc == 0u) {
		pSt->f_done = 1u;
		return 0u;
	}

	if (pSt->n_acc == 1u) {
		return (size_t)-1;
	}

	/* Implicit padding for n_acc==2 or 3. */
	nOut = b258_dec_quartet(pSt->acc, pSt->n_acc,
	                        4u - (unsigned)pSt->n_acc, aProbe);
	if (nOut < 0) {
		return (size_t)-1;
	}
	if (out == NULL || out_cap < (size_t)nOut) {
		return (size_t)-1;
	}

	nWrote = (size_t)nOut;
	for (i = 0u; i < nWrote; i++) {
		out[i] = aProbe[i];
	}

	pSt->acc[0] = 0u;
	pSt->acc[1] = 0u;
	pSt->acc[2] = 0u;
	pSt->n_acc = 0u;
	pSt->n_pad = 0u;
	pSt->f_done = 1u;
	return nWrote;
}

size_t __gj_b64_dec_final(void *state, unsigned char *out, size_t out_cap)
    __attribute__((alias("gj_b64_dec_final")));
