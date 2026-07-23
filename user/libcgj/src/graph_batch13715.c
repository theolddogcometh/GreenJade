/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13715: OpenURI portal ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_open_uri_ok_u_13715(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       OpenURI portal probe for the flatpak portal continuum.
 *   uint32_t __gj_open_uri_ok_u_13715  (alias)
 *   __libcgj_batch13715_marker = "libcgj-batch13715"
 *
 * Exclusive continuum CREATE-ONLY (13711-13720: flatpak portal soft id
 * stubs — flatpak_ok_u_13711, xdg_portal_ok_u_13712,
 * documents_ok_u_13713, file_chooser_ok_u_13714, open_uri_ok_u_13715,
 * screenshot_ok_u_13716, notification_ok_u_13717, secrets_ok_u_13718,
 * portal_soft_ready_u_13719, batch_id_13720→13720).
 * Unique surface only; no multi-def. Distinct from sibling flatpak
 * portal soft stubs in this wave and prior gj_open_uri_ok_u_13515 /
 * gj_open_uri_ok_u_13315. No parent wires. No __int128. No OpenURI
 * portal implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13715_marker[] = "libcgj-batch13715";

/* Soft OpenURI-ok lamp: always off (not a real OpenURI probe). */
#define B13715_OPEN_URI_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13715_open_uri_ok(void)
{
	return B13715_OPEN_URI_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_open_uri_ok_u_13715 - OpenURI portal ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect the OpenURI
 * portal or call libc. No parent wires.
 */
uint32_t
gj_open_uri_ok_u_13715(void)
{
	(void)NULL;
	return b13715_open_uri_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_open_uri_ok_u_13715(void)
    __attribute__((alias("gj_open_uri_ok_u_13715")));
