/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6657: eventfd counter low-half add soft stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efd_counter_add_lo_6657(uint32_t cur_lo, uint32_t delta_lo);
 *     - Soft low-32 counter add: returns (cur_lo + delta_lo) mod 2^32.
 *       Models the low half of an eventfd write increment without
 *       wide arithmetic types.
 *   uint32_t __gj_efd_counter_add_lo_6657  (alias)
 *   __libcgj_batch6657_marker = "libcgj-batch6657"
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

const char __libcgj_batch6657_marker[] = "libcgj-batch6657";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6657_add_lo(uint32_t u32Cur, uint32_t u32Delta)
{
	return u32Cur + u32Delta;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efd_counter_add_lo_6657 - soft eventfd counter low-half add.
 *
 * cur_lo:   current counter low 32 bits
 * delta_lo: addend low 32 bits (from write value)
 *
 * Returns wrapped sum of the low halves. Pure integer. No parent wires.
 */
uint32_t
gj_efd_counter_add_lo_6657(uint32_t cur_lo, uint32_t delta_lo)
{
	(void)NULL;
	return b6657_add_lo(cur_lo, delta_lo);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efd_counter_add_lo_6657(uint32_t cur_lo, uint32_t delta_lo)
    __attribute__((alias("gj_efd_counter_add_lo_6657")));
