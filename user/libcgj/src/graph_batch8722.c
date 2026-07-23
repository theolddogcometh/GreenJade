/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8722: clone CLONE_FS flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clone_fs_id_8722(void);
 *     - Return soft clone-flag catalog id for CLONE_FS (0x00000200).
 *   uint32_t __gj_clone_fs_id_8722  (alias)
 *   __libcgj_batch8722_marker = "libcgj-batch8722"
 *
 * Exclusive continuum CREATE-ONLY (8721-8730: clone/fork flag id stubs —
 * clone_vm_id, clone_fs_id, clone_files_id, clone_sighand_id,
 * clone_thread_id, clone_newns_id, clone_newpid_id, fork_ready_u,
 * vfork_ready_u, batch_id_8730). Unique gj_clone_fs_id_8722 surface
 * only; no multi-def. Soft pure-data catalog only; does not call clone.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8722_marker[] = "libcgj-batch8722";

/* Soft clone-flag catalog: CLONE_FS share filesystem info (0x00000200). */
#define B8722_CLONE_FS  ((uint32_t)0x00000200u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8722_clone_fs_id(void)
{
	return B8722_CLONE_FS;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clone_fs_id_8722 - soft CLONE_FS flag catalog id.
 *
 * Always returns 0x00000200 (CLONE_FS). Pure data tag; does not share
 * fs_struct or chroot state. No parent wires.
 */
uint32_t
gj_clone_fs_id_8722(void)
{
	(void)NULL;
	return b8722_clone_fs_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clone_fs_id_8722(void)
    __attribute__((alias("gj_clone_fs_id_8722")));
