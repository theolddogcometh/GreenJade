/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8753: mode S_IXUSR bit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_ixusr_id_8753(void);
 *     - Return soft mode bit id for S_IXUSR (0100 / 0x40).
 *   uint32_t __gj_mode_ixusr_id_8753  (alias)
 *   __libcgj_batch8753_marker = "libcgj-batch8753"
 *
 * Exclusive continuum CREATE-ONLY (8751-8760: acl/mode bit id stubs —
 * irusr, iwusr, ixusr, irgrp, iwgrp, ixgrp, iroth, iwoth, ixoth,
 * batch_id_8760). Unique gj_mode_ixusr_id_8753 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8753_marker[] = "libcgj-batch8753";

/* Soft S_IXUSR: owner-execute permission (0100). */
#define B8753_S_IXUSR  ((uint32_t)0x00000040u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8753_mode_ixusr_id(void)
{
	return B8753_S_IXUSR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_ixusr_id_8753 - soft S_IXUSR mode bit id.
 *
 * Always returns 0x40. Pure constant tag; no parent wires. Does not
 * call chmod/stat or inspect a live mode word.
 */
uint32_t
gj_mode_ixusr_id_8753(void)
{
	(void)NULL;
	return b8753_mode_ixusr_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_ixusr_id_8753(void)
    __attribute__((alias("gj_mode_ixusr_id_8753")));
