/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6659: eventfd counter visible soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efd_counter_visible_6659(uint32_t cur, uint32_t sem_mode);
 *     - Soft read-visible value: when sem_mode is non-zero, returns 1
 *       if cur > 0 else 0 (semaphore read reports unit); when
 *       sem_mode is zero, returns cur (normal counter read reports
 *       the full truncated value).
 *   uint32_t __gj_efd_counter_visible_6659  (alias)
 *   __libcgj_batch6659_marker = "libcgj-batch6659"
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

const char __libcgj_batch6659_marker[] = "libcgj-batch6659";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6659_visible(uint32_t u32Cur, uint32_t u32Sem)
{
	if (u32Sem != 0u) {
		if (u32Cur > 0u) {
			return 1u;
		}
		return 0u;
	}
	return u32Cur;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efd_counter_visible_6659 - soft eventfd read-visible counter value.
 *
 * cur:      current counter (truncated soft view)
 * sem_mode: non-zero when EFD_SEMAPHORE semantics apply
 *
 * Returns unit (0/1) in semaphore mode, else cur. Pure integer.
 * No parent wires.
 */
uint32_t
gj_efd_counter_visible_6659(uint32_t cur, uint32_t sem_mode)
{
	(void)NULL;
	return b6659_visible(cur, sem_mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efd_counter_visible_6659(uint32_t cur, uint32_t sem_mode)
    __attribute__((alias("gj_efd_counter_visible_6659")));
