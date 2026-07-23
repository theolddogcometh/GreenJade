/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9962: install partition ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_partition_ok_u_9962(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       partition layout probe for the real-hw install bar3 path.
 *   uint32_t __gj_install_partition_ok_u_9962  (alias)
 *   __libcgj_batch9962_marker = "libcgj-batch9962"
 *
 * Exclusive continuum CREATE-ONLY (9961-9970: real-hw install soft stubs
 * bar3 path — install_disk_ok_u_9961, install_partition_ok_u_9962,
 * install_esp_ok_u_9963, install_rootfs_ok_u_9964,
 * install_bootloader_ok_u_9965, install_kernel_ok_u_9966,
 * install_initrd_ok_u_9967, install_config_ok_u_9968,
 * install_soft_ready_u_9969, batch_id_9970). Unique surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9962_marker[] = "libcgj-batch9962";

/* Soft partition-ok lamp: always off (not a real partition probe). */
#define B9962_PARTITION_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9962_partition_ok(void)
{
	return B9962_PARTITION_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_partition_ok_u_9962 - install partition ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not read partition tables
 * or call libc. No parent wires.
 */
uint32_t
gj_install_partition_ok_u_9962(void)
{
	(void)NULL;
	return b9962_partition_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_install_partition_ok_u_9962(void)
    __attribute__((alias("gj_install_partition_ok_u_9962")));
