/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch470: freestanding KSUID timestamp field helpers.
 *
 * KSUID binary form is 20 octets (Segment-style layout):
 *   octets 0..3  : timestamp, big-endian uint32 (seconds since KSUID epoch)
 *   octets 4..19 : 16-byte payload (untouched by these helpers)
 *
 * Surface (unique symbols):
 *   void     gj_ksuid_set_time(uint8_t k[20], uint32_t epoch_s);
 *     — write epoch_s into the first 4 octets as big-endian; leave payload.
 *   uint32_t gj_ksuid_get_time(const uint8_t k[20]);
 *     — read first 4 octets as big-endian uint32; return 0 if k is NULL.
 *   void     __gj_ksuid_set_time(...)  (alias)
 *   uint32_t __gj_ksuid_get_time(...)  (alias)
 *   __libcgj_batch470_marker = "libcgj-batch470"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No epoch-offset arithmetic — the
 * caller supplies/receives the raw 32-bit timestamp field value.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch470_marker[] = "libcgj-batch470";

/* ---- gj_ksuid_set_time ------------------------------------------------- */

/*
 * Stamp a big-endian 32-bit timestamp into KSUID octets 0..3.
 * Payload octets 4..19 are left untouched. No-op if k is NULL.
 */
void
gj_ksuid_set_time(uint8_t k[20], uint32_t epoch_s)
{
	if (k == NULL) {
		return;
	}

	k[0] = (uint8_t)((epoch_s >> 24) & 0xffu);
	k[1] = (uint8_t)((epoch_s >> 16) & 0xffu);
	k[2] = (uint8_t)((epoch_s >> 8) & 0xffu);
	k[3] = (uint8_t)(epoch_s & 0xffu);
}

void __gj_ksuid_set_time(uint8_t k[20], uint32_t epoch_s)
    __attribute__((alias("gj_ksuid_set_time")));

/* ---- gj_ksuid_get_time ------------------------------------------------- */

/*
 * Extract the big-endian 32-bit timestamp from KSUID octets 0..3.
 * Returns 0 if k is NULL.
 */
uint32_t
gj_ksuid_get_time(const uint8_t k[20])
{
	if (k == NULL) {
		return 0u;
	}

	return ((uint32_t)k[0] << 24) |
	       ((uint32_t)k[1] << 16) |
	       ((uint32_t)k[2] << 8) |
	       (uint32_t)k[3];
}

uint32_t __gj_ksuid_get_time(const uint8_t k[20])
    __attribute__((alias("gj_ksuid_get_time")));
