/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14011: Flatpak portal soft path base (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_portal_soft_14011(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       xdg-desktop-portal / Flatpak portal path probe.
 *   uint32_t __gj_flatpak_portal_soft_14011  (alias)
 *   __libcgj_batch14011_marker = "libcgj-batch14011"
 *
 * Exclusive continuum CREATE-ONLY (14011-14020: flatpak/portal soft
 * path deepen — flatpak_portal_soft_14011, flatpak_portal_doc_soft_14012,
 * flatpak_portal_file_soft_14013, flatpak_portal_openuri_soft_14014,
 * flatpak_portal_inhibit_soft_14015, flatpak_portal_notif_soft_14016,
 * flatpak_portal_screencast_soft_14017, flatpak_portal_secret_soft_14018,
 * flatpak_portal_ready_soft_14019, batch_id_14020). Unique surface only;
 * no multi-def. No parent wires. No __int128. No Flatpak/portal impl.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14011_marker[] = "libcgj-batch14011";

/* Soft Flatpak portal path base lamp: always off (not a real probe). */
#define B14011_PORTAL_SOFT  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14011_portal_soft(void)
{
	return B14011_PORTAL_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_portal_soft_14011 - Flatpak portal soft path base flag.
 *
 * Always returns 0. Soft continuum stub; does not talk to portals or
 * call Flatpak/libc. No parent wires.
 */
uint32_t
gj_flatpak_portal_soft_14011(void)
{
	(void)NULL;
	return b14011_portal_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_portal_soft_14011(void)
    __attribute__((alias("gj_flatpak_portal_soft_14011")));
