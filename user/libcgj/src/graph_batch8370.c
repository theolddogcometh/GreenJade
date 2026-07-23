/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8370: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8370(void);
 *     - Returns the compile-time graph batch number for this TU (8370).
 *   uint32_t __gj_batch_id_8370  (alias)
 *   __libcgj_batch8370_marker = "libcgj-batch8370"
 *
 * Exclusive continuum CREATE-ONLY (8361-8370: fd/flags bit stubs —
 * fd_valid, fd_stdin_id, fd_stdout_id, fd_stderr_id, oflag_rdonly,
 * oflag_wronly, oflag_rdwr, oflag_creat, oflag_trunc, batch_id_8370).
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior
 * batch_id_* symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8370_marker[] = "libcgj-batch8370";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8370_id(void)
{
	return 8370u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8370 - report this TU's graph batch number.
 *
 * Always returns 8370. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8370(void)
{
	(void)NULL;
	return b8370_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8370(void)
    __attribute__((alias("gj_batch_id_8370")));
