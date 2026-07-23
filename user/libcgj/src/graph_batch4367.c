/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4367: gj_crc32_finalize_u - IEEE CRC-32 xorout.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32_finalize_u(uint32_t crc);
 *     - Finalize a running IEEE CRC-32 internal state by XOR with
 *       0xFFFFFFFF (xorout). Empty stream with init 0xFFFFFFFF yields 0.
 *   uint32_t __gj_crc32_finalize_u  (alias)
 *   __libcgj_batch4367_marker = "libcgj-batch4367"
 *
 * Exclusive continuum CREATE-ONLY (4361-4370). Companion to
 * gj_crc32_update_byte_u / gj_crc32_buf_u (batch4365/4366). Distinct from
 * gj_crc32_final_u (batch2652) — unique gj_crc32_finalize_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4367_marker[] = "libcgj-batch4367";

/* IEEE / zlib CRC-32 final XOR-out. */
#define B4367_XOROUT 0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4367_finalize(uint32_t u32Crc)
{
	return u32Crc ^ B4367_XOROUT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32_finalize_u - finalize a streaming IEEE CRC-32 (XOR 0xFFFFFFFF).
 *
 * crc: internal CRC from gj_crc32_update_byte_u / gj_crc32_buf_u
 *      (or init 0xFFFFFFFF if empty)
 *
 * Returns the finalized 32-bit CRC. Self-contained; no parent wires.
 */
uint32_t
gj_crc32_finalize_u(uint32_t u32Crc)
{
	(void)NULL;
	return b4367_finalize(u32Crc);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32_finalize_u(uint32_t u32Crc)
    __attribute__((alias("gj_crc32_finalize_u")));
