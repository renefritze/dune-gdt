# ~~~
# This file is part of the dune-xt project:
#   https://zivgitlab.uni-muenster.de/ag-ohlberger/dune-community/dune-xt
# Copyright 2009-2021 dune-xt developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   René Fritze    (2018 - 2019)
#   Tobias Leibner (2018 - 2020)
# ~~~

from dune.xt.codegen import have_eigen
from dune.xt.codegen import typeid_to_typedef_name as safe_name

matrix = [
    "EigenDenseMatrix<double>",
    "FieldMatrix<double, 6, 6>",
    "CommonDenseMatrix<double>",
    "CommonSparseMatrix<double>",
    "XT::Common::BlockedFieldMatrix<double, 1, 6, 6>",
]


def _ok(ft):
    if "Eigen" in ft:
        return have_eigen(cache)  # noqa: F821
    return True


testtypes = [(safe_name("_".join(ft)), ft) for ft in matrix if _ok(ft)]
