/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 78100 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_78100.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_78100 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_78100 \
 *   user/libcgj/tests/cgj_soft_milestone_78100.c -ldl
 * Direct: -DCGJ_SOFT_M78100_DIRECT + graph_batch78091.c … graph_batch78100.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M78100_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 78100
#define CGJ_SOFT_B0 78091
#define CGJ_SOFT_B1 78092
#define CGJ_SOFT_B2 78093
#define CGJ_SOFT_B3 78094
#define CGJ_SOFT_B4 78095
#define CGJ_SOFT_B5 78096
#define CGJ_SOFT_B6 78097
#define CGJ_SOFT_B7 78098
#define CGJ_SOFT_B8 78099
#include "_cgj_soft_m_template.inc"
