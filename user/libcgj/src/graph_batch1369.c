/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1369: token-bucket fill after elapsed ms.
 *
 * Surface (unique symbols):
 *   uint64_t gj_rate_limit_tokens(uint64_t tokens, uint64_t capacity,
 *                                uint64_t rate_per_sec, uint64_t dt_ms);
 *     — Pure refill step: tokens' = min(capacity,
 *       tokens + rate_per_sec * dt_ms / 1000). No state object; caller
 *       owns the bucket. Distinct from gj_tb_init/try_take (batch174).
 *   uint64_t __gj_rate_limit_tokens  (alias)
 *   __libcgj_batch1369_marker = "libcgj-batch1369"
 *
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1369_marker[] = "libcgj-batch1369";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating add of two uint64 values. */
static uint64_t
b1369_sat_add(uint64_t u64A, uint64_t u64B)
{
	if (u64A > UINT64_MAX - u64B) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/*
 * Fill amount = rate_per_sec * dt_ms / 1000 with overflow-safe multiply.
 * Saturates to UINT64_MAX on overflow (caller clamps to capacity).
 */
static uint64_t
b1369_fill(uint64_t u64RatePerSec, uint64_t u64DtMs)
{
	uint64_t u64Whole;
	uint64_t u64Rem;
	uint64_t u64Hi;
	uint64_t u64Lo;

	if (u64RatePerSec == 0ull || u64DtMs == 0ull) {
		return 0ull;
	}

	/* Prefer (rate * dt) / 1000 when product fits. */
	if (u64RatePerSec <= UINT64_MAX / u64DtMs) {
		return (u64RatePerSec * u64DtMs) / 1000ull;
	}

	/*
	 * Large factors: rate * (dt/1000) + rate * (dt%1000) / 1000.
	 * Guard each multiply against overflow → saturate.
	 */
	u64Whole = u64DtMs / 1000ull;
	u64Rem = u64DtMs % 1000ull;

	if (u64Whole != 0ull && u64RatePerSec > UINT64_MAX / u64Whole) {
		return UINT64_MAX;
	}
	u64Hi = u64RatePerSec * u64Whole;

	if (u64Rem != 0ull && u64RatePerSec > UINT64_MAX / u64Rem) {
		u64Lo = UINT64_MAX; /* /1000 still huge */
	} else {
		u64Lo = (u64RatePerSec * u64Rem) / 1000ull;
	}
	return b1369_sat_add(u64Hi, u64Lo);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rate_limit_tokens — token-bucket level after dt_ms of refill.
 *
 * u64Tokens:     current tokens
 * u64Capacity:   bucket cap (result never exceeds this)
 * u64RatePerSec: refill rate in tokens per second
 * u64DtMs:       elapsed milliseconds
 *
 * If capacity is 0, returns 0. Pure; no side effects.
 */
uint64_t
gj_rate_limit_tokens(uint64_t u64Tokens, uint64_t u64Capacity,
    uint64_t u64RatePerSec, uint64_t u64DtMs)
{
	uint64_t u64Add;
	uint64_t u64Next;

	if (u64Capacity == 0ull) {
		return 0ull;
	}

	/* Already full — no growth past capacity. */
	if (u64Tokens >= u64Capacity) {
		return u64Capacity;
	}

	u64Add = b1369_fill(u64RatePerSec, u64DtMs);
	u64Next = b1369_sat_add(u64Tokens, u64Add);
	if (u64Next > u64Capacity) {
		return u64Capacity;
	}
	return u64Next;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rate_limit_tokens(uint64_t u64Tokens, uint64_t u64Capacity,
    uint64_t u64RatePerSec, uint64_t u64DtMs)
    __attribute__((alias("gj_rate_limit_tokens")));
