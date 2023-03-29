//
// Created by Andrey Solovyev on 16/02/2023.
//

#pragma once

#include <type_traits>

#ifdef __cpp_concepts
#include <concepts>
#endif

#ifndef TYPE_REQUIREMENTS_CONTAINER_H
#define TYPE_REQUIREMENTS_CONTAINER_H

namespace culib::requirements {
  /**
   * @details
   * Requirements to restrict containers only to be used as a serie
   * */

#ifndef __cpp_concepts
  template<typename Container, typename = void>
  struct MaybeContainer : std::false_type {};

  template<typename Container>
  struct MaybeContainer<Container,
					 std::void_t<
							 decltype(std::declval<Container>().begin()),
							 decltype(std::declval<Container>().end())
					 >
  > : std::true_type {};

  template<typename Container>
  constexpr bool isContainer_v() { return MaybeContainer<Container>::value; }

  template<typename Container>
  using IsContainer = std::enable_if_t<isContainer_v<Container>(), bool>;

  template <typename... Args>
  constexpr bool areAllContainers_v () {
	  bool result {true};
	  return ((result = result && isContainer_v<Args>()),...);
  }

  template<typename... MaybeContainer>
  using AreAllContainers = std::enable_if_t<areAllContainers_v<MaybeContainer...>(), bool>;

  template<typename... MaybeContainer>
  using NotAreAllContainers = std::enable_if_t<!areAllContainers_v<MaybeContainer...>(), bool>;

  template<typename Element>
  using IsDefaultConstructible = std::enable_if_t<std::is_default_constructible_v<Element>, bool>;

#else

  template<typename C>
  concept IsContainer = requires (C c) {
	  c.begin();
	  c.end();
  };

  template<typename C>
  concept IsNotContainer = !IsContainer<C>;

  template <IsContainer C>
  constexpr bool isContainer_v () {return true;}

  template <IsNotContainer C>
  constexpr bool isContainer_v () {return false;}

  template<typename... MaybeContainer>
  concept AreAllContainers = requires () {requires ((IsContainer<MaybeContainer>),...);};


//  template<typename Element>
//  using IsDefaultConstructible = std::enable_if_t<std::is_default_constructible_v<Element>, bool>;

#endif


}//!namespace

#endif //TYPE_REQUIREMENTS_CONTAINER_H
