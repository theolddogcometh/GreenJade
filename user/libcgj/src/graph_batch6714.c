/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6714: soft futex FUTEX_CMP_REQUEUE opcode.
 *
 * Surface (unique symbols):
 *   uint32_t gj_futex_op_cmp_requeue_6714(void);
 *     - Returns 4 (FUTEX_CMP_REQUEUE soft catalog value). Pure-data
 *       opcode stub; does not compare-requeue or call futex syscalls.
 *   uint32_t __gj_futex_op_cmp_requeue_6714  (alias)
 *   __libcgj_batch6714_marker = "libcgj-batch6714"
 *
 * Exclusive continuum CREATE-ONLY (6711-6720: futex op stubs).
 * Unique gj_futex_op_cmp_requeue_6714 surface only; no multi-def.
 * Distinct from requeue (batch6713). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6714_marker[] = "libcgj-batch6714";

/* Soft FUTEX_CMP_REQUEUE catalog value (linux/futex.h base op 4). */
#define B6714_OP_CMP_REQUEUE  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6714_op_cmp_requeue(void)
{
	return B6714_OP_CMP_REQUEUE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_futex_op_cmp_requeue_6714 - soft FUTEX_CMP_REQUEUE opcode value.
 *
 * Always returns 4. Soft compile-time opcode id; does not requeue.
 * No parent wires.
 */
uint32_t
gj_futex_op_cmp_requeue_6714(void)
{
	(void)NULL;
	return b6714_op_cmp_requeue();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_futex_op_cmp_requeue_6714(void)
    __attribute__((alias("gj_futex_op_cmp_requeue_6714")));
