/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6972: process_vm soft WRITEV op stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pvm_is_writev_u_6972(uint32_t op);
 *     - Return 1 if op is soft PVM_WRITEV (2), else 0.
 *   uint32_t __gj_pvm_is_writev_u_6972  (alias)
 *   __libcgj_batch6972_marker = "libcgj-batch6972"
 *
 * Exclusive continuum CREATE-ONLY (6971-6980: process_vm_readv/writev stubs —
 * is_readv, is_writev, op_ok, readv_id, writev_id, pack, flags_ok,
 * iovcnt_ok, errorish, batch_id_6980). Unique
 * gj_pvm_is_writev_u_6972 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6972_marker[] = "libcgj-batch6972";

/* Soft process_vm op catalog: WRITEV = local->remote (value 2). */
#define B6972_PVM_WRITEV ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6972_pvm_is_writev(uint32_t u32Op)
{
	return (u32Op == B6972_PVM_WRITEV) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pvm_is_writev_u_6972 - 1 if op is soft PVM_WRITEV.
 *
 * op: soft process_vm direction/op catalog id
 *
 * Returns 1 when op equals soft PVM_WRITEV (2), else 0.
 * Pure integer compare; does not call process_vm_writev. No parent wires.
 */
uint32_t
gj_pvm_is_writev_u_6972(uint32_t u32Op)
{
	(void)NULL;
	return b6972_pvm_is_writev(u32Op);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pvm_is_writev_u_6972(uint32_t u32Op)
    __attribute__((alias("gj_pvm_is_writev_u_6972")));
