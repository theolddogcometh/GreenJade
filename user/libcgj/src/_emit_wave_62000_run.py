#!/usr/bin/env python3
"""Bootstrap: import and run _emit_wave_62000_once.main()."""
import runpy
from pathlib import Path

p = Path(__file__).resolve().parent / "_emit_wave_62000_once.py"
runpy.run_path(str(p), run_name="__main__")
