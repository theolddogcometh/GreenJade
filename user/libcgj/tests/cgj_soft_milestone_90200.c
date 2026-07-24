/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 90200 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_90200.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_90200 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_90200 \
 *   user/libcgj/tests/cgj_soft_milestone_90200.c -ldl
 * Direct: -DCGJ_SOFT_M90200_DIRECT + graph_batch90191.c … graph_batch90200.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M90200_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 90200
#define CGJ_SOFT_B0 90191
#define CGJ_SOFT_B1 90192
#define CGJ_SOFT_B2 90193
#define CGJ_SOFT_B3 90194
#define CGJ_SOFT_B4 90195
#define CGJ_SOFT_B5 90196
#define CGJ_SOFT_B6 90197
#define CGJ_SOFT_B7 90198
#define CGJ_SOFT_B8 90199
#include "_cgj_soft_m_template.inc"
