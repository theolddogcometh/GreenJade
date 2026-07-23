/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8752: mode S_IWUSR bit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_iwusr_id_8752(void);
 *     - Return soft mode bit id for S_IWUSR (0200 / 0x80).
 *   uint32_t __gj_mode_iwusr_id_8752  (alias)
 *   __libcgj_batch8752_marker = "libcgj-batch8752"
 *
 * Exclusive continuum CREATE-ONLY (8751-8760: acl/mode bit id stubs —
 * irusr, iwusr, ixusr, irgrp, iwgrp, ixgrp, iroth, iwoth, ixoth,
 * batch_id_8760). Unique gj_mode_iwusr_id_8752 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8752_marker[] = "libcgj-batch8752";

/* Soft S_IWUSR: owner-write permission (0200). */
#define B8752_S_IWUSR  ((uint32_t)0x00000080u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8752_mode_iwusr_id(void)
{
	return B8752_S_IWUSR;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_iwusr_id_8752 - soft S_IWUSR mode bit id.
 *
 * Always returns 0x80. Pure constant tag; no parent wires. Does not
 * call chmod/stat or inspect a live mode word.
 */
uint32_t
gj_mode_iwusr_id_8752(void)
{
	(void)NULL;
	return b8752_mode_iwusr_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_iwusr_id_8752(void)
    __attribute__((alias("gj_mode_iwusr_id_8752")));
