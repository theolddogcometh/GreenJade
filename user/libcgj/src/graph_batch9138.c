/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9138: soft tar ustar-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_tar_ustar_ok_u_9138(void);
 *     - Returns 1 (tar ustar soft-id continuum ok). Pure-data product
 *       tag; does not parse ustar headers or checksum tar blocks.
 *   uint32_t __gj_tar_ustar_ok_u_9138  (alias)
 *   __libcgj_batch9138_marker = "libcgj-batch9138"
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

const char __libcgj_batch9138_marker[] = "libcgj-batch9138";

#define B9138_TAR_USTAR_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9138_ustar_ok(void)
{
	return B9138_TAR_USTAR_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tar_ustar_ok_u_9138 - tar ustar soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not parse ustar
 * magic, checksum headers, or extract tar members. No parent wires.
 */
uint32_t
gj_tar_ustar_ok_u_9138(void)
{
	(void)NULL;
	return b9138_ustar_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_tar_ustar_ok_u_9138(void)
    __attribute__((alias("gj_tar_ustar_ok_u_9138")));
