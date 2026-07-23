/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch469: freestanding ULID 48-bit timestamp field.
 *
 * Surface (unique symbols):
 *   void     gj_ulid_set_time(uint8_t ulid[16], uint64_t ms);
 *     — Write the ULID timestamp: first 48 bits of the 16-byte binary
 *       ULID as big-endian milliseconds. High bits of ms above 48 are
 *       discarded. Entropy bytes ulid[6..15] are left untouched.
 *   uint64_t gj_ulid_get_time(const uint8_t ulid[16]);
 *     — Read the first 48 bits of a binary ULID as big-endian ms.
 *   void     __gj_ulid_set_time  (alias)
 *   uint64_t __gj_ulid_get_time  (alias)
 *   __libcgj_batch469_marker = "libcgj-batch469"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch469_marker[] = "libcgj-batch469";

/* 48-bit mask: ULID timestamp width. */
#define B469_TS_MASK  ((uint64_t)0x0000ffffffffffffULL)

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ulid_set_time — store ms in ulid[0..5] big-endian (48-bit).
 *
 * Edge cases:
 *   ulid == NULL → no-op
 *   ms bits above 48 → discarded (masked)
 *   ulid[6..15] → preserved
 */
void
gj_ulid_set_time(uint8_t ulid[16], uint64_t ms)
{
	uint64_t u64Ts;

	if (ulid == NULL) {
		return;
	}

	u64Ts = ms & B469_TS_MASK;
	ulid[0] = (uint8_t)((u64Ts >> 40) & 0xffu);
	ulid[1] = (uint8_t)((u64Ts >> 32) & 0xffu);
	ulid[2] = (uint8_t)((u64Ts >> 24) & 0xffu);
	ulid[3] = (uint8_t)((u64Ts >> 16) & 0xffu);
	ulid[4] = (uint8_t)((u64Ts >> 8) & 0xffu);
	ulid[5] = (uint8_t)(u64Ts & 0xffu);
}

/*
 * gj_ulid_get_time — load ulid[0..5] big-endian as 48-bit ms (uint64_t).
 *
 * Edge cases:
 *   ulid == NULL → 0
 */
uint64_t
gj_ulid_get_time(const uint8_t ulid[16])
{
	uint64_t u64Ts;

	if (ulid == NULL) {
		return 0u;
	}

	u64Ts = ((uint64_t)ulid[0] << 40)
	      | ((uint64_t)ulid[1] << 32)
	      | ((uint64_t)ulid[2] << 24)
	      | ((uint64_t)ulid[3] << 16)
	      | ((uint64_t)ulid[4] << 8)
	      | ((uint64_t)ulid[5]);
	return u64Ts;
}

/* ---- underscored aliases ----------------------------------------------- */

void __gj_ulid_set_time(uint8_t ulid[16], uint64_t ms)
    __attribute__((alias("gj_ulid_set_time")));

uint64_t __gj_ulid_get_time(const uint8_t ulid[16])
    __attribute__((alias("gj_ulid_get_time")));
