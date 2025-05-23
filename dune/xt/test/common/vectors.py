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


def vectors(cache):
    vecs = ["FieldVector", "DynamicVector", "CommonDenseVector", "CommonSparseVector"]
    if codegen.have_eigen(cache):
        vecs.extend(["EigenDenseVector", "EigenMappedDenseVector"])
    if codegen.have_istl(cache):
        vecs.append("IstlDenseVector")
    return vecs


def vectortype(c, f):
    if c == "FieldVector":
        f = f"{f}, 4"
        return commontype(c, f)
    elif c == "DynamicVector":
        return dunetype(c, f)
    else:
        return latype(c, f)


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
