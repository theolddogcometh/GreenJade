/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7520: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_7520(void);
 *     - Returns the compile-time graph batch number for this TU (7520).
 *   uint32_t __gj_batch_id_7520  (alias)
 *   __libcgj_batch7520_marker = "libcgj-batch7520"
 *
 * Exclusive continuum CREATE-ONLY (7511-7520: waitpid options stubs —
 * wnohang_id, wuntraced_id, wcontinued_id, wexited_id, opts_mask_id,
 * has_wnohang, has_wuntraced, opts_ok, opts_errorish, batch_id_7520).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7520_marker[] = "libcgj-batch7520";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7520_id(void)
{
	return 7520u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_7520 - report this TU's graph batch number.
 *
 * Always returns 7520. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_7520(void)
{
	(void)NULL;
	return b7520_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_7520(void)
    __attribute__((alias("gj_batch_id_7520")));
