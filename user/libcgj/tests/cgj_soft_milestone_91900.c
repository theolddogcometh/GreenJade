/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 91900 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_91900.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_91900 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_91900 \
 *   user/libcgj/tests/cgj_soft_milestone_91900.c -ldl
 * Direct: -DCGJ_SOFT_M91900_DIRECT + graph_batch91891.c … graph_batch91900.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M91900_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 91900
#define CGJ_SOFT_B0 91891
#define CGJ_SOFT_B1 91892
#define CGJ_SOFT_B2 91893
#define CGJ_SOFT_B3 91894
#define CGJ_SOFT_B4 91895
#define CGJ_SOFT_B5 91896
#define CGJ_SOFT_B6 91897
#define CGJ_SOFT_B7 91898
#define CGJ_SOFT_B8 91899
#include "_cgj_soft_m_template.inc"
