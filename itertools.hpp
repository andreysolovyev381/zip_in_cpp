//
// Created by Andrey Solovyev on 23/02/2023.
//

#pragma once

#include "types_requirements/container.h"
#include "types_requirements/iterators.h"

#include <utility>
#include <tuple>
#include <stdexcept>
#include <string>

#ifndef ITERTOOLS_H
#define ITERTOOLS_H

namespace itertools {

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

#ifndef __cpp_concepts
  template<typename... Iterators>
#else
  template<culib::requirements::AreAllIterators ...Iterators>
#endif
  class ZipIterator {
  private:

	  template<typename SomeIter>
	  using ValueTypeFor = typename SomeIter::value_type;

	  template<typename SomeIter>
	  using ReferenceTypeFor = typename SomeIter::reference;

	  template<typename SomeIter>
	  using DifferenceTypeFor = typename SomeIter::difference_type;

	  template<typename SomeIter>
	  using IteratorCategoryFor = typename std::iterator_traits<SomeIter>::iterator_category;

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
	  using iterator_type = ZipIterator;
	  using iterator_category = std::common_type_t<IteratorCategoryFor<Iterators>...>;
	  using value_type = typename std::tuple<ValueTypeFor<Iterators>...>;
	  using reference = typename std::tuple<ReferenceTypeFor<Iterators>...>;
	  using pointer = arrowProxy<reference>;
	  using difference_type = std::common_type_t<DifferenceTypeFor<Iterators>...>;

	  ZipIterator() = delete;
	  ZipIterator(ZipIterator const&) = default;
	  ZipIterator(ZipIterator &&) = default;
	  ZipIterator& operator = (ZipIterator const&) = default;
	  ZipIterator& operator = (ZipIterator &&) = default;

#ifndef __cpp_concepts
	  template<culib::requirements::AreAllIterators<Iterators...> = true>
#endif
	  explicit
	  ZipIterator(Iterators&&... iters) :iterators (std::make_tuple(std::forward<Iterators>(iters)...))
	  {}
	  ZipIterator& operator++() & {
		  std::apply([](Iterators&... iter){ ((++iter), ...); }, iterators);
		  return *this;
	  }
	  ZipIterator operator++(int) {
		  ZipIterator tmp(*this);
		  operator++();
		  return tmp;
	  }
	  //todo: add requirement that it works for bidirectional and higher
	  ZipIterator& operator--() & {
		  std::apply([](Iterators&... iter){ ((--iter), ...); }, iterators);
		  return *this;
	  }
	  ZipIterator operator--(int) {
		  ZipIterator tmp(*this);
		  operator--();
		  return tmp;
	  }
	  bool operator==(ZipIterator const& other) const {
		  return base::utils::weakComparison(this->iterators, other.iterators);
	  }
	  bool operator!=(ZipIterator const& other) const { return !(*this == other); }
	  reference operator*() { return makeRefs(); }
	  reference operator*() const { return makeRefs(); }
	  //it is supposed to survive just a drill-down
	  pointer operator->() { return pointer{makeRefs()}; }

	  /**
	   * @details
	   * Having this for being able to decompose\n
	   * this type itself using structural\n
	   * bindings.\n
	   * See the tests for the details - \n
	   * the one, where zipping is for iterators.\n
	   *
	   * */
	  template<std::size_t Index>
	  decltype(auto) get() &  { return std::get<Index>(iterators).operator*(); }
	  template<std::size_t Index>
	  decltype(auto) get() && { return std::get<Index>(iterators).operator*(); }
	  template<std::size_t Index>
	  decltype(auto) get() const &  { return std::get<Index>(iterators).operator*(); }
	  template<std::size_t Index>
	  decltype(auto) get() const && { return std::get<Index>(iterators).operator*(); }

  private:

	  std::tuple<Iterators...> iterators;

	  template <std::size_t... I>
	  auto makeRefsImpl (std::index_sequence<I...>) {
		  return reference ({ std::get<I>(iterators).operator*()... });
	  }
	  auto makeRefs () {
		  return makeRefsImpl (std::make_index_sequence<sizeof...(Iterators)>{});
	  }
  };

#ifndef __cpp_concepts
  template<typename... Containers>
#else
  template<culib::requirements::AreAllContainers... Containers>
#endif
  class Zipper {
  private:

	  /**
	   * @details
	   * This seemingly innocent iterator type selection serves\n
	   * as a guard - no Type without an iterator can pass this.\n
	   * \n
	   * */
	  template<typename Input>
	  using IteratorTypeSelect = std::conditional_t<
			  std::is_const_v<std::remove_reference_t<Input>>,
			  typename std::remove_reference_t<Input>::const_iterator,
			  typename std::remove_reference_t<Input>::iterator>;

  public:
	  using zip_type = ZipIterator<IteratorTypeSelect<Containers>...>;

	  Zipper() = delete;

#ifndef __cpp_concepts
	  template<culib::requirements::AreAllContainers<Containers...> = true>
#endif
	  explicit
	  Zipper(Containers&&... inputs)
			  : begin_ (std::forward<Containers>(inputs).begin()...)
			  , end_ (std::forward<Containers>(inputs).end()...)
	  {}

	  zip_type begin() const { return begin_ ; }
	  zip_type end() const { return end_; }
  private:
	  zip_type begin_, end_;
  };

  /**
   * @details
   * Considering all the options, there should be at least a pair\n
   * of iterators to provide a valid range.\n\n
   * So all of the inputs should satisfy the requirements to have\n
   * both .begin() and end(), therefore each argument is, or  \n
   * should be, a container.\n
   *
   * */

#ifndef __cpp_concepts
  template<typename... Containers, culib::requirements::AreAllContainers<Containers...> = true>
#else
  template<culib::requirements::AreAllContainers... Containers>
#endif
  auto zip(Containers&&... containers) {
	  return Zipper<Containers...> (std::forward<Containers>(containers)...);
  }
  /**
   * @details
   * Just a ZipIterator will be provided from this func.\n
   * */

#ifndef __cpp_concepts
  template<typename... Iterators, culib::requirements::AreAllIterators<Iterators...> = true>
#else
  template<culib::requirements::AreAllIterators... Iterators>
#endif
  auto zip(Iterators&&... iterators) {
	  return ZipIterator<Iterators...> (std::forward<Iterators>(iterators)...);
  }

}//!namespace


/**
 * @details
 * Extending namespace std:: for user-defined structural binding.\n
 *
 * */

namespace std {
  template<typename... Iterators>
  struct tuple_size<itertools::ZipIterator<Iterators...>> :
  public std::integral_constant<std::size_t, sizeof...(Iterators)> {};

template<std::size_t Index, typename... Iterators>
struct tuple_element<Index, itertools::ZipIterator<Iterators...>> {
	using type = decltype(std::get<Index>(std::declval<itertools::ZipIterator<Iterators...>>().operator*() ));
};


}//!namespace


#endif //ITERTOOLS_H
