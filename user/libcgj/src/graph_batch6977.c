/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6977: process_vm flags validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pvm_flags_ok_u_6977(uint32_t flags);
 *     - Return 1 if flags is 0 (only value accepted by process_vm_*), else 0.
 *   uint32_t __gj_pvm_flags_ok_u_6977  (alias)
 *   __libcgj_batch6977_marker = "libcgj-batch6977"
 *
 * Exclusive continuum CREATE-ONLY (6971-6980: process_vm_readv/writev stubs —
 * is_readv, is_writev, op_ok, readv_id, writev_id, pack, flags_ok,
 * iovcnt_ok, errorish, batch_id_6980). Unique
 * gj_pvm_flags_ok_u_6977 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6977_marker[] = "libcgj-batch6977";

/* process_vm_readv/writev flags must be zero on Linux. */
#define B6977_PVM_FLAGS_ALLOWED ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6977_pvm_flags_ok(uint32_t u32Flags)
{
	return (u32Flags == B6977_PVM_FLAGS_ALLOWED) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pvm_flags_ok_u_6977 - 1 if process_vm flags are valid (must be 0).
 *
 * flags: process_vm_readv / process_vm_writev flags argument
 *
 * Returns 1 when flags is 0, else 0. Soft catalog check only;
 * does not perform a cross-process copy. No parent wires.
 */
uint32_t
gj_pvm_flags_ok_u_6977(uint32_t u32Flags)
{
	(void)NULL;
	return b6977_pvm_flags_ok(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pvm_flags_ok_u_6977(uint32_t u32Flags)
    __attribute__((alias("gj_pvm_flags_ok_u_6977")));
