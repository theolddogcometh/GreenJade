/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6182: millicores → cpu.max quota microseconds.
 *
 * Surface (unique symbols):
 *   uint64_t gj_cg_millicores_to_quota_us_6182(uint32_t millicores,
 *                                              uint64_t period_us);
 *     - Convert container CPU millicores and a cgroup period into a
 *       cpu.max quota in microseconds: (millicores * period_us) / 1000.
 *       millicores == 0 or period_us == 0 → 0. Overflow-safe path when
 *       millicores * period_us would exceed UINT64_MAX.
 *   uint64_t __gj_cg_millicores_to_quota_us_6182  (alias)
 *   __libcgj_batch6182_marker = "libcgj-batch6182"
 *
 * Exclusive continuum CREATE-ONLY (6181-6190: container runtime cgroup
 * stubs). Unique gj_cg_millicores_to_quota_us_6182 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6182_marker[] = "libcgj-batch6182";

/* 1000 millicores = 1 full CPU. */
#define B6182_MILLI  1000ull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b6182_quota_us(uint32_t uMilli, uint64_t uPeriodUs)
{
	uint64_t uM;

	if (uMilli == 0u || uPeriodUs == 0ull) {
		return 0ull;
	}
	uM = (uint64_t)uMilli;
	if (uM > UINT64_MAX / uPeriodUs) {
		return (uPeriodUs / B6182_MILLI) * uM +
		    ((uPeriodUs % B6182_MILLI) * uM) / B6182_MILLI;
	}
	return (uM * uPeriodUs) / B6182_MILLI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cg_millicores_to_quota_us_6182 - millicores × period → quota µs.
 *
 * millicores: requested CPU share in millicores (1000 = 1 CPU).
 * period_us:  cgroup cpu.max period in microseconds.
 * Returns cpu.max-style quota_us. Soft pure-integer container runtime
 * math. Does not call libc. No parent wires.
 */
uint64_t
gj_cg_millicores_to_quota_us_6182(uint32_t uMillicores, uint64_t uPeriodUs)
{
	(void)NULL;
	return b6182_quota_us(uMillicores, uPeriodUs);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_cg_millicores_to_quota_us_6182(uint32_t uMillicores,
    uint64_t uPeriodUs)
    __attribute__((alias("gj_cg_millicores_to_quota_us_6182")));
