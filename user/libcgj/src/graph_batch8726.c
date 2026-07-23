/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8726: clone CLONE_NEWNS flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clone_newns_id_8726(void);
 *     - Return soft clone-flag catalog id for CLONE_NEWNS (0x00020000).
 *   uint32_t __gj_clone_newns_id_8726  (alias)
 *   __libcgj_batch8726_marker = "libcgj-batch8726"
 *
 * Exclusive continuum CREATE-ONLY (8721-8730: clone/fork flag id stubs —
 * clone_vm_id, clone_fs_id, clone_files_id, clone_sighand_id,
 * clone_thread_id, clone_newns_id, clone_newpid_id, fork_ready_u,
 * vfork_ready_u, batch_id_8730). Unique gj_clone_newns_id_8726 surface
 * only; no multi-def. Soft pure-data catalog only; does not call clone.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8726_marker[] = "libcgj-batch8726";

/* Soft clone-flag catalog: CLONE_NEWNS new mount namespace (0x00020000). */
#define B8726_CLONE_NEWNS  ((uint32_t)0x00020000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8726_clone_newns_id(void)
{
	return B8726_CLONE_NEWNS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clone_newns_id_8726 - soft CLONE_NEWNS flag catalog id.
 *
 * Always returns 0x00020000 (CLONE_NEWNS). Pure data tag; does not
 * create a mount namespace. No parent wires.
 */
uint32_t
gj_clone_newns_id_8726(void)
{
	(void)NULL;
	return b8726_clone_newns_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clone_newns_id_8726(void)
    __attribute__((alias("gj_clone_newns_id_8726")));
