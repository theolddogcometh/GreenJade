/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9817: efi exit boot services ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_efi_exit_boot_ok_u_9817(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       EFI ExitBootServices readiness probe.
 *   uint32_t __gj_efi_exit_boot_ok_u_9817  (alias)
 *   __libcgj_batch9817_marker = "libcgj-batch9817"
 *
 * Exclusive continuum CREATE-ONLY (9811-9820: efi boot soft id
 * stubs — efi_system_table_ok_u_9811, efi_boot_services_ok_u_9812,
 * efi_runtime_ok_u_9813, efi_gop_ok_u_9814, efi_fs_ok_u_9815,
 * efi_load_ok_u_9816, efi_exit_boot_ok_u_9817, efi_var_ok_u_9818,
 * efi_ready_u_9819, batch_id_9820). Unique surface only; no multi-def.
 * No parent wires. No __int128. No EFI implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9817_marker[] = "libcgj-batch9817";

/* Soft efi-exit-boot-ok lamp: always off (not a real EBS probe). */
#define B9817_EFI_EXIT_BOOT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9817_efi_exit_boot_ok(void)
{
	return B9817_EFI_EXIT_BOOT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efi_exit_boot_ok_u_9817 - efi exit boot services ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not call ExitBootServices
 * or libc. No parent wires.
 */
uint32_t
gj_efi_exit_boot_ok_u_9817(void)
{
	(void)NULL;
	return b9817_efi_exit_boot_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efi_exit_boot_ok_u_9817(void)
    __attribute__((alias("gj_efi_exit_boot_ok_u_9817")));
