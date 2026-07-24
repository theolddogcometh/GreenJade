/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 79800 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_79800.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_79800 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_79800 \
 *   user/libcgj/tests/cgj_soft_milestone_79800.c -ldl
 * Direct: -DCGJ_SOFT_M79800_DIRECT + graph_batch79791.c … graph_batch79800.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M79800_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 79800
#define CGJ_SOFT_B0 79791
#define CGJ_SOFT_B1 79792
#define CGJ_SOFT_B2 79793
#define CGJ_SOFT_B3 79794
#define CGJ_SOFT_B4 79795
#define CGJ_SOFT_B5 79796
#define CGJ_SOFT_B6 79797
#define CGJ_SOFT_B7 79798
#define CGJ_SOFT_B8 79799
#include "_cgj_soft_m_template.inc"
