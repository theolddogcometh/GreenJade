/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6721: clone CLONE_VM flag bit stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clone_vm_u_6721(uint32_t flags);
 *     - Return 1 if CLONE_VM (share address space) is set in flags, else 0.
 *   uint32_t __gj_clone_vm_u_6721  (alias)
 *   __libcgj_batch6721_marker = "libcgj-batch6721"
 *
 * Exclusive continuum CREATE-ONLY (6721-6730: clone flags stubs —
 * vm, fs, files, sighand, thread, sysvsem, settls, parent_settid,
 * threadish, batch_id_6730). Unique gj_clone_vm_u_6721 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6721_marker[] = "libcgj-batch6721";

/* CLONE_VM: share address space (0x00000100). */
#define B6721_CLONE_VM ((uint32_t)0x00000100u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6721_clone_vm(uint32_t u32Flags)
{
	return ((u32Flags & B6721_CLONE_VM) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clone_vm_u_6721 - 1 if CLONE_VM is set in clone flags mask.
 *
 * flags: raw clone / clone3 flags bitmask
 *
 * Returns 1 when the share-VM bit is set, else 0.
 * Pure mask test; no parent wires.
 */
uint32_t
gj_clone_vm_u_6721(uint32_t u32Flags)
{
	(void)NULL;
	return b6721_clone_vm(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clone_vm_u_6721(uint32_t u32Flags)
    __attribute__((alias("gj_clone_vm_u_6721")));
