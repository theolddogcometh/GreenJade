/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2634: Proton run-script basename soft-ok.
 *
 * Surface (unique symbols):
 *   int gj_proton_run_ok(const char *name);
 *     - Return 1 if name is a non-NULL non-empty basename of length
 *       1..64 with no '/', using only [A-Za-z0-9._-], suitable as a
 *       Proton run / entry script name (e.g. "proton", "proton.sh").
 *       Soft shape check; does not resolve or exec.
 *   int __gj_proton_run_ok  (alias)
 *   __libcgj_batch2634_marker = "libcgj-batch2634"
 *
 * Product path exclusive wave (2631-2640). Distinct from
 * gj_proton_ge_ver_ok (batch2366 version tags),
 * gj_proton_prefix_ok_u (batch2272), and gj_proton_major_from_ver
 * (batch1864) — unique gj_proton_run_ok surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2634_marker[] = "libcgj-batch2634";

/* Max basename length for a Proton run-script product name. */
#define B2634_NAME_MAX  64u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if ch is a permitted Proton run basename character. */
static int
b2634_ch_ok(char ch)
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

/* Soft basename shape check for Proton run script names. */
static int
b2634_run_ok(const char *szName)
{
	size_t nLen;

	if (szName == NULL) {
		return 0;
	}
	nLen = 0u;
	while (szName[nLen] != '\0') {
		if (nLen >= (size_t)B2634_NAME_MAX) {
			return 0;
		}
		if (szName[nLen] == '/') {
			return 0;
		}
		if (!b2634_ch_ok(szName[nLen])) {
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
 * gj_proton_run_ok - soft check for a Proton run-script basename.
 *
 * name: candidate basename (e.g. "proton", "proton.sh"); not a path
 *
 * Returns 1 when name is non-empty, length <= 64, has no '/', and uses
 * only [A-Za-z0-9._-], else 0. Does not call libc.
 */
int
gj_proton_run_ok(const char *szName)
{
	(void)NULL;
	return b2634_run_ok(szName);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_proton_run_ok(const char *szName)
    __attribute__((alias("gj_proton_run_ok")));
