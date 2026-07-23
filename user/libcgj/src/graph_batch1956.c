/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1956: icon-name soft-ok hint.
 *
 * Surface (unique symbols):
 *   int gj_icon_name_ok_hint(const char *name);
 *     — Soft validity hint: return 1 if name is a non-NULL, non-empty
 *       icon name token of at most 255 characters whose every byte is
 *       A-Za-z0-9, '-', '_', or '.' (no path separators). Pure name
 *       shape check; does not look up theme icons or open files.
 *   int __gj_icon_name_ok_hint  (alias)
 *   __libcgj_batch1956_marker = "libcgj-batch1956"
 *
 * Package/install exclusive pure-data wave (1951–1960). Clean-room
 * freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1956_marker[] = "libcgj-batch1956";

/* Soft max icon-name length (theme lookup path bound). */
#define B1956_NAME_MAX  255u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1956_char_ok(unsigned char uCh)
{
	if ((uCh >= 'A' && uCh <= 'Z') ||
	    (uCh >= 'a' && uCh <= 'z') ||
	    (uCh >= '0' && uCh <= '9')) {
		return 1;
	}
	if (uCh == '-' || uCh == '_' || uCh == '.') {
		return 1;
	}
	return 0;
}

/* 1 if sz is a soft-ok icon name token. */
static int
b1956_icon_ok(const char *szName)
{
	size_t cb;

	if (szName == NULL) {
		return 0;
	}
	if (szName[0] == '\0') {
		return 0;
	}
	cb = 0u;
	while (szName[cb] != '\0') {
		if (!b1956_char_ok((unsigned char)szName[cb])) {
			return 0;
		}
		cb++;
		if (cb > B1956_NAME_MAX) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_icon_name_ok_hint — 1 if name is a soft-ok icon name token.
 *
 * name: NUL-terminated icon name (NULL / empty / bad char / too long → 0)
 * Allowed: A-Za-z0-9 _ - . ; length 1..255.
 */
int
gj_icon_name_ok_hint(const char *szName)
{
	(void)NULL;
	return b1956_icon_ok(szName);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_icon_name_ok_hint(const char *szName)
    __attribute__((alias("gj_icon_name_ok_hint")));
