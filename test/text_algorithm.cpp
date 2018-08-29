#include <boost/text/text.hpp>
#include <boost/text/word_break.hpp>
#include <boost/text/sentence_break.hpp>
#include <boost/text/line_break.hpp>
#include <boost/text/paragraph_break.hpp>
#include <boost/text/bidirectional.hpp>

#include <gtest/gtest.h>


using namespace boost::text;
using namespace boost::text::detail;


TEST(text_algorithm, word_break)
{
    // ÷ 0061 × 005F × 0061 ÷ 002E ÷ 003A ÷ 0061 ÷	
    // ÷ [0.2] LATIN SMALL LETTER A (ALetter) × [13.1] LOW LINE (ExtendNumLet) × [13.2] LATIN SMALL LETTER A (ALetter) ÷ [999.0] FULL STOP (MidNumLet) ÷ [999.0] COLON (MidLetter) ÷ [999.0] LATIN SMALL LETTER A (ALetter) ÷ [0.3]
    text cps("\x61\x5f\x61\x2e\x3a\x61");

    {
        EXPECT_EQ(std::distance(cps.cbegin(), boost::text::prev_word_break(cps, std::next(cps.begin(), 2))), 0);
        EXPECT_EQ(std::distance(cps.cbegin(), boost::text::next_word_break(cps, std::next(cps.begin(), 5))), 6);
    }
    {
        auto const range = boost::text::word(cps, std::next(cps.begin(), 1));
        EXPECT_EQ(std::distance(cps.cbegin(), range.begin()), 0);
        EXPECT_EQ(std::distance(cps.cbegin(), range.end()), 3);
    }

    {
        auto const all_words = boost::text::words(cps);

        std::array<std::pair<int, int>, 4> const word_bounds = {
            {{0, 3}, {3, 4}, {4, 5}, {5, 6}}};

        int i = 0;
        for (auto word : all_words) {
            EXPECT_EQ(
                std::distance(cps.cbegin(), word.begin()), word_bounds[i].first)
                << "i=" << i;
            EXPECT_EQ(
                std::distance(cps.cbegin(), word.end()), word_bounds[i].second)
                << "i=" << i;
            ++i;
        }
        EXPECT_EQ(i, (int)word_bounds.size());

        auto const all_words_reversed = boost::text::reversed_words(cps);
        i = word_bounds.size();
        for (auto word : all_words_reversed) {
            --i;
            EXPECT_EQ(
                std::distance(cps.cbegin(), word.begin()), word_bounds[i].first)
                << "i=" << i;
            EXPECT_EQ(
                std::distance(cps.cbegin(), word.end()), word_bounds[i].second)
                << "i=" << i;
        }
        EXPECT_EQ(i, 0);
    }
}

TEST(text_algorithm, sentence_break)
{
    // ÷ 0065 × 0074 × 0063 × 002E ÷ 5B83 ÷
    // ÷ [0.2] LATIN SMALL LETTER E (Lower) × [998.0] LATIN SMALL LETTER T (Lower) × [998.0] LATIN SMALL LETTER C (Lower) × [998.0] FULL STOP (ATerm) ÷ [11.0] CJK UNIFIED IDEOGRAPH-5B83 (OLetter) ÷ [0.3]
    text cps("\x65\x74\x63\x2e\xe5\xae\x83");

    {
        EXPECT_EQ(std::distance(cps.cbegin(), boost::text::prev_sentence_break(cps, std::next(cps.begin(), 2))), 0);
        EXPECT_EQ(std::distance(cps.cbegin(), boost::text::next_sentence_break(cps, std::next(cps.begin(), 0))), 4);
    }
    {
        auto const range =
            boost::text::sentence(cps, std::next(cps.begin(), 1));
        EXPECT_EQ(std::distance(cps.cbegin(), range.begin()), 0);
        EXPECT_EQ(std::distance(cps.cbegin(), range.end()), 4);
    }

    {
        auto const all_sentences = boost::text::sentences(cps);

        std::array<std::pair<int, int>, 2> const sentence_bounds = {
            {{0, 4}, {4, 5}}};

        int i = 0;
        for (auto sentence : all_sentences) {
            EXPECT_EQ(
                std::distance(cps.cbegin(), sentence.begin()),
                sentence_bounds[i].first)
                << "i=" << i;
            EXPECT_EQ(
                std::distance(cps.cbegin(), sentence.end()),
                sentence_bounds[i].second)
                << "i=" << i;
            ++i;
        }
        EXPECT_EQ(i, (int)sentence_bounds.size());

        auto const all_sentences_reversed = boost::text::reversed_sentences(cps);
        i = sentence_bounds.size();
        for (auto sentence : all_sentences_reversed) {
            --i;
            EXPECT_EQ(
                std::distance(cps.cbegin(), sentence.begin()),
                sentence_bounds[i].first)
                << "i=" << i;
            EXPECT_EQ(
                std::distance(cps.cbegin(), sentence.end()),
                sentence_bounds[i].second)
                << "i=" << i;
        }
        EXPECT_EQ(i, 0);
    }
}

TEST(break_apis, line_break)
{
    // × 200B × 0020 ÷ 0030 ÷
    // × [0.3] ZERO WIDTH SPACE (ZW) × [7.01] SPACE (SP) ÷ [8.0] DIGIT ZERO (NU) ÷ [0.3]
    text cps("\xe2\x80\x8b\x20\x30");

    auto const begin = cps.begin();

    {
        auto const prev =
            boost::text::prev_hard_line_break(cps, std::next(cps.begin(), 1));
        EXPECT_EQ(std::distance(cps.cbegin(), prev), 0);

        auto const next =
            boost::text::next_hard_line_break(cps, std::next(cps.begin(), 0));
        EXPECT_EQ(std::distance(cps.cbegin(), next), 3);
    }
    {
        auto const prev = boost::text::prev_possible_line_break(
            cps, std::next(cps.begin(), 1));
        EXPECT_EQ(std::distance(cps.cbegin(), prev.iter), 0);
        EXPECT_TRUE(prev.hard_break);

        auto const next = boost::text::next_possible_line_break(
            cps, std::next(cps.begin(), 0));
        EXPECT_EQ(std::distance(cps.cbegin(), next.iter), 2);
        EXPECT_FALSE(next.hard_break);
    }

    {
        auto const range = boost::text::line(cps, begin);
        EXPECT_EQ(std::distance(cps.cbegin(), range.begin()), 0);
        EXPECT_EQ(std::distance(cps.cbegin(), range.end()), 3);
    }
    {
        auto const range = boost::text::possible_line(cps, begin);
        EXPECT_EQ(std::distance(cps.cbegin(), range.begin()), 0);
        EXPECT_EQ(std::distance(cps.cbegin(), range.end()), 2);
        EXPECT_FALSE(range.hard_break());
    }

    {
        auto const all_lines = boost::text::lines(cps);

        std::array<std::pair<int, int>, 1> const line_bounds = {{{0, 3}}};

        int i = 0;
        for (auto line : all_lines) {
            EXPECT_EQ(std::distance(cps.cbegin(), line.begin()), line_bounds[i].first)
                << "i=" << i;
            EXPECT_EQ(std::distance(cps.cbegin(), line.end()), line_bounds[i].second)
                << "i=" << i;
            ++i;
        }
        EXPECT_EQ(i, (int)line_bounds.size());

        auto const all_lines_reversed = boost::text::reversed_lines(cps);
        i = line_bounds.size();
        for (auto line : all_lines_reversed) {
            --i;
            EXPECT_EQ(std::distance(cps.cbegin(), line.begin()), line_bounds[i].first)
                << "i=" << i;
            EXPECT_EQ(std::distance(cps.cbegin(), line.end()), line_bounds[i].second)
                << "i=" << i;
        }
        EXPECT_EQ(i, 0);
    }

    {
        auto const all_lines = boost::text::possible_lines(cps);

        std::array<std::pair<int, int>, 2> const line_bounds = {
            {{0, 2}, {2, 3}}};

        int i = 0;
        for (auto line : all_lines) {
            EXPECT_EQ(std::distance(cps.cbegin(), line.begin()), line_bounds[i].first)
                << "i=" << i;
            EXPECT_EQ(std::distance(cps.cbegin(), line.end()), line_bounds[i].second)
                << "i=" << i;
            EXPECT_EQ(line.hard_break(), i == 1) << "i=" << i;
            ++i;
        }
        EXPECT_EQ(i, (int)line_bounds.size());

        auto const all_lines_reversed =
            boost::text::reversed_possible_lines(cps);
        i = line_bounds.size();
        for (auto line : all_lines_reversed) {
            --i;
            EXPECT_EQ(std::distance(cps.cbegin(), line.begin()), line_bounds[i].first)
                << "i=" << i;
            EXPECT_EQ(std::distance(cps.cbegin(), line.end()), line_bounds[i].second)
                << "i=" << i;
            EXPECT_EQ(line.hard_break(), i == 1);
        }
        EXPECT_EQ(i, 0);
    }

    // 80 columns -> don't take the possible break in the middle.
    {
        auto const _80_column_lines = boost::text::lines(
            cps,
            80,
            [](text::const_iterator::iterator_type it,
               text::const_iterator::iterator_type last) {
                return distance(it, last);
            });

        std::array<std::pair<int, int>, 1> const line_bounds = {{{0, 3}}};

        int i = 0;
        for (auto line : _80_column_lines) {
            EXPECT_EQ(std::distance(cps.cbegin(), line.begin()), line_bounds[i].first)
                << "i=" << i;
            EXPECT_EQ(std::distance(cps.cbegin(), line.end()), line_bounds[i].second)
                << "i=" << i;
            ++i;
        }
        EXPECT_EQ(i, (int)line_bounds.size());
    }
}

TEST(text_algorithm, paragraph_break)
{
    // ÷ 0065 × 0074 × 0063 × 002E ÷ 5B83 ÷
    // ÷ [0.2] LATIN SMALL LETTER E (Lower) × [998.0] LATIN SMALL LETTER T (Lower) × [998.0] LATIN SMALL LETTER C (Lower) × [998.0] FULL STOP (ATerm) ÷ [11.0] CJK UNIFIED IDEOGRAPH-5B83 (OLetter) ÷ [0.3]
    text cps("\x65\x74\x63\x2e\xe5\xae\x83");

    {
        EXPECT_EQ(std::distance(cps.cbegin(), boost::text::prev_paragraph_break(cps, std::next(cps.begin(), 2))), 0);
        EXPECT_EQ(std::distance(cps.cbegin(), boost::text::next_paragraph_break(cps, std::next(cps.begin(), 0))), 5);
    }
    {
        auto const range =
            boost::text::paragraph(cps, std::next(cps.begin(), 1));
        EXPECT_EQ(std::distance(cps.cbegin(), range.begin()), 0);
        EXPECT_EQ(std::distance(cps.cbegin(), range.end()), 5);
    }

    {
        auto const all_paragraphs = boost::text::paragraphs(cps);

        std::array<std::pair<int, int>, 1> const paragraph_bounds = {{{0, 5}}};

        int i = 0;
        for (auto paragraph : all_paragraphs) {
            EXPECT_EQ(
                std::distance(cps.cbegin(), paragraph.begin()),
                paragraph_bounds[i].first)
                << "i=" << i;
            EXPECT_EQ(
                std::distance(cps.cbegin(), paragraph.end()),
                paragraph_bounds[i].second)
                << "i=" << i;
            ++i;
        }
        EXPECT_EQ(i, (int)paragraph_bounds.size());

        auto const all_paragraphs_reversed = boost::text::reversed_paragraphs(cps);
        i = paragraph_bounds.size();
        for (auto paragraph : all_paragraphs_reversed) {
            --i;
            EXPECT_EQ(
                std::distance(cps.cbegin(), paragraph.begin()),
                paragraph_bounds[i].first)
                << "i=" << i;
            EXPECT_EQ(
                std::distance(cps.cbegin(), paragraph.end()),
                paragraph_bounds[i].second)
                << "i=" << i;
        }
        EXPECT_EQ(i, 0);
    }
}

struct bidi_stateful_cp_extent
{
    bidi_stateful_cp_extent() : i(0) {}

    template<typename Iter, typename Sentinel>
    int operator()(Iter first, Sentinel last) const noexcept
    {
        if ((int)index_counts.size() <= i)
            index_counts.resize(i + 1);
        ++index_counts[i];
        ++i;
        return std::distance(first, last);
    }

    mutable int i;

    // Unused; here just to check that moves are done properly.
    std::unique_ptr<int> ptr;

    static std::vector<int> index_counts;
};
std::vector<int> bidi_stateful_cp_extent::index_counts;

TEST(text_algorithm, bidi)
{
    // ON RLE ON FSI ON R RLO L PDF ON PDI ON PDF ON; 3 ('LTR') (line 496999)
    std::vector<uint32_t> const cp_vec = { 0x0021, 0x202B, 0x0021, 0x2068, 0x0021, 0x05BE, 0x202E, 0x0041, 0x202C, 0x0021, 0x2069, 0x0021, 0x202C, 0x0021 };
    text cps(to_string(cp_vec.begin(), cp_vec.end()));
    std::vector<uint32_t> const expected_reordered_indices = { 0, 11, 10, 9, 7, 5, 4, 3, 2, 13 };

    {
        std::vector<uint32_t> reordered;
        for (auto subrange : boost::text::bidirectional_subranges(cps, 0)) {
            for (auto grapheme : subrange) {
                for (auto cp : grapheme) {
                    reordered.push_back(cp);
                }
            }
        }
        int i = 0;
        for (int idx : expected_reordered_indices) {
            if (cp_vec[idx] < 0x2066 || 0x2069 < cp_vec[idx]) {
                EXPECT_EQ(reordered[i], cp_vec[idx])
                    << std::hex
                    << " 0x" << reordered[i]
                    << " 0x" << cp_vec[idx]
                    << std::dec << " i=" << i;
            }
            ++i;
        }
        EXPECT_EQ(i, (int)reordered.size());
    }

    {
        std::vector<uint32_t> reordered;
        for (auto subrange : boost::text::bidirectional_subranges(cps)) {
            for (auto grapheme : subrange) {
                for (auto cp : grapheme) {
                    reordered.push_back(cp);
                }
            }
        }
        int i = 0;
        for (int idx : expected_reordered_indices) {
            if (cp_vec[idx] < 0x2066 || 0x2069 < cp_vec[idx]) {
                EXPECT_EQ(reordered[i], cp_vec[idx])
                    << std::hex
                    << " 0x" << reordered[i]
                    << " 0x" << cp_vec[idx]
                    << std::dec << " i=" << i;
            }
            ++i;
        }
        EXPECT_EQ(i, (int)reordered.size());
    }

    // Extent-limited.
    {
        bidi_stateful_cp_extent::index_counts.clear();

        std::vector<uint32_t> reordered;
        for (auto subrange : boost::text::bidirectional_subranges(
                 cps, 80, bidi_stateful_cp_extent{}, 0)) {
            for (auto grapheme : subrange) {
                for (auto cp : grapheme) {
                    reordered.push_back(cp);
                }
            }
        }
        int i = 0;
        for (int idx : expected_reordered_indices) {
            if (cp_vec[idx] < 0x2066 || 0x2069 < cp_vec[idx]) {
                EXPECT_EQ(reordered[i], cp_vec[idx])
                    << std::hex
                    << " 0x" << reordered[i]
                    << " 0x" << cp_vec[idx]
                    << std::dec << " i=" << i;
            }
            ++i;
        }
        EXPECT_EQ(i, (int)reordered.size());

        EXPECT_EQ(
            std::count(
                bidi_stateful_cp_extent::index_counts.begin(),
                bidi_stateful_cp_extent::index_counts.end(),
                1),
            (std::ptrdiff_t)bidi_stateful_cp_extent::index_counts.size());
    }

    {
        bidi_stateful_cp_extent::index_counts.clear();

        std::vector<uint32_t> reordered;
        for (auto subrange : boost::text::bidirectional_subranges(
                 cps, 80, bidi_stateful_cp_extent{})) {
            for (auto grapheme : subrange) {
                for (auto cp : grapheme) {
                    reordered.push_back(cp);
                }
            }
        }
        int i = 0;
        for (int idx : expected_reordered_indices) {
            if (cp_vec[idx] < 0x2066 || 0x2069 < cp_vec[idx]) {
                EXPECT_EQ(reordered[i], cp_vec[idx])
                    << std::hex
                    << " 0x" << reordered[i]
                    << " 0x" << cp_vec[idx]
                    << std::dec << " i=" << i;
            }
            ++i;
        }
        EXPECT_EQ(i, (int)reordered.size());

        EXPECT_EQ(
            std::count(
                bidi_stateful_cp_extent::index_counts.begin(),
                bidi_stateful_cp_extent::index_counts.end(),
                1),
            (std::ptrdiff_t)bidi_stateful_cp_extent::index_counts.size());
    }
}

// TODO: search, case mapping
