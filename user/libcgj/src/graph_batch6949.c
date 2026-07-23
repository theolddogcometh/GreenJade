/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6949: sendfile EOF position predicates.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sendfile_at_eof_6949(uint64_t size, uint64_t offset);
 *     - Return 1 if offset == size (exactly at EOF).
 *   uint32_t gj_sendfile_past_eof_6949(uint64_t size, uint64_t offset);
 *     - Return 1 if offset > size (past EOF / soft EINVAL region).
 *   uint32_t __gj_sendfile_at_eof_6949  (alias)
 *   uint32_t __gj_sendfile_past_eof_6949  (alias)
 *   __libcgj_batch6949_marker = "libcgj-batch6949"
 *
 * Exclusive continuum CREATE-ONLY (6941-6950: sendfile offset stubs).
 * Unique gj_sendfile_*_6949 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6949_marker[] = "libcgj-batch6949";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6949_at_eof(uint64_t u64Size, uint64_t u64Offset)
{
	return (u64Offset == u64Size) ? 1u : 0u;
}

static uint32_t
b6949_past_eof(uint64_t u64Size, uint64_t u64Offset)
{
	return (u64Offset > u64Size) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sendfile_at_eof_6949 - offset sits exactly at file end.
 *
 * size:   soft in_fd file size
 * offset: soft sendfile file offset
 *
 * Returns 1 when offset == size (zero-byte transfer region); else 0.
 * Soft pure-data predicate; no sendfile. No parent wires.
 */
uint32_t
gj_sendfile_at_eof_6949(uint64_t u64Size, uint64_t u64Offset)
{
	(void)NULL;
	return b6949_at_eof(u64Size, u64Offset);
}

/*
 * gj_sendfile_past_eof_6949 - offset is past the file end.
 *
 * size:   soft in_fd file size
 * offset: soft sendfile file offset
 *
 * Returns 1 when offset > size (soft past-EOF / EINVAL-style region);
 * else 0. Soft pure-data predicate; no sendfile. No parent wires.
 */
uint32_t
gj_sendfile_past_eof_6949(uint64_t u64Size, uint64_t u64Offset)
{
	return b6949_past_eof(u64Size, u64Offset);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_sendfile_at_eof_6949(uint64_t u64Size, uint64_t u64Offset)
    __attribute__((alias("gj_sendfile_at_eof_6949")));

uint32_t __gj_sendfile_past_eof_6949(uint64_t u64Size, uint64_t u64Offset)
    __attribute__((alias("gj_sendfile_past_eof_6949")));
