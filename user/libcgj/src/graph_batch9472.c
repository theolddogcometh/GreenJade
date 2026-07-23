/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9472: soft libarchive TAR format catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_la_format_tar_id_9472(void);
 *     - Returns 0x30000 (soft ARCHIVE_FORMAT_TAR-class catalog id).
 *       Soft catalog constant only; no archive I/O or format probe.
 *   uint32_t __gj_la_format_tar_id_9472  (alias)
 *   __libcgj_batch9472_marker = "libcgj-batch9472"
 *
 * Exclusive continuum CREATE-ONLY (9471-9480: libarchive soft id stubs —
 * la_format_zip_id_9471, la_format_tar_id_9472, la_format_7z_id_9473,
 * la_filter_gzip_id_9474, la_filter_xz_id_9475, la_read_ok_u_9476,
 * la_write_ok_u_9477, la_entry_ok_u_9478, libarchive_ready_u_9479,
 * batch_id_9480).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No libarchive implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9472_marker[] = "libcgj-batch9472";

/* Soft libarchive TAR format catalog id (ARCHIVE_FORMAT_TAR class). */
#define B9472_TAR_ID  0x30000u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9472_tar_id(void)
{
	return B9472_TAR_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_la_format_tar_id_9472 - soft libarchive TAR format catalog id.
 *
 * Always returns 0x30000. Soft continuum constant; does not open archives
 * or probe formats. No parent wires.
 */
uint32_t
gj_la_format_tar_id_9472(void)
{
	(void)NULL;
	return b9472_tar_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_la_format_tar_id_9472(void)
    __attribute__((alias("gj_la_format_tar_id_9472")));
