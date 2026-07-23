/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9480: libarchive soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9480(void);
 *     - Returns the compile-time graph batch number for this TU (9480).
 *   uint32_t __gj_batch_id_9480  (alias)
 *   __libcgj_batch9480_marker = "libcgj-batch9480"
 *
 * Exclusive continuum CREATE-ONLY (9471-9480: libarchive soft id stubs —
 * la_format_zip_id_9471, la_format_tar_id_9472, la_format_7z_id_9473,
 * la_filter_gzip_id_9474, la_filter_xz_id_9475, la_read_ok_u_9476,
 * la_write_ok_u_9477, la_entry_ok_u_9478, libarchive_ready_u_9479,
 * batch_id_9480).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No libarchive implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9480_marker[] = "libcgj-batch9480";

#define B9480_BATCH_ID  9480u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9480_id(void)
{
	return B9480_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9480 - report this TU's graph batch number.
 *
 * Always returns 9480.
 */
uint32_t
gj_batch_id_9480(void)
{
	(void)NULL;
	return b9480_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9480(void)
    __attribute__((alias("gj_batch_id_9480")));
