/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5426: shader info-log length clamp probe.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shader_info_log_len_5426(uint32_t reported, uint32_t cap);
 *     - Clamp a GL_INFO_LOG_LENGTH-style reported size into [0, cap].
 *       When cap is 0, returns 0. Soft freestanding probe for sizing
 *       shader info-log buffers on the Deck GPU GLES path.
 *   uint32_t __gj_shader_info_log_len_5426  (alias)
 *   __libcgj_batch5426_marker = "libcgj-batch5426"
 *
 * Exclusive continuum CREATE-ONLY (5421-5430). Deck GPU path: OpenGL ES
 * / GLX / EGL / shader / texture stubs. Unique gj_*_5426 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5426_marker[] = "libcgj-batch5426";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5426_log_len(uint32_t uReported, uint32_t uCap)
{
	if (uCap == 0u) {
		return 0u;
	}
	if (uReported > uCap) {
		return uCap;
	}
	return uReported;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shader_info_log_len_5426 - clamp shader info-log length to capacity.
 *
 * reported: driver-reported GL_INFO_LOG_LENGTH (bytes including NUL).
 * cap:      caller buffer capacity in bytes.
 * Returns min(reported, cap), or 0 if cap is 0. Freestanding; no GL
 * calls. No parent wires.
 */
uint32_t
gj_shader_info_log_len_5426(uint32_t uReported, uint32_t uCap)
{
	(void)NULL;
	return b5426_log_len(uReported, uCap);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shader_info_log_len_5426(uint32_t uReported, uint32_t uCap)
    __attribute__((alias("gj_shader_info_log_len_5426")));
