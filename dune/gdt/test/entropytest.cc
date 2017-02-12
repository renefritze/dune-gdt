// This file is part of the dune-gdt project:
//   https://github.com/dune-community/dune-gdt
// Copyright 2010-2016 dune-gdt developers and contributors. All rights reserved.
// License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
// Authors:
//   Tobias Leibner  (2016)

#include <stdio.h>
#include <stdlib.h>
#include <utility>
#include <functional>
#include <vector>
#include <iostream>

#include "config.h"

#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/string.hh>
#include <dune/xt/common/parallel/threadmanager.hh>

#include <dune/xt/grid/information.hh>
#include <dune/xt/grid/gridprovider.hh>

#include <dune/xt/la/container/common.hh>
#include <dune/xt/la/container/eigen.hh>
#include <dune/xt/la/container/istl.hh>

#include <dune/grid/yaspgrid.hh>
#include <dune/grid/io/file/gmshwriter.hh>

#if HAVE_DUNE_FEM
#include <dune/fem/misc/mpimanager.hh>
#endif

#include <dune/gdt/discretefunction/datahandle.hh>
#include <dune/gdt/discretefunction/default.hh>
#include <dune/gdt/local/fluxes/interfaces.hh>
#include <dune/gdt/local/fluxes/entropybased.hh>
#include <dune/gdt/operators/fv.hh>
#include <dune/gdt/spaces/fv/product.hh>
#include <dune/gdt/timestepper/factory.hh>
#include <dune/gdt/timestepper/fractional-step.hh>
#include <dune/gdt/timestepper/implicit-rungekutta.hh>
#include <dune/gdt/test/hyperbolic/problems/fokkerplanck/sourcebeam.hh>
#include <dune/gdt/test/hyperbolic/problems/fokkerplanck/planesource.hh>
#include <dune/gdt/test/hyperbolic/problems/fokkerplanck/pointsource.hh>


int main(int argc, char** argv)
{
  using namespace Dune;
  using namespace Dune::GDT;

  // ***************** parse arguments and set up MPI and TBB
  size_t num_threads = 1;
  size_t num_save_steps = 1000;
  std::string grid_size("100"), overlap_size("1");
  double t_end = 0;
  bool visualize = true;
  for (int i = 1; i < argc; ++i) {
    if (std::string(argv[i]) == "-num_threads") {
      if (i + 1 < argc) {
        num_threads = Dune::XT::Common::from_string<size_t>(argv[++i]);
      } else {
        std::cerr << "-num_threads option requires one argument." << std::endl;
        return 1;
      }
    } else if (std::string(argv[i]) == "-num_save_steps") {
      if (i + 1 < argc) {
        num_save_steps = Dune::XT::Common::from_string<size_t>(argv[++i]);
      } else {
        std::cerr << "-num_save_steps option requires one argument." << std::endl;
        return 1;
      }
    } else if (std::string(argv[i]) == "-grid_size") {
      if (i + 1 < argc) {
        grid_size = argv[++i];
      } else {
        std::cerr << "-grid_size option requires one argument." << std::endl;
        return 1;
      }
    } else if (std::string(argv[i]) == "-overlap_size") {
      if (i + 1 < argc) {
        overlap_size = argv[++i];
      } else {
        std::cerr << "-overlap_size option requires one argument." << std::endl;
        return 1;
      }
    } else if (std::string(argv[i]) == "-t_end") {
      if (i + 1 < argc) {
        t_end = XT::Common::from_string<double>(argv[++i]);
      } else {
        std::cerr << "-t_end option requires one argument." << std::endl;
        return 1;
      }
    } else if (std::string(argv[i]) == "--no_visualization") {
      visualize = false;
    } else {
      std::cerr << "Unknown option " << std::string(argv[i]) << std::endl;
      return 1;
    }
  }

  DXTC_CONFIG.set("threading.partition_factor", 1u, true);
  Dune::XT::Common::threadManager().set_max_threads(num_threads);

#define USE_SMP_PARALLEL 1
#if HAVE_DUNE_FEM
  Dune::Fem::MPIManager::initialize(argc, argv);
#else
  MPIHelper::instance(argc, argv);
#endif

  // ********************* choose dimensions, fluxes and grid type ************************
  static const int dimDomain = 3;
  static const int momentOrder = 3;
  //  const auto numerical_flux = NumericalFluxes::kinetic;
  const auto numerical_flux = NumericalFluxes::godunov;
  const auto time_stepper_method = TimeStepperMethods::explicit_euler;
  const auto rhs_time_stepper_method = TimeStepperMethods::implicit_euler;
  const auto container_backend = Dune::XT::LA::default_sparse_backend;

  typedef typename Dune::YaspGrid<dimDomain, Dune::EquidistantOffsetCoordinates<double, dimDomain>> GridType;
  typedef typename GridType::LeafGridView GridViewType;
  typedef typename GridType::Codim<0>::Entity EntityType;

  //******************** choose ProblemType ***********************************************

  //  typedef typename Hyperbolic::Problems::
  //      SourceBeamPnHatFunctions<EntityType, double, dimDomain, double, momentOrder>
  //          ProblemType;
  //  typedef
  //      typename Hyperbolic::Problems::SourceBeamPnLegendre<EntityType, double, dimDomain, double,
  //      momentOrder>
  //          ProblemType;
  //  typedef typename Hyperbolic::Problems::
  //      SourceBeamPnLegendreLaplaceBeltrami<EntityType, double, dimDomain, double, momentOrder>
  //          ProblemType;
  //  typedef typename Hyperbolic::Problems::
  //      SourceBeamPnFirstOrderDG<EntityType, double, dimDomain, double, momentOrder>
  //          ProblemType;
  //  typedef typename Hyperbolic::Problems::
  //      PlaneSourcePnLegendre<EntityType, double, dimDomain, double, momentOrder>
  //          ProblemType;
  //  typedef typename Hyperbolic::Problems::
  //      PlaneSourcePnHatFunctions<EntityType, double, dimDomain, double, momentOrder>
  //          ProblemType;
  //  typedef typename Hyperbolic::Problems::
  //      PlaneSourcePnFirstOrderDG<EntityType, double, dimDomain, double, momentOrder>
  //          ProblemType;
  //  typedef typename Hyperbolic::Problems::
  //      PointSourcePnLegendre<EntityType, double, dimDomain, double, momentOrder>
  //          ProblemType;
  typedef typename Hyperbolic::Problems::PointSourcePnHatFunctions<EntityType, double, dimDomain, double, 26>
      ProblemType;
  //  typedef typename Hyperbolic::Problems::PointSourcePnPartialMoments<EntityType, double, dimDomain, double, 8>
  //      ProblemType;

  //******************* get typedefs and constants from ProblemType **********************//
  using DomainFieldType = typename ProblemType::DomainFieldType;
  using DomainType = typename ProblemType::DomainType;
  using RangeFieldType = typename ProblemType::RangeFieldType;
  using RangeType = typename ProblemType::RangeType;
  static const size_t dimRange = ProblemType::dimRange;
  typedef typename ProblemType::RHSType RHSType;
  typedef typename ProblemType::InitialValueType InitialValueType;
  typedef typename ProblemType::BoundaryValueType BoundaryValueType;
  static const bool linear = ProblemType::linear;

  //******************* create grid and FV space ***************************************
  auto grid_config = ProblemType::default_grid_config();
  grid_config["num_elements"] = grid_size;
  grid_config["overlap_size"] = overlap_size;
  const auto grid_ptr = Dune::XT::Grid::CubeGridProviderFactory<GridType>::create(grid_config).grid_ptr();
  const auto& grid = *grid_ptr;
  assert(grid.comm().size() == 1 || grid.overlapSize(0) > 0);
  const GridViewType& grid_view = grid_ptr->leafGridView();
  typedef FvProductSpace<GridViewType, RangeFieldType, dimRange, 1> SpaceType;
  const SpaceType fv_space(grid_view);

  // ***************** get quadrature rule *********************************************

  //  // 1D quadrature that consists of a Gauss-Legendre quadrature on each cell of the velocity grid
  //  Dune::QuadratureRule<double, dimDomain> quadrature_rule;
  //  static const int num_cells = 10;
  //  Dune::FieldVector<double, dimDomain> lower_left(-1);
  //  Dune::FieldVector<double, dimDomain> upper_right(1);
  //  static const std::array<int, dimDomain> s{num_cells};
  //  GridType velocity_grid(lower_left, upper_right, s);
  //  const auto velocity_grid_view = velocity_grid.leafGridView();

  //  for (const auto& entity : elements(velocity_grid_view)) {
  //    const auto local_quadrature_rule = Dune::QuadratureRules<double, dimDomain>::rule(
  //          entity.type(), quadrature_order, Dune::QuadratureType::GaussLegendre);
  //    for (const auto& quad : local_quadrature_rule) {
  //      quadrature_rule.push_back(Dune::QuadraturePoint<double, dimDomain>(
  //                                  entity.geometry().global(quad.position()),
  //                                  quad.weight() * entity.geometry().integrationElement(quad.position())));
  //    }
  //  }


  //  // Lebedev quadrature on unit sphere (in polar coordinates)
  //  const size_t quadrature_order = 20;
  //  const auto quadrature_rule = Hyperbolic::Problems::get_lebedev_quadrature(quadrature_order);

  // 3D quadrature on sphere (from http://www.unizar.es/galdeano/actas_pau/PDFVIII/pp61-69.pdf)
  const size_t octaeder_refinements = 1;
  const auto poly = CGALWrapper::create_octaeder_spherical_triangulation(octaeder_refinements);
  const size_t quadrature_refinements = 4;
  const auto quadrature_rule =
      GDT::Hyperbolic::Problems::get_equally_dist_quad_points_on_poly(poly, quadrature_refinements);

  // 3d adaptive quadrature on sphere (from http://www.unizar.es/galdeano/actas_pau/PDFVIII/pp61-69.pdf)
  typedef std::function<RangeType(const DomainType&)> BasisfunctionsType;
  typedef typename GDT::Hyperbolic::Problems::AdaptiveQuadrature<DomainType, RangeFieldType, RangeType>
      AdaptiveQuadratureType;
  //  std::function<RangeType(const DomainType&, const CGALWrapper::Polyhedron_3&)> basisevaluation =
  //      GDT::Hyperbolic::Problems::evaluate_linear_partial_basis<RangeType, DomainType, CGALWrapper::Polyhedron_3>;
  std::function<RangeType(const DomainType&, const CGALWrapper::Polyhedron_3&)> basisevaluation =
      GDT::Hyperbolic::Problems::evaluate_spherical_barycentric_coordinates<RangeType,
                                                                            DomainType,
                                                                            CGALWrapper::Polyhedron_3>;
  BasisfunctionsType basisfunctions(std::bind(basisevaluation, std::placeholders::_1, poly));
  AdaptiveQuadratureType adaptive_quadrature(poly);

  //******************* create ProblemType object ***************************************
  //  const auto problem_ptr = ProblemType::create(ProblemType::default_config(grid_config));
  const auto problem_ptr =
      ProblemType::create(quadrature_rule, poly, ProblemType::default_config(grid_config, quadrature_rule, poly));
  //  const auto problem_ptr = ProblemType::create(ProblemType::default_config(grid_config, true));
  const ProblemType& problem = *problem_ptr;
  const std::shared_ptr<const InitialValueType> initial_values = problem.initial_values();
  const std::shared_ptr<const BoundaryValueType> boundary_values = problem.boundary_values();
  const std::shared_ptr<const RHSType> rhs = problem.rhs();
  const RangeFieldType CFL = problem.CFL();

  // ***************** project initial values to discrete function *********************
  // create a discrete function for the solution
  typedef typename Dune::XT::LA::Container<double, container_backend>::VectorType VectorType;
  typedef DiscreteFunction<SpaceType, VectorType> DiscreteFunctionType;
  DiscreteFunctionType u(fv_space, "solution");
  // project initial values
  project(*initial_values, u);


  // *************************** get function to calculate u_iso and alpha_iso from u **********************

  // 1D legendre or 3D spherical harmonics
  auto isotropic_dist_calculator_legendre = [](const typename ProblemType::RangeType& uu) {
    typename ProblemType::RangeType u_iso(0), alpha_iso(0);
    u_iso[0] = uu[0];
    alpha_iso[0] = std::log(uu[0] / (ProblemType::dimDomain == 1 ? 2. : 4. * M_PI));
    return std::make_pair(u_iso, alpha_iso);
  };

  const auto v_points = ProblemType::create_equidistant_points();
  auto isotropic_dist_calculator_1d_firstorderdg = [v_points](const typename ProblemType::RangeType& uu) {
    typename ProblemType::RangeType alpha_iso(0);
    typename ProblemType::RangeFieldType psi_iso(0);
    for (size_t ii = 0; ii < dimRange; ii += 2) {
      psi_iso += uu[ii];
      alpha_iso[ii] = 1;
    }
    psi_iso /= 2.;
    alpha_iso *= std::log(psi_iso);
    typename ProblemType::RangeType u_iso(0);
    for (size_t ii = 0; ii < ProblemType::dimRange / 2; ++ii) {
      u_iso[2 * ii] = v_points[ii + 1] - v_points[ii];
      u_iso[2 * ii + 1] = (std::pow(v_points[ii + 1], 2) - std::pow(v_points[ii], 2)) / 2.;
    }
    u_iso *= psi_iso;
    return std::make_pair(u_iso, alpha_iso);
  };

  //  const auto v_points = ProblemType::create_equidistant_points();
  auto isotropic_dist_calculator_1d_hatfunctions = [v_points](const typename ProblemType::RangeType& uu) {
    typename ProblemType::RangeFieldType psi_iso(0);
    for (size_t ii = 0; ii < ProblemType::dimRange; ++ii)
      psi_iso += uu[ii];
    psi_iso /= 2.;
    typename ProblemType::RangeType alpha_iso(std::log(psi_iso)), u_iso;
    u_iso[0] = v_points[1] - v_points[0];
    for (size_t ii = 1; ii < dimRange - 1; ++ii)
      u_iso[ii] = v_points[ii + 1] - v_points[ii - 1];
    u_iso[dimRange - 1] = v_points[dimRange - 1] - v_points[dimRange - 2];
    u_iso *= psi_iso / 2.;
    return std::make_pair(u_iso, alpha_iso);
  };

  const auto basis_integrated = ProblemType::basisfunctions_integrated(quadrature_rule, poly);
  auto isotropic_dist_calculator_3d_hatfunctions = [basis_integrated](const typename ProblemType::RangeType& uu) {
    typename ProblemType::RangeFieldType psi_iso(0);
    for (size_t ii = 0; ii < ProblemType::dimRange; ++ii)
      psi_iso += uu[ii];
    psi_iso /= 4. * M_PI;
    ProblemType::RangeType alpha_iso(std::log(psi_iso));
    auto u_iso = basis_integrated;
    u_iso *= psi_iso;
    return std::make_pair(u_iso, alpha_iso);
  };

  auto isotropic_dist_calculator_3d_partialbasis = [basis_integrated](const typename ProblemType::RangeType& uu) {
    typename ProblemType::RangeFieldType psi_iso(0);
    ProblemType::RangeType alpha_iso(0);
    for (size_t ii = 0; ii < ProblemType::dimRange; ii += 4) {
      psi_iso += uu[ii];
      alpha_iso[ii] = 1.;
    }
    psi_iso /= 4. * M_PI;
    alpha_iso *= std::log(psi_iso);
    auto u_iso = basis_integrated;
    u_iso *= psi_iso;
    return std::make_pair(u_iso, alpha_iso);
  };


  // ********************** store evaluation of basisfunctions at quadrature points in matrix **********************
  // ********************** (for non-adaptive quadratures)                                    **********************
  using BasisValuesMatrixType = std::vector<Dune::FieldVector<double, dimRange>>;
  BasisValuesMatrixType basis_values_matrix(quadrature_rule.size());
  //  using BasisValuesMatrixType = std::vector<VectorType>;
  //  BasisValuesMatrixType basis_values_matrix(quadrature_rule.size(), VectorType(dimRange));
  for (size_t ii = 0; ii < quadrature_rule.size(); ++ii) {
    //    // 3D hatfunctions on sphere
    const auto hatfunctions_evaluated =
        Hyperbolic::Problems::evaluate_spherical_barycentric_coordinates<RangeType,
                                                                         DomainType,
                                                                         CGALWrapper::Polyhedron_3>(
            quadrature_rule[ii].position(), poly);

    // 3D partial moments
    //    const auto partial_basis_evaluated =
    //        GDT::Hyperbolic::Problems::evaluate_linear_partial_basis<RangeType, DomainType,
    //        CGALWrapper::Polyhedron_3>(
    //            quadrature_rule[ii].position(), poly);

    for (size_t nn = 0; nn < dimRange; ++nn) {
      basis_values_matrix[ii][nn] = hatfunctions_evaluated[nn];
      //      basis_values_matrix[ii][nn] = partial_basis_evaluated[nn];
      //      basis_values_matrix[ii][nn] =
      //          Hyperbolic::Problems::evaluate_legendre_polynomial(quadrature_rule[ii].position(), nn);
      //      basis_values_matrix[ii][nn] = Hyperbolic::Problems::evaluate_hat_function(
      //          quadrature_rule[ii].position()[0], nn, ProblemType::create_equidistant_points());
      //      basis_values_matrix[ii][nn] = Hyperbolic::Problems::evaluate_first_order_dg(
      //          quadrature_rule[ii].position()[0], nn, ProblemType::create_equidistant_points());
      //      basis_values_matrix[ii][nn] = Hyperbolic::Problems::evaluate_real_spherical_harmonics(
      //          quadrature_rule[ii].position()[0],
      //          quadrature_rule[ii].position()[1],
      //          Hyperbolic::Problems::get_l_and_m(nn).first,
      //          Hyperbolic::Problems::get_l_and_m(nn).second);
    }
  }


  //*********************** choose analytical flux *************************************************************

  //  typedef typename
  //      EntropyBasedLocalFlux<GridViewType, EntityType, double, dimDomain, double, dimRange, 1>
  //          AnalyticalFluxType;

  //  typedef AdaptiveEntropyBasedLocalFlux<GridViewType, EntityType, double, dimDomain, double, dimRange, 1>
  //      AnalyticalFluxType;

  //  typedef typename EntropyBasedLocalFlux3D<GridViewType,
  //                                                      EntityType,
  //                                                      double,
  //                                                      dimDomain,
  //                                                      double,
  //                                                      dimRange,
  //                                                      1,
  //                                                      container_backend>
  //      AnalyticalFluxType;


  typedef typename ProblemType::FluxType AnalyticalFluxType;

  //  typedef typename EntropyBasedLocalFluxHatFunctions<GridViewType,
  //                                                                typename SpaceType::EntityType,
  //                                                                double,
  //                                                                dimDomain,
  //                                                                double,
  //                                                                dimRange,
  //                                                                1>
  //      AnalyticalFluxType;


  // ************************* create analytical flux object ***************************************

  const std::shared_ptr<const AnalyticalFluxType> analytical_flux = problem.flux();
  //  const auto analytical_flux =
  //      std::make_shared<const AnalyticalFluxType>(grid_view, ProblemType::create_equidistant_points());
  //  const auto analytical_flux = std::make_shared<const AnalyticalFluxType>(
  //      grid_view, quadrature_rule, basis_values_matrix, ProblemType::create_equidistant_points());
  //  const auto analytical_flux =
  //      std::make_shared<const AnalyticalFluxType>(grid_view, quadrature_rule, basis_values_matrix, poly,
  //      "umfpack");

  //  const auto analytical_flux = std::make_shared<const AnalyticalFluxType>(
  //      grid_view,
  //      quadrature_rule,
  //      basis_values_matrix,
  //      // isotropic_dist_calculator_3d_partialbasis,
  //      isotropic_dist_calculator_3d_hatfunctions,
  //      basisfunctions,
  //      adaptive_quadrature);


  // ******************** choose flux and rhs operator and timestepper
  // *************************************************

  typedef typename Dune::XT::Functions::ConstantFunction<EntityType, DomainFieldType, dimDomain, RangeFieldType, 1, 1>
      ConstantFunctionType;
  typedef AdvectionRHSOperator<RHSType> RHSOperatorType;
  typedef typename std::
      conditional<numerical_flux == NumericalFluxes::kinetic,
                  AdvectionKineticOperator<AnalyticalFluxType, BoundaryValueType>,
                  std::conditional<numerical_flux == NumericalFluxes::laxfriedrichs
                                       || numerical_flux == NumericalFluxes::laxfriedrichs_with_reconstruction
                                       || numerical_flux == NumericalFluxes::local_laxfriedrichs
                                       || numerical_flux == NumericalFluxes::local_laxfriedrichs_with_reconstruction,
                                   AdvectionLaxFriedrichsOperator<AnalyticalFluxType,
                                                                  BoundaryValueType,
                                                                  ConstantFunctionType>,
                                   AdvectionGodunovOperator<AnalyticalFluxType, BoundaryValueType>>::type>::type
          AdvectionOperatorType;
  typedef
      typename TimeStepperFactory<AdvectionOperatorType, DiscreteFunctionType, RangeFieldType, time_stepper_method>::
          TimeStepperType OperatorTimeStepperType;
  typedef typename TimeStepperFactory<RHSOperatorType,
                                      DiscreteFunctionType,
                                      RangeFieldType,
                                      rhs_time_stepper_method,
                                      Dune::XT::LA::default_sparse_backend>::TimeStepperType RHSOperatorTimeStepperType;
  typedef FractionalTimeStepper<OperatorTimeStepperType, RHSOperatorTimeStepperType> TimeStepperType;

  // *************** choose t_end and initial dt **************************************
  // calculate dx and choose initial dt
  Dune::XT::Grid::Dimensions<typename SpaceType::GridViewType> dimensions(grid_view);
  RangeFieldType dx = dimensions.entity_width.max();
  if (dimDomain == 2)
    dx /= std::sqrt(2);
  if (dimDomain == 3)
    dx /= std::sqrt(3);
  RangeFieldType dt = 0.5 * CFL * dx;
  t_end = XT::Common::FloatCmp::eq(t_end, 0.) ? problem.t_end() : t_end;


  // *********************** create operators and timesteppers ************************************
  const ConstantFunctionType dx_function(dx);
  AdvectionOperatorType advection_operator =
      internal::AdvectionOperatorCreator<AdvectionOperatorType, numerical_flux>::create(
          *analytical_flux, *boundary_values, dx_function, dt, linear);
  RHSOperatorType rhs_operator(*rhs);

  // create timestepper
  OperatorTimeStepperType timestepper_op(advection_operator, u, -1.0);

  // ******************************** do the time steps ***********************************************************
  if (problem.has_non_zero_rhs()) {
    // use fractional step method
    RHSOperatorTimeStepperType timestepper_rhs(rhs_operator, u);
    TimeStepperType timestepper(timestepper_op, timestepper_rhs);
    std::string filename = ProblemType::static_id();
    filename +=
        std::string("_") + (std::is_same<typename ProblemType::FluxType, AnalyticalFluxType>::value ? "p" : "m");
    filename += Dune::XT::Common::to_string(momentOrder);
    filename += rhs_time_stepper_method == TimeStepperMethods::implicit_euler ? "_implicit" : "_explicit";

    timestepper.solve(t_end, dt, num_save_steps, false, true, visualize, filename, 1);
  } else {
    timestepper_op.solve(t_end, dt, num_save_steps, false, true, visualize, "entropy_implicit_trapezoidal");
  }
}
