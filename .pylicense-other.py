# ~~~
# This file is part of the dune-gdt project:
#   https://zivgitlab.uni-muenster.de/ag-ohlberger/dune-community/dune-gdt
# Copyright 2010-2021 dune-gdt developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2022)
#   René Fritze     (2018 - 2019, 2022)
#   Tobias Leibner  (2019 - 2021)
# ~~~

name = "This file is part of the dune-gdt project:"
url = "https://zivgitlab.uni-muenster.de/ag-ohlberger/dune-community/dune-gdt"
copyright_statement = (
    "Copyright 2010-2021 dune-gdt developers and contributors. All rights reserved."
)
license = """Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
          with "runtime exception" (http://www.dune-project.org/license.html)"""
prefix = "#"
lead_in = "# ~~~"
lead_out = "# ~~~"

include_patterns = (
    "*.txt",
    "*.cmake",
    "*.py",
    "*.py.in",
    "*.pc.in",
    "*.sh",
    "*.bash",
    "*.dgf",
    "*.msh",
    "*.gdb",
    "*.cfg",
    "*.travis.*",
    "*.gitignore",
    "*.mailmap",
    "*.gitattributes",
    "*gitignore-*",
    "*stamp-vc",
    "*dune.module",
    "*Doxylocal",
    "*.clang-format",
    "*COPYING-CMAKE-SCRIPTS",
    "*README",
    "*LICENSE",
    "*mainpage",
    "*switch-build_dir",
    "*dune-xt.pc.in",
    "*CMakeLists.txt",
    "*.cmake.in",
    "*.py.in*",
)
exclude_patterns = (
    "*config.h.cmake",
    "*.vcsetup*",
    "FindEigen3.cmake",
    "*.dgf",
    "*builder_definitions.cmake",
    "*/deps/*",
    "*/wheelhouse/*",
    "*builder_definitions.cmake",
    "*.ci/shared/*",
)
