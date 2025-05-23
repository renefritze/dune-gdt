// This file is part of the dune-xt project:
//   https://zivgitlab.uni-muenster.de/ag-ohlberger/dune-community/dune-xt
// Copyright 2009-2021 dune-xt developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2020)
//   René Fritze     (2020)
//   Tim Keil        (2021)
//   Tobias Leibner  (2021)

#include "config.h"

#include <dune/xt/grid/dd/glued.hh>
#include <dune/xt/grid/grids.hh>
#include <dune/xt/grid/view/coupling.hh>

#include <python/xt/dune/xt/grid/gridprovider.hh>
#include <python/xt/dune/xt/grid/filters/intersection.hh>
#include <python/xt/dune/xt/grid/grids.bindings.hh>


template <template <class> class Filter, class GridTypes = Dune::XT::Grid::bindings::AvailableGridTypes>
struct InitlessIntersectionFilter_for_all_grids
{
  using G = Dune::XT::Common::tuple_head_t<GridTypes>;
  using LGV = typename G::LeafGridView;
  static const size_t d = G::dimension;

  static void bind(pybind11::module& m, const std::string& class_id)
  {
    using Dune::XT::Grid::bindings::grid_name;
    Dune::XT::Grid::bindings::InitlessIntersectionFilter<Filter, LGV>::bind(m, class_id, "leaf");
    Dune::XT::Grid::bindings::InitlessIntersectionFilter<Filter, LGV>::bind_leaf_factory(m, class_id);
#if HAVE_DUNE_GRID_GLUE
    if constexpr (d < 3) {
      using GridGlueType = Dune::XT::Grid::DD::Glued<G, G, Dune::XT::Grid::Layers::leaf>;
      using CGV = Dune::XT::Grid::CouplingGridView<GridGlueType>;
      Dune::XT::Grid::bindings::InitlessIntersectionFilter<Filter, CGV>::bind(m, class_id, "coupling");
      Dune::XT::Grid::bindings::InitlessIntersectionFilter<Filter, CGV>::bind_coupling_factory(m, class_id);
    }
#endif
    InitlessIntersectionFilter_for_all_grids<Filter, Dune::XT::Common::tuple_tail_t<GridTypes>>::bind(m, class_id);
  }
};

template <template <class> class Filter>
struct InitlessIntersectionFilter_for_all_grids<Filter, Dune::XT::Common::tuple_null_type>
{
  static void bind(pybind11::module& /*m*/, const std::string& /*class_id*/) {}
};


template <class GridTypes = Dune::XT::Grid::bindings::AvailableGridTypes>
struct CustomBoundaryIntersectionFilter_for_all_grids
{
  using G = Dune::XT::Common::tuple_head_t<GridTypes>;
  using LGV = typename G::LeafGridView;
  static const size_t d = G::dimension;

  static void bind(pybind11::module& m)
  {
    using Dune::XT::Grid::bindings::grid_name;
    Dune::XT::Grid::bindings::CustomBoundaryIntersectionsFilter<LGV>::bind(m, grid_name<G>::value(), "leaf");
    Dune::XT::Grid::bindings::CustomBoundaryIntersectionsFilter<LGV>::bind_leaf_factory(m);
#if HAVE_DUNE_GRID_GLUE
    if constexpr (d < 3) {
      using GridGlueType = Dune::XT::Grid::DD::Glued<G, G, Dune::XT::Grid::Layers::leaf>;
      using CGV = Dune::XT::Grid::CouplingGridView<GridGlueType>;
      Dune::XT::Grid::bindings::CustomBoundaryIntersectionsFilter<CGV>::bind(m, grid_name<G>::value(), "coupling");
      Dune::XT::Grid::bindings::CustomBoundaryIntersectionsFilter<CGV>::bind_coupling_factory(m);
    }
#endif
    CustomBoundaryIntersectionFilter_for_all_grids<Dune::XT::Common::tuple_tail_t<GridTypes>>::bind(m);
  }
};

template <>
struct CustomBoundaryIntersectionFilter_for_all_grids<Dune::XT::Common::tuple_null_type>
{
  static void bind(pybind11::module& /*m*/) {}
};


template <class GridTypes = Dune::XT::Grid::bindings::AvailableGridTypes>
struct GridProvider_for_all_grids
{
  static void bind(pybind11::module& m)
  {
    Dune::XT::Grid::bindings::GridProvider<Dune::XT::Common::tuple_head_t<GridTypes>>::bind(m);
    GridProvider_for_all_grids<Dune::XT::Common::tuple_tail_t<GridTypes>>::bind(m);
  }
};

template <>
struct GridProvider_for_all_grids<Dune::XT::Common::tuple_null_type>
{
  static void bind(pybind11::module& /*m*/) {}
};


PYBIND11_MODULE(_grid_gridprovider_provider, m)
{
  namespace py = pybind11;
  using namespace Dune::XT::Grid;

  py::module::import("dune.xt.common");
  py::module::import("dune.xt.la");
  py::module::import("dune.xt.grid._grid_boundaryinfo_interfaces");
  py::module::import("dune.xt.grid._grid_boundaryinfo_types");
  py::module::import("dune.xt.grid._grid_element");
  py::module::import("dune.xt.grid._grid_filters_base");

#define BIND_(NAME) InitlessIntersectionFilter_for_all_grids<ApplyOn::NAME>::bind(m, std::string("ApplyOn") + #NAME)

  BIND_(AllIntersections);
  BIND_(AllIntersectionsOnce);
  BIND_(NoIntersections);
  BIND_(InnerIntersections);
  BIND_(InnerIntersectionsOnce);
  //  BIND_(PartitionSetInnerIntersectionsOnce); <- requires partition set as template argument
  BIND_(BoundaryIntersections);
  BIND_(NonPeriodicBoundaryIntersections);
  BIND_(PeriodicBoundaryIntersections);
  BIND_(PeriodicBoundaryIntersectionsOnce);
  //  BIND_(GenericFilteredIntersections); <- requires lambda in init
  //  BIND_(CustomBoundaryAndProcessIntersections); <- requires boundary type and info in init
  BIND_(ProcessIntersections);

#undef BIND_

  CustomBoundaryIntersectionFilter_for_all_grids<>::bind(m);

  GridProvider_for_all_grids<>::bind(m);
}
