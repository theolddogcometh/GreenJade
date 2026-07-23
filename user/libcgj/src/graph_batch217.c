/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch217: Boyer–Moore–Horspool substring search.
 *
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * Builds under userspace -msse2; this TU does not require SSE. No
 * third-party BMH source was copied.
 *
 * Surface (unique symbols):
 *   ssize_t gj_bmh_find(const char *hay, size_t hlen,
 *                       const char *needle, size_t nlen);
 *     — Find the first occurrence of needle[0..nlen) inside
 *       hay[0..hlen). Returns the byte offset of the match, or -1 if
 *       not found / invalid args.
 *     Empty needle (nlen == 0) matches at offset 0 (memmem-like).
 *     Characters compared as unsigned bytes (no locale / case fold).
 *   __gj_bmh_find  (alias)
 *   __libcgj_batch217_marker = "libcgj-batch217"
 *
 * Algorithm (Horspool bad-character shift):
 *   Precompute skip[c] = nlen for every byte, then for each
 *   needle[i] with i < nlen-1 set skip[needle[i]] = nlen-1-i.
 *   Align a window of length nlen over hay; compare right-to-left.
 *   On full match return the window base. On mismatch advance by
 *   skip[hay[window_end]] (end-of-window character).
 *   Average O(hlen/nlen) character inspections; O(1) stack (256-entry
 *   skip table). Distinct from KMP / classic full Boyer–Moore.
 */

#include <stddef.h>
#include <sys/types.h>

const char __libcgj_batch217_marker[] = "libcgj-batch217";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bmh_find — Boyer–Moore–Horspool first-match search over explicit
 * length buffers (NUL is just another byte; lengths are authoritative).
 *
 * Returns:
 *   >= 0  — offset into hay of the first match
 *   -1    — no match, or hay/needle is NULL when the corresponding
 *           length is non-zero
 */
ssize_t
gj_bmh_find(const char *hay, size_t hlen, const char *needle, size_t nlen)
{
	size_t skip[256];
	size_t i;
	size_t pos;
	size_t k;
	unsigned char endc;
	const unsigned char *h;
	const unsigned char *n;

	/* Empty pattern: conventional memmem / strstr empty match at 0. */
	if (nlen == 0u) {
		if (hay == NULL && hlen != 0u) {
			return (ssize_t)-1;
		}
		return (ssize_t)0;
	}

	if (hay == NULL || needle == NULL) {
		return (ssize_t)-1;
	}
	if (nlen > hlen) {
		return (ssize_t)-1;
	}

	h = (const unsigned char *)hay;
	n = (const unsigned char *)needle;

	/* Default shift is full needle length. */
	for (i = 0u; i < 256u; i++) {
		skip[i] = nlen;
	}
	/* Last occurrence of each prefix byte (exclude final needle byte). */
	for (i = 0u; i + 1u < nlen; i++) {
		skip[(size_t)n[i]] = nlen - 1u - i;
	}

	pos = 0u;
	while (pos + nlen <= hlen) {
		/* Right-to-left compare of the aligned window. */
		k = nlen;
		while (k > 0u) {
			k--;
			if (h[pos + k] != n[k]) {
				break;
			}
			if (k == 0u) {
				return (ssize_t)pos;
			}
		}
		/* Mismatch (or broken early): Horspool shift on window end. */
		endc = h[pos + nlen - 1u];
		pos += skip[(size_t)endc];
	}

	return (ssize_t)-1;
}

ssize_t __gj_bmh_find(const char *hay, size_t hlen, const char *needle,
    size_t nlen) __attribute__((alias("gj_bmh_find")));
