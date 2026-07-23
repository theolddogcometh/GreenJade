/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6651: eventfd initval soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efd_initval_ok_6651(uint32_t initval);
 *     - Soft eventfd create initval gate: always 1 (any unsigned
 *       initval is accepted by the kernel eventfd2 path in this stub
 *       model; presence tags the continuum start).
 *   uint32_t __gj_efd_initval_ok_6651  (alias)
 *   __libcgj_batch6651_marker = "libcgj-batch6651"
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

const char __libcgj_batch6651_marker[] = "libcgj-batch6651";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6651_initval_ok(uint32_t u32Init)
{
	(void)u32Init;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efd_initval_ok_6651 - soft eventfd create initval validity.
 *
 * initval: initial counter value passed to eventfd/eventfd2
 *
 * Returns 1 for any initval (soft always-ok). Pure integer.
 * No parent wires.
 */
uint32_t
gj_efd_initval_ok_6651(uint32_t initval)
{
	(void)NULL;
	return b6651_initval_ok(initval);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efd_initval_ok_6651(uint32_t initval)
    __attribute__((alias("gj_efd_initval_ok_6651")));
