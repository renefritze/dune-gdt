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

from matrices import commontype, dunetype, latype

from dune.xt.codegen import have_eigen, have_istl
from dune.xt.codegen import typeid_to_typedef_name as safe_name


def la_types_creator():
    return [
        f.split("_")
        for f in [
            "CommonDenseMatrix_CommonDenseVector_CommonDenseVector_double",
            "CommonSparseMatrixCsr_CommonDenseVector_CommonDenseVector_double",
            "CommonSparseMatrixCsc_CommonDenseVector_CommonDenseVector_double",
            "CommonSparseMatrix_CommonSparseVector_CommonSparseVector_double",
            "EigenDenseMatrix_EigenDenseVector_EigenDenseVector_double",
            "EigenDenseMatrix_EigenDenseVector_EigenMappedDenseVector_double",
            "EigenDenseMatrix_EigenMappedDenseVector_EigenDenseVector_double",
            "EigenDenseMatrix_EigenMappedDenseVector_EigenMappedDenseVector_double",
            "EigenRowMajorSparseMatrix_EigenDenseVector_EigenDenseVector_double",
            "IstlRowMajorSparseMatrix_IstlDenseVector_IstlDenseVector_double",
        ]
    ]


la_types = la_types_creator()

common_types = [
    f.split("_")
    for f in [
        "FieldMatrix_FieldVector_FieldVector_double,3,3",
        "BlockedFieldMatrix_FieldVector_FieldVector_double,1,3,3",
        "BlockedFieldMatrix_BlockedFieldVector_BlockedFieldVector_double,1,3,3",
    ]
]

dune_types = [
    f.split("_")
    for f in [
        "FieldMatrix_FieldVector_FieldVector_double,3,3",
        "DynamicMatrix_DynamicVector_DynamicVector_double",
    ]
]


def la_test_tuple(args):
    o, r, s, f = args
    if f == "complex":
        f = "std::complex<double>"
    return (safe_name(f"{o}_{r}_{s}_{f}"), latype(o, f), latype(r, f), latype(s, f))


def common_test_tuple(args, xt):
    typefunc = commontype if xt == "XT_" else dunetype
    o, r, s, f = args
    fs = f.split(",")
    if len(fs) == 1:
        fvec = f
    elif r.startswith("FieldVector"):
        fvec = "{},{}".format(fs[0], fs[1] if len(fs) == 3 else int(fs[1]) * int(fs[2]))
    else:
        fvec = f"{fs[0]},{fs[1]},{fs[2]}"
    f.replace("complex", "std::complex<double>")
    return (
        safe_name(f"{xt}{o}_{r}_{s}_{f}"),
        typefunc(o, f),
        typefunc(r, fvec),
        typefunc(s, fvec),
    )


def type_ok(t, cache):
    if sum(["Eigen" in x for x in t]):
        return have_eigen(cache)
    if sum(["Istl" in x for x in t]):
        return have_istl(cache)
    return True


def testtypes_creator(la_types, common_types, dune_types, cache):
    return (
        [la_test_tuple(item) for item in la_types if type_ok(item, cache)]
        + [common_test_tuple(item, "XT_") for item in common_types]
        + [common_test_tuple(item, "") for item in dune_types]
    )
