/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch547: simple sequential RCU-style epoch stamps.
 *
 * Freestanding helpers for cooperative / single-threaded epoch counters.
 * No atomics required (or available under -ffreestanding pure C): callers
 * serialize access themselves.
 *
 * Surface (unique symbols):
 *   uint64_t gj_rcu_stamp(uint64_t *epoch);
 *     — Read *epoch. Returns 0 if epoch is NULL.
 *   void gj_rcu_tick(uint64_t *epoch);
 *     — (*epoch)++ if epoch is non-NULL; no-op if NULL.
 *   __gj_rcu_stamp  (alias)
 *   __gj_rcu_tick   (alias)
 *   __libcgj_batch547_marker = "libcgj-batch547"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch547_marker[] = "libcgj-batch547";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rcu_stamp — sample the current epoch counter.
 *
 * epoch: pointer to caller-owned uint64_t epoch; NULL yields 0.
 *
 * Returns *epoch, or 0 if epoch is NULL. Plain load; not atomic.
 */
uint64_t
gj_rcu_stamp(uint64_t *epoch)
{
	if (epoch == NULL) {
		return 0u;
	}
	return *epoch;
}

/*
 * gj_rcu_tick — advance the epoch counter by one.
 *
 * epoch: pointer to caller-owned uint64_t epoch; NULL is a no-op.
 *
 * Post-increment style: (*epoch)++. Wraps on uint64_t overflow (accepted).
 * Plain store; not atomic.
 */
void
gj_rcu_tick(uint64_t *epoch)
{
	if (epoch == NULL) {
		return;
	}
	(*epoch)++;
}

/* ---- underscored aliases ----------------------------------------------- */

uint64_t __gj_rcu_stamp(uint64_t *epoch)
    __attribute__((alias("gj_rcu_stamp")));

void __gj_rcu_tick(uint64_t *epoch)
    __attribute__((alias("gj_rcu_tick")));
