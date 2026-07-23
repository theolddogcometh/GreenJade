/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6886: statx mask INO|SIZE|BLOCKS soft tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_statx_mask_ino_6886(void);
 *     - Returns soft STATX_INO bit (0x00000100).
 *   uint32_t gj_statx_mask_size_6886(void);
 *     - Returns soft STATX_SIZE bit (0x00000200).
 *   uint32_t gj_statx_mask_blocks_6886(void);
 *     - Returns soft STATX_BLOCKS bit (0x00000400).
 *   uint32_t gj_statx_mask_ino_size_blocks_6886(void);
 *     - Returns soft STATX_INO|SIZE|BLOCKS (0x00000700).
 *   uint32_t __gj_statx_mask_ino_6886  (alias)
 *   uint32_t __gj_statx_mask_size_6886  (alias)
 *   uint32_t __gj_statx_mask_blocks_6886  (alias)
 *   uint32_t __gj_statx_mask_ino_size_blocks_6886  (alias)
 *   __libcgj_batch6886_marker = "libcgj-batch6886"
 *
 * Exclusive continuum CREATE-ONLY (6881-6890: statx mask stubs).
 * Unique gj_statx_mask_*_6886 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6886_marker[] = "libcgj-batch6886";

/* Soft Linux STATX_INO / STATX_SIZE / STATX_BLOCKS. */
#define B6886_STATX_INO     0x00000100u
#define B6886_STATX_SIZE    0x00000200u
#define B6886_STATX_BLOCKS  0x00000400u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6886_ino(void)
{
	return B6886_STATX_INO;
}

static uint32_t
b6886_size(void)
{
	return B6886_STATX_SIZE;
}

static uint32_t
b6886_blocks(void)
{
	return B6886_STATX_BLOCKS;
}

static uint32_t
b6886_ino_size_blocks(void)
{
	return B6886_STATX_INO | B6886_STATX_SIZE | B6886_STATX_BLOCKS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_statx_mask_ino_6886 - soft STATX_INO request bit.
 *
 * Always returns 0x00000100. Soft pure-data product tag; no statx.
 */
uint32_t
gj_statx_mask_ino_6886(void)
{
	(void)NULL;
	return b6886_ino();
}

/*
 * gj_statx_mask_size_6886 - soft STATX_SIZE request bit.
 *
 * Always returns 0x00000200.
 */
uint32_t
gj_statx_mask_size_6886(void)
{
	return b6886_size();
}

/*
 * gj_statx_mask_blocks_6886 - soft STATX_BLOCKS request bit.
 *
 * Always returns 0x00000400.
 */
uint32_t
gj_statx_mask_blocks_6886(void)
{
	return b6886_blocks();
}

/*
 * gj_statx_mask_ino_size_blocks_6886 - soft STATX_INO|SIZE|BLOCKS.
 *
 * Always returns 0x00000700. Content-identity trio for size probes.
 */
uint32_t
gj_statx_mask_ino_size_blocks_6886(void)
{
	return b6886_ino_size_blocks();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_statx_mask_ino_6886(void)
    __attribute__((alias("gj_statx_mask_ino_6886")));

uint32_t __gj_statx_mask_size_6886(void)
    __attribute__((alias("gj_statx_mask_size_6886")));

uint32_t __gj_statx_mask_blocks_6886(void)
    __attribute__((alias("gj_statx_mask_blocks_6886")));

uint32_t __gj_statx_mask_ino_size_blocks_6886(void)
    __attribute__((alias("gj_statx_mask_ino_size_blocks_6886")));
