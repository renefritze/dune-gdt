// This file is part of the dune-gdt project:
//   https://github.com/dune-community/dune-gdt
// Copyright 2010-2016 dune-gdt developers and contributors. All rights reserved.
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2015 - 2016)

#include <dune/xt/common/test/main.hxx>

#include "projections/l2-local.hh"
#include "spaces/rt/pdelab.hh"

using namespace Dune::GDT::Test;

#if HAVE_DUNE_PDELAB


typedef testing::Types<SPACES_RT_PDELAB
#if HAVE_DUNE_ALUGRID
                       ,
                       SPACES_RT_PDELAB_ALUGRID
#endif
                       >
    SpaceTypes;

TYPED_TEST_CASE(L2LocalProjectionOperatorTest, SpaceTypes);
TYPED_TEST(L2LocalProjectionOperatorTest, constructible_by_ctor)
{
  this->constructible_by_ctor();
}
TYPED_TEST(L2LocalProjectionOperatorTest, constructible_by_factory)
{
  this->constructible_by_factory();
}
TYPED_TEST(L2LocalProjectionOperatorTest, produces_correct_results)
{
  this->produces_correct_results(0.0925927);
}


#else // HAVE_DUNE_PDELAB


TEST(DISABLED_L2LocalProjectionOperatorTest, constructible_by_ctor)
{
}
TEST(DISABLED_L2LocalProjectionOperatorTest, constructible_by_factory)
{
}
TEST(DISABLED_L2LocalProjectionOperatorTest, produces_correct_results)
{
}


#endif // HAVE_DUNE_PDELAB
