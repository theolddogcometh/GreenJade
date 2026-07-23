/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch204: TLS 1.3 HkdfLabel-style HKDF-Expand-Label
 * (RFC 8446 §7.1 simplified, SHA-256).
 *
 * Builds the TLS 1.3 HkdfLabel info blob and expands via existing
 * hkdf_sha256_expand (graph_batch71). Does not redefine HKDF extract/
 * expand one-shots (graph_batch40/71) or HMAC-SHA256 (graph_batch38).
 *
 * Surface (unique symbols):
 *   int gj_hkdf_expand_label(const unsigned char *secret, size_t slen,
 *                            const char *label,
 *                            const unsigned char *ctx, size_t clen,
 *                            unsigned char *out, size_t outlen);
 *     — HKDF-Expand-Label(Secret, Label, Context, Length):
 *         info = uint16_be(outlen) || u8(labellen) || "tls13 " || label
 *                || u8(clen) || ctx
 *         then HKDF-Expand-SHA256(secret, info, outlen).
 *       Returns 0 on success, -1 on error.
 *   __gj_hkdf_expand_label  (alias)
 *   __libcgj_batch204_marker = "libcgj-batch204"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno (freestanding-safe).
 * No third-party TLS source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch204_marker[] = "libcgj-batch204";

/* Existing expand surface (batch71) — extern only. */
int hkdf_sha256_expand(const unsigned char *pPrk, size_t cbPrk,
                       const void *pInfo, size_t cbInfo, unsigned char *pOkm,
                       size_t cbOkm);

/* "tls13 " prefix (RFC 8446); length 6. */
#define B204_TLS13_PREFIX     "tls13 "
#define B204_TLS13_PREFIX_LEN 6u

/*
 * hkdf_sha256_expand caps cbInfo at 256; HkdfLabel must fit that stack
 * bound. Length field is uint16, so outlen also fits in 16 bits.
 */
#define B204_INFO_MAX   256u
#define B204_HASH_LEN   32u
#define B204_OKM_MAX    (255u * B204_HASH_LEN)
#define B204_CTX_MAX    255u
#define B204_LABEL_MAX  255u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b204_strlen(const char *s)
{
	size_t n = 0u;

	if (s == NULL) {
		return 0u;
	}
	while (s[n] != '\0') {
		n++;
	}
	return n;
}

static void
b204_memcpy(void *pDst, const void *pSrc, size_t cb)
{
	unsigned char *d = (unsigned char *)pDst;
	const unsigned char *s = (const unsigned char *)pSrc;
	size_t i;

	for (i = 0u; i < cb; i++) {
		d[i] = s[i];
	}
}

static void
b204_memzero(void *pDst, size_t cb)
{
	unsigned char *p = (unsigned char *)pDst;
	size_t i;

	for (i = 0u; i < cb; i++) {
		p[i] = 0;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hkdf_expand_label — TLS 1.3 HKDF-Expand-Label (SHA-256).
 *
 * secret/slen: PRK (typically HashLen); secret non-NULL, slen > 0.
 * label: C string (may be empty); concatenated after "tls13 ".
 * ctx/clen: application context; ctx may be NULL only when clen == 0.
 *           clen must be <= 255.
 * out/outlen: output OKM; outlen in 1..255*HashLen and <= 65535.
 *
 * The encoded label field ("tls13 " || label) must be 1..255 octets and
 * the full HkdfLabel must fit in 256 octets (expand info cap).
 *
 * Returns 0 on success, -1 on invalid args or expand failure.
 */
int
gj_hkdf_expand_label(const unsigned char *secret, size_t slen,
                     const char *label, const unsigned char *ctx, size_t clen,
                     unsigned char *out, size_t outlen)
{
	unsigned char aInfo[B204_INFO_MAX];
	size_t cbLabelBody;
	size_t cbFullLabel;
	size_t n;
	int rc;

	if (secret == NULL || slen == 0u || out == NULL || outlen == 0u) {
		return -1;
	}
	if (label == NULL) {
		return -1;
	}
	if (ctx == NULL && clen != 0u) {
		return -1;
	}
	if (clen > B204_CTX_MAX) {
		return -1;
	}
	if (outlen > B204_OKM_MAX || outlen > 0xffffu) {
		return -1;
	}

	cbLabelBody = b204_strlen(label);
	cbFullLabel = B204_TLS13_PREFIX_LEN + cbLabelBody;
	if (cbFullLabel == 0u || cbFullLabel > B204_LABEL_MAX) {
		return -1;
	}

	/* 2 (length) + 1 (labellen) + full_label + 1 (ctxlen) + ctx */
	n = 2u + 1u + cbFullLabel + 1u + clen;
	if (n > B204_INFO_MAX) {
		return -1;
	}

	/* uint16 length = outlen (big-endian) */
	aInfo[0] = (unsigned char)((outlen >> 8) & 0xffu);
	aInfo[1] = (unsigned char)(outlen & 0xffu);

	/* opaque label<7..255> = "tls13 " + Label  (simplified: 1..255) */
	aInfo[2] = (unsigned char)cbFullLabel;
	b204_memcpy(aInfo + 3u, B204_TLS13_PREFIX, B204_TLS13_PREFIX_LEN);
	if (cbLabelBody > 0u) {
		b204_memcpy(aInfo + 3u + B204_TLS13_PREFIX_LEN, label,
		            cbLabelBody);
	}

	/* opaque context<0..255> */
	aInfo[3u + cbFullLabel] = (unsigned char)clen;
	if (clen > 0u) {
		b204_memcpy(aInfo + 4u + cbFullLabel, ctx, clen);
	}

	rc = hkdf_sha256_expand(secret, slen, aInfo, n, out, outlen);
	b204_memzero(aInfo, sizeof(aInfo));
	return (rc == 0) ? 0 : -1;
}

int __gj_hkdf_expand_label(const unsigned char *secret, size_t slen,
                           const char *label, const unsigned char *ctx,
                           size_t clen, unsigned char *out, size_t outlen)
    __attribute__((alias("gj_hkdf_expand_label")));
