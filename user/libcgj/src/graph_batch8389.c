/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8389: main-thread soft id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_thread_main_id_8389(void);
 *     - Return soft main-thread id (1) for the freestanding single-
 *       thread model.
 *   uint32_t __gj_thread_main_id_8389  (alias)
 *   __libcgj_batch8389_marker = "libcgj-batch8389"
 *
 * Exclusive continuum CREATE-ONLY (8381-8390: pthread/tls id stubs —
 * once_done_u, key_ok_u, tls_slot_ok_u, mutex_unlocked_id,
 * mutex_locked_id, cond_idle_id, rwlock_r_id, rwlock_w_id,
 * thread_main_id, batch_id_8390). Unique
 * gj_thread_main_id_8389 surface only; no multi-def.
 * Soft pure-data catalog only; no real threads, no pthread_* libc.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8389_marker[] = "libcgj-batch8389";

/* Soft main-thread id in the single-thread freestanding catalog. */
#define B8389_THREAD_MAIN_ID  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8389_main_id(void)
{
	return B8389_THREAD_MAIN_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_thread_main_id_8389 - soft main-thread catalog id.
 *
 * Always returns 1. Documents that the freestanding product model has
 * a single main thread identity; does not call pthread_self or spawn
 * threads. No parent wires.
 */
uint32_t
gj_thread_main_id_8389(void)
{
	(void)NULL;
	return b8389_main_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_thread_main_id_8389(void)
    __attribute__((alias("gj_thread_main_id_8389")));
