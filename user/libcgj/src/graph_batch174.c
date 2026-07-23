/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch174: token-bucket rate limiter helpers.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party rate-limit source was copied.
 *
 * This TU adds only unique symbols:
 *   typedef struct {
 *       uint64_t tokens;
 *       uint64_t capacity;
 *       uint64_t refill_per_sec;
 *       uint64_t last_ms;
 *   } gj_token_bucket_t;
 *   void gj_tb_init(gj_token_bucket_t *tb, uint64_t capacity,
 *                   uint64_t refill_per_sec, uint64_t now_ms);
 *     — fill bucket to capacity; record refill rate and last_ms = now_ms.
 *   int  gj_tb_try_take(gj_token_bucket_t *tb, uint64_t n, uint64_t now_ms);
 *     — refill from elapsed wall time, then try to deduct n tokens.
 *       Returns 0 if taken, -1 if insufficient / NULL.
 *   __gj_tb_init / __gj_tb_try_take  (aliases)
 *   __libcgj_batch174_marker = "libcgj-batch174"
 *
 * Notes:
 *   - Time unit is milliseconds (caller supplies now_ms).
 *   - Refill: floor(elapsed_ms * refill_per_sec / 1000), capped at capacity.
 *   - No errno (freestanding-safe); callers use return codes only.
 *   - Single-threaded / external-sync; no atomics in this TU.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch174_marker[] = "libcgj-batch174";

/* Token bucket: tokens grow at refill_per_sec up to capacity. */
typedef struct {
	uint64_t tokens;
	uint64_t capacity;
	uint64_t refill_per_sec;
	uint64_t last_ms;
} gj_token_bucket_t;

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Add floor(elapsed_ms * refill_per_sec / 1000) tokens, saturating at
 * capacity. Advances last_ms to now_ms when elapsed is positive.
 * Handles clock going backwards (no refill, leave last_ms alone).
 * Guards against overflow of the product elapsed * rate.
 */
static void
b174_refill(gj_token_bucket_t *pTb, uint64_t uNowMs)
{
	uint64_t uElapsed;
	uint64_t uAdd;
	uint64_t uRoom;
	uint64_t uRate;
	uint64_t uMaxElapsed;

	if (uNowMs < pTb->last_ms) {
		/* Clock skew / wrap: do not refill; resync last_ms. */
		pTb->last_ms = uNowMs;
		return;
	}

	uElapsed = uNowMs - pTb->last_ms;
	if (uElapsed == 0u || pTb->refill_per_sec == 0u) {
		return;
	}

	/*
	 * tokens += (elapsed_ms * refill_per_sec) / 1000
	 * Cap elapsed so the product fits in uint64_t:
	 *   elapsed <= UINT64_MAX / refill_per_sec
	 * When capped, the add still saturates at capacity below.
	 */
	uRate = pTb->refill_per_sec;
	uMaxElapsed = UINT64_MAX / uRate;
	if (uElapsed > uMaxElapsed) {
		uElapsed = uMaxElapsed;
	}

	uAdd = (uElapsed * uRate) / 1000u;
	if (uAdd == 0u) {
		/* Not enough elapsed time for a whole token — leave last_ms
		 * so partial progress accumulates across calls.
		 */
		return;
	}

	/*
	 * Advance last_ms by the exact ms consumed for uAdd whole tokens
	 * (floor), preserving the remainder for the next refill.
	 * ms_used = floor(uAdd * 1000 / uRate); if product overflows, fall
	 * back to syncing last_ms to now.
	 */
	if (uAdd <= UINT64_MAX / 1000u) {
		uint64_t uMsUsed = (uAdd * 1000u) / uRate;

		if (uMsUsed > 0u && uMsUsed <= (uNowMs - pTb->last_ms)) {
			pTb->last_ms += uMsUsed;
		} else {
			pTb->last_ms = uNowMs;
		}
	} else {
		pTb->last_ms = uNowMs;
	}

	if (pTb->tokens >= pTb->capacity) {
		pTb->tokens = pTb->capacity;
		return;
	}

	uRoom = pTb->capacity - pTb->tokens;
	if (uAdd > uRoom) {
		pTb->tokens = pTb->capacity;
	} else {
		pTb->tokens += uAdd;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * Initialise a token bucket full (tokens = capacity).
 * capacity and refill_per_sec may be zero (bucket never grants / never refills).
 * No-op if tb is NULL.
 */
void
gj_tb_init(gj_token_bucket_t *pTb, uint64_t uCapacity, uint64_t uRefillPerSec,
    uint64_t uNowMs)
{
	if (pTb == NULL) {
		return;
	}
	pTb->capacity = uCapacity;
	pTb->refill_per_sec = uRefillPerSec;
	pTb->tokens = uCapacity;
	pTb->last_ms = uNowMs;
}

void __gj_tb_init(gj_token_bucket_t *pTb, uint64_t uCapacity,
    uint64_t uRefillPerSec, uint64_t uNowMs)
    __attribute__((alias("gj_tb_init")));

/*
 * Refill from wall-clock progress, then try to take n tokens.
 * Returns 0 if n tokens were deducted, -1 if tb is NULL or tokens < n.
 * Taking n == 0 always succeeds when tb is non-NULL (after refill).
 */
int
gj_tb_try_take(gj_token_bucket_t *pTb, uint64_t uN, uint64_t uNowMs)
{
	if (pTb == NULL) {
		return -1;
	}

	b174_refill(pTb, uNowMs);

	if (uN == 0u) {
		return 0;
	}
	if (pTb->tokens < uN) {
		return -1;
	}
	pTb->tokens -= uN;
	return 0;
}

int __gj_tb_try_take(gj_token_bucket_t *pTb, uint64_t uN, uint64_t uNowMs)
    __attribute__((alias("gj_tb_try_take")));
