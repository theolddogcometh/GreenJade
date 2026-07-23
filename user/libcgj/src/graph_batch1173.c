/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1173: JWT compact serialization part counter.
 *
 * Surface (unique symbols):
 *   int gj_jwt_part_count(const char *jwt);
 *     — Count '.' separators in jwt and return dots + 1 (number of
 *       parts). Empty string → 1. NULL → 0. Does not validate base64.
 *       A classic JWS has 2 dots → return 3.
 *   int __gj_jwt_part_count  (alias)
 *   __libcgj_batch1173_marker = "libcgj-batch1173"
 *
 * Distinct from gj_jwt_split (batch162) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1173_marker[] = "libcgj-batch1173";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_jwt_part_count — return (number of '.') + 1; NULL → 0.
 */
int
gj_jwt_part_count(const char *szJwt)
{
	const char *p;
	int cDots;

	if (szJwt == NULL) {
		return 0;
	}
	cDots = 0;
	for (p = szJwt; *p != '\0'; p++) {
		if (*p == '.') {
			cDots++;
		}
	}
	return cDots + 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_jwt_part_count(const char *szJwt)
    __attribute__((alias("gj_jwt_part_count")));
