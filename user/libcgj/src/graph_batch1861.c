/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1861: Steam appid validity hint.
 *
 * Surface (unique symbols):
 *   int gj_appid_is_valid_hint(uint32_t appid);
 *     — Return 1 if appid > 0 (plausible Steam AppID), else 0.
 *       Soft hint only; does not consult a catalog.
 *   int __gj_appid_is_valid_hint  (alias)
 *   __libcgj_batch1861_marker = "libcgj-batch1861"
 *
 * Steam/desktop product exclusive pure-data wave (1861–1870).
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1861_marker[] = "libcgj-batch1861";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1861_appid_ok(uint32_t uApp)
{
	return (uApp > 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_appid_is_valid_hint — 1 iff appid is strictly positive.
 *
 * appid: Steam AppID as an unsigned 32-bit value.
 * Returns 1 when appid > 0, else 0 (including the sentinel 0).
 */
int
gj_appid_is_valid_hint(uint32_t uAppid)
{
	(void)sizeof(NULL);
	return b1861_appid_ok(uAppid);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_appid_is_valid_hint(uint32_t uAppid)
    __attribute__((alias("gj_appid_is_valid_hint")));
