/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9137: soft ZIP size-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_zip_size_ok_u_9137(void);
 *     - Returns 1 (zip size soft-id continuum ok). Pure-data product
 *       tag; does not check compressed/uncompressed sizes.
 *   uint32_t __gj_zip_size_ok_u_9137  (alias)
 *   __libcgj_batch9137_marker = "libcgj-batch9137"
 *
 * Exclusive continuum CREATE-ONLY (9131-9140: archive/zip soft id stubs —
 * zip_local_sig_id_9131, zip_central_sig_id_9132, zip_end_sig_id_9133,
 * zip_store_id_9134, zip_deflate_id_9135, zip_name_ok_u_9136,
 * zip_size_ok_u_9137, tar_ustar_ok_u_9138, archive_ready_u_9139,
 * batch_id_9140).
 * Unique surface only; no multi-def. Distinct from
 * gj_zip_name_ok_u_9136 (batch9136). No parent wires. No __int128.
 * No archive implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9137_marker[] = "libcgj-batch9137";

#define B9137_ZIP_SIZE_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9137_size_ok(void)
{
	return B9137_ZIP_SIZE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zip_size_ok_u_9137 - ZIP size soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not validate
 * compressed or uncompressed sizes. No parent wires.
 */
uint32_t
gj_zip_size_ok_u_9137(void)
{
	(void)NULL;
	return b9137_size_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zip_size_ok_u_9137(void)
    __attribute__((alias("gj_zip_size_ok_u_9137")));
