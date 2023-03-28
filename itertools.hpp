//
// Created by Andrey Solovyev on 23/02/2023.
//

#pragma once

#include <utility>
#include <tuple>
#include <type_traits>
#ifdef __cpp_concepts
#include <concepts>
#endif


#ifndef ITERTOOLS_H
#define ITERTOOLS_H

namespace itertools {

  namespace requirements {
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

  namespace base::utils {
	template<typename... T1, typename... T2, std::size_t... I>
	constexpr auto weakComparisonImpl(std::tuple<T1...> const& t1, std::tuple<T2...> const& t2, std::index_sequence<I...>) {
		bool result {false};
		((result = result || std::get<I>(t1) == std::get<I>(t2)) ,...);
		return result;
	}
	template<typename... T1, typename... T2>
	constexpr auto weakComparison(std::tuple<T1...> const& t1, std::tuple<T2...> const& t2) {
		static_assert(sizeof...(T1) == sizeof...(T2));
		return weakComparisonImpl(t1, t2, std::make_index_sequence<sizeof...(T1)>{});
	}
  }

  template<typename... Iter>
  class ZipIterator {
  private:

	  template<typename SomeIter>
	  using ValueTypeFor = typename SomeIter::value_type;

	  template<typename SomeIter>
	  using ReferenceTypeFor = typename SomeIter::reference;

	  template<typename SomeIter>
	  using DifferenceTypeFor = typename SomeIter::difference_type;

	  /**
 * @details
 * It is taken from O'Dwyer blog's post, see there some explanation.\n\n
 * https://quuxplusone.github.io/blog/2019/02/06/arrow-proxy/ \n\n
 * But basically, this is requried to have a proper \n
 * operator -> () \n
 * thing for this iterator.\n\n
 * */
	  template <typename Reference>
	  struct arrowProxy {
		  Reference R;
		  Reference *operator->() { return &R; } // non const
	  };

  public:
	  using iterator_type = ZipIterator<Iter...>;
	  using iterator_category = std::bidirectional_iterator_tag;
	  using value_type = typename std::tuple<ValueTypeFor<Iter>...>;
	  using reference = typename std::tuple<ReferenceTypeFor<Iter>...>;
	  using difference_type = typename std::tuple<DifferenceTypeFor<Iter>...>;
	  using pointer = arrowProxy<reference>;

	  ZipIterator() = delete;

	  explicit
	  ZipIterator(Iter... iter) :iterators (std::make_tuple(iter...))
	  {}
	  ZipIterator& operator++() {
		  std::apply([](Iter&... iter){ ((++iter), ...); }, iterators);
		  return *this;
	  }
	  ZipIterator& operator--() {
		  std::apply([](Iter&... iter){ ((--iter), ...); }, iterators);
		  return *this;
	  }
	  bool operator==(ZipIterator const& other) const {
		  return base::utils::weakComparison(this->iterators, other.iterators);
	  }
	  bool operator!=(ZipIterator const& other) const {
		  return !(*this == other);
	  }
	  value_type operator*() {
		  return makeRefs();
	  }
	  pointer operator->() { //it is supposed to survive just a drill-down
		  return pointer{makeRefs()};
	  }

  private:
	  std::tuple<Iter...> iterators;

	  template <std::size_t... I>
	  auto makeRefsImpl (std::index_sequence<I...>) {
		  return reference ({ std::get<I>(iterators).operator*()... });
	  }
	  auto makeRefs () {
		  return makeRefsImpl (std::make_index_sequence<sizeof...(Iter)>{});
	  }
  };

  template<typename... Container>
  class Zipper {
  private:

	  /**
	   * @brief
	   * This seemingly innocent iterator type selection serves
	   * as a guard - no Type without an iterator can pass this.
	   * */
	  template<typename Type>
	  using IteratorTypeSelect = std::conditional_t<
			  std::is_const_v<std::remove_reference_t<Type>>,
			  typename std::remove_reference_t<Type>::const_iterator,
			  typename std::remove_reference_t<Type>::iterator>;

  public:
	  using zip_type = ZipIterator<IteratorTypeSelect<Container>...>;

	  Zipper() = delete;

	  explicit
	  Zipper(Container&&... containers)
			  : begin_ (containers.begin()...)
			  , end_ (containers.end()...)
	  {}

	  zip_type begin() const { return begin_ ; }
	  zip_type end() const { return end_; }
  private:
	  zip_type begin_, end_;
  };



#ifndef __cpp_concepts
  template<typename... Containers, requirements::AreAllContainers<Containers...> = true>
#else
  template<requirements::AreAllContainers... Containers>
#endif
  auto zip(Containers&&... containers) {
	  return Zipper<Containers...> (std::forward<Containers>(containers)...);
  }
}//!namespace
#endif //ITERTOOLS_H
