/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8721: clone CLONE_VM flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clone_vm_id_8721(void);
 *     - Return soft clone-flag catalog id for CLONE_VM (0x00000100).
 *   uint32_t __gj_clone_vm_id_8721  (alias)
 *   __libcgj_batch8721_marker = "libcgj-batch8721"
 *
 * Exclusive continuum CREATE-ONLY (8721-8730: clone/fork flag id stubs —
 * clone_vm_id, clone_fs_id, clone_files_id, clone_sighand_id,
 * clone_thread_id, clone_newns_id, clone_newpid_id, fork_ready_u,
 * vfork_ready_u, batch_id_8730). Unique gj_clone_vm_id_8721 surface
 * only; no multi-def. Soft pure-data catalog only; does not call clone.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8721_marker[] = "libcgj-batch8721";

/* Soft clone-flag catalog: CLONE_VM share address space (0x00000100). */
#define B8721_CLONE_VM  ((uint32_t)0x00000100u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8721_clone_vm_id(void)
{
	return B8721_CLONE_VM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clone_vm_id_8721 - soft CLONE_VM flag catalog id.
 *
 * Always returns 0x00000100 (CLONE_VM). Pure data tag; does not clone
 * a process or share mm_struct. No parent wires.
 */
uint32_t
gj_clone_vm_id_8721(void)
{
	(void)NULL;
	return b8721_clone_vm_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clone_vm_id_8721(void)
    __attribute__((alias("gj_clone_vm_id_8721")));
