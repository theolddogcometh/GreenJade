/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6973: process_vm soft op validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pvm_op_ok_u_6973(uint32_t op);
 *     - Return 1 if op is a known soft PVM op (1..2), else 0.
 *   uint32_t __gj_pvm_op_ok_u_6973  (alias)
 *   __libcgj_batch6973_marker = "libcgj-batch6973"
 *
 * Exclusive continuum CREATE-ONLY (6971-6980: process_vm_readv/writev stubs —
 * is_readv, is_writev, op_ok, readv_id, writev_id, pack, flags_ok,
 * iovcnt_ok, errorish, batch_id_6980). Unique
 * gj_pvm_op_ok_u_6973 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6973_marker[] = "libcgj-batch6973";

/* Known soft process_vm ops: READV=1, WRITEV=2. */
#define B6973_PVM_OP_MIN ((uint32_t)1u)
#define B6973_PVM_OP_MAX ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6973_pvm_op_ok(uint32_t u32Op)
{
	return (u32Op >= B6973_PVM_OP_MIN &&
	        u32Op <= B6973_PVM_OP_MAX) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pvm_op_ok_u_6973 - 1 if op is a known soft process_vm op.
 *
 * op: soft process_vm direction/op catalog id
 *
 * Returns 1 for READV (1) or WRITEV (2), else 0.
 * Soft range check; no parent wires.
 */
uint32_t
gj_pvm_op_ok_u_6973(uint32_t u32Op)
{
	(void)NULL;
	return b6973_pvm_op_ok(u32Op);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pvm_op_ok_u_6973(uint32_t u32Op)
    __attribute__((alias("gj_pvm_op_ok_u_6973")));
