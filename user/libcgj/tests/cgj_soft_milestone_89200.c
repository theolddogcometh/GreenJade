/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 89200 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_89200.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_89200 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_89200 \
 *   user/libcgj/tests/cgj_soft_milestone_89200.c -ldl
 * Direct: -DCGJ_SOFT_M89200_DIRECT + graph_batch89191.c … graph_batch89200.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M89200_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 89200
#define CGJ_SOFT_B0 89191
#define CGJ_SOFT_B1 89192
#define CGJ_SOFT_B2 89193
#define CGJ_SOFT_B3 89194
#define CGJ_SOFT_B4 89195
#define CGJ_SOFT_B5 89196
#define CGJ_SOFT_B6 89197
#define CGJ_SOFT_B7 89198
#define CGJ_SOFT_B8 89199
#include "_cgj_soft_m_template.inc"
