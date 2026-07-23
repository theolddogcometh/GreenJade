/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9046: soft Fontconfig FC_SLANT_ROMAN id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_slant_roman_id_9046(void);
 *     - Returns 0 (FC_SLANT_ROMAN catalog id). Soft slant constant only;
 *       does not open fonts or call fontconfig.
 *   uint32_t __gj_fc_slant_roman_id_9046  (alias)
 *   __libcgj_batch9046_marker = "libcgj-batch9046"
 *
 * Exclusive continuum CREATE-ONLY (9041-9050: fontconfig soft id stubs —
 * fc_family_ok_u_9041, fc_style_ok_u_9042, fc_size_ok_u_9043,
 * fc_weight_normal_id_9044, fc_weight_bold_id_9045,
 * fc_slant_roman_id_9046, fc_slant_italic_id_9047, fc_match_ok_u_9048,
 * fontconfig_ready_u_9049, batch_id_9050).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No fontconfig / FreeType implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9046_marker[] = "libcgj-batch9046";

/* Soft FC_SLANT_ROMAN catalog id. */
#define B9046_SLANT_ROMAN  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9046_slant_roman_id(void)
{
	return B9046_SLANT_ROMAN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_slant_roman_id_9046 - soft FC_SLANT_ROMAN catalog id.
 *
 * Always returns 0. Soft continuum constant; does not match patterns or
 * call fontconfig. No parent wires.
 */
uint32_t
gj_fc_slant_roman_id_9046(void)
{
	(void)NULL;
	return b9046_slant_roman_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_slant_roman_id_9046(void)
    __attribute__((alias("gj_fc_slant_roman_id_9046")));
