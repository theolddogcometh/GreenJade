/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5425: shader compile status probe.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shader_compile_ok_5425(uint32_t status);
 *     - Return 1 when status is non-zero (GL_TRUE-style compile success),
 *       else 0. Soft freestanding model of GL_COMPILE_STATUS probes for
 *       the Deck GPU GLES shader path; does not call glGetShaderiv.
 *   uint32_t __gj_shader_compile_ok_5425  (alias)
 *   __libcgj_batch5425_marker = "libcgj-batch5425"
 *
 * Exclusive continuum CREATE-ONLY (5421-5430). Deck GPU path: OpenGL ES
 * / GLX / EGL / shader / texture stubs. Unique gj_*_5425 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5425_marker[] = "libcgj-batch5425";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5425_compile_ok(uint32_t uStatus)
{
	return (uStatus != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shader_compile_ok_5425 - soft GL_COMPILE_STATUS success probe.
 *
 * status: raw compile status word (0 = fail, non-zero = ok).
 * Returns 1 on success, 0 on failure. Freestanding integer stub for
 * Deck GPU shader compile probes. No parent wires.
 */
uint32_t
gj_shader_compile_ok_5425(uint32_t uStatus)
{
	(void)NULL;
	return b5425_compile_ok(uStatus);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shader_compile_ok_5425(uint32_t uStatus)
    __attribute__((alias("gj_shader_compile_ok_5425")));
