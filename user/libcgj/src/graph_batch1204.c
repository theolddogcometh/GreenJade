/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1204: in-place strip of CR octets.
 *
 * Surface (unique symbols):
 *   char *gj_str_strip_cr(char *s);
 *     — Remove every '\r' from the NUL-terminated string at s in place
 *       by compacting remaining octets leftward and writing a new NUL.
 *       Returns s (or NULL if s is NULL). Empty string → empty.
 *   char *__gj_str_strip_cr  (alias)
 *   __libcgj_batch1204_marker = "libcgj-batch1204"
 *
 * Distinct from trim / normalize_newlines — unique name; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1204_marker[] = "libcgj-batch1204";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_strip_cr — remove all CR characters from s in place.
 */
char *
gj_str_strip_cr(char *sz)
{
	char *pRd;
	char *pWr;

	if (sz == NULL) {
		return NULL;
	}

	pRd = sz;
	pWr = sz;
	while (*pRd != '\0') {
		if (*pRd != '\r') {
			*pWr = *pRd;
			pWr++;
		}
		pRd++;
	}
	*pWr = '\0';
	return sz;
}

/* ---- underscored alias ------------------------------------------------- */

char *__gj_str_strip_cr(char *sz)
    __attribute__((alias("gj_str_strip_cr")));
