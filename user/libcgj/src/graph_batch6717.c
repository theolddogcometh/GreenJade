/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6717: soft futex FUTEX_UNLOCK_PI opcode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_futex_op_unlock_pi_6717(void);
 *     - Returns 7 (FUTEX_UNLOCK_PI soft catalog value). Pure-data opcode
 *       stub; does not release a PI lock or call futex syscalls.
 *   uint32_t __gj_futex_op_unlock_pi_6717  (alias)
 *   __libcgj_batch6717_marker = "libcgj-batch6717"
 *
 * Exclusive continuum CREATE-ONLY (6711-6720: futex op stubs).
 * Unique gj_futex_op_unlock_pi_6717 surface only; no multi-def.
 * Distinct from lock_pi (batch6716). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6717_marker[] = "libcgj-batch6717";

/* Soft FUTEX_UNLOCK_PI catalog value (linux/futex.h base op 7). */
#define B6717_OP_UNLOCK_PI  7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6717_op_unlock_pi(void)
{
	return B6717_OP_UNLOCK_PI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_futex_op_unlock_pi_6717 - soft FUTEX_UNLOCK_PI opcode catalog value.
 *
 * Always returns 7. Soft compile-time opcode id; does not unlock PI.
 * No parent wires.
 */
uint32_t
gj_futex_op_unlock_pi_6717(void)
{
	(void)NULL;
	return b6717_op_unlock_pi();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_futex_op_unlock_pi_6717(void)
    __attribute__((alias("gj_futex_op_unlock_pi_6717")));
