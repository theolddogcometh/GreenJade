/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6656: eventfd semaphore mode soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efd_sem_mode_6656(uint32_t flags);
 *     - Soft semaphore-mode probe: 1 when EFD_SEMAPHORE (bit 0) is set
 *       in flags; else 0. Pure flag bit test.
 *   uint32_t __gj_efd_sem_mode_6656  (alias)
 *   __libcgj_batch6656_marker = "libcgj-batch6656"
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

const char __libcgj_batch6656_marker[] = "libcgj-batch6656";

#define B6656_EFD_SEMAPHORE  0x00000001u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6656_sem_mode(uint32_t u32Flags)
{
	if ((u32Flags & B6656_EFD_SEMAPHORE) != 0u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efd_sem_mode_6656 - soft eventfd EFD_SEMAPHORE mode probe.
 *
 * flags: eventfd2 flags word
 *
 * Returns 1 when bit 0 (EFD_SEMAPHORE) is set, else 0.
 * Pure integer. No parent wires.
 */
uint32_t
gj_efd_sem_mode_6656(uint32_t flags)
{
	(void)NULL;
	return b6656_sem_mode(flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efd_sem_mode_6656(uint32_t flags)
    __attribute__((alias("gj_efd_sem_mode_6656")));
