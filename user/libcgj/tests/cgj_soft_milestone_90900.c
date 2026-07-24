/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 90900 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_90900.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_90900 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_90900 \
 *   user/libcgj/tests/cgj_soft_milestone_90900.c -ldl
 * Direct: -DCGJ_SOFT_M90900_DIRECT + graph_batch90891.c … graph_batch90900.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M90900_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 90900
#define CGJ_SOFT_B0 90891
#define CGJ_SOFT_B1 90892
#define CGJ_SOFT_B2 90893
#define CGJ_SOFT_B3 90894
#define CGJ_SOFT_B4 90895
#define CGJ_SOFT_B5 90896
#define CGJ_SOFT_B6 90897
#define CGJ_SOFT_B7 90898
#define CGJ_SOFT_B8 90899
#include "_cgj_soft_m_template.inc"
