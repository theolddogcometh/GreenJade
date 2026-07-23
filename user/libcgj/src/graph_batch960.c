/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch960: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_960(void);
 *     — Returns the compile-time graph batch number for this TU (960).
 *   uint32_t __gj_batch_id_960  (alias)
 *   __libcgj_batch960_marker = "libcgj-batch960"

 *
 * Does NOT define gj_batch_id / gj_batch_id_950 / prior batch_id_* — avoid multi-def.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch960_marker[] = "libcgj-batch960";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_960 — report this TU's graph batch number.
 *
 * Always returns 960.
 */
uint32_t
gj_batch_id_960(void)
{
	return 960u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_960(void)
    __attribute__((alias("gj_batch_id_960")));
