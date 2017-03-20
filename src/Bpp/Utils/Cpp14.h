// File: Cpp14.h
// Authors:
//   Francois Gindraud (2017)
// Created: 23/02/2017

/*
Copyright or © or Copr. Bio++ Development Team, (November 16, 2004)

This software is a computer program whose purpose is to provide classes
for phylogenetic data analysis.

This software is governed by the CeCILL  license under French law and
abiding by the rules of distribution of free software.  You can  use,
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info".

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability.

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or
data to be ensured and,  more generally, to use and operate it in the
same conditions as regards security.

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/

#ifndef _UTILS_CPP14_H_
#define _UTILS_CPP14_H_

/**
 * @file Cpp14.h
 * Compatibility header that adds some utilities missing in C++11.
 * Utilities:
 * - make_unique
 * - IndexSequence / MakeIndexSequence / IndexSequenceFor
 *
 * They are placed in a Cpp14 namespace for now to prevent name clashes with other stuff.
 * FIXME it should be removed when moving to c++14.
 * TODO better doxygen below
 */

#include <memory>  // unique_ptr
#include <utility> // index_sequence

namespace bpp
{
  namespace Cpp14
  {
    /**
     * @brief Implements std::make_unique if not available.
     */
#if (__cplusplus >= 201402L)
    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
      return std::make_unique<T>(std::forward<Args>(args)...);
    }
#else
    template <typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args)
    {
      return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
#endif

    /**
     * @brief Implement std::index_sequence and std::index_sequence_for.
     */
#if (__cplusplus >= 201402L)
    template <std::size_t... Is>
    using IndexSequence = std::index_sequence<Is...>;
    template <std::size_t N>
    using MakeIndexSequence = std::make_index_sequence<N>;
    template <typename... Types>
    using IndexSequenceFor = std::index_sequence_for<Types...>;
#else
    template <std::size_t... Is>
    struct IndexSequence
    {
      static constexpr std::size_t size(void) { return sizeof...(Is); }
    };

    namespace detail
    {
      // Seq1<0, ..., N-1> + Seq2<0, ..., M-1> => Seq<0, ..., N+M-1>
      template <typename Seq1, typename Seq2>
      struct ConcatImpl;
      template <std::size_t... Is1, std::size_t... Is2>
      struct ConcatImpl<IndexSequence<Is1...>, IndexSequence<Is2...>>
      {
        using Type = IndexSequence<Is1..., (sizeof...(Is1) + Is2)...>;
      };
      template <typename Seq1, typename Seq2>
      using Concat = typename ConcatImpl<Seq1, Seq2>::Type;

      // N => Seq<0, ..., N-1>
      template <std::size_t N>
      struct MakeIndexSequenceImpl;
      template <std::size_t N>
      using MakeIndexSequence = typename MakeIndexSequenceImpl<N>::Type;

      // Recursive implementation in log(n)
      template <std::size_t N>
      struct MakeIndexSequenceImpl
      {
        using Type = Concat<MakeIndexSequence<N / 2>, MakeIndexSequence<N - N / 2>>;
      };
      template <>
      struct MakeIndexSequenceImpl<0>
      {
        using Type = IndexSequence<>;
      };
      template <>
      struct MakeIndexSequenceImpl<1>
      {
        using Type = IndexSequence<0>;
      };
    }
    template <std::size_t N>
    using MakeIndexSequence = detail::MakeIndexSequence<N>;
    template <typename... Types>
    using IndexSequenceFor = MakeIndexSequence<sizeof...(Types)>;

#endif
  } // end namespace Cpp14
} // end namespace bpp
#endif // _UTILS_CPP14_H_
