/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1829: XML/HTML comment open predicate.
 *
 * Surface (unique symbols):
 *   int gj_xml_comment_open_p(const char *s);
 *     — Return 1 if s begins with the four-byte comment open mark
 *       "<!--" (exact ASCII). Does not require a closing "-->" and
 *       does not validate the comment body. NULL / shorter /
 *       mismatch → 0.
 *   int __gj_xml_comment_open_p  (alias)
 *   __libcgj_batch1829_marker = "libcgj-batch1829"
 *
 * XML/HTML tag exclusive wave (1821–1829). Clean-room freestanding
 * pure C. Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1829_marker[] = "libcgj-batch1829";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1829_comment_open_p(const char *sz)
{
	if (sz == NULL) {
		return 0;
	}
	if (sz[0] != '<' || sz[1] != '!' || sz[2] != '-' || sz[3] != '-') {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xml_comment_open_p — 1 if s starts with "<!--".
 *
 * Exact four-byte match. Use as a scanner branch before skipping to
 * the matching "-->" (caller owns body validation).
 */
int
gj_xml_comment_open_p(const char *sz)
{
	return b1829_comment_open_p(sz);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_xml_comment_open_p(const char *sz)
    __attribute__((alias("gj_xml_comment_open_p")));
