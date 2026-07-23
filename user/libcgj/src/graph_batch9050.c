/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9050: fontconfig soft id continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9050(void);
 *     - Returns the compile-time graph batch number for this TU (9050).
 *   uint32_t __gj_batch_id_9050  (alias)
 *   __libcgj_batch9050_marker = "libcgj-batch9050"
 *
 * Exclusive continuum CREATE-ONLY (9041-9050: fontconfig soft id stubs —
 * fc_family_ok_u_9041, fc_style_ok_u_9042, fc_size_ok_u_9043,
 * fc_weight_normal_id_9044, fc_weight_bold_id_9045,
 * fc_slant_roman_id_9046, fc_slant_italic_id_9047, fc_match_ok_u_9048,
 * fontconfig_ready_u_9049, batch_id_9050).
 * Unique surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128. No fontconfig / FreeType implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9050_marker[] = "libcgj-batch9050";

#define B9050_BATCH_ID  9050u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9050_id(void)
{
	return B9050_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9050 - report this TU's graph batch number.
 *
 * Always returns 9050. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker. No
 * parent wires.
 */
uint32_t
gj_batch_id_9050(void)
{
	(void)NULL;
	return b9050_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9050(void)
    __attribute__((alias("gj_batch_id_9050")));
