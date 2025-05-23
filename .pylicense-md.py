# ~~~
# This file is part of the dune-gdt project:
#   https://zivgitlab.uni-muenster.de/ag-ohlberger/dune-community/dune-gdt
# Copyright 2010-2021 dune-gdt developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   René Fritze    (2018 - 2019)
#   Tobias Leibner (2019 - 2021)
#
# This file is part of the dune-xt project:
# ~~~

name = "This file is part of the dune-xt project:"
url = "https://zivgitlab.uni-muenster.de/ag-ohlberger/dune-community/dune-xt"
copyright_statement = (
    "Copyright 2009-2021 dune-xt developers and contributors. All rights reserved."
)
license = """Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
          with "runtime exception" (http://www.dune-project.org/license.html)"""
prefix = "#"
lead_in = "```"
lead_out = "```"

include_patterns = ("*.md",)
exclude_patterns = (
    "*/deps/*",
    "*/wheelhouse/*",
)
