/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6976: process_vm soft op request pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pvm_op_pack_u_6976(uint32_t want_read, uint32_t want_write);
 *     - Pack soft readv/writev request flags into a 2-bit mask.
 *   uint32_t __gj_pvm_op_pack_u_6976  (alias)
 *   __libcgj_batch6976_marker = "libcgj-batch6976"
 *
 * Exclusive continuum CREATE-ONLY (6971-6980: process_vm_readv/writev stubs —
 * is_readv, is_writev, op_ok, readv_id, writev_id, pack, flags_ok,
 * iovcnt_ok, errorish, batch_id_6980). Unique
 * gj_pvm_op_pack_u_6976 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6976_marker[] = "libcgj-batch6976";

/* Soft request bits: bit0 = READV, bit1 = WRITEV. */
#define B6976_WANT_READV  ((uint32_t)0x1u)
#define B6976_WANT_WRITEV ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6976_pvm_op_pack(uint32_t u32WantRead, uint32_t u32WantWrite)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantRead != 0u)
		u32Pack |= B6976_WANT_READV;
	if (u32WantWrite != 0u)
		u32Pack |= B6976_WANT_WRITEV;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pvm_op_pack_u_6976 - soft-pack readv/writev op wants.
 *
 * want_read:  non-zero to request process_vm_readv support
 * want_write: non-zero to request process_vm_writev support
 *
 * Returns bitmask: bit0=readv, bit1=writev. Pure integer pack; no parent wires.
 */
uint32_t
gj_pvm_op_pack_u_6976(uint32_t u32WantRead, uint32_t u32WantWrite)
{
	(void)NULL;
	return b6976_pvm_op_pack(u32WantRead, u32WantWrite);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pvm_op_pack_u_6976(uint32_t u32WantRead, uint32_t u32WantWrite)
    __attribute__((alias("gj_pvm_op_pack_u_6976")));
