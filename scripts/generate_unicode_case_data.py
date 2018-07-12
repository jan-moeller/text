#!/usr/bin/env python

constants_header_form = '''\
// Warning! This file is autogenerated.
#ifndef BOOST_TEXT_DETAIL_CASE_CONSTANTS_HPP
#define BOOST_TEXT_DETAIL_CASE_CONSTANTS_HPP

#include <array>

#include <cstdint>


namespace boost {{ namespace text {{ namespace detail {{

    enum class case_condition : uint16_t {{
{0}
    }};

}}}}}}

#endif
'''

case_impl_file_form = '''\
// Warning! This file is autogenerated.
#include <boost/text/trie_map.hpp>

#include <boost/text/detail/case_mapping_data.hpp>


namespace boost {{ namespace text {{ namespace detail {{

    std::array<uint32_t, {1}> case_cps_array()
    {{
return {{{{
        {0}
    }}}};
    }}

    std::array<case_mapping_to, {3}> case_mapping_to_array()
    {{
return {{{{
        {2}
    }}}};
    }}

    namespace {{

    constexpr std::array<uint32_t, {5}> cased_cps()
    {{
return {{{{
        {4}
    }}}};
    }}

    constexpr std::array<uint32_t, {7}> case_ignorable_cps()
    {{
return {{{{
        {6}
    }}}};
    }}

    constexpr std::array<uint32_t, {9}> soft_dotted_cps()
    {{
return {{{{
        {8}
    }}}};
    }}

    constexpr std::array<case_mapping, {11}> to_lower_mappings()
    {{
return {{{{
        {10}
    }}}};
    }}

    constexpr std::array<case_mapping, {13}> to_title_mappings()
    {{
return {{{{
        {12}
    }}}};
    }}

    constexpr std::array<case_mapping, {15}> to_upper_mappings()
    {{
return {{{{
        {14}
    }}}};
    }}

    }}

    case_map_t make_to_lower_map()
    {{
        case_map_t retval;
        for (auto datum : to_lower_mappings()) {{
            retval[datum.from_] =
                case_elements{{datum.first_, datum.last_}};
        }}
        return retval;
    }}

    case_map_t make_to_title_map()
    {{
        case_map_t retval;
        for (auto datum : to_title_mappings()) {{
            retval[datum.from_] =
                case_elements{{datum.first_, datum.last_}};
        }}
        return retval;
    }}

    case_map_t make_to_upper_map()
    {{
        case_map_t retval;
        for (auto datum : to_upper_mappings()) {{
            retval[datum.from_] =
                case_elements{{datum.first_, datum.last_}};
        }}
        return retval;
    }}

    std::vector<uint32_t> make_soft_dotted_cps()
    {{
        return std::vector<uint32_t>(soft_dotted_cps().begin(), soft_dotted_cps().end());
    }}

    std::unordered_set<uint32_t> make_cased_cps()
    {{
        return std::unordered_set<uint32_t>(cased_cps().begin(), cased_cps().end());
    }}

    std::unordered_set<uint32_t> make_case_ignorable_cps()
    {{
        return std::unordered_set<uint32_t>(
            case_ignorable_cps().begin(), case_ignorable_cps().end());
    }}

}}}}}}
'''

def get_case_mappings(unicode_data, special_casing, prop_list, derived_core_props):
    to_lower = {}
    to_title = {}
    to_upper = {}

    all_tuples = set()
    conditions = set()

    def init_dict_elem(k, m):
        if k not in m or (len(m[k]) and m[k][0][2] == 'from_unicode_data'):
            m[k] = []

    lines = open(unicode_data, 'r').readlines()
    for line in lines:
        line = line[:-1]
        if not line.startswith('#') and len(line) != 0:
            comment_start = line.find('#')
            comment = ''
            if comment_start != -1:
                comment = line[comment_start + 1:].strip()
                line = line[:comment_start]
            fields = map(lambda x: x.strip(), line.split(';'))
            cp = fields[0]
            upper = fields[12]
            lower = fields[13]
            title = fields[14]
            if lower != '':
                init_dict_elem(cp, to_lower)
                to_lower[cp].append(([lower], [], 'from_unicode_data'))
                all_tuples.add((lower, None, None))
            if title != '':
                init_dict_elem(cp, to_title)
                to_title[cp].append(([title], [], 'from_unicode_data'))
                all_tuples.add((title, None, None))
            if upper != '':
                init_dict_elem(cp, to_upper)
                to_upper[cp].append(([upper], [], 'from_unicode_data'))
                all_tuples.add((upper, None, None))

    def to_tuple_2(l):
        if len(l) == 1:
            return (l[0], None)
        if len(l) == 2:
            return (l[0], l[1])
        return None

    def to_tuple_3(l):
        if len(l) == 1:
            return (l[0], None, None)
        if len(l) == 2:
            return (l[0], l[1], None)
        if len(l) == 3:
            return (l[0], l[1], l[2])
        return None

    def from_tuple(t):
        retval = []
        retval.append(t[0])
        if t[1] != None:
            retval.append(t[1])
        if 2 < len(t) and t[2] != None:
            retval.append(t[2])
        return retval

    lines = open(special_casing, 'r').readlines()
    for line in lines:
        line = line[:-1]
        if not line.startswith('#') and len(line) != 0:
            fields = map(lambda x: x.strip(), line.split(';'))
            cp = fields[0].strip()
            lower = fields[1].strip().split(' ')
            if lower == ['']:
                lower = []
            title = fields[2].strip().split(' ')
            if title == ['']:
                title = []
            upper = fields[3].strip().split(' ')
            if upper == ['']:
                upper = []
            conditions_ = []
            if 3 < len(fields) and '#' not in fields[4]:
                conditions_ = fields[4].strip().split(' ')
                for c in conditions_:
                    conditions.add(c)
            if len(lower):
                init_dict_elem(cp, to_lower)
                to_lower[cp].append((lower, conditions_, None))
                all_tuples.add(to_tuple_3(lower))
            if len(title):
                init_dict_elem(cp, to_title)
                to_title[cp].append((title, conditions_, None))
                all_tuples.add(to_tuple_3(title))
            if len(upper):
                init_dict_elem(cp, to_upper)
                to_upper[cp].append((upper, conditions_, None))
                all_tuples.add(to_tuple_3(upper))

    all_tuples = sorted(map(from_tuple, all_tuples))
    conditions = sorted(conditions)

    def subsequence(seq, subseq):
        i = 0
        while i < len(seq):
            if seq[i] == subseq[0]:
                break
            i += 1
        if i == len(seq):
            return (i, i)
        lo = i
        sub_i = 0
        while i < len(seq) and sub_i < len(subseq) and seq[i] == subseq[sub_i]:
            i += 1
            sub_i += 1
        if sub_i == len(subseq):
            return (lo, i)
        return (lo, lo)

    cps = []
    tuple_offsets = []
    tuple_offset = 0
    for i in range(len(all_tuples)):
        subseq = subsequence(cps, all_tuples[i])
        if subseq[0] != subseq[1]:
            tuple_offsets.append(subseq)
            continue
        cps += all_tuples[i]
        lo = tuple_offset
        tuple_offset += len(all_tuples[i])
        hi = tuple_offset
        tuple_offsets.append((lo, hi))
    def cp_indices(t):
        return tuple_offsets[all_tuples.index(from_tuple(t))]

    def to_cond_bitset(conds):
        retval = ' | '.join(map(lambda x: '(uint16_t)case_condition::' + x, conds))
        if retval == '':
            retval = '0'
        return retval

    all_mapped_tos = []

    def filter_dupes(l):
        retval = []
        for x in l:
            if x not in retval:
                retval.append(x)
        return retval

    def unconditioned_last(l):
        unconditioned = None
        retval = []
        for x in l:
            if x[1] == '0':
                unconditioned = x
            else:
                retval.append(x)
        if unconditioned != None:
            retval.append(unconditioned)
        return retval

    tmp = to_lower
    to_lower = []
    for k,v in tmp.items():
        lo = len(all_mapped_tos)
        mapped_tos = map(lambda x: (cp_indices(to_tuple_3(x[0])), to_cond_bitset(x[1])), v)
        mapped_tos = unconditioned_last(filter_dupes(mapped_tos))
        subseq = subsequence(all_mapped_tos, mapped_tos)
        if subseq[0] != subseq[1]:
            to_lower.append((k, subseq))
            continue
        all_mapped_tos += mapped_tos
        hi = len(all_mapped_tos)
        to_lower.append((k, (lo, hi)))

    tmp = to_title
    to_title = []
    for k,v in tmp.items():
        lo = len(all_mapped_tos)
        mapped_tos = map(lambda x: (cp_indices(to_tuple_3(x[0])), to_cond_bitset(x[1])), v)
        mapped_tos = unconditioned_last(filter_dupes(mapped_tos))
        subseq = subsequence(all_mapped_tos, mapped_tos)
        if subseq[0] != subseq[1]:
            to_title.append((k, subseq))
            continue
        all_mapped_tos += mapped_tos
        hi = len(all_mapped_tos)
        to_title.append((k, (lo, hi)))

    tmp = to_upper
    to_upper = []
    for k,v in tmp.items():
        lo = len(all_mapped_tos)
        mapped_tos = map(lambda x: (cp_indices(to_tuple_3(x[0])), to_cond_bitset(x[1])), v)
        mapped_tos = unconditioned_last(filter_dupes(mapped_tos))
        subseq = subsequence(all_mapped_tos, mapped_tos)
        if subseq[0] != subseq[1]:
            to_upper.append((k, subseq))
            continue
        all_mapped_tos += mapped_tos
        hi = len(all_mapped_tos)
        to_upper.append((k, (lo, hi)))

    soft_dotteds = []
    lines = open(prop_list, 'r').readlines()
    for line in lines:
        line = line[:-1]
        if not line.startswith('#') and len(line) != 0:
            fields = map(lambda x: x.strip(), line.split(';'))
            if fields[1].startswith('Soft_Dotted'):
                cps_ = fields[0].split('.')
                soft_dotteds.append(cps_[0])
                if 1 < len(cps_):
                    for i in range(int(cps_[0], 16) + 1, int(cps_[2], 16) + 1):
                         soft_dotteds.append(hex(i).upper()[2:])

    cased_cps = []
    cased_ignorable_cps = []
    lines = open(derived_core_props, 'r').readlines()
    for line in lines:
        line = line[:-1]
        if not line.startswith('#') and len(line) != 0:
            fields = map(lambda x: x.strip(), line.split(';'))
            if fields[1].startswith('Cased') or fields[1].startswith('Case_Ignorable'):
                cps_ = fields[0].split('.')
                if fields[1].startswith('Cased'):
                    cased_cps.append(cps_[0])
                else:
                    cased_ignorable_cps.append(cps_[0])
                if 1 < len(cps_):
                    for i in range(int(cps_[0], 16) + 1, int(cps_[2], 16) + 1):
                         if fields[1].startswith('Cased'):
                             cased_cps.append(hex(i).upper()[2:])
                         else:
                             cased_ignorable_cps.append(hex(i).upper()[2:])

    return to_lower, to_title, to_upper, cps, conditions, soft_dotteds, \
        cased_cps, cased_ignorable_cps, all_mapped_tos

to_lower, to_title, to_upper, cps, conditions, soft_dotteds, \
    cased_cps, cased_ignorable_cps, all_mapped_tos = \
    get_case_mappings('UnicodeData.txt', 'SpecialCasing.txt', \
                      'PropList.txt', 'DerivedCoreProperties.txt')

hpp_file = open('case_constants.hpp', 'w')
condition_enums = []
for i in range(len(conditions)):
    c = conditions[i]
    condition_enums.append('        {} = {},'.format(c, 1 << i))
hpp_file.write(constants_header_form.format('\n'.join(condition_enums)))

def make_case_mapping_to(t):
    return '{{ {}, {}, {} }}'.format(t[0][0], t[0][1], t[1])

def make_case_mapping(t):
    return '{{ 0x{}, {}, {} }}'.format(t[0], t[1][0], t[1][1])

cpp_file = open('case_mapping.cpp', 'w')
cpp_file.write(case_impl_file_form.format(
    ',\n        '.join(map(lambda x: '0x' + x, cps)),
    len(cps),
    ',\n        '.join(map(make_case_mapping_to, all_mapped_tos)),
    len(all_mapped_tos),
    ',\n        '.join(map(lambda x: '0x' + x, cased_cps)),
    len(cased_cps),
    ',\n        '.join(map(lambda x: '0x' + x, cased_ignorable_cps)),
    len(cased_ignorable_cps),
    ',\n        '.join(map(lambda x: '0x' + x, soft_dotteds)),
    len(soft_dotteds),
    ',\n        '.join(map(make_case_mapping, to_lower)),
    len(to_lower),
    ',\n        '.join(map(make_case_mapping, to_title)),
    len(to_title),
    ',\n        '.join(map(make_case_mapping, to_upper)),
    len(to_upper)))
