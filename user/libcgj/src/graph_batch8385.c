/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8385: mutex locked soft state-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mutex_locked_id_8385(void);
 *     - Return soft mutex locked state id (1).
 *   uint32_t __gj_mutex_locked_id_8385  (alias)
 *   __libcgj_batch8385_marker = "libcgj-batch8385"
 *
 * Exclusive continuum CREATE-ONLY (8381-8390: pthread/tls id stubs —
 * once_done_u, key_ok_u, tls_slot_ok_u, mutex_unlocked_id,
 * mutex_locked_id, cond_idle_id, rwlock_r_id, rwlock_w_id,
 * thread_main_id, batch_id_8390). Unique
 * gj_mutex_locked_id_8385 surface only; no multi-def.
 * Soft pure-data catalog only; no real threads, no pthread_* libc.
 * Pair of gj_mutex_unlocked_id_8384. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8385_marker[] = "libcgj-batch8385";

/* Soft mutex locked / held state id. */
#define B8385_MUTEX_LOCKED  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8385_locked_id(void)
{
	return B8385_MUTEX_LOCKED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mutex_locked_id_8385 - soft mutex locked state catalog id.
 *
 * Always returns 1. Complements gj_mutex_unlocked_id_8384 (returns 0).
 * Does not lock or inspect real pthread_mutex_t. No parent wires.
 */
uint32_t
gj_mutex_locked_id_8385(void)
{
	(void)NULL;
	return b8385_locked_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mutex_locked_id_8385(void)
    __attribute__((alias("gj_mutex_locked_id_8385")));
