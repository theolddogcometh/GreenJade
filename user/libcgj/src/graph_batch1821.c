/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1821: HTML void-tag name hint.
 *
 * Surface (unique symbols):
 *   int gj_html_is_void_tag_hint(const char *name);
 *     — Return 1 if name matches a common HTML5 void element tag
 *       (case-insensitive ASCII): area, base, br, col, embed, hr,
 *       img, input, link, meta, param, source, track, wbr.
 *       NULL / non-match → 0. Lightweight hint only — not a full
 *       HTML tokenizer.
 *   int __gj_html_is_void_tag_hint  (alias)
 *   __libcgj_batch1821_marker = "libcgj-batch1821"
 *
 * XML/HTML tag exclusive wave (1821–1829). Clean-room freestanding
 * pure C. Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1821_marker[] = "libcgj-batch1821";

/* ---- freestanding helpers ---------------------------------------------- */

/* ASCII fold A-Z → a-z; other bytes unchanged. */
static char
b1821_fold(char ch)
{
	if (ch >= 'A' && ch <= 'Z') {
		return (char)(ch - 'A' + 'a');
	}
	return ch;
}

/* Case-insensitive ASCII equality against a lowercase literal. */
static int
b1821_eq_ci(const char *szName, const char *szLit)
{
	const char *pA;
	const char *pB;

	pA = szName;
	pB = szLit;
	while (*pB != '\0') {
		if (*pA == '\0') {
			return 0;
		}
		if (b1821_fold(*pA) != *pB) {
			return 0;
		}
		pA++;
		pB++;
	}
	return (*pA == '\0') ? 1 : 0;
}

static int
b1821_void_hint(const char *szName)
{
	if (szName == NULL) {
		return 0;
	}
	if (b1821_eq_ci(szName, "area") ||
	    b1821_eq_ci(szName, "base") ||
	    b1821_eq_ci(szName, "br") ||
	    b1821_eq_ci(szName, "col") ||
	    b1821_eq_ci(szName, "embed") ||
	    b1821_eq_ci(szName, "hr") ||
	    b1821_eq_ci(szName, "img") ||
	    b1821_eq_ci(szName, "input") ||
	    b1821_eq_ci(szName, "link") ||
	    b1821_eq_ci(szName, "meta") ||
	    b1821_eq_ci(szName, "param") ||
	    b1821_eq_ci(szName, "source") ||
	    b1821_eq_ci(szName, "track") ||
	    b1821_eq_ci(szName, "wbr")) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_html_is_void_tag_hint — 1 if name is a common HTML5 void tag.
 *
 * Compares the bare tag name only (no '<', '/', or attributes). Case
 * folding is ASCII-only. Suitable as a parse-hint before emitting a
 * self-closing form; not a full HTML5 tree builder.
 */
int
gj_html_is_void_tag_hint(const char *szName)
{
	return b1821_void_hint(szName);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_html_is_void_tag_hint(const char *szName)
    __attribute__((alias("gj_html_is_void_tag_hint")));
