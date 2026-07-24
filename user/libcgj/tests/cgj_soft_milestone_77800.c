/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 77800 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_77800.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_77800 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_77800 \
 *   user/libcgj/tests/cgj_soft_milestone_77800.c -ldl
 * Direct: -DCGJ_SOFT_M77800_DIRECT + graph_batch77791.c … graph_batch77800.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M77800_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 77800
#define CGJ_SOFT_B0 77791
#define CGJ_SOFT_B1 77792
#define CGJ_SOFT_B2 77793
#define CGJ_SOFT_B3 77794
#define CGJ_SOFT_B4 77795
#define CGJ_SOFT_B5 77796
#define CGJ_SOFT_B6 77797
#define CGJ_SOFT_B7 77798
#define CGJ_SOFT_B8 77799
#include "_cgj_soft_m_template.inc"
