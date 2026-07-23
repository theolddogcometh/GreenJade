/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch629: UUID nil predicate.
 * batch467 already owns gj_uuid_is_nil — this TU uses gj_uuid_nil_p.
 *
 * Surface:
 *   int gj_uuid_nil_p(const unsigned char u[16]);
 *   int __gj_uuid_nil_p (alias)
 *   __libcgj_batch629_marker = "libcgj-batch629"
 *
 * 1 if all 16 octets are zero; NULL → 0.
 * Freestanding pure C. No malloc/errno/libc.
 */

#include <stddef.h>

const char __libcgj_batch629_marker[] = "libcgj-batch629";

int
gj_uuid_nil_p(const unsigned char u[16])
{
	size_t i;

	if (u == NULL) {
		return 0;
	}
	for (i = 0u; i < 16u; i++) {
		if (u[i] != 0u) {
			return 0;
		}
	}
	return 1;
}

int __gj_uuid_nil_p(const unsigned char u[16])
    __attribute__((alias("gj_uuid_nil_p")));
