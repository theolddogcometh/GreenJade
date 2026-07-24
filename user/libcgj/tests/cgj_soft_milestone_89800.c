/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 89800 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_89800.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_89800 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_89800 \
 *   user/libcgj/tests/cgj_soft_milestone_89800.c -ldl
 * Direct: -DCGJ_SOFT_M89800_DIRECT + graph_batch89791.c … graph_batch89800.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M89800_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 89800
#define CGJ_SOFT_B0 89791
#define CGJ_SOFT_B1 89792
#define CGJ_SOFT_B2 89793
#define CGJ_SOFT_B3 89794
#define CGJ_SOFT_B4 89795
#define CGJ_SOFT_B5 89796
#define CGJ_SOFT_B6 89797
#define CGJ_SOFT_B7 89798
#define CGJ_SOFT_B8 89799
#include "_cgj_soft_m_template.inc"
