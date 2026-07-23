/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9185: install partition ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_partition_ok_u_9185(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       partition layout probe for the real-hw install/image continuum.
 *   uint32_t __gj_install_partition_ok_u_9185  (alias)
 *   __libcgj_batch9185_marker = "libcgj-batch9185"
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

const char __libcgj_batch9185_marker[] = "libcgj-batch9185";

/* Soft partition-ok lamp: always off (not a real partition probe). */
#define B9185_PARTITION_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9185_partition_ok(void)
{
	return B9185_PARTITION_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_partition_ok_u_9185 - install partition ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not read partition tables
 * or call libc. No parent wires.
 */
uint32_t
gj_install_partition_ok_u_9185(void)
{
	(void)NULL;
	return b9185_partition_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_partition_ok_u_9185(void)
    __attribute__((alias("gj_install_partition_ok_u_9185")));
