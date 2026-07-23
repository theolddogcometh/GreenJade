/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6654: eventfd read length soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efd_read_len_ok_6654(uint32_t nbytes);
 *     - Soft read-buffer gate: 1 when nbytes >= 8 (kernel requires a
 *       full 8-byte counter buffer); else 0.
 *   uint32_t __gj_efd_read_len_ok_6654  (alias)
 *   __libcgj_batch6654_marker = "libcgj-batch6654"
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

const char __libcgj_batch6654_marker[] = "libcgj-batch6654";

#define B6654_READ_MIN  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6654_read_ok(uint32_t u32N)
{
	if (u32N < B6654_READ_MIN) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efd_read_len_ok_6654 - soft eventfd read buffer length gate.
 *
 * nbytes: caller buffer size in bytes for read(2) of the counter
 *
 * Returns 1 when nbytes >= 8, else 0. Pure integer. No parent wires.
 */
uint32_t
gj_efd_read_len_ok_6654(uint32_t nbytes)
{
	(void)NULL;
	return b6654_read_ok(nbytes);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efd_read_len_ok_6654(uint32_t nbytes)
    __attribute__((alias("gj_efd_read_len_ok_6654")));
