/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch344: Unix epoch seconds ↔ NTP 32-bit seconds
 * (1900-01-01 era-0 seconds field, RFC 5905).
 *
 * Surface (unique symbols):
 *   uint32_t gj_unix_to_ntp32(uint64_t unix_sec);
 *     — Convert seconds since 1970-01-01 00:00:00 UTC to the classic
 *       32-bit NTP seconds field (seconds since 1900-01-01 00:00:00 UTC,
 *       truncated/wrapping into uint32_t). Offset is exactly
 *       2208988800 seconds (70 years + 17 leap days, 1900 not leap).
 *   uint64_t gj_ntp32_to_unix(uint32_t ntp_sec);
 *     — Inverse for era-0 values at/after the Unix epoch: ntp_sec minus
 *       2208988800. Pre-Unix NTP values (ntp_sec < 2208988800) return 0
 *       (no signed time_t; freestanding uint64 domain).
 *   __gj_unix_to_ntp32 / __gj_ntp32_to_unix  (aliases)
 *   __libcgj_batch344_marker = "libcgj-batch344"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party NTP source copied.
 */

#include <stdint.h>

const char __libcgj_batch344_marker[] = "libcgj-batch344";

/*
 * Seconds from NTP era-0 epoch (1900-01-01) to Unix epoch (1970-01-01).
 * 70 * 365 + 17 leap days (1904..1968) = 25567 days * 86400.
 */
#define B344_NTP_UNIX_DELTA 2208988800ULL

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_unix_to_ntp32 — Unix seconds → 32-bit NTP seconds (host order).
 *
 * unix_sec: seconds since 1970-01-01 UTC (may be any uint64; result wraps).
 *
 * Returns the low 32 bits of (unix_sec + 2208988800).
 */
uint32_t
gj_unix_to_ntp32(uint64_t unix_sec)
{
	return (uint32_t)(unix_sec + B344_NTP_UNIX_DELTA);
}

/*
 * gj_ntp32_to_unix — 32-bit NTP seconds → Unix seconds (host order).
 *
 * ntp_sec: era-0 NTP seconds field (seconds since 1900-01-01 UTC).
 *
 * Returns unix seconds, or 0 if ntp_sec is before the Unix epoch.
 */
uint64_t
gj_ntp32_to_unix(uint32_t ntp_sec)
{
	uint64_t u64Ntp;

	u64Ntp = (uint64_t)ntp_sec;
	if (u64Ntp < B344_NTP_UNIX_DELTA) {
		return 0ull;
	}
	return u64Ntp - B344_NTP_UNIX_DELTA;
}

uint32_t __gj_unix_to_ntp32(uint64_t unix_sec)
    __attribute__((alias("gj_unix_to_ntp32")));
uint64_t __gj_ntp32_to_unix(uint32_t ntp_sec)
    __attribute__((alias("gj_ntp32_to_unix")));
