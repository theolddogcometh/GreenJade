/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6943: sendfile offset remaining bytes.
 *
 * Surface (unique symbols):
 *   uint64_t gj_sendfile_offset_remaining_6943(uint64_t size, uint64_t offset);
 *     - Return size - offset when offset < size; else 0 (at/past EOF).
 *   uint64_t __gj_sendfile_offset_remaining_6943  (alias)
 *   __libcgj_batch6943_marker = "libcgj-batch6943"
 *
 * Exclusive continuum CREATE-ONLY (6941-6950: sendfile offset stubs).
 * Unique gj_sendfile_offset_remaining_6943 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6943_marker[] = "libcgj-batch6943";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b6943_remaining(uint64_t u64Size, uint64_t u64Offset)
{
	if (u64Offset >= u64Size) {
		return 0ull;
	}
	return u64Size - u64Offset;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sendfile_offset_remaining_6943 - bytes available after offset.
 *
 * size:   soft in_fd file size in bytes
 * offset: soft sendfile file offset
 *
 * Returns size - offset when offset < size; else 0. Soft pure-data
 * remaining-length helper; no sendfile syscall. No parent wires.
 */
uint64_t
gj_sendfile_offset_remaining_6943(uint64_t u64Size, uint64_t u64Offset)
{
	(void)NULL;
	return b6943_remaining(u64Size, u64Offset);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_sendfile_offset_remaining_6943(uint64_t u64Size, uint64_t u64Offset)
    __attribute__((alias("gj_sendfile_offset_remaining_6943")));
