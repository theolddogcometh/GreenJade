/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * batch746: RFC4122 variant field from byte 8 (NULL → 0).
 * batch163 owns gj_uuid_variant — this TU uses gj_uuid_var_u.
 * Returns top 2 bits of octet 8 as 0..3 (10xx → 2 for RFC4122).
 */
#include <stdint.h>
const char __libcgj_batch746_marker[] = "libcgj-batch746";
unsigned
gj_uuid_var_u(const unsigned char u[16])
{
	if (u == 0) {
		return 0u;
	}
	return (unsigned)((u[8] >> 6) & 0x03u);
}
unsigned __gj_uuid_var_u(const unsigned char u[16])
    __attribute__((alias("gj_uuid_var_u")));
