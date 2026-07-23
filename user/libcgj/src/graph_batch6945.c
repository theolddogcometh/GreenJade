/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6945: sendfile offset advance after transfer.
 *
 * Surface (unique symbols):
 *   uint64_t gj_sendfile_offset_advance_6945(uint64_t offset, uint64_t xfer);
 *     - Return offset + xfer with saturating add (overflow => UINT64_MAX).
 *   uint64_t __gj_sendfile_offset_advance_6945  (alias)
 *   __libcgj_batch6945_marker = "libcgj-batch6945"
 *
 * Exclusive continuum CREATE-ONLY (6941-6950: sendfile offset stubs).
 * Unique gj_sendfile_offset_advance_6945 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6945_marker[] = "libcgj-batch6945";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b6945_advance(uint64_t u64Offset, uint64_t u64Xfer)
{
	if (u64Xfer > (UINT64_MAX - u64Offset)) {
		return UINT64_MAX;
	}
	return u64Offset + u64Xfer;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sendfile_offset_advance_6945 - advance offset by transferred bytes.
 *
 * offset: soft pre-transfer file offset
 * xfer:   soft number of bytes transferred
 *
 * Returns offset + xfer, saturating at UINT64_MAX on overflow. Soft
 * pure-data model of *offset update after a successful sendfile.
 * No parent wires.
 */
uint64_t
gj_sendfile_offset_advance_6945(uint64_t u64Offset, uint64_t u64Xfer)
{
	(void)NULL;
	return b6945_advance(u64Offset, u64Xfer);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sendfile_offset_advance_6945(uint64_t u64Offset, uint64_t u64Xfer)
    __attribute__((alias("gj_sendfile_offset_advance_6945")));
