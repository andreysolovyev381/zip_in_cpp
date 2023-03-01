//
// Created by Andrey Solovyev on 23/02/2023.
//

#pragma once

#include <utility>
#include <tuple>
#include <type_traits>

#ifndef ITERTOOLS_H
#define ITERTOOLS_H

namespace itertools {

  namespace base::utils {
	template<typename... T1, typename... T2, std::size_t... I>
	constexpr auto weakComparisonImpl(const std::tuple<T1...>& t1, const std::tuple<T2...>& t2, std::index_sequence<I...>) {
		bool result {false};
		((result = result || std::get<I>(t1) == std::get<I>(t2)) ,...);
		return result;
	}
	template<typename... T1, typename... T2>
	constexpr auto weakComparison(const std::tuple<T1...>& t1, const std::tuple<T2...>& t2) {
		static_assert(sizeof...(T1) == sizeof...(T2));
		return weakComparisonImpl(t1, t2, std::make_index_sequence<sizeof...(T1)>{});
	}
  }

  template<typename... Iter>
  class ZipIterator {
  private:
	  template<typename SomeIter>
	  using AccessTypeFor = typename SomeIter::reference;

  public:
	  using value_type = typename std::tuple<AccessTypeFor<Iter>...>;

	  ZipIterator() = delete;

	  explicit
	  ZipIterator(Iter... iter) :iterators (std::make_tuple(iter...)) {}

	  ZipIterator& operator++() {
		  std::apply([](Iter&... iter){ ((++iter), ...); }, iterators);
		  return *this;
	  }

	  bool operator==(ZipIterator const& other) const {
		  return base::utils::weakComparison(this->iterators, other.iterators);
	  }
	  bool operator!=(ZipIterator const& other) const { return !(*this == other); }
	  value_type operator*() { return makeRefs(); }

  private:
	  std::tuple<Iter...> iterators;

	  template <std::size_t... I>
	  auto makeRefsImpl (std::index_sequence<I...>) {
		  return std::tuple<AccessTypeFor<Iter>...> ({ std::get<I>(iterators).operator*()... });
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
			  typename std::decay_t<Type>::const_iterator,
			  typename std::decay_t<Type>::iterator>;

  public:
	  using zip_type = ZipIterator<IteratorTypeSelect<Container>...>;

	  Zipper() = delete;

	  explicit
	  Zipper(Container&&... containers)
			  : begin_ (containers.begin()...)
			  , end_ (containers.end()...)
	  {}

	  zip_type begin() { return begin_ ; }
	  zip_type end() { return end_; }
  private:
	  zip_type begin_, end_;
  };


  template<typename... Container>
  auto zip(Container&&... container) {
	  return Zipper<Container...> (std::forward<Container>(container)...);
  }
}//!namespace
#endif //ITERTOOLS_H
