/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 78000 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_78000.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_78000 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_78000 \
 *   user/libcgj/tests/cgj_soft_milestone_78000.c -ldl
 * Direct: -DCGJ_SOFT_M78000_DIRECT + graph_batch77991.c … graph_batch78000.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M78000_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 78000
#define CGJ_SOFT_B0 77991
#define CGJ_SOFT_B1 77992
#define CGJ_SOFT_B2 77993
#define CGJ_SOFT_B3 77994
#define CGJ_SOFT_B4 77995
#define CGJ_SOFT_B5 77996
#define CGJ_SOFT_B6 77997
#define CGJ_SOFT_B7 77998
#define CGJ_SOFT_B8 77999
#include "_cgj_soft_m_template.inc"
