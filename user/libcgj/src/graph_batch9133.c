/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9133: soft ZIP end-of-central-directory sig id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_zip_end_sig_id_9133(void);
 *     - Returns 0x06054B50 (PK\\x05\\x06 EOCD soft catalog value).
 *       Pure-data signature id stub; does not locate EOCD or parse CD.
 *   uint32_t __gj_zip_end_sig_id_9133  (alias)
 *   __libcgj_batch9133_marker = "libcgj-batch9133"
 *
 * Exclusive continuum CREATE-ONLY (9131-9140: archive/zip soft id stubs —
 * zip_local_sig_id_9131, zip_central_sig_id_9132, zip_end_sig_id_9133,
 * zip_store_id_9134, zip_deflate_id_9135, zip_name_ok_u_9136,
 * zip_size_ok_u_9137, tar_ustar_ok_u_9138, archive_ready_u_9139,
 * batch_id_9140).
 * Unique surface only; no multi-def. Distinct from
 * gj_zip_local_sig_id_9131 / gj_zip_central_sig_id_9132. No parent wires.
 * No __int128. No archive implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9133_marker[] = "libcgj-batch9133";

/* Soft ZIP end-of-central-directory signature (PK\\x05\\x06, LE). */
#define B9133_ZIP_END_SIG  0x06054B50u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9133_end_sig(void)
{
	return B9133_ZIP_END_SIG;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_zip_end_sig_id_9133 - soft ZIP EOCD signature catalog id.
 *
 * Always returns 0x06054B50. Soft continuum constant; does not scan for
 * EOCD or parse central directory offsets. No parent wires.
 */
uint32_t
gj_zip_end_sig_id_9133(void)
{
	(void)NULL;
	return b9133_end_sig();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_zip_end_sig_id_9133(void)
    __attribute__((alias("gj_zip_end_sig_id_9133")));
