/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9160: image codec soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9160(void);
 *     - Returns the compile-time graph batch number for this TU (9160).
 *   uint32_t __gj_batch_id_9160  (alias)
 *   __libcgj_batch9160_marker = "libcgj-batch9160"
 *
 * Exclusive continuum CREATE-ONLY (9151-9160: image codec soft id stubs —
 * png_sig_ok_u_9151, jpeg_soi_id_9152, gif_sig_ok_u_9153,
 * webp_ready_u_9154, bmp_header_ok_u_9155, tga_ready_u_9156,
 * ico_ready_u_9157, image_wh_ok_u_9158, image_ready_u_9159,
 * batch_id_9160).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No image codec implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9160_marker[] = "libcgj-batch9160";

#define B9160_BATCH_ID  9160u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9160_id(void)
{
	return B9160_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9160 - report this TU's graph batch number.
 *
 * Always returns 9160. Link-time presence tags the exclusive
 * image-codec soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9160(void)
{
	(void)NULL;
	return b9160_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9160(void)
    __attribute__((alias("gj_batch_id_9160")));
