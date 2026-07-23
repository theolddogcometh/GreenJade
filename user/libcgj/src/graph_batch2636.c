/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2636: product game id soft-ok (unsigned).
 *
 * Surface (unique symbols):
 *   int gj_game_id_ok_u(uint32_t game_id);
 *     - Return 1 if game_id is a plausible product game identifier:
 *       nonzero and not the all-bits-one sentinel (0xffffffff). Else 0.
 *       Soft catalog-free hint for launch/install metadata; not a store
 *       API lookup. Distinct name from Steam AppID helpers.
 *   int __gj_game_id_ok_u  (alias)
 *   __libcgj_batch2636_marker = "libcgj-batch2636"
 *
 * Product path exclusive wave (2631-2640). Distinct from
 * gj_steam_appid_ok_u (batch2271) and gj_appid_is_valid_hint
 * (batch1861) — unique gj_game_id_ok_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2636_marker[] = "libcgj-batch2636";

/* All-bits-one is treated as an unset or error game-id sentinel. */
#define B2636_GAME_ID_BAD  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if game_id is nonzero and not the bad sentinel. */
static int
b2636_id_ok(uint32_t uGameId)
{
	if (uGameId == 0u) {
		return 0;
	}
	if (uGameId == B2636_GAME_ID_BAD) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_game_id_ok_u - soft check that game_id is a plausible product id.
 *
 * game_id: candidate game identifier (unsigned product metadata)
 *
 * Returns 1 when 0 < game_id and game_id != 0xffffffff, else 0.
 * Does not call libc.
 */
int
gj_game_id_ok_u(uint32_t uGameId)
{
	(void)NULL;
	return b2636_id_ok(uGameId);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_game_id_ok_u(uint32_t uGameId)
    __attribute__((alias("gj_game_id_ok_u")));
