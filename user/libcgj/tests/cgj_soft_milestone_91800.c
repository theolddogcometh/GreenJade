/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 91800 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_91800.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_91800 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_91800 \
 *   user/libcgj/tests/cgj_soft_milestone_91800.c -ldl
 * Direct: -DCGJ_SOFT_M91800_DIRECT + graph_batch91791.c … graph_batch91800.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M91800_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 91800
#define CGJ_SOFT_B0 91791
#define CGJ_SOFT_B1 91792
#define CGJ_SOFT_B2 91793
#define CGJ_SOFT_B3 91794
#define CGJ_SOFT_B4 91795
#define CGJ_SOFT_B5 91796
#define CGJ_SOFT_B6 91797
#define CGJ_SOFT_B7 91798
#define CGJ_SOFT_B8 91799
#include "_cgj_soft_m_template.inc"
