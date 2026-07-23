/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8751: mode S_IRUSR bit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_irusr_id_8751(void);
 *     - Return soft mode bit id for S_IRUSR (0400 / 0x100).
 *   uint32_t __gj_mode_irusr_id_8751  (alias)
 *   __libcgj_batch8751_marker = "libcgj-batch8751"
 *
 * Exclusive continuum CREATE-ONLY (8751-8760: acl/mode bit id stubs —
 * irusr, iwusr, ixusr, irgrp, iwgrp, ixgrp, iroth, iwoth, ixoth,
 * batch_id_8760). Unique gj_mode_irusr_id_8751 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8751_marker[] = "libcgj-batch8751";

/* Soft S_IRUSR: owner-read permission (0400). */
#define B8751_S_IRUSR  ((uint32_t)0x00000100u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8751_mode_irusr_id(void)
{
	return B8751_S_IRUSR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_irusr_id_8751 - soft S_IRUSR mode bit id.
 *
 * Always returns 0x100. Pure constant tag; no parent wires. Does not
 * call chmod/stat or inspect a live mode word.
 */
uint32_t
gj_mode_irusr_id_8751(void)
{
	(void)NULL;
	return b8751_mode_irusr_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_irusr_id_8751(void)
    __attribute__((alias("gj_mode_irusr_id_8751")));
