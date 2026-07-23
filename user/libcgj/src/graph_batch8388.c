/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8388: rwlock writer soft hold-id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rwlock_w_id_8388(void);
 *     - Return soft rwlock writer-hold family id (2).
 *   uint32_t __gj_rwlock_w_id_8388  (alias)
 *   __libcgj_batch8388_marker = "libcgj-batch8388"
 *
 * Exclusive continuum CREATE-ONLY (8381-8390: pthread/tls id stubs —
 * once_done_u, key_ok_u, tls_slot_ok_u, mutex_unlocked_id,
 * mutex_locked_id, cond_idle_id, rwlock_r_id, rwlock_w_id,
 * thread_main_id, batch_id_8390). Unique
 * gj_rwlock_w_id_8388 surface only; no multi-def.
 * Soft pure-data catalog only; no real threads, no pthread_* libc.
 * Pair of gj_rwlock_r_id_8387. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8388_marker[] = "libcgj-batch8388";

/* Soft rwlock writer-hold family id (2 = wrlock path). */
#define B8388_RWLOCK_W_ID  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8388_w_id(void)
{
	return B8388_RWLOCK_W_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rwlock_w_id_8388 - soft pthread_rwlock writer-hold catalog id.
 *
 * Always returns 2. Complements gj_rwlock_r_id_8387 (returns 1).
 * Does not call pthread_rwlock_wrlock. No parent wires.
 */
uint32_t
gj_rwlock_w_id_8388(void)
{
	(void)NULL;
	return b8388_w_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rwlock_w_id_8388(void)
    __attribute__((alias("gj_rwlock_w_id_8388")));
