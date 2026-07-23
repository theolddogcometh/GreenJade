/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6979: process_vm soft op errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pvm_op_errorish_u_6979(uint32_t op);
 *     - Return 1 if op is unknown/invalid (not 1..2), else 0.
 *   uint32_t __gj_pvm_op_errorish_u_6979  (alias)
 *   __libcgj_batch6979_marker = "libcgj-batch6979"
 *
 * Exclusive continuum CREATE-ONLY (6971-6980: process_vm_readv/writev stubs —
 * is_readv, is_writev, op_ok, readv_id, writev_id, pack, flags_ok,
 * iovcnt_ok, errorish, batch_id_6980). Unique
 * gj_pvm_op_errorish_u_6979 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6979_marker[] = "libcgj-batch6979";

/* Known soft process_vm ops: READV=1, WRITEV=2. */
#define B6979_PVM_OP_MIN ((uint32_t)1u)
#define B6979_PVM_OP_MAX ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6979_pvm_op_errorish(uint32_t u32Op)
{
	return (u32Op < B6979_PVM_OP_MIN ||
	        u32Op > B6979_PVM_OP_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pvm_op_errorish_u_6979 - 1 if op is unknown/invalid.
 *
 * op: soft process_vm direction/op catalog id
 *
 * Returns 1 when op is outside the known set (1..2), including 0
 * and future/unknown values. Soft inverse of op_ok; no parent wires.
 */
uint32_t
gj_pvm_op_errorish_u_6979(uint32_t u32Op)
{
	(void)NULL;
	return b6979_pvm_op_errorish(u32Op);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pvm_op_errorish_u_6979(uint32_t u32Op)
    __attribute__((alias("gj_pvm_op_errorish_u_6979")));
