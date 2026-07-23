/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5406: partition layout (sizes / type tags).
 *
 * Surface (unique symbols):
 *   uint32_t gj_part_layout_esp_mib_5406(void);
 *     - Returns 512 (soft recommended ESP size in MiB).
 *   uint32_t gj_part_layout_type_efi_5406(void);
 *     - Returns 0xEF00 (soft GPT EFI System type tag as uint32_t).
 *   uint32_t gj_part_layout_type_linux_5406(void);
 *     - Returns 0x8300 (soft GPT Linux filesystem type tag).
 *   uint32_t gj_part_layout_score_5406(void);
 *     - Returns 5406 (wave-tagged partition layout score).
 *   uint32_t __gj_part_layout_esp_mib_5406  (alias)
 *   uint32_t __gj_part_layout_type_efi_5406  (alias)
 *   uint32_t __gj_part_layout_type_linux_5406  (alias)
 *   uint32_t __gj_part_layout_score_5406  (alias)
 *   __libcgj_batch5406_marker = "libcgj-batch5406"
 *
 * Exclusive continuum CREATE-ONLY (5401-5410). Partition layout toward
 * bar #3 real-hw install. Distinct from gj_part_layout_*_5405. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5406_marker[] = "libcgj-batch5406";

#define B5406_ESP_MIB     512u
#define B5406_TYPE_EFI    0xEF00u
#define B5406_TYPE_LINUX  0x8300u
#define B5406_SCORE       5406u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5406_esp_mib(void)
{
	return B5406_ESP_MIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_part_layout_esp_mib_5406 - soft recommended ESP size in MiB.
 *
 * Always returns 512. Product-path constant for Deck install planning.
 */
uint32_t
gj_part_layout_esp_mib_5406(void)
{
	(void)NULL;
	return b5406_esp_mib();
}

/*
 * gj_part_layout_type_efi_5406 - soft GPT EFI System partition type tag.
 *
 * Always returns 0xEF00 (sgdisk-style EFI type code as uint32_t).
 */
uint32_t
gj_part_layout_type_efi_5406(void)
{
	return B5406_TYPE_EFI;
}

/*
 * gj_part_layout_type_linux_5406 - soft GPT Linux filesystem type tag.
 *
 * Always returns 0x8300.
 */
uint32_t
gj_part_layout_type_linux_5406(void)
{
	return B5406_TYPE_LINUX;
}

/*
 * gj_part_layout_score_5406 - wave-tagged partition layout score.
 *
 * Always returns 5406.
 */
uint32_t
gj_part_layout_score_5406(void)
{
	return B5406_SCORE;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_part_layout_esp_mib_5406(void)
    __attribute__((alias("gj_part_layout_esp_mib_5406")));

uint32_t __gj_part_layout_type_efi_5406(void)
    __attribute__((alias("gj_part_layout_type_efi_5406")));

uint32_t __gj_part_layout_type_linux_5406(void)
    __attribute__((alias("gj_part_layout_type_linux_5406")));

uint32_t __gj_part_layout_score_5406(void)
    __attribute__((alias("gj_part_layout_score_5406")));
