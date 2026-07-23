/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6971: process_vm soft READV op stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pvm_is_readv_u_6971(uint32_t op);
 *     - Return 1 if op is soft PVM_READV (1), else 0.
 *   uint32_t __gj_pvm_is_readv_u_6971  (alias)
 *   __libcgj_batch6971_marker = "libcgj-batch6971"
 *
 * Exclusive continuum CREATE-ONLY (6971-6980: process_vm_readv/writev stubs —
 * is_readv, is_writev, op_ok, readv_id, writev_id, pack, flags_ok,
 * iovcnt_ok, errorish, batch_id_6980). Unique
 * gj_pvm_is_readv_u_6971 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6971_marker[] = "libcgj-batch6971";

/* Soft process_vm op catalog: READV = remote->local (value 1). */
#define B6971_PVM_READV ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6971_pvm_is_readv(uint32_t u32Op)
{
	return (u32Op == B6971_PVM_READV) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pvm_is_readv_u_6971 - 1 if op is soft PVM_READV.
 *
 * op: soft process_vm direction/op catalog id
 *
 * Returns 1 when op equals soft PVM_READV (1), else 0.
 * Pure integer compare; does not call process_vm_readv. No parent wires.
 */
uint32_t
gj_pvm_is_readv_u_6971(uint32_t u32Op)
{
	(void)NULL;
	return b6971_pvm_is_readv(u32Op);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pvm_is_readv_u_6971(uint32_t u32Op)
    __attribute__((alias("gj_pvm_is_readv_u_6971")));
