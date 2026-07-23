/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9380: flatpak soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9380(void);
 *     - Returns the compile-time graph batch number for this TU (9380).
 *   uint32_t __gj_batch_id_9380  (alias)
 *   __libcgj_batch9380_marker = "libcgj-batch9380"
 *
 * Exclusive continuum CREATE-ONLY (9371-9380: flatpak soft id stubs —
 * fp_ref_ok_u_9371, fp_remote_ok_u_9372, fp_install_ok_u_9373,
 * fp_run_ok_u_9374, fp_update_ok_u_9375, fp_portal_ok_u_9376,
 * fp_sandbox_ok_u_9377, fp_export_ok_u_9378, flatpak_ready_u_9379,
 * batch_id_9380). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9380_marker[] = "libcgj-batch9380";

#define B9380_BATCH_ID  9380u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9380_id(void)
{
	return B9380_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9380 - report this TU's graph batch number.
 *
 * Always returns 9380.
 */
uint32_t
gj_batch_id_9380(void)
{
	(void)NULL;
	return b9380_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9380(void)
    __attribute__((alias("gj_batch_id_9380")));
