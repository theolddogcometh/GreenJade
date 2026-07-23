/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2228: table load factor in milli-units.
 *
 * Surface (unique symbols):
 *   uint32_t gj_load_factor_milli(size_t used, size_t cap);
 *     - Return floor(used * 1000 / cap), the load factor in parts per
 *       thousand. cap == 0 yields 0. Overflow-safe via 64-bit multiply.
 *       Typical resize threshold: milli >= 750 (75 percent).
 *   uint32_t __gj_load_factor_milli  (alias)
 *   __libcgj_batch2228_marker = "libcgj-batch2228"
 *
 * Post-2220 table exclusive wave (2221-2230). Unique
 * gj_load_factor_milli surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2228_marker[] = "libcgj-batch2228";

/* Parts-per-thousand scale for load-factor reporting. */
#define B2228_MILLI  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor(used * 1000 / cap). cap == 0 -> 0. Result saturates to UINT32_MAX
 * if the quotient somehow exceeds that (not expected when used <= cap).
 */
static uint32_t
b2228_milli(size_t cUsed, size_t cCap)
{
	uint64_t u64Q;

	if (cCap == 0u) {
		return 0u;
	}
	u64Q = ((uint64_t)cUsed * B2228_MILLI) / (uint64_t)cCap;
	if (u64Q > 0xFFFFFFFFull) {
		return 0xFFFFFFFFu;
	}
	return (uint32_t)u64Q;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_load_factor_milli - table occupancy in parts per thousand.
 *
 * used: occupied live entries (caller-defined; tombs optional)
 * cap:  table capacity in slots
 *
 * Returns floor(used * 1000 / cap), or 0 when cap is 0.
 * Examples: used=3 cap=4 -> 750; used=0 cap=8 -> 0; used=1 cap=0 -> 0.
 */
uint32_t
gj_load_factor_milli(size_t used, size_t cap)
{
	(void)NULL;
	return b2228_milli(used, cap);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_load_factor_milli(size_t used, size_t cap)
    __attribute__((alias("gj_load_factor_milli")));
