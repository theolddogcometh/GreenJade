/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8723: clone CLONE_FILES flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clone_files_id_8723(void);
 *     - Return soft clone-flag catalog id for CLONE_FILES (0x00000400).
 *   uint32_t __gj_clone_files_id_8723  (alias)
 *   __libcgj_batch8723_marker = "libcgj-batch8723"
 *
 * Exclusive continuum CREATE-ONLY (8721-8730: clone/fork flag id stubs —
 * clone_vm_id, clone_fs_id, clone_files_id, clone_sighand_id,
 * clone_thread_id, clone_newns_id, clone_newpid_id, fork_ready_u,
 * vfork_ready_u, batch_id_8730). Unique gj_clone_files_id_8723 surface
 * only; no multi-def. Soft pure-data catalog only; does not call clone.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8723_marker[] = "libcgj-batch8723";

/* Soft clone-flag catalog: CLONE_FILES share fd table (0x00000400). */
#define B8723_CLONE_FILES  ((uint32_t)0x00000400u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8723_clone_files_id(void)
{
	return B8723_CLONE_FILES;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clone_files_id_8723 - soft CLONE_FILES flag catalog id.
 *
 * Always returns 0x00000400 (CLONE_FILES). Pure data tag; does not share
 * a file descriptor table. No parent wires.
 */
uint32_t
gj_clone_files_id_8723(void)
{
	(void)NULL;
	return b8723_clone_files_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clone_files_id_8723(void)
    __attribute__((alias("gj_clone_files_id_8723")));
