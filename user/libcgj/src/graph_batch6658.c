/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6658: eventfd semaphore take soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efd_counter_sem_take_6658(uint32_t cur);
 *     - Soft EFD_SEMAPHORE-style take on a truncated counter view:
 *       if cur > 0 return cur - 1, else return 0. Models read that
 *       decrements by one when non-zero.
 *   uint32_t __gj_efd_counter_sem_take_6658  (alias)
 *   __libcgj_batch6658_marker = "libcgj-batch6658"
 *
 * Exclusive continuum CREATE-ONLY (6651-6660: eventfd counter stubs —
 * initval_ok_6651, flags_ok_6652, counter_bytes_6653, read_len_ok_6654,
 * write_val_ok_6655, sem_mode_6656, counter_add_lo_6657,
 * counter_sem_take_6658, counter_visible_6659, counter_ready +
 * batch_id_6660). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6658_marker[] = "libcgj-batch6658";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6658_sem_take(uint32_t u32Cur)
{
	if (u32Cur == 0u) {
		return 0u;
	}
	return u32Cur - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efd_counter_sem_take_6658 - soft eventfd semaphore-mode take.
 *
 * cur: current counter (truncated soft view)
 *
 * Returns cur - 1 when cur > 0, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_efd_counter_sem_take_6658(uint32_t cur)
{
	(void)NULL;
	return b6658_sem_take(cur);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efd_counter_sem_take_6658(uint32_t cur)
    __attribute__((alias("gj_efd_counter_sem_take_6658")));
