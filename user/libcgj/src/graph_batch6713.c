/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6713: soft futex FUTEX_REQUEUE opcode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_futex_op_requeue_6713(void);
 *     - Returns 3 (FUTEX_REQUEUE soft catalog value). Pure-data opcode
 *       stub; does not requeue waiters or call futex syscalls.
 *   uint32_t __gj_futex_op_requeue_6713  (alias)
 *   __libcgj_batch6713_marker = "libcgj-batch6713"
 *
 * Exclusive continuum CREATE-ONLY (6711-6720: futex op stubs).
 * Unique gj_futex_op_requeue_6713 surface only; no multi-def.
 * Distinct from cmp_requeue (batch6714). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6713_marker[] = "libcgj-batch6713";

/* Soft FUTEX_REQUEUE catalog value (linux/futex.h base op 3). */
#define B6713_OP_REQUEUE  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6713_op_requeue(void)
{
	return B6713_OP_REQUEUE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_futex_op_requeue_6713 - soft FUTEX_REQUEUE opcode catalog value.
 *
 * Always returns 3. Soft compile-time opcode id; does not requeue.
 * No parent wires.
 */
uint32_t
gj_futex_op_requeue_6713(void)
{
	(void)NULL;
	return b6713_op_requeue();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_futex_op_requeue_6713(void)
    __attribute__((alias("gj_futex_op_requeue_6713")));
