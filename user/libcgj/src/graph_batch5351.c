/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5351: Steam AppID placeholder tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_steam_appid_placeholder_u(void);
 *     - Returns 0 (no real AppID bound; placeholder only). Soft
 *       compile-time product-path tag for Steam/Deck continuum.
 *   uint32_t __gj_steam_appid_placeholder_u  (alias)
 *   __libcgj_batch5351_marker = "libcgj-batch5351"
 *
 * Exclusive continuum CREATE-ONLY (5351-5360: Steam/Deck product path
 * helpers unique — steam_appid_placeholder_u, deck_res_w_u, deck_res_h_u,
 * deck_is_native_u, proton_abi_ok_u, bar3_install_ready_u,
 * bar3_steam_ready_u, bar3_top50_ready_u, product_path_score_u,
 * batch_id_5360). Unique gj_steam_appid_placeholder_u surface only;
 * no multi-def. Distinct from gj_steam_path_is_compatdata (batch1865).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5351_marker[] = "libcgj-batch5351";

/* Placeholder AppID (unbound). */
#define B5351_APPID_PLACEHOLDER  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5351_appid_placeholder(void)
{
	return B5351_APPID_PLACEHOLDER;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_appid_placeholder_u - report the unbound Steam AppID placeholder.
 *
 * Always returns 0 (no real AppID assigned). Soft pure-data product-path
 * tag only; does not probe Steam. Does not call libc. No parent wires.
 */
uint32_t
gj_steam_appid_placeholder_u(void)
{
	(void)NULL;
	return b5351_appid_placeholder();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_steam_appid_placeholder_u(void)
    __attribute__((alias("gj_steam_appid_placeholder_u")));
