/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1968: Vulkan vertex shader stage bit predicate.
 *
 * Surface (unique symbols):
 *   int gj_shader_stage_vert_p(uint32_t stages);
 *     — Return 1 if VK_SHADER_STAGE_VERTEX_BIT (0x1) is set in stages,
 *       else 0. Pure stage-mask bit predicate; does not inspect modules.
 *   int __gj_shader_stage_vert_p  (alias)
 *   __libcgj_batch1968_marker = "libcgj-batch1968"
 *
 * Vulkan/GL enum exclusive pure-data wave (1961–1970). Clean-room
 * freestanding pure C (integer only). Compiles with -ffreestanding
 * -msse2 -Wall -Wextra -Werror. No malloc, no errno, no libc. No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1968_marker[] = "libcgj-batch1968";

/* VK_SHADER_STAGE_VERTEX_BIT */
#define B1968_STAGE_VERT  0x00000001u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if vertex stage bit is set. */
static int
b1968_vert_p(uint32_t uStages)
{
	return ((uStages & B1968_STAGE_VERT) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shader_stage_vert_p — 1 if VERTEX stage bit is set.
 *
 * stages: VkShaderStageFlags as uint32_t
 * Returns 1 when bit 0 is set, else 0.
 */
int
gj_shader_stage_vert_p(uint32_t uStages)
{
	(void)NULL;
	return b1968_vert_p(uStages);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_shader_stage_vert_p(uint32_t uStages)
    __attribute__((alias("gj_shader_stage_vert_p")));
