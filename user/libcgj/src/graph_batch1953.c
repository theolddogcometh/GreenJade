/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1953: package-epoch soft-ok predicate.
 *
 * Surface (unique symbols):
 *   int gj_pkg_epoch_ok(const char *epoch);
 *     — Soft validity hint: return 1 if epoch is a non-NULL, non-empty
 *       decimal digit string of at most 10 characters (soft packaging
 *       epoch bound), else 0. Does not parse into an integer or check
 *       against a real package database.
 *   int __gj_pkg_epoch_ok  (alias)
 *   __libcgj_batch1953_marker = "libcgj-batch1953"
 *
 * Package/install exclusive pure-data wave (1951–1960). Clean-room
 * freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1953_marker[] = "libcgj-batch1953";

/* Soft max digit count for packaging epoch strings. */
#define B1953_EPOCH_MAX  10u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if sz is non-empty pure digits, length 1..B1953_EPOCH_MAX. */
static int
b1953_epoch_ok(const char *szEpoch)
{
	size_t cb;

	if (szEpoch == NULL) {
		return 0;
	}
	if (szEpoch[0] == '\0') {
		return 0;
	}
	cb = 0u;
	while (szEpoch[cb] != '\0') {
		if (szEpoch[cb] < '0' || szEpoch[cb] > '9') {
			return 0;
		}
		cb++;
		if (cb > B1953_EPOCH_MAX) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pkg_epoch_ok — 1 if epoch is a soft-ok decimal epoch token.
 *
 * epoch: NUL-terminated digit string (NULL / empty / non-digit → 0)
 * Returns 1 when 1..10 pure decimal digits, else 0.
 */
int
gj_pkg_epoch_ok(const char *szEpoch)
{
	(void)NULL;
	return b1953_epoch_ok(szEpoch);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_pkg_epoch_ok(const char *szEpoch)
    __attribute__((alias("gj_pkg_epoch_ok")));
