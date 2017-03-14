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


#ifndef _INDEXWAVELETTREE_H
#define _INDEXWAVELETTREE_H

#include "Index.hpp"

#include <sdsl/wavelet_trees.hpp>


namespace cdat {

class IndexWaveletTree : public Index {

 public:
  static const uint INDEX_TYPE;

  IndexWaveletTree() : Index() {};
  IndexWaveletTree(size_type word_size, size_type shift);
  IndexWaveletTree(size_type word_size, size_type shift, size_type text_length,
                   size_type additional_text_length, sdsl::bit_vector *bit_vector,
                   sdsl::bit_vector::rank_1_type *rank1,
                   sdsl::bit_vector::select_1_type *select1,
                   Permutation *permutation, Alphabet *alphabet,
                   sdsl::wt_huff<sdsl::bit_vector, sdsl::rank_support_v<>,
                                 sdsl::select_support_scan<>, sdsl::select_support_scan<> > *text);

  ~IndexWaveletTree();

  int extract(const ulong from, const ulong to, std::string *text, ulong *length) const;
  int save_index(std::ostream& out) const;
  void load(std::istream& in);

  double get_size_in_mega_bytes() const;

 private:

  /*********   FUNCTIONS  ********/

  value_type extract_value(size_type const from, size_type const length) const;

  int count_full_words(std::string const &pattern, ulong length,
                       bool const locate, ulong *numocc, std::vector<ulong> *occ) const;

  bool check_word(size_type const position, size_type const length, const char *pattern) const;

  void create_text(char const *const filename);

  /**********  FIELDS  ***********/
  sdsl::wt_huff<sdsl::bit_vector, sdsl::rank_support_v<>,
                sdsl::select_support_scan<>, sdsl::select_support_scan<> > *m_text;

};

inline double IndexWaveletTree::get_size_in_mega_bytes() const {
    double result = Index::get_size_in_mega_bytes();
    result += sdsl::size_in_mega_bytes(*m_text);

    return result;
}

inline void IndexWaveletTree::create_text(char const *const filename) {
    m_text = new sdsl::wt_huff<sdsl::bit_vector, sdsl::rank_support_v<>,
                               sdsl::select_support_scan<>, sdsl::select_support_scan<> >();
    construct(*m_text, filename, 1);
}

}
#endif
