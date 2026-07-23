/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9815: efi simple file system ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_efi_fs_ok_u_9815(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       EFI Simple File System Protocol readiness probe.
 *   uint32_t __gj_efi_fs_ok_u_9815  (alias)
 *   __libcgj_batch9815_marker = "libcgj-batch9815"
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

const char __libcgj_batch9815_marker[] = "libcgj-batch9815";

/* Soft efi-fs-ok lamp: always off (not a real SFS probe). */
#define B9815_EFI_FS_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9815_efi_fs_ok(void)
{
	return B9815_EFI_FS_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_efi_fs_ok_u_9815 - efi simple file system ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not open EFI volumes
 * or call libc. No parent wires.
 */
uint32_t
gj_efi_fs_ok_u_9815(void)
{
	(void)NULL;
	return b9815_efi_fs_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_efi_fs_ok_u_9815(void)
    __attribute__((alias("gj_efi_fs_ok_u_9815")));
