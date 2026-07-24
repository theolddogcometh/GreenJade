/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 89300 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_89300.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_89300 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_89300 \
 *   user/libcgj/tests/cgj_soft_milestone_89300.c -ldl
 * Direct: -DCGJ_SOFT_M89300_DIRECT + graph_batch89291.c … graph_batch89300.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M89300_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 89300
#define CGJ_SOFT_B0 89291
#define CGJ_SOFT_B1 89292
#define CGJ_SOFT_B2 89293
#define CGJ_SOFT_B3 89294
#define CGJ_SOFT_B4 89295
#define CGJ_SOFT_B5 89296
#define CGJ_SOFT_B6 89297
#define CGJ_SOFT_B7 89298
#define CGJ_SOFT_B8 89299
#include "_cgj_soft_m_template.inc"
