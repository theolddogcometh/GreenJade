/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9182: install EFI soft layout tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_efi_tag_9182(void);
 *     - Returns the soft install-image EFI path layout tag for the
 *       real-hw install path soft continuum (always 0x49454649, fourCC
 *       "IEFI"). Not an EFI variable or ESP path probe.
 *   uint32_t __gj_install_efi_tag_9182  (alias)
 *   __libcgj_batch9182_marker = "libcgj-batch9182"
 *
 * Exclusive continuum CREATE-ONLY (9181-9190: install/image soft stubs —
 * install_root_tag_9181, install_efi_tag_9182, install_boot_tag_9183,
 * install_rootfs_tag_9184, install_partition_ok_u_9185,
 * install_write_ready_u_9186, install_verify_ready_u_9187,
 * install_reboot_ready_u_9188, install_soft_ready_u_9189,
 * batch_id_9190). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9182_marker[] = "libcgj-batch9182";

/* Soft install EFI path fourCC "IEFI". */
#define B9182_INSTALL_EFI_TAG  0x49454649u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9182_efi_tag(void)
{
	return B9182_INSTALL_EFI_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_efi_tag_9182 - soft install EFI path layout fourCC tag.
 *
 * Always returns 0x49454649 ("IEFI"). Soft pure-data layout tag for the
 * 9181-9190 continuum. Does not open ESP mounts or call libc.
 * No parent wires.
 */
uint32_t
gj_install_efi_tag_9182(void)
{
	(void)NULL;
	return b9182_efi_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_efi_tag_9182(void)
    __attribute__((alias("gj_install_efi_tag_9182")));
