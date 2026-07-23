/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9969: install soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_soft_ready_u_9969(void);
 *     - Returns 1 (soft lamp only). Indicates the real-hw install bar3
 *       path soft-stub surfaces in this continuum are present; not disk,
 *       partition, ESP, rootfs, bootloader, kernel, initrd, or config ok.
 *   uint32_t __gj_install_soft_ready_u_9969  (alias)
 *   __libcgj_batch9969_marker = "libcgj-batch9969"
 *
 * Exclusive continuum CREATE-ONLY (9961-9970: real-hw install soft stubs
 * bar3 path — install_disk_ok_u_9961, install_partition_ok_u_9962,
 * install_esp_ok_u_9963, install_rootfs_ok_u_9964,
 * install_bootloader_ok_u_9965, install_kernel_ok_u_9966,
 * install_initrd_ok_u_9967, install_config_ok_u_9968,
 * install_soft_ready_u_9969, batch_id_9970). Unique surface only; no
 * multi-def. Disk/partition/ESP/rootfs/bootloader/kernel/initrd/config
 * ok units remain 0. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9969_marker[] = "libcgj-batch9969";

/* Soft continuum lamp: surfaces present (soft ready only). */
#define B9969_SOFT_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9969_soft_ready(void)
{
	return B9969_SOFT_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_soft_ready_u_9969 - install soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 9961-9970 surfaces are present. Does not claim disk/partition/ESP/
 * rootfs/bootloader/kernel/initrd/config ok and does not call libc.
 * No parent wires.
 */
uint32_t
gj_install_soft_ready_u_9969(void)
{
	(void)NULL;
	return b9969_soft_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_soft_ready_u_9969(void)
    __attribute__((alias("gj_install_soft_ready_u_9969")));
