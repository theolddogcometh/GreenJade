/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8381: pthread_once soft-done value stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pthread_once_done_u_8381(void);
 *     - Return soft pthread_once "done" control value (1). Catalog of
 *       the completed-once state; not a real pthread_once(3).
 *   uint32_t __gj_pthread_once_done_u_8381  (alias)
 *   __libcgj_batch8381_marker = "libcgj-batch8381"
 *
 * Exclusive continuum CREATE-ONLY (8381-8390: pthread/tls id stubs —
 * once_done_u, key_ok_u, tls_slot_ok_u, mutex_unlocked_id,
 * mutex_locked_id, cond_idle_id, rwlock_r_id, rwlock_w_id,
 * thread_main_id, batch_id_8390). Unique
 * gj_pthread_once_done_u_8381 surface only; no multi-def.
 * Soft pure-data catalog only; no real threads, no pthread_* libc.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8381_marker[] = "libcgj-batch8381";

/* Soft once-control "done" value (PTHREAD_ONCE_INIT-style 0 is not done). */
#define B8381_ONCE_DONE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8381_once_done(void)
{
	return B8381_ONCE_DONE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pthread_once_done_u_8381 - soft pthread_once completed-state value.
 *
 * Always returns 1. Compile-time product tag for the once-done control
 * word; does not call pthread_once or inspect real once_t. No parent
 * wires.
 */
uint32_t
gj_pthread_once_done_u_8381(void)
{
	(void)NULL;
	return b8381_once_done();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pthread_once_done_u_8381(void)
    __attribute__((alias("gj_pthread_once_done_u_8381")));
