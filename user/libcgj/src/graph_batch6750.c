/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6750: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_6750(void);
 *     - Returns the compile-time graph batch number for this TU (6750).
 *   uint32_t __gj_batch_id_6750  (alias)
 *   __libcgj_batch6750_marker = "libcgj-batch6750"
 *
 * Exclusive continuum CREATE-ONLY (6741-6750: seccomp filter action stubs —
 * allow, errno, trap, kill_thread, kill_process, trace, log, user_notif,
 * action_full, batch_id_6750). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6750_marker[] = "libcgj-batch6750";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6750_id(void)
{
	return 6750u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_6750 - report this TU's graph batch number.
 *
 * Always returns 6750. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_6750(void)
{
	(void)NULL;
	return b6750_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_6750(void)
    __attribute__((alias("gj_batch_id_6750")));
