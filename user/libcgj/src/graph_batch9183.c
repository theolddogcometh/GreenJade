/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9183: install boot soft layout tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_boot_tag_9183(void);
 *     - Returns the soft install-image boot path layout tag for the
 *       real-hw install path soft continuum (always 0x49424f54, fourCC
 *       "IBOT"). Not a bootloader or /boot path probe.
 *   uint32_t __gj_install_boot_tag_9183  (alias)
 *   __libcgj_batch9183_marker = "libcgj-batch9183"
 *
 * Exclusive continuum CREATE-ONLY (9181-9190: install/image soft stubs —
 * install_root_tag_9181, install_efi_tag_9182, install_boot_tag_9183,
 * install_rootfs_tag_9184, install_partition_ok_u_9185,
 * install_write_ready_u_9186, install_verify_ready_u_9187,
 * install_reboot_ready_u_9188, install_soft_ready_u_9189,
 * batch_id_9190). Unique surface only; no multi-def. Distinct from
 * gj_install_bootloader_weight_pct_5647 / progress_5648. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9183_marker[] = "libcgj-batch9183";

/* Soft install boot path fourCC "IBOT". */
#define B9183_INSTALL_BOOT_TAG  0x49424f54u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9183_boot_tag(void)
{
	return B9183_INSTALL_BOOT_TAG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_boot_tag_9183 - soft install boot path layout fourCC tag.
 *
 * Always returns 0x49424f54 ("IBOT"). Soft pure-data layout tag for the
 * 9181-9190 continuum. Does not inspect bootloaders or call libc.
 * No parent wires.
 */
uint32_t
gj_install_boot_tag_9183(void)
{
	(void)NULL;
	return b9183_boot_tag();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_boot_tag_9183(void)
    __attribute__((alias("gj_install_boot_tag_9183")));
