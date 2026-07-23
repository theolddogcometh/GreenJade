/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1826: XML/HTML ampersand entity start hint.
 *
 * Surface (unique symbols):
 *   int gj_xml_is_amp_entity_start(const char *s);
 *     — Return 1 if s begins a plausible character/entity reference:
 *       '&' followed by either '#' (numeric) or an ASCII NameStart
 *       letter / '_' (named). Does not parse the full entity or
 *       require a trailing ';'. NULL / bare '&' / other → 0.
 *   int __gj_xml_is_amp_entity_start  (alias)
 *   __libcgj_batch1826_marker = "libcgj-batch1826"
 *
 * XML/HTML tag exclusive wave (1821–1829). Clean-room freestanding
 * pure C. Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno, no libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1826_marker[] = "libcgj-batch1826";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1826_name_start(int nCh)
{
	if (nCh == '_' || nCh == ':') {
		return 1;
	}
	if ((nCh >= 'A' && nCh <= 'Z') || (nCh >= 'a' && nCh <= 'z')) {
		return 1;
	}
	return 0;
}

static int
b1826_amp_entity_start(const char *sz)
{
	int nNext;

	if (sz == NULL) {
		return 0;
	}
	if (sz[0] != '&') {
		return 0;
	}
	nNext = (unsigned char)sz[1];
	if (nNext == '#') {
		return 1;
	}
	if (b1826_name_start(nNext)) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xml_is_amp_entity_start — 1 if s looks like &name or &#… start.
 *
 * Lightweight lexer hint for entity decoding paths. Does not validate
 * the entity body or require a closing ';'.
 */
int
gj_xml_is_amp_entity_start(const char *sz)
{
	return b1826_amp_entity_start(sz);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_xml_is_amp_entity_start(const char *sz)
    __attribute__((alias("gj_xml_is_amp_entity_start")));
