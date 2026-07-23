/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8431: CRC-32 init integer stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc32_init_8431(void);
 *     - Return the classic CRC-32/ISO-HDLC init value 0xFFFFFFFFu
 *       (zlib / PNG / Ethernet catalog init).
 *   uint32_t __gj_crc32_init_8431  (alias)
 *   __libcgj_batch8431_marker = "libcgj-batch8431"
 *
 * Exclusive continuum CREATE-ONLY (8431-8440: crc/poly integer stubs).
 * Unique gj_crc32_init_8431 surface only; no multi-def. Does not
 * redefine crc32 / gj_crc32 implementations. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8431_marker[] = "libcgj-batch8431";

/* CRC-32/ISO-HDLC (zlib/PNG) init. */
#define B8431_CRC32_INIT 0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8431_crc32_init(void)
{
	return B8431_CRC32_INIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc32_init_8431 - classic CRC-32 init constant.
 *
 * Always returns 0xFFFFFFFFu. Soft catalog only; does not run a CRC.
 * No parent wires.
 */
uint32_t
gj_crc32_init_8431(void)
{
	(void)NULL;
	return b8431_crc32_init();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc32_init_8431(void)
    __attribute__((alias("gj_crc32_init_8431")));
