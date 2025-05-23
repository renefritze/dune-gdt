// This file is part of the dune-gdt project:
//   https://github.com/dune-community/dune-gdt
// Copyright 2010-2018 dune-gdt developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2019)

#include <dune/xt/test/main.hxx> // <- this one has to come first (includes the config.h)!

#include <dune/xt/grid/grids.hh>

#include "oswald-interpolation.hh"


using Simplicial2dGrids = ::testing::Types<
#if HAVE_DUNE_ALUGRID
    ALU_2D_SIMPLEX_CONFORMING,
    ALU_2D_SIMPLEX_NONCONFORMING
#endif
#if HAVE_DUNE_UGGRID || HAVE_UG
    ,
    UG_2D
#endif
    >;

#if HAVE_DUNE_UGGRID || HAVE_UG || HAVE_DUNE_ALUGRID
template <class G>
using OswaldInterpolationOperator = Dune::GDT::Test::OswaldInterpolationOperatorOnLeafViewTest<G>;
TYPED_TEST_SUITE(OswaldInterpolationOperator, Simplicial2dGrids);
TYPED_TEST(OswaldInterpolationOperator, fulfills_l2_interpolation_estimate)
{
  this->fulfills_l2_interpolation_estimate();
}
TYPED_TEST(OswaldInterpolationOperator, fulfills_h1_interpolation_estimate)
{
  this->fulfills_h1_interpolation_estimate();
}
#endif // HAVE_DUNE_UGGRID || HAVE_UG || HAVE_DUNE_ALUGRID