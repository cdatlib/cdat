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


#include "IndexPerm.hpp"

namespace cdat {

uint const IndexPerm::INDEX_TYPE = 256;

IndexPerm::IndexPerm(size_type word_size, size_type shift) :
    Index(word_size, shift) {}

IndexPerm::IndexPerm(size_type word_size, size_type shift, size_type text_length,
                     size_type additional_text_length, sdsl::bit_vector *bit_vector,
                     sdsl::bit_vector::rank_1_type *rank1,
                     sdsl::bit_vector::select_0_type *select0,
                     sdsl::bit_vector::select_1_type *select1,
                     Permutation *permutation, Alphabet *alphabet) :

    Index(word_size, shift, text_length, additional_text_length,
          bit_vector, rank1, select1, permutation, alphabet), m_select0(select0) {}

IndexPerm::~IndexPerm() {
}

void IndexPerm::create_bit_vector_support() {
    Index::create_bit_vector_support();
    m_select0 = new sdsl::bit_vector::select_0_type(m_bit_vector);
}

void IndexPerm::count_right_side_values(
    std::vector<std::pair<size_t, uint> > &right_side_values,
    std::string const &pattern) const {
    for (size_type i = m_word_size; i < pattern.length(); i += m_word_size) {
        right_side_values[(i / m_word_size) - 1] = std::make_pair(m_alphabet->get_word_value(
            pattern, i, (uint) std::min(i + m_word_size, pattern.length())),
                                                                  std::min(m_word_size, pattern.length() - i));
    }
}

int IndexPerm::count_full_words(std::string const &pattern, ulong length,
                                bool const locate, ulong *numocc,
                                std::vector<ulong> *occ) const {
    if (pattern.length() == m_word_size) {
        return count_exact_size_word(pattern, length, numocc, locate, occ);
    }

    size_t left_side_value = 0;
    size_type array_size = ((pattern.length() - m_word_size) / m_word_size) + 1;
    if ((pattern.length() - m_word_size) % m_word_size == 0)
        array_size--;

    std::vector<std::pair<size_t, uint> > right_side_values(array_size);
    count_right_side_values(right_side_values, pattern);

    size_type limit = m_shift;
    if (m_word_size + m_shift - 1 > pattern.length())
        limit = pattern.length() - m_word_size + 1;

    uint start = 0;
    while (start < limit) {
        size_t word_value = m_alphabet->get_word_value(pattern, start,
                                                       start + m_word_size) + 1;
        size_t position = m_select1->select(word_value) - word_value + 1;
        size_t next_position = m_select1->select(word_value + 1) - word_value;

        for (ulong i = position; i < next_position; ++i) {
            size_type right_end = start + m_word_size;
            size_type curr_word_position = m_permutation->pi(i);
            size_type word_index_position = curr_word_position * m_shift;

            if ((word_index_position < start) ||
                (m_text_length < (word_index_position + m_word_size) +
                    (pattern.length() - right_end)))
                continue;

            if (start != 0) {
                if (left_side_value != extract_value(word_index_position - start, start)) {
                    continue;
                }
            }

            if (start + m_word_size < pattern.length()) {
                if (!check_and_extract_value(word_index_position, array_size, right_side_values))
                    continue;
            }

            if (locate) {
                occ->push_back(word_index_position - start);
            }

            (*numocc)++;
        }

        left_side_value *= m_alphabet->size();
        left_side_value += m_alphabet->get_char_value(pattern[start]);
        start++;
        right_side_values[0].first %=
            m_alphabet->pow_wsize(right_side_values[0].second - 1);

        if (pattern.length() > start + m_word_size)
            right_side_values[0].second--;

    }

    return 0;
}

bool IndexPerm::check_and_extract_value(size_type const position, size_type const array_size,
                                        std::vector<std::pair<size_t, uint> > const &right_side_values) const {
    size_type start = position + m_word_size;
    size_type perm_position = start / m_shift;
    if (perm_position > m_permutation->get_size())
        perm_position = m_permutation->get_size() - 1;
    size_type word_position = perm_position * m_shift;

    value_type word_value = m_permutation->revpi(perm_position) + 1;
    auto temp = m_select0->select(word_value);
    word_value = m_rank1->rank(temp) - 1;

    for (uint i = 0; i < array_size; ++i) {
        auto to = start + right_side_values[i].second;
        ulong result = 0;
        while (start < to) {
            value_type tmp_word_value = word_value;
            size_t word_length = m_word_size;
            if (word_position < start) {
                tmp_word_value %= m_alphabet->pow_wsize(m_word_size - (start - word_position));
                word_length -= start - word_position;
            }
            if (to < word_position + m_word_size) {
                tmp_word_value /= m_alphabet->pow_wsize(word_position + m_word_size - to);
                word_length -= word_position + m_word_size - to;
            }

            start += word_length;
            result *= m_alphabet->pow_wsize(word_length);
            result += tmp_word_value;

            if (start < to) {
                perm_position = start / m_shift;
                if (perm_position > m_permutation->get_size())
                    perm_position = m_permutation->get_size() - 1;
                word_position = perm_position * m_shift;

                word_value = m_permutation->revpi(perm_position) + 1;
                temp = m_select0->select(word_value);
                word_value = m_rank1->rank(temp) - 1;

            }
        }

        if (result != right_side_values[i].first) {
            return false;
        }

        start = to;
        if (i < array_size - 1 && word_position + m_word_size <= start) {
            perm_position = start / m_shift;
            if (perm_position > m_permutation->get_size())
                perm_position = m_permutation->get_size() - 1;
            word_position = perm_position * m_shift;

            word_value = m_permutation->revpi(perm_position) + 1;
            temp = m_select0->select(word_value);
            word_value = m_rank1->rank(temp) - 1;
        }
    }

    return true;
}

IndexPerm::value_type IndexPerm::extract_value(size_type const from, size_type const length) const{
    value_type result = 0;
    size_type to = from + length;
    size_type start = from;

    while (start < to) {
        size_type position = (start / m_shift);
        if (position >= m_permutation->get_size()) {
            position = m_permutation->get_size() - 1;

        }
        size_type word_position = position * m_shift;

        value_type word_value = m_permutation->revpi(position) + 1;
        auto temp = m_select0->select(word_value);
        word_value = m_rank1->rank(temp) - 1;

        size_type word_length = m_word_size;

        if (word_position < start) {
            word_value %= m_alphabet->pow_wsize(m_word_size - (start - word_position));
            word_length -= start - word_position;
        }
        if (to < word_position + m_word_size) {
            word_value /= m_alphabet->pow_wsize(word_position + m_word_size - to);
            word_length -= word_position + m_word_size - to;
        }

        start += word_length;
        result *= m_alphabet->pow_wsize(word_length);
        result += word_value;
    }

    return result;
}

std::string IndexPerm::get_word(size_type const start, size_type const length) const {
    ulong position = start / m_shift;
    if (position >= m_permutation->get_size())
        position = m_permutation->get_size() - 1;

    size_t word_value = m_permutation->revpi(position) + 1;
    word_value = m_rank1->rank(m_select0->select(word_value));
    return m_alphabet->get_word_from_value(word_value - 1, m_word_size).
        substr(start - (position * m_shift), length);
}

int IndexPerm::extract(ulong const from, ulong const to, std::string *text, ulong *length) const {
    ulong _to = std::min(to, m_text_length);
    if (_to < from) {
        *length = 0;
        return -1;
    }

    *length = to - from;
    text->reserve(*length);

    ulong current_length = 0;

    while (current_length < *length) {
        std::string word = get_word(from + current_length,
                                    std::min(m_word_size, *length - current_length));
        current_length += word.length();
        *text += word;
    }

    return 0;
}

int IndexPerm::save_index(std::ostream &out) const {
    out.write((char *) &IndexPerm::INDEX_TYPE, sizeof(uint));
    Index::save_index(out);
    m_select0->serialize(out);

    return 0;
}

void IndexPerm::load(std::istream& in) {
    uint index_type;
    in.read((char *) &index_type, sizeof(uint));

     if (index_type != IndexPerm::INDEX_TYPE) {
        std::cerr << "Wrong index type!\n";
    }

    Index::load(in);
    m_permutation = RevPermutation::load(in);

    m_select0 = new sdsl::bit_vector::select_0_type;
    m_select0->load(in, m_bit_vector);
}

}
