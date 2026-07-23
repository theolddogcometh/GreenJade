/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4266: freestanding 4 KiB pages spanned (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_pages_spanned_u(uint64_t addr, uint64_t len);
 *     - Number of distinct 4096-byte pages touched by the half-open
 *       range [addr, addr+len). Returns 0 if len == 0.
 *       Formula (no wrap): ((addr & 4095) + len + 4095) / 4096.
 *       On overflow of the intermediate sum, returns 0.
 *   uint64_t __gj_u64_pages_spanned_u  (alias)
 *   __libcgj_batch4266_marker = "libcgj-batch4266"
 *
 * Exclusive continuum CREATE-ONLY (4261-4270): align/page wave.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4266_marker[] = "libcgj-batch4266";

#define B4266_PAGE_4K      4096ull
#define B4266_PAGE_MASK    (B4266_PAGE_4K - 1ull)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Pages spanned by [addr, addr+len). len == 0 → 0.
 * ((addr & 4095) + len + 4095) / 4096 with overflow → 0.
 */
static uint64_t
b4266_pages_spanned(uint64_t u64Addr, uint64_t u64Len)
{
	uint64_t u64Off;
	uint64_t u64Sum;

	if (u64Len == 0ull) {
		return 0ull;
	}

	u64Off = u64Addr & B4266_PAGE_MASK;

	/* u64Off + len may overflow. */
	if (u64Len > (UINT64_MAX - u64Off)) {
		return 0ull;
	}
	u64Sum = u64Off + u64Len;

	/* u64Sum + 4095 may overflow. */
	if (u64Sum > (UINT64_MAX - B4266_PAGE_MASK)) {
		return 0ull;
	}
	u64Sum = u64Sum + B4266_PAGE_MASK;

	return u64Sum / B4266_PAGE_4K;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_pages_spanned_u - count 4 KiB pages covering [addr, addr+len).
 *
 * addr: start address
 * len:  byte length (0 → 0 pages)
 * Overflow of the intermediate → 0. No parent wires.
 */
uint64_t
gj_u64_pages_spanned_u(uint64_t u64Addr, uint64_t u64Len)
{
	(void)NULL;
	return b4266_pages_spanned(u64Addr, u64Len);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_pages_spanned_u(uint64_t u64Addr, uint64_t u64Len)
    __attribute__((alias("gj_u64_pages_spanned_u")));
