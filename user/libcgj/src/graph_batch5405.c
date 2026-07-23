/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5405: partition layout (indices).
 *
 * Surface (unique symbols):
 *   uint32_t gj_part_layout_esp_idx_5405(void);
 *     - Returns 1 (soft 1-based ESP partition index for Deck layout).
 *   uint32_t gj_part_layout_root_idx_5405(void);
 *     - Returns 2 (soft 1-based rootfs partition index).
 *   uint32_t gj_part_layout_home_idx_5405(void);
 *     - Returns 3 (soft 1-based home/data partition index).
 *   uint32_t gj_part_layout_ok_5405(uint32_t n_parts);
 *     - Return 1 when n_parts >= 3 (min Deck install layout), else 0.
 *   uint32_t __gj_part_layout_esp_idx_5405  (alias)
 *   uint32_t __gj_part_layout_root_idx_5405  (alias)
 *   uint32_t __gj_part_layout_home_idx_5405  (alias)
 *   uint32_t __gj_part_layout_ok_5405  (alias)
 *   __libcgj_batch5405_marker = "libcgj-batch5405"
 *
 * Exclusive continuum CREATE-ONLY (5401-5410). Partition layout toward
 * bar #3 Steam Deck install / real-hw. Unique batch-suffixed symbols;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5405_marker[] = "libcgj-batch5405";

#define B5405_ESP_IDX   1u
#define B5405_ROOT_IDX  2u
#define B5405_HOME_IDX  3u
#define B5405_MIN_PARTS 3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5405_layout_ok(uint32_t u32NParts)
{
	return (u32NParts >= B5405_MIN_PARTS) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_part_layout_esp_idx_5405 - soft ESP partition index (1-based).
 *
 * Always returns 1. Typical Deck install layout: ESP first.
 */
uint32_t
gj_part_layout_esp_idx_5405(void)
{
	(void)NULL;
	return B5405_ESP_IDX;
}

/*
 * gj_part_layout_root_idx_5405 - soft rootfs partition index (1-based).
 *
 * Always returns 2.
 */
uint32_t
gj_part_layout_root_idx_5405(void)
{
	return B5405_ROOT_IDX;
}

/*
 * gj_part_layout_home_idx_5405 - soft home/data partition index (1-based).
 *
 * Always returns 3.
 */
uint32_t
gj_part_layout_home_idx_5405(void)
{
	return B5405_HOME_IDX;
}

/*
 * gj_part_layout_ok_5405 - soft min-partition-count check for Deck layout.
 *
 * n_parts: number of GPT partitions observed (or planned)
 *
 * Returns 1 when n_parts >= 3, else 0. Does not parse GPT tables.
 */
uint32_t
gj_part_layout_ok_5405(uint32_t u32NParts)
{
	return b5405_layout_ok(u32NParts);
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_part_layout_esp_idx_5405(void)
    __attribute__((alias("gj_part_layout_esp_idx_5405")));

uint32_t __gj_part_layout_root_idx_5405(void)
    __attribute__((alias("gj_part_layout_root_idx_5405")));

uint32_t __gj_part_layout_home_idx_5405(void)
    __attribute__((alias("gj_part_layout_home_idx_5405")));

uint32_t __gj_part_layout_ok_5405(uint32_t u32NParts)
    __attribute__((alias("gj_part_layout_ok_5405")));
