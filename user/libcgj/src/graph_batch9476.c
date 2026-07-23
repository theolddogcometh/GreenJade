/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9476: soft libarchive read-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_la_read_ok_u_9476(void);
 *     - Returns 0 (libarchive soft-id continuum read not-ok / idle).
 *       Pure-data product tag; does not open or read archives.
 *   uint32_t __gj_la_read_ok_u_9476  (alias)
 *   __libcgj_batch9476_marker = "libcgj-batch9476"
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

const char __libcgj_batch9476_marker[] = "libcgj-batch9476";

/* Soft libarchive read-ok flag: idle / not-ok (0). */
#define B9476_READ_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9476_read_ok(void)
{
	return B9476_READ_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_la_read_ok_u_9476 - libarchive soft-id continuum read-ok unit flag.
 *
 * Always returns 0. Soft pure-data product tag; does not read archives
 * or call libc. No parent wires.
 */
uint32_t
gj_la_read_ok_u_9476(void)
{
	(void)NULL;
	return b9476_read_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_la_read_ok_u_9476(void)
    __attribute__((alias("gj_la_read_ok_u_9476")));
