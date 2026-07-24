/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 79200 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_79200.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_79200 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_79200 \
 *   user/libcgj/tests/cgj_soft_milestone_79200.c -ldl
 * Direct: -DCGJ_SOFT_M79200_DIRECT + graph_batch79191.c … graph_batch79200.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M79200_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 79200
#define CGJ_SOFT_B0 79191
#define CGJ_SOFT_B1 79192
#define CGJ_SOFT_B2 79193
#define CGJ_SOFT_B3 79194
#define CGJ_SOFT_B4 79195
#define CGJ_SOFT_B5 79196
#define CGJ_SOFT_B6 79197
#define CGJ_SOFT_B7 79198
#define CGJ_SOFT_B8 79199
#include "_cgj_soft_m_template.inc"
