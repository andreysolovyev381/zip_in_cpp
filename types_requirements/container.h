//
// Created by Andrey Solovyev on 16/02/2023.
//

#pragma once

#include <type_traits>

#ifdef __cpp_concepts
#include <concepts>
#endif

#ifndef TYPE_REQUIREMENTS_CONTAINERS_H
#define TYPE_REQUIREMENTS_CONTAINERS_H

namespace culib::requirements {
  /**
   * @details
   * Requirements to restrict containers only to be used as a serie
   * */

#ifndef __cpp_concepts
  namespace details {
	template<typename Container, typename = void>
	struct MaybeContainer : std::false_type { };

	template<typename Container>
	struct MaybeContainer<Container,
						  std::void_t<
								  decltype(std::declval<Container>().begin()),
								  decltype(std::declval<Container>().end())
						  >
	> : std::true_type {};
  }//!namespace
  template<typename Container>
  inline constexpr bool is_container_v { details::MaybeContainer<Container>::value };

  template<typename Container>
  using IsContainer = std::enable_if_t<is_container_v<Container>, bool>;

  template <typename... Args>
  constexpr bool areAllContainers_v () {
	  bool result {true};
	  return ((result = result && is_container_v<Args>),...);
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
  inline constexpr bool is_container_v { IsContainer<C> ? true : false };

  template<typename... MaybeContainer>
  concept AreAllContainers = requires () {requires ((IsContainer<MaybeContainer>),...);};

#endif


}//!namespace

#endif //TYPE_REQUIREMENTS_CONTAINERS_H
