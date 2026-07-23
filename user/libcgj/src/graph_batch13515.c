/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13515: OpenURI portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_open_uri_ok_u_13515(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       OpenURI portal probe for the flatpak portal continuum.
 *   uint32_t __gj_open_uri_ok_u_13515  (alias)
 *   __libcgj_batch13515_marker = "libcgj-batch13515"
 *
 * Exclusive continuum CREATE-ONLY (13511-13520: flatpak portal soft id
 * stubs — flatpak_ok_u_13511, xdg_portal_ok_u_13512,
 * documents_ok_u_13513, file_chooser_ok_u_13514, open_uri_ok_u_13515,
 * screenshot_ok_u_13516, notification_ok_u_13517, secrets_ok_u_13518,
 * portal_soft_ready_u_13519, batch_id_13520→13520).
 * Unique surface only; no multi-def. Distinct from sibling flatpak
 * portal soft stubs in this wave and prior gj_open_uri_ok_u_13315.
 * No parent wires. No __int128. No OpenURI portal implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13515_marker[] = "libcgj-batch13515";

/* Soft OpenURI-ok lamp: always off (not a real OpenURI probe). */
#define B13515_OPEN_URI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13515_open_uri_ok(void)
{
	return B13515_OPEN_URI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_open_uri_ok_u_13515 - OpenURI portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect the OpenURI
 * portal or call libc. No parent wires.
 */
uint32_t
gj_open_uri_ok_u_13515(void)
{
	(void)NULL;
	return b13515_open_uri_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_open_uri_ok_u_13515(void)
    __attribute__((alias("gj_open_uri_ok_u_13515")));
