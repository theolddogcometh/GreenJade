/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9478: soft libarchive entry-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_la_entry_ok_u_9478(void);
 *     - Returns 1 (libarchive soft-id continuum entry ok). Pure-data
 *       product tag; does not allocate or walk archive entries.
 *   uint32_t __gj_la_entry_ok_u_9478  (alias)
 *   __libcgj_batch9478_marker = "libcgj-batch9478"
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

const char __libcgj_batch9478_marker[] = "libcgj-batch9478";

#define B9478_ENTRY_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9478_entry_ok(void)
{
	return B9478_ENTRY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_la_entry_ok_u_9478 - libarchive soft-id continuum entry-ok unit flag.
 *
 * Always returns 1. Soft pure-data product tag; does not walk entries
 * or call libc. No parent wires.
 */
uint32_t
gj_la_entry_ok_u_9478(void)
{
	(void)NULL;
	return b9478_entry_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_la_entry_ok_u_9478(void)
    __attribute__((alias("gj_la_entry_ok_u_9478")));
