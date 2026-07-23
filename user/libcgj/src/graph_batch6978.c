/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6978: process_vm iovec count validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_pvm_iovcnt_ok_u_6978(uint32_t liovcnt, uint32_t riovcnt);
 *     - Return 1 if both local and remote iov counts are non-zero, else 0.
 *   uint32_t __gj_pvm_iovcnt_ok_u_6978  (alias)
 *   __libcgj_batch6978_marker = "libcgj-batch6978"
 *
 * Exclusive continuum CREATE-ONLY (6971-6980: process_vm_readv/writev stubs —
 * is_readv, is_writev, op_ok, readv_id, writev_id, pack, flags_ok,
 * iovcnt_ok, errorish, batch_id_6980). Unique
 * gj_pvm_iovcnt_ok_u_6978 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6978_marker[] = "libcgj-batch6978";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6978_pvm_iovcnt_ok(uint32_t u32LIovCnt, uint32_t u32RIovCnt)
{
	return (u32LIovCnt != 0u && u32RIovCnt != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pvm_iovcnt_ok_u_6978 - 1 if both iov counts are non-zero.
 *
 * liovcnt: local iovec count (process_vm_* liovcnt)
 * riovcnt: remote iovec count (process_vm_* riovcnt)
 *
 * Returns 1 when both counts are non-zero, else 0. Soft pre-check for
 * empty local/remote vectors; does not walk iovecs. No parent wires.
 */
uint32_t
gj_pvm_iovcnt_ok_u_6978(uint32_t u32LIovCnt, uint32_t u32RIovCnt)
{
	(void)NULL;
	return b6978_pvm_iovcnt_ok(u32LIovCnt, u32RIovCnt);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_pvm_iovcnt_ok_u_6978(uint32_t u32LIovCnt, uint32_t u32RIovCnt)
    __attribute__((alias("gj_pvm_iovcnt_ok_u_6978")));
