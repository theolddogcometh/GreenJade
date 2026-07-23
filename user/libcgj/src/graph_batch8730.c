/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8730: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_8730(void);
 *     - Returns the compile-time graph batch number for this TU (8730).
 *   uint32_t __gj_batch_id_8730  (alias)
 *   __libcgj_batch8730_marker = "libcgj-batch8730"
 *
 * Exclusive continuum CREATE-ONLY (8721-8730: clone/fork flag id stubs —
 * clone_vm_id, clone_fs_id, clone_files_id, clone_sighand_id,
 * clone_thread_id, clone_newns_id, clone_newpid_id, fork_ready_u,
 * vfork_ready_u, batch_id_8730). Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8730_marker[] = "libcgj-batch8730";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8730_id(void)
{
	return 8730u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_8730 - report this TU's graph batch number.
 *
 * Always returns 8730. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 */
uint32_t
gj_batch_id_8730(void)
{
	(void)NULL;
	return b8730_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_8730(void)
    __attribute__((alias("gj_batch_id_8730")));
