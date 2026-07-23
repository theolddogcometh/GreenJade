/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8383: TLS slot soft validity predicate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tls_slot_ok_u_8383(uint32_t slot);
 *     - Return 1 if slot is in [0, 127] (soft TLS slot range), else 0.
 *   uint32_t __gj_tls_slot_ok_u_8383  (alias)
 *   __libcgj_batch8383_marker = "libcgj-batch8383"
 *
 * Exclusive continuum CREATE-ONLY (8381-8390: pthread/tls id stubs —
 * once_done_u, key_ok_u, tls_slot_ok_u, mutex_unlocked_id,
 * mutex_locked_id, cond_idle_id, rwlock_r_id, rwlock_w_id,
 * thread_main_id, batch_id_8390). Unique
 * gj_tls_slot_ok_u_8383 surface only; no multi-def.
 * Soft pure-data catalog only; no real threads, no pthread_* libc.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8383_marker[] = "libcgj-batch8383";

/* Soft TLS slot upper bound (exclusive); not a real TCB layout limit. */
#define B8383_TLS_SLOT_MAX  128u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8383_slot_ok(uint32_t u32Slot)
{
	if (u32Slot < B8383_TLS_SLOT_MAX)
		return 1u;
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tls_slot_ok_u_8383 - soft TLS slot-index validity predicate.
 *
 * slot: soft TLS key/slot index
 *
 * Returns 1 when slot is in [0, 127], else 0. Catalog only; does not
 * touch real thread-local storage. No parent wires.
 */
uint32_t
gj_tls_slot_ok_u_8383(uint32_t u32Slot)
{
	(void)NULL;
	return b8383_slot_ok(u32Slot);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tls_slot_ok_u_8383(uint32_t u32Slot)
    __attribute__((alias("gj_tls_slot_ok_u_8383")));
