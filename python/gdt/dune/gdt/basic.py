from dune.xt.common.config import config

if config.HAVE_K3D:
    from dune.gdt import visualize_function

# flake8: noqa

from dune.gdt._discretefunction_bochner import DiscreteBochnerFunction
from dune.gdt._discretefunction_discretefunction import DiscreteFunction
from dune.gdt._functionals_vector_based import VectorFunctional
from dune.gdt._interpolations_boundary import boundary_interpolation
from dune.gdt._interpolations_default import default_interpolation
from dune.gdt._interpolations_oswald import oswald_interpolation
from dune.gdt._local_bilinear_forms_coupling_intersection_integrals import (
    LocalCouplingIntersectionIntegralBilinearForm,
)
from dune.gdt._local_bilinear_forms_element_integrals import (
    LocalElementIntegralBilinearForm,
)
from dune.gdt._local_bilinear_forms_intersection_integrals import (
    LocalIntersectionIntegralBilinearForm,
)
from dune.gdt._local_bilinear_forms_restricted_coupling_intersection_integrals import (
    LocalCouplingIntersectionRestrictedIntegralBilinearForm,
)
from dune.gdt._local_bilinear_forms_restricted_intersection_integrals import (
    LocalIntersectionRestrictedIntegralBilinearForm,
)
from dune.gdt._local_bilinear_forms_vectorized_element_integrals import (
    VectorizedLocalElementIntegralBilinearForm,
)
from dune.gdt._local_functionals_element_integrals import LocalElementIntegralFunctional
from dune.gdt._local_functionals_intersection_integrals import (
    LocalIntersectionIntegralFunctional,
)
from dune.gdt._local_functionals_restricted_intersection_integrals import (
    LocalIntersectionRestrictedIntegralFunctional,
)
from dune.gdt._local_functionals_vectorized_element_integrals import (
    VectorizedLocalElementIntegralFunctional,
)
from dune.gdt._local_integrands_element_product import LocalElementProductIntegrand
from dune.gdt._local_integrands_intersection_product import (
    LocalIntersectionNormalComponentProductIntegrand,
    LocalIntersectionProductIntegrand,
)
from dune.gdt._local_integrands_ipdg_boundary_penalty import (
    LocalIPDGBoundaryPenaltyIntegrand,
)
from dune.gdt._local_integrands_ipdg_inner_penalty import LocalIPDGInnerPenaltyIntegrand
from dune.gdt._local_integrands_jump_boundary import LocalBoundaryJumpIntegrand
from dune.gdt._local_integrands_jump_inner import LocalInnerJumpIntegrand
from dune.gdt._local_integrands_laplace import LocalLaplaceIntegrand
from dune.gdt._local_integrands_laplace_ipdg_dirichlet_coupling import (
    LocalLaplaceIPDGDirichletCouplingIntegrand,
)
from dune.gdt._local_integrands_laplace_ipdg_inner_coupling import (
    LocalLaplaceIPDGInnerCouplingIntegrand,
)
from dune.gdt._local_integrands_linear_advection import LocalLinearAdvectionIntegrand
from dune.gdt._local_integrands_linear_advection_upwind_dirichlet_coupling import (
    LocalLinearAdvectionUpwindDirichletCouplingIntegrand,
)
from dune.gdt._local_integrands_linear_advection_upwind_inner_coupling import (
    LocalLinearAdvectionUpwindInnerCouplingIntegrand,
)
from dune.gdt._local_operators_coupling_intersection_indicator import (
    LocalCouplingIntersectionBilinearFormIndicatorOperator,
)
from dune.gdt._local_operators_element_indicator import (
    LocalElementBilinearFormIndicatorOperator,
)
from dune.gdt._local_operators_intersection_indicator import (
    LocalIntersectionBilinearFormIndicatorOperator,
)
from dune.gdt._operators_bilinear_form import BilinearForm
from dune.gdt._operators_laplace_ipdg_flux_reconstruction import (
    LaplaceIpdgFluxReconstructionOperator,
)
from dune.gdt._operators_matrix_based_factory import (
    IstlSparseMatrixOperator,
    MatrixOperator,
)
from dune.gdt._operators_operator import Operator
from dune.gdt._prolongations import prolong, prolong_space_time
from dune.gdt._spaces_bochner import BochnerSpace
from dune.gdt._spaces_h1_continuous_lagrange import ContinuousLagrangeSpace
from dune.gdt._spaces_hdiv_raviart_thomas import RaviartThomasSpace
from dune.gdt._spaces_l2_discontinuous_lagrange import DiscontinuousLagrangeSpace
from dune.gdt._spaces_l2_finite_volume import FiniteVolumeSpace
from dune.gdt._spaces_skeleton_finite_volume import FiniteVolumeSkeletonSpace
from dune.gdt._tools_adaptation_helper import AdaptationHelper
from dune.gdt._tools_dirichlet_constraints import DirichletConstraints
from dune.gdt._tools_grid_quality_estimates import (
    estimate_combined_inverse_trace_inequality_constant,
    estimate_element_to_intersection_equivalence_constant,
    estimate_inverse_inequality_constant,
)
from dune.gdt._tools_sparsity_pattern import (
    make_element_and_intersection_sparsity_pattern,
    make_element_sparsity_pattern,
    make_intersection_sparsity_pattern,
)
