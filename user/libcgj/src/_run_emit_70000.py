#!/usr/bin/env python3
# SPDX-License-Identifier: MIT OR Apache-2.0
"""Bootstrap: run _emit_wave_70000_once.main()."""
import runpy
from pathlib import Path

runpy.run_path(
    str(Path(__file__).resolve().parent / "_emit_wave_70000_once.py"),
    run_name="__main__",
)
