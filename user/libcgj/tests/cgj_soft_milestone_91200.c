/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 91200 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_91200.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_91200 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_91200 \
 *   user/libcgj/tests/cgj_soft_milestone_91200.c -ldl
 * Direct: -DCGJ_SOFT_M91200_DIRECT + graph_batch91191.c … graph_batch91200.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M91200_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 91200
#define CGJ_SOFT_B0 91191
#define CGJ_SOFT_B1 91192
#define CGJ_SOFT_B2 91193
#define CGJ_SOFT_B3 91194
#define CGJ_SOFT_B4 91195
#define CGJ_SOFT_B5 91196
#define CGJ_SOFT_B6 91197
#define CGJ_SOFT_B7 91198
#define CGJ_SOFT_B8 91199
#include "_cgj_soft_m_template.inc"
