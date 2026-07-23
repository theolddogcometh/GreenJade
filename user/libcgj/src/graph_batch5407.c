/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5407: bootloader EFI path (core).
 *
 * Surface (unique symbols):
 *   uint32_t gj_bootloader_efi_path_ok_5407(const char *path);
 *     - Return 1 if path is absolute with length >= 2, else 0.
 *   uint32_t gj_bootloader_efi_default_5407(void);
 *     - Returns 1 (soft default EFI loader path known for product).
 *   uint32_t gj_bootloader_efi_vendor_max_5407(void);
 *     - Returns 64 (soft max vendor directory name length).
 *   uint32_t gj_efi_boot_order_slot_5407(void);
 *     - Returns 0 (soft primary BootOrder slot index).
 *   uint32_t __gj_bootloader_efi_path_ok_5407  (alias)
 *   uint32_t __gj_bootloader_efi_default_5407  (alias)
 *   uint32_t __gj_bootloader_efi_vendor_max_5407  (alias)
 *   uint32_t __gj_efi_boot_order_slot_5407  (alias)
 *   __libcgj_batch5407_marker = "libcgj-batch5407"
 *
 * Exclusive continuum CREATE-ONLY (5401-5410). Bootloader EFI path
 * toward bar #3 Deck install / real-hw. Unique batch-suffixed symbols;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5407_marker[] = "libcgj-batch5407";

#define B5407_DEFAULT_OK  1u
#define B5407_VENDOR_MAX  64u
#define B5407_BOOT_SLOT   0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5407_path_ok(const char *szPath)
{
	if (szPath == NULL) {
		return 0u;
	}
	if (szPath[0] != '/') {
		return 0u;
	}
	if (szPath[1] == '\0') {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bootloader_efi_path_ok_5407 - soft absolute EFI loader path check.
 *
 * path: NUL-terminated path under ESP (NULL -> 0)
 *
 * Returns 1 when path starts with '/' and length >= 2, else 0.
 * Does not open the ESP or call libc.
 */
uint32_t
gj_bootloader_efi_path_ok_5407(const char *szPath)
{
	(void)NULL;
	return b5407_path_ok(szPath);
}

/*
 * gj_bootloader_efi_default_5407 - soft default EFI path known lamp.
 *
 * Always returns 1. Product-path tag only.
 */
uint32_t
gj_bootloader_efi_default_5407(void)
{
	return B5407_DEFAULT_OK;
}

/*
 * gj_bootloader_efi_vendor_max_5407 - soft max EFI vendor dir name length.
 *
 * Always returns 64.
 */
uint32_t
gj_bootloader_efi_vendor_max_5407(void)
{
	return B5407_VENDOR_MAX;
}

/*
 * gj_efi_boot_order_slot_5407 - soft primary BootOrder slot index.
 *
 * Always returns 0 (first slot).
 */
uint32_t
gj_efi_boot_order_slot_5407(void)
{
	return B5407_BOOT_SLOT;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_bootloader_efi_path_ok_5407(const char *szPath)
    __attribute__((alias("gj_bootloader_efi_path_ok_5407")));

uint32_t __gj_bootloader_efi_default_5407(void)
    __attribute__((alias("gj_bootloader_efi_default_5407")));

uint32_t __gj_bootloader_efi_vendor_max_5407(void)
    __attribute__((alias("gj_bootloader_efi_vendor_max_5407")));

uint32_t __gj_efi_boot_order_slot_5407(void)
    __attribute__((alias("gj_efi_boot_order_slot_5407")));
