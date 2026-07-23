/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1989: freestanding temporary-path shape hint.
 *
 * Surface (unique symbols):
 *   int gj_tmp_path_hint_p(const char *szPath);
 *     — Soft hint: return 1 if szPath looks like a common Unix temp
 *       location prefix, else 0. Recognized absolute prefixes only:
 *         "/tmp"      (exact or "/tmp/...")
 *         "/var/tmp"  (exact or "/var/tmp/...")
 *         "/dev/shm"  (exact or "/dev/shm/...")
 *       Case-sensitive. Not a security boundary; pure string shape.
 *       NULL or empty → 0.
 *   int __gj_tmp_path_hint_p  (alias)
 *   __libcgj_batch1989_marker = "libcgj-batch1989"
 *
 * String path product exclusive wave (1981–1990). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied.
 */

#include <stddef.h>

const char __libcgj_batch1989_marker[] = "libcgj-batch1989";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * 1 if szPath equals szPref, or szPath is szPref + '/' + rest.
 * cbPref is the length of szPref (no NUL counted).
 */
static int
b1989_pref(const char *szPath, const char *szPref, size_t cbPref)
{
	size_t i;

	for (i = 0u; i < cbPref; i++) {
		if (szPath[i] != szPref[i]) {
			return 0;
		}
	}
	/* Exact match or next byte is '/' or end already handled. */
	if (szPath[cbPref] == '\0' || szPath[cbPref] == '/') {
		return 1;
	}
	return 0;
}

static int
b1989_hint(const char *szPath)
{
	if (szPath == NULL || szPath[0] == '\0') {
		return 0;
	}
	/* "/tmp" = 4, "/var/tmp" = 8, "/dev/shm" = 8 */
	if (b1989_pref(szPath, "/tmp", 4u)) {
		return 1;
	}
	if (b1989_pref(szPath, "/var/tmp", 8u)) {
		return 1;
	}
	if (b1989_pref(szPath, "/dev/shm", 8u)) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tmp_path_hint_p — 1 if path has a common temp prefix shape.
 *
 * szPath: NUL-terminated path, or NULL
 * Returns 1 for /tmp, /var/tmp, /dev/shm prefixes, else 0.
 */
int
gj_tmp_path_hint_p(const char *szPath)
{
	(void)NULL;
	return b1989_hint(szPath);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_tmp_path_hint_p(const char *szPath)
    __attribute__((alias("gj_tmp_path_hint_p")));
