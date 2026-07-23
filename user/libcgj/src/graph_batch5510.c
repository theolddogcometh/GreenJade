/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5510: ESP size ok predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_esp_size_ok_5510(uint32_t size_mib);
 *     - Return 1 when size_mib >= 512 (soft ESP minimum), else 0.
 *   uint32_t __gj_esp_size_ok_5510  (alias)
 *   __libcgj_batch5510_marker = "libcgj-batch5510"
 *
 * Post-milestone 5500 exclusive continuum CREATE-ONLY (5501-5510:
 * deck_media_magic_ok_5501, deck_media_min_mib_5502,
 * deck_media_size_ok_5503, deck_media_checksum_5504,
 * deck_media_checksum_eq_5505, vol_label_max_len_5506,
 * vol_label_len_ok_5507, vol_label_char_ok_5508, esp_size_min_mib_5509,
 * esp_size_ok_5510). Unique gj_esp_size_ok_5510 surface only; no
 * multi-def. Distinct from gj_esp_size_min_mib_5509 (batch5509).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5510_marker[] = "libcgj-batch5510";

/* Soft recommended ESP minimum (MiB); matches batch5509. */
#define B5510_ESP_MIN_MIB  512u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5510_esp_size_ok(uint32_t u32SizeMib)
{
	return (u32SizeMib >= B5510_ESP_MIN_MIB) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_esp_size_ok_5510 - true when ESP size meets soft minimum.
 *
 * size_mib: reported ESP size in mebibytes
 *
 * Returns 1 if size_mib >= 512, else 0. Pure threshold for Deck
 * install partitioning checks. No parent wires.
 */
uint32_t
gj_esp_size_ok_5510(uint32_t u32SizeMib)
{
	(void)NULL;
	return b5510_esp_size_ok(u32SizeMib);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_esp_size_ok_5510(uint32_t u32SizeMib)
    __attribute__((alias("gj_esp_size_ok_5510")));
