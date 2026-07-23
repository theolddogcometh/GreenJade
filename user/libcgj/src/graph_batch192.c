/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch192: PEM block count + null-sep label list.
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * Builds under userspace -msse2. Complements batch115 (gj_pem_find /
 * gj_pem_decode) without redefining those symbols.
 *
 * Surface (unique symbols; RFC 7468-shaped -----BEGIN <label>-----):
 *   int gj_pem_count(const char *pem);
 *     — number of PEM BEGIN encapsulation boundaries in pem.
 *       NULL or empty → 0. Does not validate matching END lines.
 *   int gj_pem_list_labels(const char *pem, char *out, size_t cap);
 *     — write labels as null-separated strings into out[0..cap).
 *       Returns the number of labels found (same as gj_pem_count).
 *       out may be NULL or cap==0 to query count only (no write).
 *       Writes complete labels only; if the buffer cannot hold every
 *       label plus its trailing NUL, still returns the full count but
 *       stops writing (no partial label bytes). Final buffer contents
 *       are a sequence of C strings with no extra trailing empty entry.
 *   __gj_pem_count / __gj_pem_list_labels  (aliases)
 *   __libcgj_batch192_marker = "libcgj-batch192"
 *
 * No malloc, no errno (freestanding-safe). No third-party PEM source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch192_marker[] = "libcgj-batch192";

/* ---- freestanding helpers ---------------------------------------------- */

/* True if p is at buffer start or immediately after a newline. */
static int
b192_at_bol(const char *szPem, const char *p)
{
	if (p == NULL || szPem == NULL) {
		return 0;
	}
	if (p == szPem) {
		return 1;
	}
	return p[-1] == '\n';
}

/*
 * If p points at "-----BEGIN " at a line start shape already checked by
 * caller, return pointer to the first label character; else NULL.
 * Does not consume the label or the trailing -----.
 */
static const char *
b192_begin_prefix(const char *p)
{
	static const char pre[] = "-----BEGIN ";
	size_t i;

	if (p == NULL) {
		return NULL;
	}
	for (i = 0u; i < sizeof(pre) - 1u; i++) {
		if (p[i] != pre[i]) {
			return NULL;
		}
	}
	return p + (sizeof(pre) - 1u);
}

/*
 * Parse -----BEGIN <label>----- at line start p.
 * On success: *ppLabel = label start, *pcbLabel = label length,
 * returns pointer just past the trailing ----- (may be CR/LF/NUL/other).
 * Label is non-empty, case-sensitive, no internal newlines; stops before
 * the five dashes of the trailer. Returns NULL on mismatch.
 */
static const char *
b192_parse_begin(const char *p, const char **ppLabel, size_t *pcbLabel)
{
	const char *pLab;
	const char *pEndLab;
	size_t cb;

	if (p == NULL || ppLabel == NULL || pcbLabel == NULL) {
		return NULL;
	}

	pLab = b192_begin_prefix(p);
	if (pLab == NULL) {
		return NULL;
	}

	/* Label: non-empty run of non-dash, non-CR/LF bytes until "-----". */
	pEndLab = pLab;
	while (*pEndLab != '\0' && *pEndLab != '\r' && *pEndLab != '\n') {
		/* Trailer starts with five dashes. */
		if (pEndLab[0] == '-' && pEndLab[1] == '-' &&
		    pEndLab[2] == '-' && pEndLab[3] == '-' &&
		    pEndLab[4] == '-') {
			break;
		}
		/* Dashes mid-label are allowed by RFC 7468 in some labels;
		 * only a run of five ends the label field. Single dashes
		 * (e.g. "RSA PRIVATE KEY" has spaces only) — copy through
		 * anything that is not the five-dash trailer. */
		pEndLab++;
	}

	cb = (size_t)(pEndLab - pLab);
	if (cb == 0u) {
		return NULL;
	}
	/* Require trailing ----- after label. */
	if (!(pEndLab[0] == '-' && pEndLab[1] == '-' && pEndLab[2] == '-' &&
	      pEndLab[3] == '-' && pEndLab[4] == '-')) {
		return NULL;
	}

	*ppLabel = pLab;
	*pcbLabel = cb;
	return pEndLab + 5;
}

/*
 * Copy n bytes from src to dst (no overlap assumed). Freestanding — no
 * memcpy builtin reliance under -fno-builtin.
 */
static void
b192_memcpy(char *dst, const char *src, size_t n)
{
	size_t i;

	for (i = 0u; i < n; i++) {
		dst[i] = src[i];
	}
}

/* ---- gj_pem_count ------------------------------------------------------ */

/*
 * Count RFC 7468-shaped -----BEGIN <label>----- lines in pem.
 * Only boundaries at the start of the buffer or after '\n' count.
 * Matching END lines are not required. Returns 0 if pem is NULL.
 */
int
gj_pem_count(const char *pem)
{
	const char *p;
	const char *pLab;
	size_t cbLab;
	int n;

	if (pem == NULL) {
		return 0;
	}

	n = 0;
	p = pem;
	while (*p != '\0') {
		if (b192_at_bol(pem, p)) {
			const char *pRest = b192_parse_begin(p, &pLab, &cbLab);

			if (pRest != NULL) {
				/* Prefer EOL (or EOF) after trailer, like batch115. */
				if (*pRest == '\r' || *pRest == '\n' ||
				    *pRest == '\0') {
					if (n < 0x7fffffff) {
						n++;
					}
					p = pRest;
					continue;
				}
			}
		}
		p++;
	}
	return n;
}

int __gj_pem_count(const char *pem)
    __attribute__((alias("gj_pem_count")));

/* ---- gj_pem_list_labels ------------------------------------------------ */

/*
 * List PEM BEGIN labels as consecutive null-terminated strings in out.
 * Returns the number of labels present in pem (identical semantics to
 * gj_pem_count for the count). When out is non-NULL and cap > 0, writes
 * as many complete "label\\0" entries as fit in out[0..cap). Does not
 * write a partial label. When out is NULL or cap is 0, only counts.
 *
 * Example for two blocks: out receives "CERTIFICATE\\0PRIVATE KEY\\0"
 * (cap must be at least sum of (label_len+1) for each).
 */
int
gj_pem_list_labels(const char *pem, char *out, size_t cap)
{
	const char *p;
	const char *pLab;
	size_t cbLab;
	size_t off;
	int n;
	int writing;

	if (pem == NULL) {
		return 0;
	}

	writing = (out != NULL && cap > 0u) ? 1 : 0;
	off = 0u;
	n = 0;
	p = pem;
	while (*p != '\0') {
		if (b192_at_bol(pem, p)) {
			const char *pRest = b192_parse_begin(p, &pLab, &cbLab);

			if (pRest != NULL) {
				if (*pRest == '\r' || *pRest == '\n' ||
				    *pRest == '\0') {
					if (writing != 0) {
						/* Need cbLab + 1 (NUL) bytes. */
						if (off <= cap &&
						    (cap - off) >= (cbLab + 1u)) {
							b192_memcpy(out + off,
							            pLab, cbLab);
							off += cbLab;
							out[off] = '\0';
							off++;
						} else {
							/* Stop further writes;
							 * still count remaining. */
							writing = 0;
						}
					}
					if (n < 0x7fffffff) {
						n++;
					}
					p = pRest;
					continue;
				}
			}
		}
		p++;
	}
	return n;
}

int __gj_pem_list_labels(const char *pem, char *out, size_t cap)
    __attribute__((alias("gj_pem_list_labels")));
