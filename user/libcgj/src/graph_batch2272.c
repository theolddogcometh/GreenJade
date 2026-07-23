/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2272: Proton compatdata prefix-id soft-ok
 * predicate (unsigned).
 *
 * Surface (unique symbols):
 *   int gj_proton_prefix_ok_u(uint32_t prefix_id);
 *     - Return 1 if prefix_id is a plausible Proton compatdata prefix
 *       directory id (typically a Steam AppID under steamapps/compatdata):
 *       nonzero and not 0xffffffff. Else 0. Soft path-free hint only.
 *   int __gj_proton_prefix_ok_u  (alias)
 *   __libcgj_batch2272_marker = "libcgj-batch2272"
 *
 * Post-2270 product exclusive wave (2271-2280). Distinct from
 * gj_steam_path_is_compatdata (batch1865) and gj_steam_appid_ok_u
 * (batch2271) - unique gj_proton_prefix_ok_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2272_marker[] = "libcgj-batch2272";

/* Unset or error compatdata prefix sentinel. */
#define B2272_PFX_BAD  0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if prefix_id is a plausible Proton pfx / compatdata id. */
static int
b2272_pfx_ok(uint32_t uPfx)
{
	if (uPfx == 0u) {
		return 0;
	}
	if (uPfx == B2272_PFX_BAD) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_proton_prefix_ok_u - soft check for a Proton compatdata prefix id.
 *
 * prefix_id: candidate directory id under steamapps/compatdata
 *
 * Returns 1 when 0 < prefix_id and prefix_id != 0xffffffff, else 0.
 * Does not call libc or inspect the filesystem.
 */
int
gj_proton_prefix_ok_u(uint32_t uPrefixId)
{
	(void)NULL;
	return b2272_pfx_ok(uPrefixId);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_proton_prefix_ok_u(uint32_t uPrefixId)
    __attribute__((alias("gj_proton_prefix_ok_u")));
