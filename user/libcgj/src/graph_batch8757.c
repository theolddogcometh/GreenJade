/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8757: mode S_IROTH bit id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mode_iroth_id_8757(void);
 *     - Return soft mode bit id for S_IROTH (0004 / 0x04).
 *   uint32_t __gj_mode_iroth_id_8757  (alias)
 *   __libcgj_batch8757_marker = "libcgj-batch8757"
 *
 * Exclusive continuum CREATE-ONLY (8751-8760: acl/mode bit id stubs —
 * irusr, iwusr, ixusr, irgrp, iwgrp, ixgrp, iroth, iwoth, ixoth,
 * batch_id_8760). Unique gj_mode_iroth_id_8757 surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8757_marker[] = "libcgj-batch8757";

/* Soft S_IROTH: other-read permission (0004). */
#define B8757_S_IROTH  ((uint32_t)0x00000004u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8757_mode_iroth_id(void)
{
	return B8757_S_IROTH;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mode_iroth_id_8757 - soft S_IROTH mode bit id.
 *
 * Always returns 0x04. Pure constant tag; no parent wires. Does not
 * call chmod/stat or inspect a live mode word.
 */
uint32_t
gj_mode_iroth_id_8757(void)
{
	(void)NULL;
	return b8757_mode_iroth_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mode_iroth_id_8757(void)
    __attribute__((alias("gj_mode_iroth_id_8757")));
