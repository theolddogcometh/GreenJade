/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch237: longest common prefix length of two C
 * strings.
 *
 * Surface (unique symbols):
 *   size_t gj_lcp_len(const char *a, const char *b);
 *     — number of leading characters that match between a and b
 *       (NUL-terminated). Stops at the first mismatch or either string's
 *       terminator. Returns 0 if either pointer is NULL or either string
 *       is empty (or if the first characters already differ).
 *   __gj_lcp_len  (alias)
 *   __libcgj_batch237_marker = "libcgj-batch237"
 *
 * Clean-room freestanding pure C — integer/pointer only (no SSE
 * required; builds under userspace -msse2). No malloc, no libc string
 * dependency (-fno-builtin safe). No third-party source was copied.
 *
 * Distinct from LCS length (batch214) and character Hamming (batch213):
 * LCP is a single linear scan of the shared prefix only.
 */

#include <stddef.h>

const char __libcgj_batch237_marker[] = "libcgj-batch237";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b237_byte_eq — compare two characters as unsigned bytes.
 * Returns 1 if equal, 0 otherwise. NUL is a valid byte value here only
 * when both sides are NUL (caller stops before that case for length).
 */
static int
b237_byte_eq(unsigned char ca, unsigned char cb)
{
	return (ca == cb) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lcp_len — longest common prefix length of two C strings.
 *
 * Walks a and b in lockstep while corresponding bytes match and neither
 * has ended. NULL either argument yields 0. Empty string yields 0.
 * Characters are compared as unsigned bytes (no locale / case folding).
 */
size_t
gj_lcp_len(const char *a, const char *b)
{
	size_t n;

	if (a == NULL || b == NULL) {
		return 0u;
	}

	n = 0u;
	while (a[n] != '\0' && b[n] != '\0' &&
	       b237_byte_eq((unsigned char)a[n], (unsigned char)b[n])) {
		n++;
	}
	return n;
}

size_t __gj_lcp_len(const char *a, const char *b)
    __attribute__((alias("gj_lcp_len")));
