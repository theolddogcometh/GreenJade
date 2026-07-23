/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2366: Proton-GE version string soft-ok
 * predicate (post-2360 product path exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_proton_ge_ver_ok(const char *ver);
 *     - Return 1 if ver is a non-NULL non-empty string of length
 *       1..64 using only [A-Za-z0-9._-] (shape of tags such as
 *       "GE-Proton9-20" or "9-20"). Soft pure-data check; no semver
 *       parse and no filesystem probe.
 *   int __gj_proton_ge_ver_ok  (alias)
 *   __libcgj_batch2366_marker = "libcgj-batch2366"
 *
 * Post-2360 product path exclusive wave (2361-2370). Distinct from
 * gj_proton_major_from_ver (batch1864) and gj_proton_prefix_ok_u
 * (batch2272) - unique ge_ver_ok surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2366_marker[] = "libcgj-batch2366";

/* Max length for a Proton-GE version tag. */
#define B2366_VER_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if ch is permitted in a GE-Proton version tag. */
static int
b2366_ch_ok(char ch)
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

/* Soft shape check for Proton-GE version strings. */
static int
b2366_ver_ok(const char *szVer)
{
	size_t nLen;

	if (szVer == NULL) {
		return 0;
	}
	nLen = 0u;
	while (szVer[nLen] != '\0') {
		if (nLen >= B2366_VER_MAX) {
			return 0;
		}
		if (!b2366_ch_ok(szVer[nLen])) {
			return 0;
		}
		nLen++;
	}
	if (nLen == 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_ge_ver_ok - soft check for a Proton-GE version tag.
 *
 * ver: candidate version string (e.g. "GE-Proton9-20")
 *
 * Returns 1 when ver is non-empty, length <= 64, and uses only
 * [A-Za-z0-9._-], else 0. Does not call libc.
 */
int
gj_proton_ge_ver_ok(const char *szVer)
{
	(void)NULL;
	return b2366_ver_ok(szVer);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_proton_ge_ver_ok(const char *szVer)
    __attribute__((alias("gj_proton_ge_ver_ok")));
