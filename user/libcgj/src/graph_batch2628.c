/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2628: freestanding C-string substring search.
 *
 * Surface (unique symbols):
 *   char *gj_str_str_u(const char *hay, const char *needle);
 *     - Return a pointer to the first occurrence of needle as a
 *       contiguous substring of hay (strstr-shaped), or NULL if none.
 *       Empty needle → hay (conventional empty-match-at-zero). hay ==
 *       NULL or needle == NULL → NULL.
 *   char *__gj_str_str_u  (alias)
 *   __libcgj_batch2628_marker = "libcgj-batch2628"
 *
 * Post-2620 classic str exclusive wave (2621-2630). Distinct from
 * gj_str_contains (batch603 boolean) — unique pointer _u surface; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>

const char __libcgj_batch2628_marker[] = "libcgj-batch2628";

/* ---- freestanding helpers ---------------------------------------------- */

static char *
b2628_str(const char *szHay, const char *szNeedle)
{
	size_t i;
	size_t j;

	if (szHay == NULL || szNeedle == NULL) {
		return NULL;
	}

	/* Empty needle matches at the start of any non-NULL hay. */
	if (szNeedle[0] == '\0') {
		return (char *)(void *)szHay;
	}

	for (i = 0u; szHay[i] != '\0'; i++) {
		if (szHay[i] != szNeedle[0]) {
			continue;
		}
		j = 0u;
		for (;;) {
			if (szNeedle[j] == '\0') {
				return (char *)(void *)(szHay + i);
			}
			if (szHay[i + j] == '\0' || szHay[i + j] != szNeedle[j]) {
				break;
			}
			j++;
		}
	}
	return NULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_str_u - first pointer to needle inside hay, or NULL.
 *
 * hay:    haystack C string (NULL → NULL)
 * needle: required substring (NULL → NULL; empty → hay)
 *
 * Comparison is unsigned-byte exact (no locale). Does not call libc strstr.
 */
char *
gj_str_str_u(const char *hay, const char *needle)
{
	(void)NULL;
	return b2628_str(hay, needle);
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_str_u(const char *hay, const char *needle)
    __attribute__((alias("gj_str_str_u")));
