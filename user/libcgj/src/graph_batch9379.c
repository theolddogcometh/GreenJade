/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9379: Flatpak soft continuum lamp (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_flatpak_ready_u_9379(void);
 *     - Returns 1 (soft lamp only). Indicates the Flatpak soft-stub
 *       surfaces in this continuum are present; not ref/remote/
 *       install/run/update/portal/sandbox/export readiness.
 *   uint32_t __gj_flatpak_ready_u_9379  (alias)
 *   __libcgj_batch9379_marker = "libcgj-batch9379"
 *
 * Exclusive continuum CREATE-ONLY (9371-9380: flatpak soft id stubs —
 * fp_ref_ok_u_9371, fp_remote_ok_u_9372, fp_install_ok_u_9373,
 * fp_run_ok_u_9374, fp_update_ok_u_9375, fp_portal_ok_u_9376,
 * fp_sandbox_ok_u_9377, fp_export_ok_u_9378, flatpak_ready_u_9379,
 * batch_id_9380). Unique surface only; no multi-def. Ref/remote/
 * install/run/update/portal/sandbox/export ok units remain 0. No
 * parent wires. No __int128. No Flatpak implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9379_marker[] = "libcgj-batch9379";

/* Soft continuum lamp: Flatpak soft-stub surfaces present. */
#define B9379_FLATPAK_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9379_flatpak_ready(void)
{
	return B9379_FLATPAK_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_ready_u_9379 - Flatpak soft continuum ready lamp.
 *
 * Always returns 1. Soft pure-data product tag that exclusive wave
 * 9371-9380 surfaces are present. Does not claim ref/remote/install/
 * run/update/portal/sandbox/export ready and does not call
 * Flatpak/libc. No parent wires.
 */
uint32_t
gj_flatpak_ready_u_9379(void)
{
	(void)NULL;
	return b9379_flatpak_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flatpak_ready_u_9379(void)
    __attribute__((alias("gj_flatpak_ready_u_9379")));
