#ifndef BOOST_TEXT_DETAIL_NORMALIZATION_DATA_HPP
#define BOOST_TEXT_DETAIL_NORMALIZATION_DATA_HPP

#include <boost/text/utf8.hpp>
#include <boost/text/detail/lzw.hpp>

#include <boost/container/small_vector.hpp>

#include <algorithm>
#include <array>
#include <unordered_map>
#include <unordered_set>


namespace boost { namespace text { namespace detail {

    struct code_points
    {
        using iterator = uint32_t *;
        using const_iterator = uint32_t const *;

        code_points(const_iterator first, const_iterator last) :
            size_(last - first)
        {
            r_ = range{first, last};
        }

        code_points(uint32_t cp) : size_(-1) { cps_ = storage_type{{cp}}; }

        code_points(uint32_t cp0, uint32_t cp1) : size_(-2)
        {
            cps_ = storage_type{{cp0, cp1}};
        }

        code_points(uint32_t cp0, uint32_t cp1, uint32_t cp2) : size_(-3)
        {
            cps_ = storage_type{{cp0, cp1, cp2}};
        }

        int size() const noexcept { return size_ < 0 ? -size_ : size_; }

        const_iterator begin() const noexcept
        {
            return 0 <= size_ ? r_.first_ : &*cps_.begin();
        }
        const_iterator end() const noexcept
        {
            return 0 <= size_ ? r_.last_ : &*cps_.begin() + -size_;
        }

        iterator begin() noexcept
        {
            assert(size_ < 0);
            return &*cps_.begin();
        }
        iterator end() noexcept { return begin() + -size_; }

    private:
        using storage_type = std::array<uint32_t, 3>;
        struct range
        {
            const_iterator first_;
            const_iterator last_;
        };
        union
        {
            range r_;
            storage_type cps_;
        };
        int size_;
    };

    /** The possible results returned by the single code point quick check
        functions.  A result of maybe indicates that a quick check is not
        possible and a full check must be performed. */
    enum class quick_check : uint8_t { yes, no, maybe };

    struct cp_range
    {
        uint16_t first_;
        uint16_t last_;
    };

    BOOST_TEXT_DECL std::array<uint32_t, 3404>
    make_all_canonical_decompositions();
    BOOST_TEXT_DECL std::array<uint32_t, 8974>
    make_all_compatible_decompositions();

    inline uint32_t const * all_canonical_decompositions_ptr()
    {
        static auto const retval = make_all_canonical_decompositions();
        return retval.data();
    }

    inline uint32_t const * all_compatible_decompositions_ptr()
    {
        static auto const retval = make_all_compatible_decompositions();
        return retval.data();
    }

    BOOST_TEXT_DECL std::unordered_map<uint64_t, uint32_t>
    make_composition_map();

    inline std::unordered_map<uint64_t, uint32_t> const & composition_map()
    {
        static auto const retval = make_composition_map();
        return retval;
    }

    struct cp_props
    {
        cp_range canonical_decomposition_;
        cp_range compatible_decomposition_;
        uint8_t ccc_;
        uint8_t nfd_quick_check_ : 4;
        uint8_t nfkd_quick_check_ : 4;
        uint8_t nfc_quick_check_ : 4;
        uint8_t nfkc_quick_check_ : 4;
    };

    static_assert(sizeof(cp_props) == 12, "");

    BOOST_TEXT_DECL std::unordered_map<uint32_t, cp_props> make_cp_props_map();

    inline std::unordered_map<uint32_t, cp_props> const & cp_props_map()
    {
        static auto const retval = make_cp_props_map();
        return retval;
    }

    inline constexpr bool hangul_syllable(uint32_t cp) noexcept
    {
        return 0xAC00 <= cp && cp <= 0xD7A3;
    }

    // Hangul decomposition as described in Unicode 11.0 Section 3.12.
    inline code_points decompose_hangul_syllable(uint32_t cp) noexcept
    {
        BOOST_ASSERT(hangul_syllable(cp));

        uint32_t const SBase = 0xAC00;
        uint32_t const LBase = 0x1100;
        uint32_t const VBase = 0x1161;
        uint32_t const TBase = 0x11A7;
        // uint32_t const LCount = 19;
        uint32_t const VCount = 21;
        uint32_t const TCount = 28;
        uint32_t const NCount = VCount * TCount; // 588
        // uint32_t const SCount = LCount * NCount; // 11172

        auto const SIndex = cp - SBase;

        auto const LIndex = SIndex / NCount;
        auto const VIndex = (SIndex % NCount) / TCount;
        auto const TIndex = SIndex % TCount;
        auto const LPart = LBase + LIndex;
        auto const VPart = VBase + VIndex;
        if (TIndex == 0) {
            return code_points(LPart, VPart);
        } else {
            auto const TPart = TBase + TIndex;
            return code_points(LPart, VPart, TPart);
        }
    }

    inline std::unordered_multimap<uint32_t, uint32_t> const &
    compositions_whose_decompositions_start_with_cp_map()
    {
        static std::unordered_multimap<uint32_t, uint32_t> retval;
        if (retval.empty()) {
            auto const & map = detail::cp_props_map();
            for (auto const & pair : map) {
                auto const decomp_it =
                    detail::all_canonical_decompositions_ptr() +
                    pair.second.canonical_decomposition_.first_;
                auto const decomp_end =
                    detail::all_canonical_decompositions_ptr() +
                    pair.second.canonical_decomposition_.last_;
                if (decomp_it != decomp_end) {
                    retval.insert(
                        std::pair<uint32_t, uint32_t>(*decomp_it, pair.first));
                }
            }

            for (uint32_t cp = 0xAC00, end = 0xD7A3 + 1; cp < end; ++cp) {
                auto const decomp = detail::decompose_hangul_syllable(cp);
                retval.insert(
                    std::pair<uint32_t, uint32_t>(*decomp.begin(), cp));
            }
        }
        return retval;
    }

    inline std::unordered_set<uint32_t> const &
    appears_at_noninitial_position_of_decomp_set()
    {
        static std::unordered_set<uint32_t> retval;
        if (retval.empty()) {
            auto const & map = detail::cp_props_map();
            for (auto const & pair : map) {
                auto const decomp_it =
                    detail::all_canonical_decompositions_ptr() +
                    pair.second.canonical_decomposition_.first_;
                auto const decomp_end =
                    detail::all_canonical_decompositions_ptr() +
                    pair.second.canonical_decomposition_.last_;
                if (2 <= decomp_end - decomp_it)
                    retval.insert(*std::next(decomp_it));
            }

            for (uint32_t cp = 0xAC00, end = 0xD7A3 + 1; cp < end; ++cp) {
                auto const decomp = detail::decompose_hangul_syllable(cp);
                if (2 <= decomp.size())
                    retval.insert(*std::next(decomp.begin()));
            }
        }
        return retval;
    }

    inline constexpr uint64_t key(uint64_t cp0, uint32_t cp1) noexcept
    {
        return (cp0 << 32) | cp1;
    }

    inline code_points canonical_decompose(uint32_t cp) noexcept
    {
        if (detail::hangul_syllable(cp))
            return detail::decompose_hangul_syllable(cp);

        auto const lo = 0xc0;
        auto const hi = 0x2fa1d;
        if (cp < lo || (0x30fe < cp && cp < 0xf900) ||
            (0x1d1c0 < cp && cp < 0x2f800) || hi < cp) {
            return code_points(cp);
        }

        auto const & map = detail::cp_props_map();
        auto const it = map.find(cp);
        if (it == map.end() || it->second.canonical_decomposition_.last_ ==
                                   it->second.canonical_decomposition_.first_) {
            return code_points(cp);
        }

        return code_points(
            detail::all_canonical_decompositions_ptr() +
                it->second.canonical_decomposition_.first_,
            detail::all_canonical_decompositions_ptr() +
                it->second.canonical_decomposition_.last_);
    }

    inline code_points compatible_decompose(uint32_t cp) noexcept
    {
        if (detail::hangul_syllable(cp))
            return detail::decompose_hangul_syllable(cp);

        auto const lo = 0xc0;
        auto const hi = 0x2fa1d;
        if (cp < lo || (0x33ff < cp && cp < 0xa69c) ||
            (0xab5f < cp && cp < 0xf900) || (0x1f251 < cp && cp < 0x2f800) ||
            hi < cp) {
            return code_points(cp);
        }

        auto const & map = detail::cp_props_map();
        auto const it = map.find(cp);
        if (it == map.end() ||
            it->second.compatible_decomposition_.last_ ==
                it->second.compatible_decomposition_.first_) {
            return code_points(cp);
        }

        return code_points(
            detail::all_compatible_decompositions_ptr() +
                it->second.compatible_decomposition_.first_,
            detail::all_compatible_decompositions_ptr() +
                it->second.compatible_decomposition_.last_);
    }

    inline uint32_t
    compose_hangul(uint32_t cp0, uint32_t cp1, uint32_t cp2 = 0) noexcept
    {
        uint32_t const SBase = 0xAC00;
        uint32_t const LBase = 0x1100;
        uint32_t const VBase = 0x1161;
        uint32_t const TBase = 0x11A7;
        // uint32_t const LCount = 19;
        uint32_t const VCount = 21;
        uint32_t const TCount = 28;
        uint32_t const NCount = VCount * TCount; // 588
        // uint32_t const SCount = LCount * NCount; // 11172

        auto const LIndex = cp0 - LBase;
        auto const VIndex = cp1 - VBase;

        auto const LVIndex = LIndex * NCount + VIndex * TCount;
        if (cp2) {
            auto const TIndex = cp2 - TBase;
            return SBase + LVIndex + TIndex;
        } else {
            return SBase + LVIndex;
        }
    }

    inline uint32_t compose_unblocked(uint32_t cp0, uint32_t cp1) noexcept
    {
        if (cp0 < 0x3c || cp1 < 0x300 || 0x115b9 < cp0 || 0x115af < cp1)
            return 0;
        auto const & map = detail::composition_map();
        auto const it = map.find(detail::key(cp0, cp1));
        if (it == map.end())
            return 0;
        return it->second;
    }

    struct ccc_rle_element
    {
        uint32_t cp_;
        uint8_t ccc_;
        uint8_t run_length_;
    };

    constexpr ccc_rle_element ccc_rle_table[] = {
        {0x300, 230, 21},   {0x315, 232, 1},   {0x316, 220, 4},
        {0x31a, 232, 1},    {0x31b, 216, 1},   {0x31c, 220, 5},
        {0x321, 202, 2},    {0x323, 220, 4},   {0x327, 202, 2},
        {0x329, 220, 11},   {0x334, 1, 5},     {0x339, 220, 4},
        {0x33d, 230, 8},    {0x345, 240, 1},   {0x346, 230, 1},
        {0x347, 220, 3},    {0x34a, 230, 3},   {0x34d, 220, 2},
        {0x350, 230, 3},    {0x353, 220, 4},   {0x357, 230, 1},
        {0x358, 232, 1},    {0x359, 220, 2},   {0x35b, 230, 1},
        {0x35c, 233, 1},    {0x35d, 234, 2},   {0x35f, 233, 1},
        {0x360, 234, 2},    {0x362, 233, 1},   {0x363, 230, 13},
        {0x483, 230, 5},    {0x591, 220, 1},   {0x592, 230, 4},
        {0x596, 220, 1},    {0x597, 230, 3},   {0x59a, 222, 1},
        {0x59b, 220, 1},    {0x59c, 230, 6},   {0x5a2, 220, 6},
        {0x5a8, 230, 2},    {0x5aa, 220, 1},   {0x5ab, 230, 2},
        {0x5ad, 222, 1},    {0x5ae, 228, 1},   {0x5af, 230, 1},
        {0x5b0, 10, 1},     {0x5b1, 11, 1},    {0x5b2, 12, 1},
        {0x5b3, 13, 1},     {0x5b4, 14, 1},    {0x5b5, 15, 1},
        {0x5b6, 16, 1},     {0x5b7, 17, 1},    {0x5b8, 18, 1},
        {0x5b9, 19, 2},     {0x5bb, 20, 1},    {0x5bc, 21, 1},
        {0x5bd, 22, 1},     {0x5bf, 23, 1},    {0x5c1, 24, 1},
        {0x5c2, 25, 1},     {0x5c4, 230, 1},   {0x5c5, 220, 1},
        {0x5c7, 18, 1},     {0x610, 230, 8},   {0x618, 30, 1},
        {0x619, 31, 1},     {0x61a, 32, 1},    {0x64b, 27, 1},
        {0x64c, 28, 1},     {0x64d, 29, 1},    {0x64e, 30, 1},
        {0x64f, 31, 1},     {0x650, 32, 1},    {0x651, 33, 1},
        {0x652, 34, 1},     {0x653, 230, 2},   {0x655, 220, 2},
        {0x657, 230, 5},    {0x65c, 220, 1},   {0x65d, 230, 2},
        {0x65f, 220, 1},    {0x670, 35, 1},    {0x6d6, 230, 7},
        {0x6df, 230, 4},    {0x6e3, 220, 1},   {0x6e4, 230, 1},
        {0x6e7, 230, 2},    {0x6ea, 220, 1},   {0x6eb, 230, 2},
        {0x6ed, 220, 1},    {0x711, 36, 1},    {0x730, 230, 1},
        {0x731, 220, 1},    {0x732, 230, 2},   {0x734, 220, 1},
        {0x735, 230, 2},    {0x737, 220, 3},   {0x73a, 230, 1},
        {0x73b, 220, 2},    {0x73d, 230, 1},   {0x73e, 220, 1},
        {0x73f, 230, 3},    {0x742, 220, 1},   {0x743, 230, 1},
        {0x744, 220, 1},    {0x745, 230, 1},   {0x746, 220, 1},
        {0x747, 230, 1},    {0x748, 220, 1},   {0x749, 230, 2},
        {0x7eb, 230, 7},    {0x7f2, 220, 1},   {0x7f3, 230, 1},
        {0x7fd, 220, 1},    {0x816, 230, 4},   {0x81b, 230, 9},
        {0x825, 230, 3},    {0x829, 230, 5},   {0x859, 220, 3},
        {0x8d3, 220, 1},    {0x8d4, 230, 14},  {0x8e3, 220, 1},
        {0x8e4, 230, 2},    {0x8e6, 220, 1},   {0x8e7, 230, 2},
        {0x8e9, 220, 1},    {0x8ea, 230, 3},   {0x8ed, 220, 3},
        {0x8f0, 27, 1},     {0x8f1, 28, 1},    {0x8f2, 29, 1},
        {0x8f3, 230, 3},    {0x8f6, 220, 1},   {0x8f7, 230, 2},
        {0x8f9, 220, 2},    {0x8fb, 230, 5},   {0x93c, 7, 1},
        {0x94d, 9, 1},      {0x951, 230, 1},   {0x952, 220, 1},
        {0x953, 230, 2},    {0x9bc, 7, 1},     {0x9cd, 9, 1},
        {0x9fe, 230, 1},    {0xa3c, 7, 1},     {0xa4d, 9, 1},
        {0xabc, 7, 1},      {0xacd, 9, 1},     {0xb3c, 7, 1},
        {0xb4d, 9, 1},      {0xbcd, 9, 1},     {0xc4d, 9, 1},
        {0xc55, 84, 1},     {0xc56, 91, 1},    {0xcbc, 7, 1},
        {0xccd, 9, 1},      {0xd3b, 9, 2},     {0xd4d, 9, 1},
        {0xdca, 9, 1},      {0xe38, 103, 2},   {0xe3a, 9, 1},
        {0xe48, 107, 4},    {0xeb8, 118, 2},   {0xec8, 122, 4},
        {0xf18, 220, 2},    {0xf35, 220, 1},   {0xf37, 220, 1},
        {0xf39, 216, 1},    {0xf71, 129, 1},   {0xf72, 130, 1},
        {0xf74, 132, 1},    {0xf7a, 130, 4},   {0xf80, 130, 1},
        {0xf82, 230, 2},    {0xf84, 9, 1},     {0xf86, 230, 2},
        {0xfc6, 220, 1},    {0x1037, 7, 1},    {0x1039, 9, 2},
        {0x108d, 220, 1},   {0x135d, 230, 3},  {0x1714, 9, 1},
        {0x1734, 9, 1},     {0x17d2, 9, 1},    {0x17dd, 230, 1},
        {0x18a9, 228, 1},   {0x1939, 222, 1},  {0x193a, 230, 1},
        {0x193b, 220, 1},   {0x1a17, 230, 1},  {0x1a18, 220, 1},
        {0x1a60, 9, 1},     {0x1a75, 230, 8},  {0x1a7f, 220, 1},
        {0x1ab0, 230, 5},   {0x1ab5, 220, 6},  {0x1abb, 230, 2},
        {0x1abd, 220, 1},   {0x1b34, 7, 1},    {0x1b44, 9, 1},
        {0x1b6b, 230, 1},   {0x1b6c, 220, 1},  {0x1b6d, 230, 7},
        {0x1baa, 9, 2},     {0x1be6, 7, 1},    {0x1bf2, 9, 2},
        {0x1c37, 7, 1},     {0x1cd0, 230, 3},  {0x1cd4, 1, 1},
        {0x1cd5, 220, 5},   {0x1cda, 230, 2},  {0x1cdc, 220, 4},
        {0x1ce0, 230, 1},   {0x1ce2, 1, 7},    {0x1ced, 220, 1},
        {0x1cf4, 230, 1},   {0x1cf8, 230, 2},  {0x1dc0, 230, 2},
        {0x1dc2, 220, 1},   {0x1dc3, 230, 7},  {0x1dca, 220, 1},
        {0x1dcb, 230, 2},   {0x1dcd, 234, 1},  {0x1dce, 214, 1},
        {0x1dcf, 220, 1},   {0x1dd0, 202, 1},  {0x1dd1, 230, 37},
        {0x1df6, 232, 1},   {0x1df7, 228, 2},  {0x1df9, 220, 1},
        {0x1dfb, 230, 1},   {0x1dfc, 233, 1},  {0x1dfd, 220, 1},
        {0x1dfe, 230, 1},   {0x1dff, 220, 1},  {0x20d0, 230, 2},
        {0x20d2, 1, 2},     {0x20d4, 230, 4},  {0x20d8, 1, 3},
        {0x20db, 230, 2},   {0x20e1, 230, 1},  {0x20e5, 1, 2},
        {0x20e7, 230, 1},   {0x20e8, 220, 1},  {0x20e9, 230, 1},
        {0x20ea, 1, 2},     {0x20ec, 220, 4},  {0x20f0, 230, 1},
        {0x2cef, 230, 3},   {0x2d7f, 9, 1},    {0x2de0, 230, 32},
        {0x302a, 218, 1},   {0x302b, 228, 1},  {0x302c, 232, 1},
        {0x302d, 222, 1},   {0x302e, 224, 2},  {0x3099, 8, 2},
        {0xa66f, 230, 1},   {0xa674, 230, 10}, {0xa69e, 230, 2},
        {0xa6f0, 230, 2},   {0xa806, 9, 1},    {0xa8c4, 9, 1},
        {0xa8e0, 230, 18},  {0xa92b, 220, 3},  {0xa953, 9, 1},
        {0xa9b3, 7, 1},     {0xa9c0, 9, 1},    {0xaab0, 230, 1},
        {0xaab2, 230, 2},   {0xaab4, 220, 1},  {0xaab7, 230, 2},
        {0xaabe, 230, 2},   {0xaac1, 230, 1},  {0xaaf6, 9, 1},
        {0xabed, 9, 1},     {0xfb1e, 26, 1},   {0xfe20, 230, 7},
        {0xfe27, 220, 7},   {0xfe2e, 230, 2},  {0x101fd, 220, 1},
        {0x102e0, 220, 1},  {0x10376, 230, 5}, {0x10a0d, 220, 1},
        {0x10a0f, 230, 1},  {0x10a38, 230, 1}, {0x10a39, 1, 1},
        {0x10a3a, 220, 1},  {0x10a3f, 9, 1},   {0x10ae5, 230, 1},
        {0x10ae6, 220, 1},  {0x10d24, 230, 4}, {0x10f46, 220, 2},
        {0x10f48, 230, 3},  {0x10f4b, 220, 1}, {0x10f4c, 230, 1},
        {0x10f4d, 220, 4},  {0x11046, 9, 1},   {0x1107f, 9, 1},
        {0x110b9, 9, 1},    {0x110ba, 7, 1},   {0x11100, 230, 3},
        {0x11133, 9, 2},    {0x11173, 7, 1},   {0x111c0, 9, 1},
        {0x111ca, 7, 1},    {0x11235, 9, 1},   {0x11236, 7, 1},
        {0x112e9, 7, 1},    {0x112ea, 9, 1},   {0x1133b, 7, 2},
        {0x1134d, 9, 1},    {0x11366, 230, 7}, {0x11370, 230, 5},
        {0x11442, 9, 1},    {0x11446, 7, 1},   {0x1145e, 230, 1},
        {0x114c2, 9, 1},    {0x114c3, 7, 1},   {0x115bf, 9, 1},
        {0x115c0, 7, 1},    {0x1163f, 9, 1},   {0x116b6, 9, 1},
        {0x116b7, 7, 1},    {0x1172b, 9, 1},   {0x11839, 9, 1},
        {0x1183a, 7, 1},    {0x11a34, 9, 1},   {0x11a47, 9, 1},
        {0x11a99, 9, 1},    {0x11c3f, 9, 1},   {0x11d42, 7, 1},
        {0x11d44, 9, 2},    {0x11d97, 9, 1},   {0x16af0, 1, 5},
        {0x16b30, 230, 7},  {0x1bc9e, 1, 1},   {0x1d165, 216, 2},
        {0x1d167, 1, 3},    {0x1d16d, 226, 1}, {0x1d16e, 216, 5},
        {0x1d17b, 220, 8},  {0x1d185, 230, 5}, {0x1d18a, 220, 2},
        {0x1d1aa, 230, 4},  {0x1d242, 230, 3}, {0x1e000, 230, 7},
        {0x1e008, 230, 17}, {0x1e01b, 230, 7}, {0x1e023, 230, 2},
        {0x1e026, 230, 5},  {0x1e8d0, 220, 7}, {0x1e944, 230, 6},
        {0x1e94a, 7, 1},
    };
    constexpr uint8_t ccc_low_byte_masks[] = {
        0x7f, 0x83, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfd, 0xfd, 0xff, 0xff, 0xfa,
        0xff, 0xbf, 0,    0,    0x5d, 0,    0,    0,    0xff, 0xa9, 0x3b, 0xff,
        0xff, 0xff, 0xff, 0,    0,    0xff, 0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0xef, 0xff, 0,    0,    0xbf, 0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0xff, 0,    0xe6, 0xfb, 0xff,
        0xed, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0x1e, 0,    0,    0x2f,
        0,    0,    0xfd, 0xe0, 0x76, 0,    0,    0,    0,    0,    0,    0xff,
        0,    0,    0x24, 0,    0x4f, 0xff, 0xfb, 0xff, 0x7f, 0xdf, 0xff, 0xbf,
        0x2b, 0x3b, 0,    0xff, 0,    0x3f, 0xd7, 0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0xf0,
        0x30, 0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0x9e, 0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0xff, 0x42, 0,    0,    0,    0,
        0,    0,    0,    0,    0,    0,    0,    0,    0,    0x3f, 0,    0,
        0,    0,    0,    0,    0,    0xd0};

    inline int ccc(uint32_t cp)
    {
        if (cp < 0x300 || 0x1e94a < cp)
            return 0;
        auto const low_byte = cp & 0xff;
        if ((ccc_low_byte_masks[(cp - 0x300) >> 8] & low_byte) != low_byte)
            return 0;
        auto const last = std::end(ccc_rle_table);
        auto const it = std::lower_bound(
            std::begin(ccc_rle_table),
            last,
            ccc_rle_element{cp},
            [](ccc_rle_element lhs, ccc_rle_element rhs) {
                // lhs.high <= rhs.low
                return lhs.cp_ + lhs.run_length_ <= rhs.cp_;
            });
        if (it == last || cp < it->cp_ || it->cp_ + it->run_length_ <= cp)
            return 0;
        return it->ccc_;
    }

    /** Returns yes, no, or maybe if the given code point indicates that the
        sequence in which it is found is normalized NFD. */
    inline quick_check quick_check_nfd_code_point(uint32_t cp) noexcept
    {
        auto const & map = detail::cp_props_map();
        auto const it = map.find(cp);
        if (it == map.end())
            return quick_check::yes;
        return quick_check(it->second.nfd_quick_check_);
    }

    /** Returns yes, no, or maybe if the given code point indicates that the
        sequence in which it is found is normalized NFKD. */
    inline quick_check quick_check_nfkd_code_point(uint32_t cp) noexcept
    {
        auto const & map = detail::cp_props_map();
        auto const it = map.find(cp);
        if (it == map.end())
            return quick_check::yes;
        return quick_check(it->second.nfkd_quick_check_);
    }

    /** Returns yes, no, or maybe if the given code point indicates that the
        sequence in which it is found is normalized NFC. */
    inline quick_check quick_check_nfc_code_point(uint32_t cp) noexcept
    {
        auto const & map = detail::cp_props_map();
        auto const it = map.find(cp);
        if (it == map.end())
            return quick_check::yes;
        return quick_check(it->second.nfc_quick_check_);
    }

    /** Returns yes, no, or maybe if the given code point indicates that the
        sequence in which it is found is normalized NFKC. */
    inline quick_check quick_check_nfkc_code_point(uint32_t cp) noexcept
    {
        auto const & map = detail::cp_props_map();
        auto const it = map.find(cp);
        if (it == map.end())
            return quick_check::yes;
        return quick_check(it->second.nfkc_quick_check_);
    }

    /** Returns true iff cp is a stable code point under FCC normalization
        (meaning that it is ccc=0 and Quick_Check_NFC=Yes).

        \see https://www.unicode.org/reports/tr15/#Stable_Code_Points */
    inline bool stable_fcc_code_point(uint32_t cp) noexcept
    {
        auto const & map = detail::cp_props_map();
        auto const it = map.find(cp);
        if (it == map.end())
            return true;
        return it->second.ccc_ == 0 &&
               quick_check(it->second.nfc_quick_check_) == quick_check::yes;
    }

    struct lzw_to_cp_props_iter
    {
        using value_type = std::pair<uint32_t, cp_props>;
        using difference_type = int;
        using pointer = value_type *;
        using reference = value_type &;
        using iterator_category = std::output_iterator_tag;
        using buffer_t = container::small_vector<unsigned char, 256>;

        lzw_to_cp_props_iter(
            std::unordered_map<uint32_t, cp_props> & map, buffer_t & buf) :
            map_(&map),
            buf_(&buf)
        {}

        template<typename BidiRange>
        lzw_to_cp_props_iter & operator=(BidiRange const & r)
        {
            buf_->insert(buf_->end(), r.rbegin(), r.rend());
            auto const element_bytes = 3 + 2 + 2 + 2 + 2 + 1 + 1 + 1;
            auto it = buf_->begin();
            for (auto end = buf_->end() - buf_->size() % element_bytes;
                 it != end;
                 it += element_bytes) {
                unsigned char * ptr = &*it;

                uint32_t const cp = bytes_to_cp(ptr);
                ptr += 3;

                cp_props props;
                props.canonical_decomposition_.first_ = bytes_to_uint16_t(ptr);
                ptr += 2;
                props.canonical_decomposition_.last_ = bytes_to_uint16_t(ptr);
                ptr += 2;
                props.compatible_decomposition_.first_ = bytes_to_uint16_t(ptr);
                ptr += 2;
                props.compatible_decomposition_.last_ = bytes_to_uint16_t(ptr);
                ptr += 2;
                props.ccc_ = *ptr++;
                unsigned char c = *ptr++;
                props.nfd_quick_check_ = (c >> 4) & 0xf;
                props.nfkd_quick_check_ = (c >> 0) & 0xf;
                c = *ptr++;
                props.nfc_quick_check_ = (c >> 4) & 0xf;
                props.nfkc_quick_check_ = (c >> 0) & 0xf;

                (*map_)[cp] = props;
            }
            buf_->erase(buf_->begin(), it);
            return *this;
        }
        lzw_to_cp_props_iter & operator*() { return *this; }
        lzw_to_cp_props_iter & operator++() { return *this; }
        lzw_to_cp_props_iter & operator++(int) { return *this; }

    private:
        std::unordered_map<uint32_t, cp_props> * map_;
        buffer_t * buf_;
    };

}}}

#endif
