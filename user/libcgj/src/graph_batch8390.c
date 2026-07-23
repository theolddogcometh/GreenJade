/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8390: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8390(void);
 *     - Returns the compile-time graph batch number for this TU (8390).
 *   uint32_t __gj_batch_id_8390  (alias)
 *   __libcgj_batch8390_marker = "libcgj-batch8390"
 *
 * Exclusive continuum CREATE-ONLY (8381-8390: pthread/tls id stubs —
 * once_done_u, key_ok_u, tls_slot_ok_u, mutex_unlocked_id,
 * mutex_locked_id, cond_idle_id, rwlock_r_id, rwlock_w_id,
 * thread_main_id, batch_id_8390). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * Soft pure-data catalog only; no real threads, no pthread_* libc.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8390_marker[] = "libcgj-batch8390";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8390_id(void)
{
	return 8390u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8390 - report this TU's graph batch number.
 *
 * Always returns 8390. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8390(void)
{
	(void)NULL;
	return b8390_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8390(void)
    __attribute__((alias("gj_batch_id_8390")));
