/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6655: eventfd write value soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_efd_write_val_ok_6655(uint32_t hi, uint32_t lo);
 *     - Soft write-value gate: 0 when the 64-bit counter word formed as
 *       (hi<<32)|lo equals UINT64_MAX (kernel rejects 0xffffffffffffffff
 *       on write); else 1. Split hi/lo avoids wide literals.
 *   uint32_t __gj_efd_write_val_ok_6655  (alias)
 *   __libcgj_batch6655_marker = "libcgj-batch6655"
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

const char __libcgj_batch6655_marker[] = "libcgj-batch6655";

#define B6655_ALL_ONES  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6655_write_ok(uint32_t u32Hi, uint32_t u32Lo)
{
	/* Reject the all-ones 64-bit value (eventfd write EINVAL case). */
	if (u32Hi == B6655_ALL_ONES && u32Lo == B6655_ALL_ONES) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efd_write_val_ok_6655 - soft eventfd write counter value gate.
 *
 * hi: high 32 bits of the 8-byte write value
 * lo: low 32 bits of the 8-byte write value
 *
 * Returns 0 when value is UINT64_MAX, else 1. Pure integer.
 * No parent wires.
 */
uint32_t
gj_efd_write_val_ok_6655(uint32_t hi, uint32_t lo)
{
	(void)NULL;
	return b6655_write_ok(hi, lo);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efd_write_val_ok_6655(uint32_t hi, uint32_t lo)
    __attribute__((alias("gj_efd_write_val_ok_6655")));
