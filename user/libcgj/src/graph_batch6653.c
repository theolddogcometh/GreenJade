/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6653: eventfd counter width (bytes) stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efd_counter_bytes_6653(void);
 *     - Returns 8 (eventfd counter is an 8-byte uint64 host-endian
 *       value on read/write). Soft constant for buffer sizing.
 *   uint32_t __gj_efd_counter_bytes_6653  (alias)
 *   __libcgj_batch6653_marker = "libcgj-batch6653"
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

const char __libcgj_batch6653_marker[] = "libcgj-batch6653";

/* eventfd counter transfer size in bytes. */
#define B6653_COUNTER_BYTES  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6653_bytes(void)
{
	return B6653_COUNTER_BYTES;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efd_counter_bytes_6653 - report eventfd counter byte width.
 *
 * Always returns 8. Soft sizing constant for read/write buffers.
 * Pure integer. No parent wires.
 */
uint32_t
gj_efd_counter_bytes_6653(void)
{
	(void)NULL;
	return b6653_bytes();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efd_counter_bytes_6653(void)
    __attribute__((alias("gj_efd_counter_bytes_6653")));
