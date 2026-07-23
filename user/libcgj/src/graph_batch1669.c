/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1669: wheel/root GID hint.
 *
 * Surface (unique symbols):
 *   int gj_gid_is_wheel_hint(uint32_t gid);
 *     — Return 1 if gid is 0 (traditional wheel / root group on many
 *       Unix systems), else 0. Pure local hint — does not consult
 *       /etc/group or NSS.
 *   int __gj_gid_is_wheel_hint  (alias)
 *   __libcgj_batch1669_marker = "libcgj-batch1669"
 *
 * Distinct from gj_uid_is_root_p (batch1668). Does NOT redefine prior
 * batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1669_marker[] = "libcgj-batch1669";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gid_is_wheel_hint — 1 if gid is the conventional wheel/root group.
 *
 * "Hint" because sites may place wheel at a non-zero gid; this only
 * encodes the classic gid 0 convention for pure data paths.
 */
int
gj_gid_is_wheel_hint(uint32_t u32Gid)
{
	return (u32Gid == 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_gid_is_wheel_hint(uint32_t u32Gid)
    __attribute__((alias("gj_gid_is_wheel_hint")));
