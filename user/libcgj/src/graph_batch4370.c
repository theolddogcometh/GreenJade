/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4370: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4370(void);
 *     - Returns the compile-time graph batch number for this TU (4370).
 *   uint32_t __gj_batch_id_4370  (alias)
 *   __libcgj_batch4370_marker = "libcgj-batch4370"
 *
 * Exclusive continuum CREATE-ONLY (4361-4370: crc8_update_u, crc8_buf_u,
 * crc16_ccitt_update_u, crc16_ccitt_buf_u, crc32_update_byte_u, crc32_buf_u,
 * crc32_finalize_u, adler32_update_byte_u, adler32_buf_u, batch_id_4370).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4370_marker[] = "libcgj-batch4370";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4370_id(void)
{
	return 4370u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4370 - report this TU's graph batch number.
 *
 * Always returns 4370. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4370(void)
{
	(void)NULL;
	return b4370_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4370(void)
    __attribute__((alias("gj_batch_id_4370")));
