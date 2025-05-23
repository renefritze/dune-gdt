# ~~~
# This file is part of the dune-xt project:
#   https://zivgitlab.uni-muenster.de/ag-ohlberger/dune-community/dune-xt
# Copyright 2009-2021 dune-xt developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   René Fritze    (2017 - 2019)
#   Tobias Leibner (2018 - 2020)
# ~~~

from dune.xt import codegen


def matrices(cache):
    mat = [
        "CommonDenseMatrix",
        "CommonSparseMatrixCsr",
        "CommonSparseMatrixCsc",
        "CommonSparseOrDenseMatrixCsr",
        "CommonSparseOrDenseMatrixCsc",
    ]
    if codegen.have_eigen(cache):
        mat.extend(["EigenRowMajorSparseMatrix", "EigenDenseMatrix"])
    if codegen.have_istl(cache):
        mat.append("IstlRowMajorSparseMatrix")
    return mat


def vectors(cache):
    vecs = ["CommonDenseVector", "CommonSparseVector"]
    if codegen.have_eigen(cache):
        vecs.extend(["EigenDenseVector", "EigenMappedDenseVector"])
    if codegen.have_istl(cache):
        vecs.append("IstlDenseVector")
    return vecs


def name_type_tuple(c, f):
    v = latype(c, f)
    return codegen.typeid_to_typedef_name(c + f), v


def latype(c, f):
    return f"Dune::XT::LA::{c}<{f}>"


def commontype(c, f):
    return f"Dune::XT::Common::{c}<{f}>"


def dunetype(c, f):
    return f"Dune::{c}<{f}>"


def fieldtypes(cache):
    return ["double", "std::complex<double>"]


def vector_filter(vector, field):
    return not ("EigenMappedDenseVector" in vector and "complex" in field)
