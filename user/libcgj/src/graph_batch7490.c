/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7490: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7490(void);
 *     - Returns the compile-time graph batch number for this TU (7490).
 *   uint32_t __gj_batch_id_7490  (alias)
 *   __libcgj_batch7490_marker = "libcgj-batch7490"
 *
 * Exclusive continuum CREATE-ONLY (7481-7490: mknodat type stubs —
 * s_ififo_id, s_ifchr_id, s_ifdir_id, s_ifblk_id, s_ifreg_id,
 * s_iflnk_id, s_ifsock_id, s_ifmt_id, type_ok, batch_id_7490).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7490_marker[] = "libcgj-batch7490";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7490_id(void)
{
	return 7490u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7490 - report this TU's graph batch number.
 *
 * Always returns 7490. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7490(void)
{
	(void)NULL;
	return b7490_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7490(void)
    __attribute__((alias("gj_batch_id_7490")));
