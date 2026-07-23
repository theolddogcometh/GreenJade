/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4919: freestanding UUID time_low field (BE).
 *
 * Surface (unique symbols):
 *   uint32_t gj_uuid_time_low_u(const uint8_t b[16]);
 *     - Extract the first 4 wire octets as a big-endian uint32_t
 *       (RFC 4122 time_low). NULL -> 0.
 *   uint32_t __gj_uuid_time_low_u  (alias)
 *   __libcgj_batch4919_marker = "libcgj-batch4919"
 *
 * Exclusive continuum CREATE-ONLY UUID v4 bits wave (4911-4920).
 * Unique gj_uuid_time_low_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4919_marker[] = "libcgj-batch4919";

/* ---- freestanding helpers ---------------------------------------------- */

/* Big-endian u32 from octets 0..3. pB must be non-NULL. */
static uint32_t
b4919_time_low(const uint8_t *pB)
{
	return ((uint32_t)pB[0] << 24) |
	    ((uint32_t)pB[1] << 16) |
	    ((uint32_t)pB[2] << 8) |
	    (uint32_t)pB[3];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_time_low_u - big-endian time_low from the first 4 octets.
 *
 * b: 16-octet UUID wire value (NULL -> 0)
 *
 * RFC 4122 layout: octets 0..3 are time_low in network byte order.
 * Returns that field as a host uint32_t value. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_uuid_time_low_u(const uint8_t b[16])
{
	(void)NULL;
	if (b == NULL) {
		return 0u;
	}
	return b4919_time_low(b);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uuid_time_low_u(const uint8_t b[16])
    __attribute__((alias("gj_uuid_time_low_u")));
