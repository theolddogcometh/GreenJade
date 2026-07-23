/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8386: condvar idle soft state-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cond_idle_id_8386(void);
 *     - Return soft condition-variable idle / no-waiters id (0).
 *   uint32_t __gj_cond_idle_id_8386  (alias)
 *   __libcgj_batch8386_marker = "libcgj-batch8386"
 *
 * Exclusive continuum CREATE-ONLY (8381-8390: pthread/tls id stubs —
 * once_done_u, key_ok_u, tls_slot_ok_u, mutex_unlocked_id,
 * mutex_locked_id, cond_idle_id, rwlock_r_id, rwlock_w_id,
 * thread_main_id, batch_id_8390). Unique
 * gj_cond_idle_id_8386 surface only; no multi-def.
 * Soft pure-data catalog only; no real threads, no pthread_* libc.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8386_marker[] = "libcgj-batch8386";

/* Soft cond idle state: no waiters (single-thread freestanding world). */
#define B8386_COND_IDLE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8386_idle_id(void)
{
	return B8386_COND_IDLE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cond_idle_id_8386 - soft pthread_cond idle / no-waiters catalog id.
 *
 * Always returns 0. Documents the freestanding "no waiters" cond
 * surface; does not call pthread_cond_wait/signal. No parent wires.
 */
uint32_t
gj_cond_idle_id_8386(void)
{
	(void)NULL;
	return b8386_idle_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cond_idle_id_8386(void)
    __attribute__((alias("gj_cond_idle_id_8386")));
