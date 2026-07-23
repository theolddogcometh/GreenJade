/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9374: Flatpak run ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fp_run_ok_u_9374(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Flatpak run / bwrap spawn success probe.
 *   uint32_t __gj_fp_run_ok_u_9374  (alias)
 *   __libcgj_batch9374_marker = "libcgj-batch9374"
 *
 * Exclusive continuum CREATE-ONLY (9371-9380: flatpak soft id stubs —
 * fp_ref_ok_u_9371, fp_remote_ok_u_9372, fp_install_ok_u_9373,
 * fp_run_ok_u_9374, fp_update_ok_u_9375, fp_portal_ok_u_9376,
 * fp_sandbox_ok_u_9377, fp_export_ok_u_9378, flatpak_ready_u_9379,
 * batch_id_9380). Unique surface only; no multi-def. No parent wires.
 * No __int128. No Flatpak implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9374_marker[] = "libcgj-batch9374";

/* Soft Flatpak run-ok lamp: always off (not a real run probe). */
#define B9374_FP_RUN_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9374_fp_run_ok(void)
{
	return B9374_FP_RUN_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fp_run_ok_u_9374 - Flatpak run ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not spawn sandboxed
 * apps or call Flatpak/libc. No parent wires.
 */
uint32_t
gj_fp_run_ok_u_9374(void)
{
	(void)NULL;
	return b9374_fp_run_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fp_run_ok_u_9374(void)
    __attribute__((alias("gj_fp_run_ok_u_9374")));
