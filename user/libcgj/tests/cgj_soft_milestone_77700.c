/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 77700 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_77700.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_77700 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_77700 \
 *   user/libcgj/tests/cgj_soft_milestone_77700.c -ldl
 * Direct: -DCGJ_SOFT_M77700_DIRECT + graph_batch77691.c … graph_batch77700.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M77700_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 77700
#define CGJ_SOFT_B0 77691
#define CGJ_SOFT_B1 77692
#define CGJ_SOFT_B2 77693
#define CGJ_SOFT_B3 77694
#define CGJ_SOFT_B4 77695
#define CGJ_SOFT_B5 77696
#define CGJ_SOFT_B6 77697
#define CGJ_SOFT_B7 77698
#define CGJ_SOFT_B8 77699
#include "_cgj_soft_m_template.inc"
