/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2209: first octet of s that appears in accept.
 *
 * Surface (unique symbols):
 *   char *gj_str_pbrk_u(const char *s, const char *accept);
 *     - Return a pointer to the first octet of s that appears in accept
 *       (strpbrk-shaped), or NULL if none. Empty accept → NULL. NULL s
 *       or NULL accept → NULL. Search stops at s's terminating NUL.
 *   char *__gj_str_pbrk_u  (alias)
 *   __libcgj_batch2209_marker = "libcgj-batch2209"
 *
 * Post-2200 str exclusive wave (2201-2210). Distinct from gj_str_find_first_of
 * (batch1321 index form) - unique pointer _u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch2209_marker[] = "libcgj-batch2209";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if unsigned octet ch appears in accept; 0 otherwise. */
static int
b2209_in_set(unsigned char ch, const char *accept)
{
	size_t i;

	for (i = 0u; accept[i] != '\0'; i++) {
		if ((unsigned char)accept[i] == ch) {
			return 1;
		}
	}
	return 0;
}

static char *
b2209_pbrk(const char *s, const char *accept)
{
	size_t i;

	if (s == NULL || accept == NULL) {
		return NULL;
	}
	if (accept[0] == '\0') {
		return NULL;
	}

	i = 0u;
	while (s[i] != '\0') {
		if (b2209_in_set((unsigned char)s[i], accept) != 0) {
			return (char *)(void *)(s + i);
		}
		i++;
	}
	return NULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_pbrk_u - pointer to first s octet present in accept, or NULL.
 *
 * s:      NUL-terminated subject (NULL → NULL)
 * accept: NUL-terminated set of accepted octets (NULL → NULL)
 *
 * Does not call libc strpbrk.
 */
char *
gj_str_pbrk_u(const char *s, const char *accept)
{
	(void)NULL;
	return b2209_pbrk(s, accept);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_pbrk_u(const char *s, const char *accept)
    __attribute__((alias("gj_str_pbrk_u")));
