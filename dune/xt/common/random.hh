// This file is part of the dune-xt project:
//   https://zivgitlab.uni-muenster.de/ag-ohlberger/dune-community/dune-xt
// Copyright 2009-2021 dune-xt developers and contributors. All rights reserved.
// License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
//      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
//          with "runtime exception" (http://www.dune-project.org/license.html)
// Authors:
//   Felix Schindler (2014 - 2017)
//   René Fritze     (2012 - 2020)
//   Tobias Leibner  (2014, 2020)

#ifndef DUNE_XT_COMMON_RANDOM_HH
#define DUNE_XT_COMMON_RANDOM_HH

#include <random>
#include <limits>
#include <complex>

#include <dune/xt/common/exceptions.hh>
#include <dune/xt/common/numeric_cast.hh>
#include <dune/xt/common/vector.hh>

namespace Dune::XT::Common {

//! Helper class to abstract away selecting an integer or real valued distribution
template <typename T, bool = std::is_integral<T>::value>
struct UniformDistributionSelector
{};

template <typename T>
struct UniformDistributionSelector<T, true>
{
  using type = std::uniform_int_distribution<T>;
};
template <typename T>
struct UniformDistributionSelector<T, false>
{
  using type = std::uniform_real_distribution<T>;
};

/** RandomNumberGenerator adapter
 * \template T type of generated numbers
 * \template EngineImp randomization algorithm/engine implementation
 * \template DistributionImp class with an EngineImp accepting call operator that returns the actual RNGs
 * \ref DefaultRNG for default choice of distribution, engine and init args
 **/
template <class T, class DistributionImp, class EngineImp = std::mt19937>
struct RNG
{
  using DistributionType = DistributionImp;
  using EngineType = EngineImp;
  EngineType generator;
  DistributionType distribution;
  RNG(EngineType g, DistributionType d)
    : generator(g)
    , distribution(d)
  {
  }

  inline T operator()()
  {
    return distribution(generator);
  }
};

template <class T, class DistributionImp, class EngineImp>
struct RNG<std::complex<T>, DistributionImp, EngineImp>
{
  using DistributionType = DistributionImp;
  using EngineType = EngineImp;
  EngineType generator;
  DistributionType distribution;
  RNG(EngineType g, DistributionType d)
    : generator(g)
    , distribution(d)
  {
  }

  inline std::complex<T> operator()()
  {
    return std::complex<T>(distribution(generator), distribution(generator));
  }
};

namespace {
const std::string alphanums("abcdefghijklmnopqrstuvwxyz"
                            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "1234567890");
} // namespace

//! RNG that represents strings of given length
class RandomStrings : public RNG<std::string, std::uniform_int_distribution<int>, std::mt19937>
{
  using BaseType = RNG<std::string, std::uniform_int_distribution<int>, std::mt19937>;
  const size_t length;

public:
  explicit RandomStrings(size_t l, std::random_device::result_type seed = std::random_device()())
    : BaseType(std::mt19937(seed), std::uniform_int_distribution<int>(0, numeric_cast<int>(alphanums.size() - 1)))
    , length(l)
  {
  }

  inline std::string operator()()
  {
    std::string ret(length, '\0');
    std::generate(std::begin(ret), std::end(ret), [this]() { return alphanums[distribution(generator)]; });
    return ret;
  }
};

//! defaultrng with choice of uniform distribution and stl's default random engine based on T and its numeric_limits
template <class T, bool = is_vector<T>::value>
class DefaultRNG : public RNG<T, typename UniformDistributionSelector<T>::type, std::default_random_engine>
{
  using BaseType = RNG<T, typename UniformDistributionSelector<T>::type, std::default_random_engine>;

public:
  explicit DefaultRNG(T min = std::numeric_limits<T>::min(),
                      T max = std::numeric_limits<T>::max(),
                      std::random_device::result_type seed = std::random_device()())
    : BaseType(std::default_random_engine(seed), typename UniformDistributionSelector<T>::type(min, max))
  {
  }
};

template <class T>
class DefaultRNG<std::complex<T>, false>
  : public RNG<std::complex<T>, typename UniformDistributionSelector<T>::type, std::default_random_engine>
{
  using BaseType = RNG<std::complex<T>, typename UniformDistributionSelector<T>::type, std::default_random_engine>;

public:
  DefaultRNG(T min = std::numeric_limits<T>::min(),
             T max = std::numeric_limits<T>::max(),
             std::random_device::result_type seed = std::random_device()())
    : BaseType(std::default_random_engine(seed), typename UniformDistributionSelector<T>::type(min, max))
  {
  }
};

template <class VectorType>
class DefaultRNG<VectorType, true>
{
  using T = typename VectorAbstraction<VectorType>::S;
  using RngType = DefaultRNG<T>;

public:
  DefaultRNG(VectorType min_vec = VectorAbstraction<VectorType>::create(VectorAbstraction<VectorType>::has_static_size
                                                                            ? VectorAbstraction<VectorType>::static_size
                                                                            : 1,
                                                                        std::numeric_limits<T>::min()),
             VectorType max_vec = VectorAbstraction<VectorType>::create(VectorAbstraction<VectorType>::has_static_size
                                                                            ? VectorAbstraction<VectorType>::static_size
                                                                            : 1,
                                                                        std::numeric_limits<T>::max()),
             std::random_device::result_type seed = std::random_device()())
  {
    if (min_vec.size() != max_vec.size())
      DUNE_THROW(Exceptions::shapes_do_not_match,
                 "min_vec.size() = " << min_vec.size() << "\n   max_vec.size() = " << max_vec.size());
    for (size_t idx = 0; idx < min_vec.size(); ++idx)
      rngs_.emplace_back(min_vec[idx], max_vec[idx], seed);
  }

  inline VectorType operator()()
  {
    VectorType result_vec = VectorAbstraction<VectorType>::create(rngs_.size());
    std::size_t idx = 0;
    for (auto&& res : result_vec) {
      res = rngs_.at(idx)();
      ++idx;
    }
    return result_vec;
  }

private:
  std::vector<RngType> rngs_;
};

constexpr size_t default_rng_string_length{12};

template <>
class DefaultRNG<std::string> : public RandomStrings
{
public:
  DefaultRNG(size_t ilength = default_rng_string_length, std::random_device::result_type seed = std::random_device()())
    : RandomStrings(ilength, seed)
  {
  }
};

} // namespace Dune::XT::Common

#endif // DUNE_XT_COMMON_RANDOM_HH
