/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4820: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4820(void);
 *     - Returns the compile-time graph batch number for this TU (4820).
 *   uint32_t __gj_batch_id_4820  (alias)
 *   __libcgj_batch4820_marker = "libcgj-batch4820"
 *
 * Exclusive continuum CREATE-ONLY (4811-4820: vfs_is_root_u,
 * vfs_is_sep_u, vfs_skip_seps_u, vfs_component_len_u, vfs_is_dot_u,
 * vfs_is_dotdot_u, vfs_depth_delta_u, vfs_join_need_u, vfs_hash_path_u,
 * batch_id_4820). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4820_marker[] = "libcgj-batch4820";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4820_id(void)
{
	return 4820u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4820 - report this TU's graph batch number.
 *
 * Always returns 4820. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_4820(void)
{
	(void)NULL;
	return b4820_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4820(void)
    __attribute__((alias("gj_batch_id_4820")));
