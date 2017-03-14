/* cdat - compressed direct-address tables

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see http://www.gnu.org/licenses/ .
*/


#ifndef _INDEXPERM_H
#define _INDEXPERM_H

#include "Index.hpp"
#include "RevPermutation.hpp"

namespace cdat {

class IndexPerm : public Index {

 public:
  static const uint INDEX_TYPE;

  IndexPerm() : Index() {};
  IndexPerm(size_type word_size, size_type shift);
  IndexPerm(size_type word_size, size_type shift, size_type text_length,
            size_type additional_text_length, sdsl::bit_vector *bit_vector,
            sdsl::bit_vector::rank_1_type *rank1,
            sdsl::bit_vector::select_0_type *select0,
            sdsl::bit_vector::select_1_type *select1,
            Permutation *permutation, Alphabet *alphabet);
  ~IndexPerm();

  int extract(const ulong from, const ulong to, std::string *text, ulong *length) const;
  int save_index(std::ostream& out) const;
  void load(std::istream& in);

 private:

  value_type extract_value(size_type const from, size_type const length) const;

  int count_full_words(std::string const &pattern, ulong length,
                       bool const locate, ulong *numocc, std::vector<ulong> *occ) const;

  void count_right_side_values(
      std::vector<std::pair<size_t, uint> > &right_side_values,
      std::string const &pattern) const;

  bool check_and_extract_value(size_type  const position, size_type  const array_size,
                               std::vector<std::pair<size_t, uint> > const &right_side_values) const;

  std::string get_word(size_type const start, size_type const length) const;

  Permutation *create_permutation(size_type const size) const;
  virtual void create_bit_vector_support();

  sdsl::bit_vector::select_0_type *m_select0;
};

inline Permutation *IndexPerm::create_permutation(size_type const size) const {
    return new RevPermutation(size);
}

}
#endif
