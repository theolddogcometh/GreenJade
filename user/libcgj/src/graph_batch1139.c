/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1139: INI section-name validation.
 *
 * Surface (unique symbols):
 *   int gj_ini_section_name_ok(const char *name);
 *     — Return 1 if name is a valid INI section name (without brackets):
 *       length 1..64, no NUL already, printable ASCII 0x21..0x7e
 *       excluding '[' and ']' (no control, no space). NULL / empty → 0.
 *   int __gj_ini_section_name_ok  (alias)
 *   __libcgj_batch1139_marker = "libcgj-batch1139"
 *
 * Distinct from gj_ini_get (batch104) — unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1139_marker[] = "libcgj-batch1139";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ini_section_name_ok — 1 if szName is a safe bare INI section name.
 */
int
gj_ini_section_name_ok(const char *szName)
{
	size_t cb;
	size_t i;
	unsigned char uCh;

	if (szName == NULL) {
		return 0;
	}

	cb = 0u;
	while (szName[cb] != '\0') {
		cb++;
		if (cb > 64u) {
			return 0;
		}
	}
	if (cb == 0u || cb > 64u) {
		return 0;
	}

	for (i = 0u; i < cb; i++) {
		uCh = (unsigned char)szName[i];
		if (uCh < 0x21u || uCh > 0x7eu) {
			return 0;
		}
		if (uCh == '[' || uCh == ']') {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ini_section_name_ok(const char *szName)
    __attribute__((alias("gj_ini_section_name_ok")));
