/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5408: bootloader EFI path (loader / fallback).
 *
 * Surface (unique symbols):
 *   uint32_t gj_efi_loader_path_ok_5408(const char *path);
 *     - Return 1 if path is absolute with length >= 2, else 0.
 *   uint32_t gj_efi_fallback_path_ok_5408(const char *path);
 *     - Same soft absolute-path check for EFI fallback path.
 *   uint32_t gj_efi_boot_mgr_ready_5408(void);
 *     - Returns 0 (soft: EFI boot manager not ready at compile time).
 *   uint32_t gj_efi_path_score_5408(void);
 *     - Returns 5408 (wave-tagged EFI path score).
 *   uint32_t __gj_efi_loader_path_ok_5408  (alias)
 *   uint32_t __gj_efi_fallback_path_ok_5408  (alias)
 *   uint32_t __gj_efi_boot_mgr_ready_5408  (alias)
 *   uint32_t __gj_efi_path_score_5408  (alias)
 *   __libcgj_batch5408_marker = "libcgj-batch5408"
 *
 * Exclusive continuum CREATE-ONLY (5401-5410). Bootloader EFI path
 * toward bar #3. Distinct from gj_bootloader_efi_*_5407. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5408_marker[] = "libcgj-batch5408";

#define B5408_NOT_READY  0u
#define B5408_SCORE      5408u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5408_path_ok(const char *szPath)
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
 * gj_efi_loader_path_ok_5408 - soft absolute EFI loader path check.
 *
 * path: NUL-terminated loader path (NULL -> 0)
 *
 * Returns 1 when path starts with '/' and length >= 2, else 0.
 */
uint32_t
gj_efi_loader_path_ok_5408(const char *szPath)
{
	(void)NULL;
	return b5408_path_ok(szPath);
}

/*
 * gj_efi_fallback_path_ok_5408 - soft absolute EFI fallback path check.
 *
 * path: NUL-terminated fallback path (NULL -> 0)
 *
 * Returns 1 when path starts with '/' and length >= 2, else 0.
 */
uint32_t
gj_efi_fallback_path_ok_5408(const char *szPath)
{
	return b5408_path_ok(szPath);
}

/*
 * gj_efi_boot_mgr_ready_5408 - soft EFI boot-manager ready lamp.
 *
 * Always returns 0 (not ready at compile time). No efivar probe.
 */
uint32_t
gj_efi_boot_mgr_ready_5408(void)
{
	return B5408_NOT_READY;
}

/*
 * gj_efi_path_score_5408 - wave-tagged EFI path score.
 *
 * Always returns 5408.
 */
uint32_t
gj_efi_path_score_5408(void)
{
	return B5408_SCORE;
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_efi_loader_path_ok_5408(const char *szPath)
    __attribute__((alias("gj_efi_loader_path_ok_5408")));

uint32_t __gj_efi_fallback_path_ok_5408(const char *szPath)
    __attribute__((alias("gj_efi_fallback_path_ok_5408")));

uint32_t __gj_efi_boot_mgr_ready_5408(void)
    __attribute__((alias("gj_efi_boot_mgr_ready_5408")));

uint32_t __gj_efi_path_score_5408(void)
    __attribute__((alias("gj_efi_path_score_5408")));
