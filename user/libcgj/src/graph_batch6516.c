/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6516: Steam client package-dir name soft-ok.
 *
 * Surface (unique symbols):
 *   uint32_t gj_scli_pkgdir_ok_6516(const char *name);
 *     - Return 1 if name is a non-NULL non-empty package-directory
 *       basename of length 1..64 with no '/', using only [A-Za-z0-9._-],
 *       suitable as a Steam client package dir (e.g. "ubuntu12_32",
 *       "ubuntu12_64", "package"). Soft shape check; does not resolve.
 *   uint32_t __gj_scli_pkgdir_ok_6516  (alias)
 *   __libcgj_batch6516_marker = "libcgj-batch6516"
 *
 * Exclusive continuum CREATE-ONLY (6511-6520: post-6500 steam client
 * deepen). Unique gj_scli_pkgdir_ok_6516 surface only; no multi-def.
 * Distinct from gj_scli_bin_ok_6511 and gj_steam_root_ok (batch2365).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6516_marker[] = "libcgj-batch6516";

#define B6516_NAME_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6516_ch_ok(char ch)
{
	if (ch >= 'a' && ch <= 'z') {
		return 1;
	}
	if (ch >= 'A' && ch <= 'Z') {
		return 1;
	}
	if (ch >= '0' && ch <= '9') {
		return 1;
	}
	if (ch == '.' || ch == '-' || ch == '_') {
		return 1;
	}
	return 0;
}

static uint32_t
b6516_pkgdir_ok(const char *szName)
{
	size_t nLen;

	if (szName == NULL) {
		return 0u;
	}
	nLen = 0u;
	while (szName[nLen] != '\0') {
		if (nLen >= (size_t)B6516_NAME_MAX) {
			return 0u;
		}
		if (szName[nLen] == '/') {
			return 0u;
		}
		if (b6516_ch_ok(szName[nLen]) == 0) {
			return 0u;
		}
		nLen++;
	}
	if (nLen == 0u) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_scli_pkgdir_ok_6516 - soft Steam client package-dir basename ok.
 *
 * name: candidate package directory basename (e.g. "ubuntu12_32")
 *
 * Returns 1 when name is non-empty, length <= 64, has no '/', and uses
 * only [A-Za-z0-9._-], else 0. Soft pure-data for deepen scoring.
 * Does not call libc. No parent wires.
 */
uint32_t
gj_scli_pkgdir_ok_6516(const char *szName)
{
	(void)NULL;
	return b6516_pkgdir_ok(szName);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_scli_pkgdir_ok_6516(const char *szName)
    __attribute__((alias("gj_scli_pkgdir_ok_6516")));
