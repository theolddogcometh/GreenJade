/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6716: soft futex FUTEX_LOCK_PI opcode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_futex_op_lock_pi_6716(void);
 *     - Returns 6 (FUTEX_LOCK_PI soft catalog value). Pure-data opcode
 *       stub; does not take a PI lock or call futex syscalls.
 *   uint32_t __gj_futex_op_lock_pi_6716  (alias)
 *   __libcgj_batch6716_marker = "libcgj-batch6716"
 *
 * Exclusive continuum CREATE-ONLY (6711-6720: futex op stubs).
 * Unique gj_futex_op_lock_pi_6716 surface only; no multi-def.
 * Distinct from unlock_pi (batch6717). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6716_marker[] = "libcgj-batch6716";

/* Soft FUTEX_LOCK_PI catalog value (linux/futex.h base op 6). */
#define B6716_OP_LOCK_PI  6u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6716_op_lock_pi(void)
{
	return B6716_OP_LOCK_PI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_futex_op_lock_pi_6716 - soft FUTEX_LOCK_PI opcode catalog value.
 *
 * Always returns 6. Soft compile-time opcode id; does not lock PI.
 * No parent wires.
 */
uint32_t
gj_futex_op_lock_pi_6716(void)
{
	(void)NULL;
	return b6716_op_lock_pi();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_futex_op_lock_pi_6716(void)
    __attribute__((alias("gj_futex_op_lock_pi_6716")));
