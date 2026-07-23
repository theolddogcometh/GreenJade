/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9135: soft ZIP deflate compression method id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_zip_deflate_id_9135(void);
 *     - Returns 8 (ZIP method DEFLATE soft catalog value). Pure-data
 *       method id stub; does not inflate or call zlib.
 *   uint32_t __gj_zip_deflate_id_9135  (alias)
 *   __libcgj_batch9135_marker = "libcgj-batch9135"
 *
 * Exclusive continuum CREATE-ONLY (9131-9140: archive/zip soft id stubs —
 * zip_local_sig_id_9131, zip_central_sig_id_9132, zip_end_sig_id_9133,
 * zip_store_id_9134, zip_deflate_id_9135, zip_name_ok_u_9136,
 * zip_size_ok_u_9137, tar_ustar_ok_u_9138, archive_ready_u_9139,
 * batch_id_9140).
 * Unique surface only; no multi-def. Distinct from
 * gj_zip_store_id_9134 (batch9134). No parent wires. No __int128.
 * No archive implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9135_marker[] = "libcgj-batch9135";

/* Soft ZIP compression method DEFLATE (8). */
#define B9135_ZIP_DEFLATE  8u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9135_deflate_id(void)
{
	return B9135_ZIP_DEFLATE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zip_deflate_id_9135 - soft ZIP DEFLATE method catalog id.
 *
 * Always returns 8. Soft continuum constant; does not inflate streams
 * or call zlib. No parent wires.
 */
uint32_t
gj_zip_deflate_id_9135(void)
{
	(void)NULL;
	return b9135_deflate_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zip_deflate_id_9135(void)
    __attribute__((alias("gj_zip_deflate_id_9135")));
