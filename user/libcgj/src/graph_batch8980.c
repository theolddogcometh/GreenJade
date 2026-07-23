/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8980: steam path soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8980(void);
 *     - Returns the compile-time graph batch number for this TU (8980).
 *   uint32_t __gj_batch_id_8980  (alias)
 *   __libcgj_batch8980_marker = "libcgj-batch8980"
 *
 * Exclusive continuum CREATE-ONLY (8971-8980: steam path soft stubs —
 * steam_root_tag_8971, steam_apps_tag_8972, steam_runtime_tag_8973,
 * steam_proton_tag_8974, steam_deck_tag_8975, steam_path_ok_u_8976,
 * steam_install_ready_u_8977, steam_launch_ready_u_8978,
 * steam_soft_ready_u_8979, batch_id_8980). Unique surfaces only; no
 * multi-def. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8980_marker[] = "libcgj-batch8980";

#define B8980_BATCH_ID  8980u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8980_id(void)
{
	return B8980_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8980 - report this TU's graph batch number.
 *
 * Always returns 8980.
 */
uint32_t
gj_batch_id_8980(void)
{
	(void)NULL;
	return b8980_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8980(void)
    __attribute__((alias("gj_batch_id_8980")));
