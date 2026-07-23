/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6947: sendfile offset range check.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sendfile_offset_ok_6947(uint64_t size, uint64_t offset);
 *     - Return 1 if offset <= size (at EOF allowed); else 0 (past EOF).
 *   uint32_t __gj_sendfile_offset_ok_6947  (alias)
 *   __libcgj_batch6947_marker = "libcgj-batch6947"
 *
 * Exclusive continuum CREATE-ONLY (6941-6950: sendfile offset stubs).
 * Unique gj_sendfile_offset_ok_6947 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6947_marker[] = "libcgj-batch6947";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6947_ok(uint64_t u64Size, uint64_t u64Offset)
{
	return (u64Offset <= u64Size) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sendfile_offset_ok_6947 - soft offset range vs file size.
 *
 * size:   soft in_fd file size
 * offset: soft sendfile file offset
 *
 * Returns 1 when offset <= size (EOF-at-end is valid and yields zero
 * transfer); 0 when offset is past EOF. Soft pure-data range check;
 * no sendfile. No parent wires.
 */
uint32_t
gj_sendfile_offset_ok_6947(uint64_t u64Size, uint64_t u64Offset)
{
	(void)NULL;
	return b6947_ok(u64Size, u64Offset);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sendfile_offset_ok_6947(uint64_t u64Size, uint64_t u64Offset)
    __attribute__((alias("gj_sendfile_offset_ok_6947")));
