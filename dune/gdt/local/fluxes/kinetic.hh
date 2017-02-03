// This file is part of the dune-gdt project:
//   https://github.com/dune-community/dune-gdt
// Copyright 2010-2016 dune-gdt developers and contributors. All rights reserved.
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Felix Schindler (2016)
//   Tobias Leibner  (2016)

#ifndef DUNE_GDT_LOCAL_FLUXES_KINETIC_HH
#define DUNE_GDT_LOCAL_FLUXES_KINETIC_HH

#include <tuple>
#include <memory>

#include <dune/common/dynmatrix.hh>
#include <dune/common/typetraits.hh>

#include <dune/geometry/referenceelements.hh>

#include <dune/grid/yaspgrid.hh>

#include <dune/xt/common/fmatrix.hh>
#include <dune/xt/functions/interfaces.hh>
#include <dune/xt/functions/constant.hh>
#include <dune/xt/la/container/eigen.hh>

#include "interfaces.hh"
#include "godunov.hh"

namespace Dune {
namespace GDT {


// forwards
template <class AnalyticalFluxImp, size_t domainDim>
class KineticLocalNumericalCouplingFlux;

template <class AnalyticalFluxImp, class BoundaryValueFunctionImp, size_t domainDim>
class KineticLocalNumericalBoundaryFlux;


namespace internal {


template <class AnalyticalFluxImp, size_t domainDim>
class KineticLocalNumericalCouplingFluxTraits
    : public GodunovLocalNumericalCouplingFluxTraits<AnalyticalFluxImp, domainDim>
{
public:
  typedef std::tuple<> LocalfunctionTupleType;
  typedef KineticLocalNumericalCouplingFlux<AnalyticalFluxImp, domainDim> derived_type;
}; // class KineticLocalNumericalCouplingFluxTraits

template <class AnalyticalFluxImp, class BoundaryValueFunctionImp, size_t domainDim>
class KineticLocalNumericalBoundaryFluxTraits
    : public KineticLocalNumericalCouplingFluxTraits<AnalyticalFluxImp, domainDim>
{
  typedef KineticLocalNumericalCouplingFluxTraits<AnalyticalFluxImp, domainDim> BaseType;

public:
  typedef BoundaryValueFunctionImp BoundaryValueFunctionType;
  typedef typename BoundaryValueFunctionType::LocalfunctionType BoundaryValueLocalfunctionType;
  typedef KineticLocalNumericalBoundaryFlux<AnalyticalFluxImp, BoundaryValueFunctionImp, domainDim> derived_type;
  typedef std::tuple<std::shared_ptr<BoundaryValueLocalfunctionType>> LocalfunctionTupleType;
}; // class KineticLocalNumericalBoundaryFluxTraits


} // namespace internal


/**
 *  \brief  Kinetic flux evaluation for inner intersections and periodic boundary intersections.
 */
template <class AnalyticalFluxImp, size_t domainDim = AnalyticalFluxImp::dimDomain>
class KineticLocalNumericalCouplingFlux
    : public LocalNumericalCouplingFluxInterface<internal::KineticLocalNumericalCouplingFluxTraits<AnalyticalFluxImp,
                                                                                                   domainDim>>
{
public:
  typedef internal::KineticLocalNumericalCouplingFluxTraits<AnalyticalFluxImp, domainDim> Traits;
  typedef typename Traits::LocalfunctionTupleType LocalfunctionTupleType;
  typedef typename Traits::EntityType EntityType;
  typedef typename Traits::DomainFieldType DomainFieldType;
  typedef typename Traits::DomainType DomainType;
  typedef typename Traits::RangeFieldType RangeFieldType;
  typedef typename Traits::AnalyticalFluxType AnalyticalFluxType;
  typedef typename Traits::FluxRangeType FluxRangeType;
  typedef typename Traits::FluxJacobianRangeType FluxJacobianRangeType;
  typedef typename Traits::RangeType RangeType;
  static const size_t dimDomain = Traits::dimDomain;
  static const size_t dimRange = Traits::dimRange;

  explicit KineticLocalNumericalCouplingFlux(const AnalyticalFluxType& analytical_flux)
    : analytical_flux_(analytical_flux)
  {
  }

  LocalfunctionTupleType local_functions(const EntityType& /*entity*/) const
  {
    return std::make_tuple();
  }

  template <class IntersectionType>
  RangeType evaluate(
      const LocalfunctionTupleType& /*local_functions_tuple_entity*/,
      const LocalfunctionTupleType& /*local_functions_tuple_neighbor*/,
      const XT::Functions::LocalfunctionInterface<EntityType, DomainFieldType, dimDomain, RangeFieldType, dimRange, 1>&
          local_source_entity,
      const XT::Functions::LocalfunctionInterface<EntityType, DomainFieldType, dimDomain, RangeFieldType, dimRange, 1>&
          local_source_neighbor,
      const IntersectionType& intersection,
      const Dune::FieldVector<DomainFieldType, dimDomain - 1>& x_intersection,
      const double t = 0) const
  {
    // get function values
    const auto x_intersection_entity_coords = intersection.geometryInInside().global(x_intersection);
    const auto x_intersection_neighbor_coords = intersection.geometryInOutside().global(x_intersection);
    const RangeType u_i = local_source_entity.evaluate(x_intersection_entity_coords);
    RangeType u_j = local_source_neighbor.evaluate(x_intersection_neighbor_coords);
    auto n_ij = intersection.unitOuterNormal(x_intersection);
    RangeType ret = analytical_flux_.calculate_flux_integral(u_i,
                                                             intersection.inside(),
                                                             x_intersection_entity_coords,
                                                             u_j,
                                                             intersection.outside(),
                                                             x_intersection_neighbor_coords,
                                                             n_ij,
                                                             t);
    ret *= intersection.geometry().volume();
    return ret;
  } // RangeType evaluate(...) const

private:
  const AnalyticalFluxType& analytical_flux_;
}; // class KineticLocalNumericalCouplingFlux

/**
*  \brief  Kinetic flux evaluation for Dirichlet boundary intersections.
*/
template <class AnalyticalFluxImp, class BoundaryValueFunctionImp, size_t domainDim = AnalyticalFluxImp::dimDomain>
class KineticLocalNumericalBoundaryFlux
    : public LocalNumericalBoundaryFluxInterface<internal::
                                                     KineticLocalNumericalBoundaryFluxTraits<AnalyticalFluxImp,
                                                                                             BoundaryValueFunctionImp,
                                                                                             domainDim>>
{
public:
  typedef internal::KineticLocalNumericalBoundaryFluxTraits<AnalyticalFluxImp, BoundaryValueFunctionImp, domainDim>
      Traits;
  typedef typename Traits::BoundaryValueFunctionType BoundaryValueFunctionType;
  typedef typename Traits::LocalfunctionTupleType LocalfunctionTupleType;
  typedef typename Traits::EntityType EntityType;
  typedef typename Traits::DomainFieldType DomainFieldType;
  typedef typename Traits::DomainType DomainType;
  typedef typename Traits::RangeFieldType RangeFieldType;
  typedef typename Traits::AnalyticalFluxType AnalyticalFluxType;
  typedef typename Traits::FluxRangeType FluxRangeType;
  typedef typename Traits::FluxJacobianRangeType FluxJacobianRangeType;
  typedef typename Traits::RangeType RangeType;
  static const size_t dimDomain = Traits::dimDomain;
  static const size_t dimRange = Traits::dimRange;

  explicit KineticLocalNumericalBoundaryFlux(const AnalyticalFluxType& analytical_flux,
                                             const BoundaryValueFunctionType& boundary_values)
    : analytical_flux_(analytical_flux)
    , boundary_values_(boundary_values)
  {
  }

  LocalfunctionTupleType local_functions(const EntityType& entity) const
  {
    return std::make_tuple(boundary_values_.local_function(entity));
  }

  template <class IntersectionType>
  RangeType evaluate(
      const LocalfunctionTupleType& local_functions_tuple,
      const XT::Functions::LocalfunctionInterface<EntityType, DomainFieldType, dimDomain, RangeFieldType, dimRange, 1>&
          local_source_entity,
      const IntersectionType& intersection,
      const Dune::FieldVector<DomainFieldType, dimDomain - 1>& x_intersection,
      const double t = 0) const
  {
    // get function values
    const auto x_intersection_entity_coords = intersection.geometryInInside().global(x_intersection);
    const RangeType u_i = local_source_entity.evaluate(x_intersection_entity_coords);
    auto u_j = std::get<0>(local_functions_tuple)->evaluate(x_intersection_entity_coords);
    auto n_ij = intersection.unitOuterNormal(x_intersection);
    RangeType ret = analytical_flux_.calculate_flux_integral(
        u_i, intersection.inside(), x_intersection_entity_coords, u_j, intersection.inside(), DomainType(200), n_ij, t);
    ret *= intersection.geometry().volume();
    return ret;
  } // RangeType evaluate(...) const

private:
  const AnalyticalFluxType& analytical_flux_;
  const BoundaryValueFunctionType& boundary_values_;
}; // class KineticLocalNumericalBoundaryFlux


} // namespace GDT
} // namespace Dune

#endif // DUNE_GDT_LOCAL_FLUXES_KINETIC_HH
