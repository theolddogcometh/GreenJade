/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch890: milestone batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_890(void);
 *     — Returns the compile-time graph batch number for this TU (890).
 *   uint32_t __gj_batch_id_890  (alias)
 *   __libcgj_batch890_marker = "libcgj-batch890"
 *
 * Does NOT define gj_batch_id / gj_batch_id_850 / prior batch_id_* — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch890_marker[] = "libcgj-batch890";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_890 — report this TU's graph batch number.
 *
 * Always returns 890.
 */
uint32_t
gj_batch_id_890(void)
{
	return 890u;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_890(void)
    __attribute__((alias("gj_batch_id_890")));
