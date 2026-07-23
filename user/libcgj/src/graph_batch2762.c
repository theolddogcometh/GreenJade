/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2762: bounded first octet of s in accept set.
 *
 * Surface (unique symbols):
 *   char *gj_str_pbrk_n_u(const char *s, const char *accept, size_t n);
 *     - Pointer to the first octet of s[0..n) that appears in accept
 *       (bounded strpbrk-shaped), or NULL if none. Empty accept -> NULL.
 *       NULL s or NULL accept -> NULL. n == 0 -> NULL. Stops at s's
 *       terminating NUL or after n octets, whichever first.
 *   char *__gj_str_pbrk_n_u  (alias)
 *   __libcgj_batch2762_marker = "libcgj-batch2762"
 *
 * Post-2760 str/mem/int exclusive wave (2761-2770). Distinct from
 * gj_str_pbrk_u (batch2209) - unique bounded _n_u surface only; no
 * multi-def. Suggested continuum name gj_str_pbrk_u already reserved
 * by batch2209.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2762_marker[] = "libcgj-batch2762";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if unsigned octet ch appears in accept; 0 otherwise. */
static int
b2762_in_set(unsigned char ch, const char *accept)
{
	size_t i;

	for (i = 0u; accept[i] != '\0'; i++) {
		if ((unsigned char)accept[i] == ch) {
			return 1;
		}
	}
	return 0;
}

/* First s[0..n) octet in accept, or NULL. Caller: s, accept non-NULL. */
static char *
b2762_pbrk_n(const char *s, const char *accept, size_t n)
{
	size_t i;

	if (accept[0] == '\0') {
		return NULL;
	}

	i = 0u;
	while (i < n && s[i] != '\0') {
		if (b2762_in_set((unsigned char)s[i], accept) != 0) {
			return (char *)(void *)(s + i);
		}
		i++;
	}
	return NULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_pbrk_n_u - pointer to first s octet present in accept within n,
 *                   or NULL.
 *
 * s:      subject string (NULL -> NULL); scanned at most n octets
 * accept: NUL-terminated set of accepted octets (NULL -> NULL)
 * n:      maximum number of subject octets to examine (0 -> NULL)
 *
 * Does not call libc strpbrk.
 */
char *
gj_str_pbrk_n_u(const char *s, const char *accept, size_t n)
{
	(void)NULL;
	if (s == NULL || accept == NULL || n == 0u) {
		return NULL;
	}
	return b2762_pbrk_n(s, accept, n);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_pbrk_n_u(const char *s, const char *accept, size_t n)
    __attribute__((alias("gj_str_pbrk_n_u")));
