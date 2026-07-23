/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6085: SD / microSD mount-path buffer maximum.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sd_mount_path_max_6085(void);
 *     - Returns 128 (soft max bytes for an SD mount-point path buffer).
 *   uint32_t __gj_sd_mount_path_max_6085  (alias)
 *   __libcgj_batch6085_marker = "libcgj-batch6085"
 *
 * Exclusive continuum CREATE-ONLY (6081-6090: SD card / microSD path
 * stubs). Unique gj_sd_mount_path_max_6085 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6085_marker[] = "libcgj-batch6085";

/* Soft mount-point path buffer bound (bytes, including NUL room). */
#define B6085_SD_MOUNT_PATH_MAX  128u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6085_mount_path_max(void)
{
	return B6085_SD_MOUNT_PATH_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sd_mount_path_max_6085 - soft SD mount path buffer maximum.
 *
 * Always returns 128. Catalog bound for install / removable microSD
 * mount paths (e.g. /mnt/sdcard...); does not allocate. No parent wires.
 */
uint32_t
gj_sd_mount_path_max_6085(void)
{
	(void)NULL;
	return b6085_mount_path_max();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sd_mount_path_max_6085(void)
    __attribute__((alias("gj_sd_mount_path_max_6085")));
