/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 79000 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_79000.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_79000 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_79000 \
 *   user/libcgj/tests/cgj_soft_milestone_79000.c -ldl
 * Direct: -DCGJ_SOFT_M79000_DIRECT + graph_batch78991.c … graph_batch79000.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M79000_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 79000
#define CGJ_SOFT_B0 78991
#define CGJ_SOFT_B1 78992
#define CGJ_SOFT_B2 78993
#define CGJ_SOFT_B3 78994
#define CGJ_SOFT_B4 78995
#define CGJ_SOFT_B5 78996
#define CGJ_SOFT_B6 78997
#define CGJ_SOFT_B7 78998
#define CGJ_SOFT_B8 78999
#include "_cgj_soft_m_template.inc"
