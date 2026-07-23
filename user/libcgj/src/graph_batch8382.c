/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8382: pthread_key soft validity predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pthread_key_ok_u_8382(uint32_t key);
 *     - Return 1 if key is nonzero (soft valid key id), else 0.
 *   uint32_t __gj_pthread_key_ok_u_8382  (alias)
 *   __libcgj_batch8382_marker = "libcgj-batch8382"
 *
 * Exclusive continuum CREATE-ONLY (8381-8390: pthread/tls id stubs —
 * once_done_u, key_ok_u, tls_slot_ok_u, mutex_unlocked_id,
 * mutex_locked_id, cond_idle_id, rwlock_r_id, rwlock_w_id,
 * thread_main_id, batch_id_8390). Unique
 * gj_pthread_key_ok_u_8382 surface only; no multi-def.
 * Soft pure-data catalog only; no real threads, no pthread_* libc.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8382_marker[] = "libcgj-batch8382";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8382_key_ok(uint32_t u32Key)
{
	if (u32Key != 0u)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pthread_key_ok_u_8382 - soft pthread_key_t validity predicate.
 *
 * key: opaque soft key id (0 treated as invalid / destroyed)
 *
 * Returns 1 when key != 0, else 0. Does not call pthread_key_create
 * or touch real TLS. No parent wires.
 */
uint32_t
gj_pthread_key_ok_u_8382(uint32_t u32Key)
{
	(void)NULL;
	return b8382_key_ok(u32Key);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pthread_key_ok_u_8382(uint32_t u32Key)
    __attribute__((alias("gj_pthread_key_ok_u_8382")));
