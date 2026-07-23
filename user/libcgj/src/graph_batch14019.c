/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch14019: Flatpak portal soft continuum ready (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_portal_ready_soft_14019(void);
 *     - Returns 1 (soft lamp only). Indicates the Flatpak/portal soft
 *       path deepen surfaces in this continuum are present; not real
 *       portal/document/file/openuri/inhibit/notif/screencast/secret
 *       readiness.
 *   uint32_t __gj_flatpak_portal_ready_soft_14019  (alias)
 *   __libcgj_batch14019_marker = "libcgj-batch14019"
 *
 * Exclusive continuum CREATE-ONLY (14011-14020: flatpak/portal soft
 * path deepen — flatpak_portal_soft_14011, flatpak_portal_doc_soft_14012,
 * flatpak_portal_file_soft_14013, flatpak_portal_openuri_soft_14014,
 * flatpak_portal_inhibit_soft_14015, flatpak_portal_notif_soft_14016,
 * flatpak_portal_screencast_soft_14017, flatpak_portal_secret_soft_14018,
 * flatpak_portal_ready_soft_14019, batch_id_14020). Unique surface only;
 * no multi-def. Portal soft path units remain 0. No parent wires.
 * No __int128. No Flatpak/portal implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch14019_marker[] = "libcgj-batch14019";

/* Soft continuum lamp: Flatpak/portal soft-stub surfaces present. */
#define B14019_PORTAL_READY_SOFT  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b14019_portal_ready_soft(void)
{
	return B14019_PORTAL_READY_SOFT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_portal_ready_soft_14019 - portal soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 14011-14020 surfaces are present. Does not claim real portal path
 * readiness and does not call Flatpak/libc. No parent wires.
 */
uint32_t
gj_flatpak_portal_ready_soft_14019(void)
{
	(void)NULL;
	return b14019_portal_ready_soft();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_portal_ready_soft_14019(void)
    __attribute__((alias("gj_flatpak_portal_ready_soft_14019")));
