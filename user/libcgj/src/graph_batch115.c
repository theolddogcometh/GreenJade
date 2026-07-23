/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch115: PEM begin/end extract + base64 decode.
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * Userspace may be built with -msse2; this TU does not require SSE.
 *
 * Surface (RFC 7468-shaped encapsulation boundaries):
 *   int gj_pem_find(const char *pem, const char *label,
 *                   const char **body, size_t *body_len);
 *     Locate -----BEGIN <label>----- … -----END <label>----- and return a
 *     pointer/length to the base64 text between the boundaries (not including
 *     the END line). Returns 0 on success, -1 on error (errno set).
 *   int gj_pem_decode(const char *pem, const char *label,
 *                     unsigned char *out, size_t outcap, size_t *outlen);
 *     gj_pem_find + base64_decode (batch40). out may be NULL to query size
 *     via outlen (same rules as base64_decode). Returns 0 / -1.
 *   __gj_pem_find / __gj_pem_decode  (aliases)
 *   __libcgj_batch115_marker = "libcgj-batch115"
 *
 * base64_decode lives in graph_batch40.c — declared extern here (no multi-def).
 * No third-party PEM source was copied.
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h> /* ssize_t for base64_decode */

const char __libcgj_batch115_marker[] = "libcgj-batch115";

/* ---- extern: base64 from graph_batch40 (do not redefine) --------------- */

extern ssize_t base64_decode(const char *szIn, size_t cbIn,
                             unsigned char *pOut, size_t cbOut);

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b115_strlen(const char *s)
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

/*
 * Match -----KIND LABEL----- at p (KIND is "BEGIN" or "END").
 * Returns pointer just past the trailing ----- on match, else NULL.
 * Label must match exactly (case-sensitive); no extra spaces.
 */
static const char *
b115_match_boundary(const char *p, const char *szKind, const char *szLabel)
{
	size_t i;
	size_t cbKind;
	size_t cbLabel;

	if (p == NULL || szKind == NULL || szLabel == NULL) {
		return NULL;
	}

	/* "-----" */
	if (p[0] != '-' || p[1] != '-' || p[2] != '-' || p[3] != '-' ||
	    p[4] != '-') {
		return NULL;
	}
	p += 5;

	cbKind = b115_strlen(szKind);
	for (i = 0u; i < cbKind; i++) {
		if (p[i] != szKind[i]) {
			return NULL;
		}
	}
	p += cbKind;

	if (*p != ' ') {
		return NULL;
	}
	p++;

	cbLabel = b115_strlen(szLabel);
	if (cbLabel == 0u) {
		return NULL;
	}
	for (i = 0u; i < cbLabel; i++) {
		if (p[i] != szLabel[i]) {
			return NULL;
		}
	}
	p += cbLabel;

	/* "-----" */
	if (p[0] != '-' || p[1] != '-' || p[2] != '-' || p[3] != '-' ||
	    p[4] != '-') {
		return NULL;
	}
	return p + 5;
}

/* Advance past optional CR and a single LF (end of encapsulation line). */
static const char *
b115_skip_eol(const char *p)
{
	if (p == NULL) {
		return NULL;
	}
	if (*p == '\r') {
		p++;
	}
	if (*p == '\n') {
		p++;
	}
	return p;
}

/* True if p is at the start of the buffer or immediately after a newline. */
static int
b115_at_bol(const char *szPem, const char *p)
{
	if (p == NULL || szPem == NULL) {
		return 0;
	}
	if (p == szPem) {
		return 1;
	}
	return p[-1] == '\n';
}

/* ---- public: find ------------------------------------------------------ */

/*
 * Extract the base64 body for a PEM block with the given label.
 * body points into pem; body_len is the byte count up to (not including)
 * the -----END line. Leading/trailing whitespace inside the body is left
 * for base64_decode to skip.
 */
int
gj_pem_find(const char *pem, const char *label, const char **body,
            size_t *body_len)
{
	const char *p;
	const char *pAfterBegin;
	const char *pBody;
	const char *pEnd;

	if (pem == NULL || label == NULL || body == NULL || body_len == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (label[0] == '\0') {
		errno = EINVAL;
		return -1;
	}

	/* Scan for -----BEGIN label----- at a line start. */
	p = pem;
	pAfterBegin = NULL;
	while (*p != '\0') {
		if (b115_at_bol(pem, p)) {
			const char *pRest = b115_match_boundary(p, "BEGIN", label);

			if (pRest != NULL) {
				/* Require EOL after the boundary (RFC 7468). */
				if (*pRest == '\r' || *pRest == '\n' ||
				    *pRest == '\0') {
					pAfterBegin = pRest;
					break;
				}
			}
		}
		p++;
	}
	if (pAfterBegin == NULL) {
		errno = EINVAL;
		return -1;
	}

	pBody = b115_skip_eol(pAfterBegin);
	/* If BEGIN had no newline and hit EOF, body is empty / invalid. */
	if (pBody == pAfterBegin && *pBody == '\0') {
		errno = EINVAL;
		return -1;
	}

	/* Find matching -----END label----- at a line start. */
	p = pBody;
	pEnd = NULL;
	while (*p != '\0') {
		if (b115_at_bol(pem, p)) {
			const char *pRest = b115_match_boundary(p, "END", label);

			if (pRest != NULL) {
				pEnd = p;
				break;
			}
		}
		p++;
	}
	if (pEnd == NULL) {
		errno = EINVAL;
		return -1;
	}

	*body = pBody;
	*body_len = (size_t)(pEnd - pBody);
	return 0;
}

/* ---- public: decode ---------------------------------------------------- */

/*
 * Locate the PEM body for label and base64-decode it into out.
 * out == NULL is allowed (base64_decode size-query path); outlen receives
 * the decoded length on success when non-NULL.
 */
int
gj_pem_decode(const char *pem, const char *label, unsigned char *out,
              size_t outcap, size_t *outlen)
{
	const char *pBody;
	size_t cbBody;
	ssize_t n;

	if (gj_pem_find(pem, label, &pBody, &cbBody) != 0) {
		return -1;
	}

	n = base64_decode(pBody, cbBody, out, outcap);
	if (n < 0) {
		return -1;
	}
	if (outlen != NULL) {
		*outlen = (size_t)n;
	}
	return 0;
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_pem_find(const char *pem, const char *label, const char **body,
                  size_t *body_len)
    __attribute__((alias("gj_pem_find")));
int __gj_pem_decode(const char *pem, const char *label, unsigned char *out,
                    size_t outcap, size_t *outlen)
    __attribute__((alias("gj_pem_decode")));
