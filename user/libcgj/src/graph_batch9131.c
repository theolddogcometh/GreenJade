/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9131: soft ZIP local-file signature id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_zip_local_sig_id_9131(void);
 *     - Returns 0x04034B50 (PK\\x03\\x04 local file header soft catalog
 *       value). Pure-data signature id stub; does not parse ZIP or
 *       inflate streams.
 *   uint32_t __gj_zip_local_sig_id_9131  (alias)
 *   __libcgj_batch9131_marker = "libcgj-batch9131"
 *
 * Exclusive continuum CREATE-ONLY (9131-9140: archive/zip soft id stubs —
 * zip_local_sig_id_9131, zip_central_sig_id_9132, zip_end_sig_id_9133,
 * zip_store_id_9134, zip_deflate_id_9135, zip_name_ok_u_9136,
 * zip_size_ok_u_9137, tar_ustar_ok_u_9138, archive_ready_u_9139,
 * batch_id_9140).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No archive implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9131_marker[] = "libcgj-batch9131";

/* Soft ZIP local file header signature (PK\\x03\\x04, LE). */
#define B9131_ZIP_LOCAL_SIG  0x04034B50u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9131_local_sig(void)
{
	return B9131_ZIP_LOCAL_SIG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zip_local_sig_id_9131 - soft ZIP local-file signature catalog id.
 *
 * Always returns 0x04034B50. Soft continuum constant; does not parse
 * ZIP local headers or call inflate. No parent wires.
 */
uint32_t
gj_zip_local_sig_id_9131(void)
{
	(void)NULL;
	return b9131_local_sig();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zip_local_sig_id_9131(void)
    __attribute__((alias("gj_zip_local_sig_id_9131")));
