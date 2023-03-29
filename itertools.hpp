//
// Created by Andrey Solovyev on 23/02/2023.
//

#pragma once

#include "types_requirements/container.h"
#include "types_requirements/iterators.h"

#include <utility>
#include <tuple>
#include <type_traits>
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
  template<typename... Iter>
#else
  template<culib::requirements::AreAllIterators ...Iter>
#endif
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

#ifndef __cpp_concepts
	  template<culib::requirements::AreAllIterators<Iter...> = true>
#endif
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
	  reference operator*() {
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

  template<typename... Inputs>
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
	  using zip_type = ZipIterator<IteratorTypeSelect<Inputs>...>;

	  Zipper() = delete;

	  explicit
	  Zipper(Inputs&&... inputs)
			  : begin_ (inputs.begin()...)
			  , end_ (inputs.end()...)
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
   * Just a ZipIterator will be provided from here.\n
   *
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
#endif //ITERTOOLS_H

/*
	  template <typename Input>
	  constexpr auto getIterator (Input&& input) {
		  using namespace std::string_literals;

		  if constexpr (culib::requirements::isIterator_v<Input>()) {
			  return std::forward<Input>(input);
		  }
		  else if constexpr (culib::requirements::isContainer_v<Input>()) {
			  if constexpr (std::is_const_v<std::remove_reference_t<Input>>){
				  return input.cbegin();
			  } else {
				  return input.begin();
			  }
		  }
		  else  {
			  throw std::invalid_argument("Can't produce an iterator from "s + typeid(Input).name());
		  }
	  }


	  auto getIterators () {

	  }

 */
