/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7320: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7320(void);
 *     - Returns the compile-time graph batch number for this TU (7320).
 *   uint32_t __gj_batch_id_7320  (alias)
 *   __libcgj_batch7320_marker = "libcgj-batch7320"
 *
 * Exclusive continuum CREATE-ONLY (7311-7320: ioctl request class stubs —
 * dir_none_id, dir_write_id, dir_read_id, dir_mask_id, has_read, has_write,
 * dir_ok, dir_pack, dir_errorish, batch_id_7320).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7320_marker[] = "libcgj-batch7320";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7320_id(void)
{
	return 7320u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7320 - report this TU's graph batch number.
 *
 * Always returns 7320. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7320(void)
{
	(void)NULL;
	return b7320_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7320(void)
    __attribute__((alias("gj_batch_id_7320")));
