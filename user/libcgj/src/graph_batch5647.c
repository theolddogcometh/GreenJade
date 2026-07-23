/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5647: installer bootloader stage identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_stage_bootloader_5647(void);
 *     - Returns 4 (soft bootloader stage ordinal in the install pipeline).
 *   uint32_t gj_install_bootloader_weight_pct_5647(void);
 *     - Returns 15 (soft weight of bootloader stage in overall progress).
 *   uint32_t gj_batch_id_5647(void);
 *     - Returns 5647 (this TU's graph batch number).
 *   uint32_t __gj_install_stage_bootloader_5647  (alias)
 *   uint32_t __gj_install_bootloader_weight_pct_5647  (alias)
 *   uint32_t __gj_batch_id_5647  (alias)
 *   __libcgj_batch5647_marker = "libcgj-batch5647"
 *
 * Exclusive continuum CREATE-ONLY (5641-5650: installer stages —
 * partition/format/copy/bootloader/config progress integers). Unique
 * batch-suffixed symbols; distinct from gj_bootloader_efi_*_5407.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5647_marker[] = "libcgj-batch5647";

#define B5647_STAGE_BOOTLOADER  4u
#define B5647_WEIGHT_PCT        15u
#define B5647_BATCH             5647u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5647_stage(void)
{
	return B5647_STAGE_BOOTLOADER;
}

static uint32_t
b5647_weight(void)
{
	return B5647_WEIGHT_PCT;
}

static uint32_t
b5647_id(void)
{
	return B5647_BATCH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_stage_bootloader_5647 - soft bootloader stage ordinal.
 *
 * Always returns 4. Product-path tag for EFI/bootloader install stage;
 * does not write NVRAM or ESP. No parent wires.
 */
uint32_t
gj_install_stage_bootloader_5647(void)
{
	(void)NULL;
	return b5647_stage();
}

/*
 * gj_install_bootloader_weight_pct_5647 - soft bootloader stage weight.
 *
 * Always returns 15 (percent points of overall install progress).
 */
uint32_t
gj_install_bootloader_weight_pct_5647(void)
{
	return b5647_weight();
}

/*
 * gj_batch_id_5647 - report this TU's graph batch number.
 *
 * Always returns 5647.
 */
uint32_t
gj_batch_id_5647(void)
{
	return b5647_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_install_stage_bootloader_5647(void)
    __attribute__((alias("gj_install_stage_bootloader_5647")));

uint32_t __gj_install_bootloader_weight_pct_5647(void)
    __attribute__((alias("gj_install_bootloader_weight_pct_5647")));

uint32_t __gj_batch_id_5647(void)
    __attribute__((alias("gj_batch_id_5647")));
