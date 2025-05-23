# ~~~
# This file is part of the dune-xt project:
#   https://zivgitlab.uni-muenster.de/ag-ohlberger/dune-community/dune-xt
# Copyright 2009-2021 dune-xt developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2016)
#   René Fritze     (2017 - 2019)
#   Tobias Leibner  (2015 - 2017, 2019 - 2020)
# ~~~

from matrices import commontype, latype

from dune.xt.codegen import have_eigen, have_istl
from dune.xt.codegen import typeid_to_typedef_name as safe_name

types = [
    f.split("_")
    for f in [
        "CommonDenseMatrix_CommonDenseVector_CommonDenseVector_complex",
        "CommonDenseMatrix_CommonDenseVector_CommonDenseVector_double",
        "EigenDenseMatrix_EigenDenseVector_EigenDenseVector_complex",
        "EigenDenseMatrix_EigenDenseVector_EigenDenseVector_double",
        "EigenDenseMatrix_EigenDenseVector_EigenMappedDenseVector_double",
        "EigenDenseMatrix_EigenMappedDenseVector_EigenDenseVector_double",
        "EigenDenseMatrix_EigenMappedDenseVector_EigenMappedDenseVector_double",
        "EigenRowMajorSparseMatrix_EigenDenseVector_EigenDenseVector_complex",
        "EigenRowMajorSparseMatrix_EigenDenseVector_EigenDenseVector_double",
        "FieldMatrix_FieldVector_FieldVector_complex",
        "FieldMatrix_FieldVector_FieldVector_double",
        "IstlRowMajorSparseMatrix_IstlDenseVector_IstlDenseVector_double",
    ]
]


def test_tuple(args):
    o, r, s, f = args
    if f == "complex":
        f = "std::complex<double>"
    if o == "FieldMatrix":
        fm = f"{f}, 10, 10"
        fv = f"{f}, 10"
        return (
            safe_name(f"{o}_{r}_{s}_{f}"),
            commontype(o, fm),
            commontype(r, fv),
            commontype(s, fv),
        )
    else:
        return (safe_name(f"{o}_{r}_{s}_{f}"), latype(o, f), latype(r, f), latype(s, f))


def type_ok(t):
    if sum(["Eigen" in x for x in t]):
        return have_eigen(cache)  # noqa: F821
    if sum(["Istl" in x for x in t]):
        return have_istl(cache)  # noqa: F821
    return True


testtypes = [test_tuple(item) for item in types if type_ok(item)]
