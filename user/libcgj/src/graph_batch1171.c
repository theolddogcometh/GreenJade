/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1171: OpenSSH public-key line prefix check.
 *
 * Surface (unique symbols):
 *   int gj_ssh_is_publickey_line(const char *line);
 *     — Return 1 (OK) if line begins with a known OpenSSH public-key
 *       algorithm token: "ssh-rsa", "ssh-ed25519", "ssh-dss",
 *       "ssh-ecdsa", or "ecdsa-sha2-" prefix; else 0 (FAIL).
 *       NULL / empty → 0. Does not validate the rest of the line.
 *   int __gj_ssh_is_publickey_line  (alias)
 *   __libcgj_batch1171_marker = "libcgj-batch1171"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1171_marker[] = "libcgj-batch1171";

/* ---- helpers ------------------------------------------------------------ */

static int
b1171_prefix(const char *sz, const char *szPref)
{
	const char *p;
	const char *q;

	if (sz == NULL || szPref == NULL) {
		return 0;
	}
	p = sz;
	q = szPref;
	while (*q != '\0') {
		if (*p != *q) {
			return 0;
		}
		p++;
		q++;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ssh_is_publickey_line — 1 if OpenSSH public-key algo prefix present.
 */
int
gj_ssh_is_publickey_line(const char *szLine)
{
	if (szLine == NULL || szLine[0] == '\0') {
		return 0;
	}
	if (b1171_prefix(szLine, "ssh-rsa")) {
		return 1;
	}
	if (b1171_prefix(szLine, "ssh-ed25519")) {
		return 1;
	}
	if (b1171_prefix(szLine, "ssh-dss")) {
		return 1;
	}
	if (b1171_prefix(szLine, "ssh-ecdsa")) {
		return 1;
	}
	if (b1171_prefix(szLine, "ecdsa-sha2-")) {
		return 1;
	}
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ssh_is_publickey_line(const char *szLine)
    __attribute__((alias("gj_ssh_is_publickey_line")));
