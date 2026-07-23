/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5060: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_5060(void);
 *     - Returns the compile-time graph batch number for this TU (5060).
 *   uint32_t __gj_batch_id_5060  (alias)
 *   __libcgj_batch5060_marker = "libcgj-batch5060"
 *
 * Exclusive continuum CREATE-ONLY (5051-5060: fs_mode_reg_u, fs_mode_dir_u,
 * fs_mode_lnk_u, fs_is_reg_u, fs_is_dir_u, fs_is_lnk_u, fs_perm_u,
 * fs_type_u, fs_sticky_u, batch_id_5060). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5060_marker[] = "libcgj-batch5060";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5060_id(void)
{
	return 5060u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_5060 - report this TU's graph batch number.
 *
 * Always returns 5060. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_5060(void)
{
	(void)NULL;
	return b5060_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_5060(void)
    __attribute__((alias("gj_batch_id_5060")));
