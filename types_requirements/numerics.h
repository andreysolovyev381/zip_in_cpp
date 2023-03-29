//
// Created by Andrey Solovyev on 29/01/2023.
//

#pragma once

#include <type_traits>
#ifdef __cpp_concepts
#include <concepts>
#endif

#ifndef TYPE_REQUIREMENTS_NUMERICS_H
#define TYPE_REQUIREMENTS_NUMERICS_H


namespace culib::requirements {

#ifndef __cpp_concepts

  template <typename Number>
  using IsFloatinPoint = std::enable_if_t<std::is_floating_point_v<Number>, bool>;
  template <typename Number>
  using IsNotFloatinPoint = std::enable_if_t<not std::is_floating_point_v<Number>, bool>;

  template <typename Number>
  using IsIntegral = std::enable_if_t<std::is_integral_v<Number>, bool>;

  template <typename Number>
  using IsNotIntegral = std::enable_if_t<not std::is_integral_v<Number>, bool>;


  template <typename I, typename = void>
  struct MaybeIncrementable : std::false_type {} ;
  template <typename I>
  struct MaybeIncrementable<I, std::void_t<
		  decltype(std::declval<I>()++),
		  decltype(++std::declval<I>())>> : std::true_type {};

  template <typename I>
  inline constexpr bool is_incrementable_v {MaybeIncrementable<I>::value} ;

  template <typename Iter>
  using IsIncrementable = std::enable_if_t<is_incrementable_v<Iter>, bool>;


  template <typename I, typename = void>
  struct MaybeDecrementable : std::false_type {} ;
  template <typename I>
  struct MaybeDecrementable<I, std::void_t<
		  decltype(std::declval<I>()--),
		  decltype(--std::declval<I>())>> : std::true_type {};

  template <typename I>
  inline constexpr bool is_decrementable_v {MaybeDecrementable<I>::value} ;


  template <typename Iter>
  using IsDecrementable = std::enable_if_t<is_decrementable_v<Iter>, bool>;


#else

  /**
   * @details
   * IsIntegral and IsFloating are the std:: \n
   * concepts, there is no need to define those\n
   * in C++20.\n
   * */


  template <typename I>
  concept Incrementable = requires(I i) {
	  ++i;
	  i++;
  };

  template <typename I>
  concept Decrementable = requires(I i) {
	  --i;
	  i--;
  };

#endif


}//!namespace
#endif //TYPE_REQUIREMENTS_NUMERICS_H

