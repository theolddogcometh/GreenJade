/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8433: CRC-16 init integer stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc16_init_8433(void);
 *     - Return the CRC-16/CCITT-FALSE init value 0xFFFFu (low 16 bits
 *       meaningful; returned as uint32_t for a uniform surface).
 *   uint32_t __gj_crc16_init_8433  (alias)
 *   __libcgj_batch8433_marker = "libcgj-batch8433"
 *
 * Exclusive continuum CREATE-ONLY (8431-8440: crc/poly integer stubs).
 * Unique gj_crc16_init_8433 surface only; no multi-def. Does not
 * redefine crc16 / gj_crc16_ccitt implementations. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8433_marker[] = "libcgj-batch8433";

/* CRC-16/CCITT-FALSE init. */
#define B8433_CRC16_INIT 0xFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8433_crc16_init(void)
{
	return B8433_CRC16_INIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc16_init_8433 - CRC-16/CCITT-FALSE init constant.
 *
 * Always returns 0xFFFFu. Soft catalog only; does not run a CRC.
 * No parent wires.
 */
uint32_t
gj_crc16_init_8433(void)
{
	(void)NULL;
	return b8433_crc16_init();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc16_init_8433(void)
    __attribute__((alias("gj_crc16_init_8433")));
