/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9041: soft Fontconfig family-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_family_ok_u_9041(void);
 *     - Returns 1 (font family soft-id continuum ok). Pure-data product
 *       tag; does not open fonts, scan FcConfig, or call fontconfig.
 *   uint32_t __gj_fc_family_ok_u_9041  (alias)
 *   __libcgj_batch9041_marker = "libcgj-batch9041"
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

const char __libcgj_batch9041_marker[] = "libcgj-batch9041";

#define B9041_FAMILY_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9041_family_ok(void)
{
	return B9041_FAMILY_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_family_ok_u_9041 - font family soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not resolve
 * FcPattern family strings or call fontconfig. No parent wires.
 */
uint32_t
gj_fc_family_ok_u_9041(void)
{
	(void)NULL;
	return b9041_family_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_family_ok_u_9041(void)
    __attribute__((alias("gj_fc_family_ok_u_9041")));
