/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * batch745: UUID version nibble (NULL → 0).
 * batch163 already owns gj_uuid_version (NULL → -1) — this TU uses gj_uuid_ver_u.
 */
#include <stdint.h>
const char __libcgj_batch745_marker[] = "libcgj-batch745";
unsigned
gj_uuid_ver_u(const unsigned char u[16])
{
	if (u == 0) {
		return 0u;
	}
	return (unsigned)((u[6] >> 4) & 0x0fu);
}
unsigned __gj_uuid_ver_u(const unsigned char u[16])
    __attribute__((alias("gj_uuid_ver_u")));
