# ~~~
# This file is part of the dune-xt project:
#   https://zivgitlab.uni-muenster.de/ag-ohlberger/dune-community/dune-xt
# Copyright 2009-2021 dune-xt developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   René Fritze    (2018 - 2019)
#   Tim Keil       (2018)
#   Tobias Leibner (2019 - 2020)
# ~~~

import itertools

import grids

dim_range = [1, 3]
dim_range_cols = [3]
dimDomain = [1, 2, 3]

multi_out = {
    grids.pretty_print(g[0], g[1]): g[0] for g in grids.type_and_dim(cache, dimDomain)
}  # noqa: F821

multi_out = {
    filename + ".cc": {
        "types": [
            (filename, grid, r, rC)
            for r, rC in itertools.product(dim_range, dim_range_cols)
        ]
    }
    for filename, grid in multi_out.items()
}
