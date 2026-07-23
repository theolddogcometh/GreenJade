/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13516: Screenshot portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_screenshot_ok_u_13516(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Screenshot portal probe for the flatpak portal continuum.
 *   uint32_t __gj_screenshot_ok_u_13516  (alias)
 *   __libcgj_batch13516_marker = "libcgj-batch13516"
 *
 * Exclusive continuum CREATE-ONLY (13511-13520: flatpak portal soft id
 * stubs — flatpak_ok_u_13511, xdg_portal_ok_u_13512,
 * documents_ok_u_13513, file_chooser_ok_u_13514, open_uri_ok_u_13515,
 * screenshot_ok_u_13516, notification_ok_u_13517, secrets_ok_u_13518,
 * portal_soft_ready_u_13519, batch_id_13520→13520).
 * Unique surface only; no multi-def. Distinct from sibling flatpak
 * portal soft stubs in this wave and prior gj_screenshot_ok_u_13316.
 * No parent wires. No __int128. No Screenshot portal implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13516_marker[] = "libcgj-batch13516";

/* Soft Screenshot-ok lamp: always off (not a real screenshot probe). */
#define B13516_SCREENSHOT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13516_screenshot_ok(void)
{
	return B13516_SCREENSHOT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_screenshot_ok_u_13516 - Screenshot portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect the
 * Screenshot portal or call libc. No parent wires.
 */
uint32_t
gj_screenshot_ok_u_13516(void)
{
	(void)NULL;
	return b13516_screenshot_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_screenshot_ok_u_13516(void)
    __attribute__((alias("gj_screenshot_ok_u_13516")));
