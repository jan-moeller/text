#define BOOST_TEXT_TESTING
#include <boost/text/text.hpp>
#include <boost/text/string_utility.hpp>

#include <boost/algorithm/cxx14/equal.hpp>

#include <gtest/gtest.h>

#include <list>


using namespace boost;

TEST(text_tests, test_empty)
{
    text::text t;

    EXPECT_EQ(t.begin(), t.end());
    EXPECT_EQ(t.cbegin(), t.cend());
    EXPECT_EQ(t.rbegin(), t.rend());
    EXPECT_EQ(t.crbegin(), t.crend());

    EXPECT_EQ(t.begin(), t.cbegin());
    EXPECT_EQ(t.end(), t.cend());
    EXPECT_EQ(t.rbegin(), t.crbegin());
    EXPECT_EQ(t.rend(), t.crend());

    EXPECT_TRUE(t.empty());
    EXPECT_EQ(t.storage_bytes(), 0);
    EXPECT_EQ(t.distance(), 0);
    EXPECT_EQ(t.begin(), t.end());

    EXPECT_EQ(t.max_bytes(), INT_MAX / 2);

    EXPECT_TRUE(t == t);
    EXPECT_FALSE(t != t);

    t.swap(t);
    EXPECT_TRUE(t == t);

    EXPECT_EQ(t.begin(), begin(t));
    EXPECT_EQ(t.end(), end(t));
    EXPECT_EQ(t.cbegin(), cbegin(t));
    EXPECT_EQ(t.cend(), cend(t));

    EXPECT_EQ(t.rbegin(), rbegin(t));
    EXPECT_EQ(t.rend(), rend(t));
    EXPECT_EQ(t.crbegin(), crbegin(t));
    EXPECT_EQ(t.crend(), crend(t));

    t.clear();
    t.shrink_to_fit();

    std::cout << "t=\"" << t << "\"\n";

    {
        using namespace text::literals;
        text::text t2 = ""_t;
        EXPECT_TRUE(t == t2);

        text::text t3 = u8""_t;
        EXPECT_TRUE(t == t3);
    }
}

TEST(text_tests, test_non_empty_const_interface)
{
    text::text t_a("a");
    text::text t_ab("ab");

    EXPECT_EQ((std::next(t_a.begin(), t_a.distance())), t_a.end());
    EXPECT_EQ((std::next(t_a.cbegin(), t_a.distance())), t_a.cend());
    EXPECT_EQ((std::next(t_a.rbegin(), t_a.distance())), t_a.rend());
    EXPECT_EQ((std::next(t_a.crbegin(), t_a.distance())), t_a.crend());

    EXPECT_EQ(t_a.begin(), t_a.cbegin());
    EXPECT_EQ(t_a.end(), t_a.cend());
    EXPECT_EQ(t_a.rbegin(), t_a.crbegin());
    EXPECT_EQ(t_a.rend(), t_a.crend());

    EXPECT_EQ(t_ab.begin(), t_ab.cbegin());
    EXPECT_EQ(t_ab.end(), t_ab.cend());
    EXPECT_EQ(t_ab.rbegin(), t_ab.crbegin());
    EXPECT_EQ(t_ab.rend(), t_ab.crend());

    EXPECT_FALSE(t_a.empty());
    EXPECT_EQ(t_a.distance(), 1);
    EXPECT_GT(t_a.capacity_bytes(), t_a.distance());

    EXPECT_FALSE(t_ab.empty());
    EXPECT_EQ(t_ab.distance(), 2);
    EXPECT_GT(t_ab.capacity_bytes(), t_ab.distance());

    EXPECT_EQ(t_a.max_bytes(), INT_MAX / 2);
    EXPECT_EQ(t_ab.max_bytes(), INT_MAX / 2);

    EXPECT_FALSE(t_a == t_ab);
    EXPECT_TRUE(t_a != t_ab);

    text::text const old_t_a(t_a);
    text::text const old_t_ab(t_ab);
    t_a.swap(t_ab);
    EXPECT_EQ(t_a, old_t_ab);
    EXPECT_EQ(t_ab, old_t_a);
    t_a.swap(t_ab);

    EXPECT_EQ(t_a.begin(), begin(t_a));
    EXPECT_EQ(t_a.end(), end(t_a));
    EXPECT_EQ(t_a.cbegin(), cbegin(t_a));
    EXPECT_EQ(t_a.cend(), cend(t_a));

    EXPECT_EQ(t_a.rbegin(), rbegin(t_a));
    EXPECT_EQ(t_a.rend(), rend(t_a));
    EXPECT_EQ(t_a.crbegin(), crbegin(t_a));
    EXPECT_EQ(t_a.crend(), crend(t_a));

    {
        using namespace text::literals;
        EXPECT_EQ(t_a, "a"_t);
        EXPECT_EQ(t_ab, "ab"_t);
    }
}

TEST(text_tests, test_ctors)
{
    using namespace text::literals;

    text::text t;
    EXPECT_EQ(t, ""_t);
    EXPECT_EQ(""_t, t);

    text::text t2("A nonemtpy string");
    EXPECT_EQ(t2, "A nonemtpy string"_t);
    EXPECT_EQ("A nonemtpy string"_t, t2);

    text::text t3(t2);
    EXPECT_EQ(t3, "A nonemtpy string"_t);
    EXPECT_EQ("A nonemtpy string"_t, t3);

    text::text t4(std::move(t2));
    EXPECT_EQ(t4, "A nonemtpy string"_t);
    EXPECT_EQ("A nonemtpy string"_t, t4);
    EXPECT_EQ(t2, ""_t);
    EXPECT_EQ(""_t, t2);

    std::string const s("An old-school string");
    text::text t5(s);
    EXPECT_EQ(t5, "An old-school string"_t);
    EXPECT_EQ("An old-school string"_t, t5);

    text::text const t5_5("a view ");
    text::text_view const tv(t5_5);
    text::text t6(tv);
    EXPECT_EQ(t6, "a view "_t);
    EXPECT_EQ("a view "_t, t6);

    text::repeated_string_view const rtv(tv, 3);
    text::text t7(rtv);
    EXPECT_EQ(t7, "a view a view a view "_t);
    EXPECT_EQ("a view a view a view "_t, t7);

    std::list<char> const char_list = {'a', ' ', 'l', 'i', 's', 't'};
    text::text t8(char_list.begin(), char_list.end());
    EXPECT_EQ(t8, "a list"_t);
    EXPECT_EQ("a list"_t, t8);
}

TEST(text_tests, test_insert)
{
    using namespace text::literals;

    text::text const ct0("a view ");
    text::text_view const tv(ct0);
    text::repeated_string_view const rtv("a view ", 3);

    {
        text::text const ct("string");

        text::text t0 = ct;
        t0.insert(std::next(t0.begin(), 0), tv);
        EXPECT_EQ(t0, "a view string"_t);

        text::text t1 = ct;
        t1.insert(std::next(t1.begin(), 1), tv);
        EXPECT_EQ(t1, "sa view tring"_t);

        text::text t2 = ct;
        t2.insert(std::next(t2.begin(), 2), tv);
        EXPECT_EQ(t2, "sta view ring"_t);

        text::text t3 = ct;
        t3.insert(std::next(t3.begin(), 3), tv);
        EXPECT_EQ(t3, "stra view ing"_t);

        text::text t4 = ct;
        t4.insert(std::next(t4.begin(), 4), tv);
        EXPECT_EQ(t4, "stria view ng"_t);

        text::text t5 = ct;
        t5.insert(std::next(t5.begin(), 5), tv);
        EXPECT_EQ(t5, "strina view g"_t);

        text::text t6 = ct;
        t6.insert(std::next(t6.begin(), 6), tv);
        EXPECT_EQ(t6, "stringa view "_t);
    }

    {
        text::text const ct("string");

        text::text t0 = ct;
        t0.insert(std::next(t0.begin(), 0), rtv);
        EXPECT_EQ(t0, "a view a view a view string"_t);

        text::text t1 = ct;
        t1.insert(std::next(t1.begin(), 1), rtv);
        EXPECT_EQ(t1, "sa view a view a view tring"_t);

        text::text t2 = ct;
        t2.insert(std::next(t2.begin(), 2), rtv);
        EXPECT_EQ(t2, "sta view a view a view ring"_t);

        text::text t3 = ct;
        t3.insert(std::next(t3.begin(), 3), rtv);
        EXPECT_EQ(t3, "stra view a view a view ing"_t);

        text::text t4 = ct;
        t4.insert(std::next(t4.begin(), 4), rtv);
        EXPECT_EQ(t4, "stria view a view a view ng"_t);

        text::text t5 = ct;
        t5.insert(std::next(t5.begin(), 5), rtv);
        EXPECT_EQ(t5, "strina view a view a view g"_t);

        text::text t6 = ct;
        t6.insert(std::next(t6.begin(), 6), rtv);
        EXPECT_EQ(t6, "stringa view a view a view "_t);
    }

    // Unicode 9, 3.9/D90
    uint32_t const utf32[] = {0x004d, 0x0430, 0x4e8c, 0x10302};

    {
        text::text const ct("string");
        auto const first = text::utf8::from_utf32_iterator<uint32_t const *>(
            utf32, utf32, utf32 + 4);
        auto const last = text::utf8::from_utf32_iterator<uint32_t const *>(
            utf32, utf32 + 4, utf32 + 4);

        text::text t0 = ct;
        t0.insert(std::next(t0.begin(), 0), first, last);
        EXPECT_EQ(t0, "\x4d\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82string"_t);

        text::text t1 = ct;
        t1.insert(std::next(t1.begin(), 1), first, last);
        EXPECT_EQ(t1, "s\x4d\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82tring"_t);

        text::text t2 = ct;
        t2.insert(std::next(t2.begin(), 2), first, last);
        EXPECT_EQ(t2, "st\x4d\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82ring"_t);

        text::text t3 = ct;
        t3.insert(std::next(t3.begin(), 3), first, last);
        EXPECT_EQ(t3, "str\x4d\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82ing"_t);

        text::text t4 = ct;
        t4.insert(std::next(t4.begin(), 4), first, last);
        EXPECT_EQ(t4, "stri\x4d\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82ng"_t);

        text::text t5 = ct;
        t5.insert(std::next(t5.begin(), 5), first, last);
        EXPECT_EQ(t5, "strin\x4d\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82g"_t);

        text::text t6 = ct;
        t6.insert(std::next(t6.begin(), 6), first, last);
        EXPECT_EQ(t6, "string\x4d\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82"_t);
    }

    {
        char const * str = "";
        text::string_view const sv(str, 1); // explicitly null-terminated
        text::repeated_string_view const rsv(sv, 3);

        {
            text::text t("text");
            t.insert(std::next(t.begin(), 2), sv);
            EXPECT_EQ(t, "text"_t); // no null in the middle
        }

        {
            text::text t("text");
            t.insert(std::next(t.begin(), 2), rsv);
            EXPECT_EQ(t, "text"_t); // no nulls in the middle
        }
    }

    {
        {
            text::text t("e");
            auto const it = t.insert(t.begin(), "f");
            EXPECT_EQ(t.distance(), 2);
            EXPECT_EQ(it, t.begin());
        }
        {
            text::text t("e");
            auto const it = t.insert(t.end(), "f");
            EXPECT_EQ(t.distance(), 2);
            EXPECT_EQ(it, std::next(t.begin()));
        }

        char const * combining_diaeresis = u8"\u0308";

        {
            text::text t("e");
            auto const it = t.insert(t.begin(), combining_diaeresis);
            EXPECT_EQ(t.distance(), 2);
            EXPECT_EQ(it, t.begin());
        }
        {
            text::text t("e");

            auto it = t.insert(t.end(), combining_diaeresis);
            EXPECT_EQ(t.distance(), 1);
            EXPECT_EQ(it, t.begin());

            it = t.insert(t.end(), combining_diaeresis);
            EXPECT_EQ(t.distance(), 1);
            EXPECT_EQ(it, t.begin());
        }
        {
            text::text t("et");

            auto it = t.insert(std::next(t.begin()), combining_diaeresis);
            EXPECT_EQ(t.distance(), 2);
            EXPECT_EQ(it, t.begin());

            it = t.insert(std::next(t.begin()), combining_diaeresis);
            EXPECT_EQ(t.distance(), 2);
            EXPECT_EQ(it, t.begin());
        }
    }

    {
        {
            text::text t;
            t.insert(t.begin(), 'g'); // text::grapheme
            EXPECT_EQ(t, text::text("g"));
        }
        {
            text::text t;
            t.insert(t.begin(), text::grapheme());
            EXPECT_EQ(t, text::text(""));
        }

        {
            text::text t_0("g");
            text::text t;
            t.insert(t.end(), *t_0.begin()); // text::grapheme_view
            EXPECT_EQ(t, text::text("g"));
        }
        {
            text::text const t_0("g");
            text::text t;
            t.insert(t.end(), *t_0.begin());
            EXPECT_EQ(t, text::text("g"));
        }
        {
            text::rope r("g");
            text::text t;
            t.insert(t.end(), *r.begin()); // text::grapheme_view
            EXPECT_EQ(t, text::text("g"));
        }
        {
            text::rope const r("g");
            text::text t;
            t.insert(t.end(), *r.begin());
            EXPECT_EQ(t, text::text("g"));
        }
        {
            text::text t;
            t.insert(
                t.begin(),
                text::grapheme_view<text::rope::iterator::iterator_type>());
            EXPECT_EQ(t, text::text(""));
        }
    }
}

TEST(text_tests, test_erase)
{
    text::text const ct("string");

    for (int j = 0, end = ct.distance(); j <= end; ++j) {
        for (int i = 0; i <= j; ++i) {
            text::text t = ct;
            text::text_view const before(t.begin(), std::next(t.begin(), i));
            text::text_view const substr(
                std::next(t.begin(), i), std::next(t.begin(), j));
            text::text const substr_copy(substr);
            text::text_view const after(std::next(ct.begin(), j), ct.end());

            text::text expected(before);
            expected += after;

            t.erase(substr);
            EXPECT_EQ(t, expected) << "i=" << i << " j=" << j << " erasing '"
                                   << substr_copy << "'";
        }
    }
}

TEST(text_tests, test_replace)
{
    using namespace text::literals;

    text::text const ct0("REP");
    text::text_view const replacement(ct0);
    // Explicitly null-terminated.
    char const * rep = "REP";
    text::string_view const replacement_with_null(rep, 4);

    {
        text::text t("string");
        t.replace(t, replacement_with_null);
        EXPECT_EQ(t, "REP"_t);
    }

    {
        text::text t("string");
        t.replace(t, replacement);
        EXPECT_EQ(t, "REP"_t);
    }

    {
        text::text t("string");
        t.replace(t, replacement);
        EXPECT_EQ(t, "REP"_t);
    }

    {
        text::text t("string");
        text::text_view const old_substr(
            std::next(t.begin(), 0), std::next(t.begin(), 3));
        text::text_view const new_substr(
            std::next(t.begin(), 2), std::next(t.begin(), 6));
        t.replace(old_substr, new_substr);
        EXPECT_EQ(t, "ringing"_t);
    }

    {
        text::text t("string");
        text::text_view const old_substr(
            std::next(t.begin(), 3), std::next(t.begin(), 6));
        text::text_view const new_substr(
            std::next(t.begin(), 0), std::next(t.begin(), 3));
        t.replace(old_substr, new_substr);
        EXPECT_EQ(t, "strstr"_t);
    }

    text::text const ct("string");

    for (int j = 0, end = ct.distance(); j <= end; ++j) {
        for (int i = 0; i <= j; ++i) {
            text::text t = ct;
            text::text_view const before(t.begin(), std::next(t.begin(), i));
            text::text_view const substr(
                std::next(t.begin(), i), std::next(t.begin(), j));
            text::text const substr_copy(substr);
            text::text_view const after(std::next(ct.begin(), j), ct.end());

            text::text expected(before);
            expected += replacement;
            expected += after;

            t.replace(substr, replacement);
            EXPECT_EQ(t, expected) << "i=" << i << " j=" << j << " erasing '"
                                   << substr_copy << "'";
        }
    }

    text::repeated_string_view const really_long_replacement(replacement, 10);

    for (int j = 0, end = ct.distance(); j <= end; ++j) {
        for (int i = 0; i <= j; ++i) {
            text::text t = ct;
            text::text_view const before(t.begin(), std::next(t.begin(), i));
            text::text_view const substr(
                std::next(t.begin(), i), std::next(t.begin(), j));
            text::text const substr_copy(substr);
            text::text_view const after(std::next(ct.begin(), j), ct.end());

            text::text expected(before);
            expected += really_long_replacement;
            expected += after;

            t.replace(substr, really_long_replacement);
            EXPECT_EQ(t, expected) << "i=" << i << " j=" << j << " erasing '"
                                   << substr_copy << "'";
        }
    }
}

TEST(text_tests, test_replace_iter)
{
    using namespace text::literals;

    // Unicode 9, 3.9/D90
    uint32_t const utf32[] = {0x004d, 0x0430, 0x4e8c, 0x10302};
    auto const first = text::utf8::from_utf32_iterator<uint32_t const *>(
        utf32, utf32, utf32 + 4);
    auto const final_cp = text::utf8::from_utf32_iterator<uint32_t const *>(
        utf32, utf32 + 3, utf32 + 4);
    auto const last = text::utf8::from_utf32_iterator<uint32_t const *>(
        utf32, utf32 + 4, utf32 + 4);

    text::text const ct_string("string");
    text::text const ct_text("text");

    {
        text::text t = ct_string;
        t.replace(t, final_cp, last);
        EXPECT_EQ(t, "\xf0\x90\x8c\x82"_t);
    }

    {
        text::text t = ct_text;
        t.replace(t, final_cp, last);
        EXPECT_EQ(t, "\xf0\x90\x8c\x82"_t);
    }

    {
        text::text t = ct_string;
        t.replace(t, first, last);
        EXPECT_EQ(t, "\x4d\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82"_t);
    }

    for (int j = 0, end = ct_string.distance(); j <= end; ++j) {
        for (int i = 0; i <= j; ++i) {
            {
                text::text t = ct_string;
                text::text_view const before(
                    t.begin(), std::next(t.begin(), i));
                text::text_view const substr(
                    std::next(t.begin(), i), std::next(t.begin(), j));
                text::text const substr_copy(substr);
                text::text_view const after(std::next(t.begin(), j), t.end());

                text::text expected(before);
                expected.insert(expected.end(), final_cp, last);
                expected += after;

                t.replace(substr, final_cp, last);
                EXPECT_EQ(t, expected) << "i=" << i << " j=" << j
                                       << " erasing '" << substr_copy << "'";
            }

            {
                text::text t = ct_string;
                text::text_view const before(
                    t.begin(), std::next(t.begin(), i));
                text::text_view const substr(
                    std::next(t.begin(), i), std::next(t.begin(), j));
                text::text const substr_copy(substr);
                text::text_view const after(std::next(t.begin(), j), t.end());

                text::text expected(before);
                expected.insert(expected.end(), first, last);
                expected += after;

                t.replace(substr, first, last);
                EXPECT_EQ(t, expected) << "i=" << i << " j=" << j
                                       << " erasing '" << substr_copy << "'";
            }
        }
    }
}

TEST(text_tests, test_replace_iter_large_insertions)
{
    // Unicode 9, 3.9/D90
    uint32_t const utf32[] = {0x004d, 0x0430, 0x4e8c, 0x10302};

    std::vector<uint32_t> utf32_repeated;
    for (int i = 0; i < 5000; ++i) {
        utf32_repeated.insert(utf32_repeated.end(), utf32, utf32 + 4);
    }
    auto const first =
        text::utf8::from_utf32_iterator<std::vector<uint32_t>::iterator>(
            utf32_repeated.begin(),
            utf32_repeated.begin(),
            utf32_repeated.end());
    auto const last =
        text::utf8::from_utf32_iterator<std::vector<uint32_t>::iterator>(
            utf32_repeated.begin(), utf32_repeated.end(), utf32_repeated.end());

    {
        text::text t("string");
        t.replace(t, first, last);
        text::text const expected(first, last);
        EXPECT_EQ(t, expected);
    }

    {
        text::text t;
        t.replace(t, first, last);
        text::text const expected(first, last);
        EXPECT_EQ(t, expected);
    }
}

TEST(text_tests, normalization)
{
    using namespace text::literals;

    uint32_t const circumflex_utf32[] = {0x302}; // ◌̂
    uint32_t const a_with_circumflex_utf32[] = {0xe2}; // â

    text::string const s_circumflex =
        text::to_string(circumflex_utf32, circumflex_utf32 + 1);
    text::string const s_a_with_circumflex =
        text::to_string(a_with_circumflex_utf32, a_with_circumflex_utf32 + 1);

    text::text const t_circumflex(s_circumflex);
    text::text const t_a_with_circumflex(s_a_with_circumflex);
    text::text const t_a_with_circumflex_2("a\xcc\x82"/*a◌̂*/);

    EXPECT_EQ(t_circumflex.distance(), 1);
    EXPECT_EQ(t_a_with_circumflex.distance(), 1);
    EXPECT_EQ(t_a_with_circumflex_2.distance(), 1);

    EXPECT_EQ(t_circumflex, "\xcc\x82"_t/*◌̂*/);
    EXPECT_EQ(t_a_with_circumflex, "\xc3\xa2"_t/*â*/);
    EXPECT_EQ(t_a_with_circumflex_2, "\xc3\xa2"_t/*â*/);

    // insert()

    {
        text::text t = "aa";
        t.insert(std::next(t.begin(), 0), "\xcc\x82"/*◌̂*/);
        EXPECT_EQ(t, text::text("\xcc\x82""aa")/*◌̂aa*/);
        EXPECT_EQ(t.distance(), 3);
    }
    {
        text::text t = "aa";
        t.insert(std::next(t.begin(), 1), "\xcc\x82"/*◌̂*/);
        EXPECT_EQ(t, text::text("\xc3\xa2""a")/*âa*/);
        EXPECT_EQ(t.distance(), 2);
    }
    {
        text::text t = "aa";
        t.insert(std::next(t.begin(), 2), "\xcc\x82"/*◌̂*/);
        EXPECT_EQ(t, text::text("a\xc3\xa2")/*aâ*/);
        EXPECT_EQ(t.distance(), 2);
    }

    {
        text::text t = "\xc3\xa2""a";
        t.insert(std::next(t.begin(), 0), "\xcc\x82"/*◌̂*/);
        EXPECT_EQ(t, text::text("\xcc\x82\xc3\xa2""a")/*◌̂âa*/);
        EXPECT_EQ(t.distance(), 3);
    }
    {
        text::text t = "\xc3\xa2""a";
        t.insert(std::next(t.begin(), 1), "\xcc\x82"/*◌̂*/);
        EXPECT_EQ(t, text::text("\xc3\xa2\xcc\x82""a")/*â◌̂a*/);
        EXPECT_EQ(t.distance(), 2); // not 3 because â◌̂ is a single grapheme
    }
    {
        text::text t = "\xc3\xa2""a";
        t.insert(std::next(t.begin(), 2), "\xcc\x82"/*◌̂*/);
        EXPECT_EQ(t, text::text("\xc3\xa2\xc3\xa2")/*ââ*/);
        EXPECT_EQ(t.distance(), 2);
    }

    {
        text::text t = "aa";
        t.insert(
            std::next(t.begin(), 0), s_circumflex.begin(), s_circumflex.end());
        EXPECT_EQ(t, text::text("\xcc\x82""aa")/*◌̂aa*/);
        EXPECT_EQ(t.distance(), 3);
    }
    {
        text::text t = "aa";
        t.insert(
            std::next(t.begin(), 1), s_circumflex.begin(), s_circumflex.end());
        EXPECT_EQ(t, text::text("\xc3\xa2""a")/*âa*/);
        EXPECT_EQ(t.distance(), 2);
    }
    {
        text::text t = "aa";
        t.insert(
            std::next(t.begin(), 2), s_circumflex.begin(), s_circumflex.end());
        EXPECT_EQ(t, text::text("a\xc3\xa2")/*aâ*/);
        EXPECT_EQ(t.distance(), 2);
    }

    {
        text::text t = "\xc3\xa2""a";
        t.insert(
            std::next(t.begin(), 0), s_circumflex.begin(), s_circumflex.end());
        EXPECT_EQ(t, text::text("\xcc\x82\xc3\xa2""a")/*◌̂âa*/);
        EXPECT_EQ(t.distance(), 3);
    }
    {
        text::text t = "\xc3\xa2""a";
        t.insert(
            std::next(t.begin(), 1), s_circumflex.begin(), s_circumflex.end());
        EXPECT_EQ(t, text::text("\xc3\xa2\xcc\x82""a")/*â◌̂a*/);
        EXPECT_EQ(t.distance(), 2); // not 3 because â◌̂ is a single grapheme
    }
    {
        text::text t = "\xc3\xa2""a";
        t.insert(
            std::next(t.begin(), 2), s_circumflex.begin(), s_circumflex.end());
        EXPECT_EQ(t, text::text("\xc3\xa2\xc3\xa2")/*ââ*/);
        EXPECT_EQ(t.distance(), 2);
    }

    // replace()

    auto first = [](text::text & t) {
        return text::text_view(t.begin(), std::next(t.begin(), 1));
    };
    auto second = [](text::text & t) {
        return text::text_view(
            std::next(t.begin(), 1), std::next(t.begin(), 2));
    };
    auto third = [](text::text & t) {
        return text::text_view(std::next(t.begin(), 2), t.end());
    };

    {
        text::text t = "aaa";
        t.replace(first(t), "\xcc\x82"/*◌̂*/);
        EXPECT_EQ(t, text::text("\xcc\x82""aa")/*◌̂aa*/);
        EXPECT_EQ(t.distance(), 3);
    }
    {
        text::text t = "aaa";
        t.replace(second(t), "\xcc\x82"/*◌̂*/);
        EXPECT_EQ(t, text::text("\xc3\xa2""a")/*âa*/);
        EXPECT_EQ(t.distance(), 2);
    }
    {
        text::text t = "aaa";
        t.replace(third(t), "\xcc\x82"/*◌̂*/);
        EXPECT_EQ(t, text::text("a\xc3\xa2")/*aâ*/);
        EXPECT_EQ(t.distance(), 2);
    }

    {
        text::text t = "\xc3\xa2""aa";
        t.replace(first(t), "\xcc\x82"/*◌̂*/);
        EXPECT_EQ(t, text::text("\xcc\x82""aa")/*◌̂aa*/);
        EXPECT_EQ(t.distance(), 3);
    }
    {
        text::text t = "\xc3\xa2""aa";
        t.replace(second(t), "\xcc\x82"/*◌̂*/);
        EXPECT_EQ(t, text::text("\xc3\xa2\xcc\x82""a")/*â◌̂a*/);
        EXPECT_EQ(t.distance(), 2); // not 3 because â◌̂ is a single grapheme
    }
    {
        text::text t = "\xc3\xa2""aa";
        t.replace(third(t), "\xcc\x82"/*◌̂*/);
        EXPECT_EQ(t, text::text("\xc3\xa2\xc3\xa2")/*ââ*/);
        EXPECT_EQ(t.distance(), 2);
    }

    {
        text::text t = "aaa";
        t.replace(first(t), s_circumflex.begin(), s_circumflex.end());
        EXPECT_EQ(t, text::text("\xcc\x82""aa")/*◌̂aa*/);
        EXPECT_EQ(t.distance(), 3);
    }
    {
        text::text t = "aaa";
        t.replace(second(t), s_circumflex.begin(), s_circumflex.end());
        EXPECT_EQ(t, text::text("\xc3\xa2""a")/*âa*/);
        EXPECT_EQ(t.distance(), 2);
    }
    {
        text::text t = "aaa";
        t.replace(third(t), s_circumflex.begin(), s_circumflex.end());
        EXPECT_EQ(t, text::text("a\xc3\xa2")/*aâ*/);
        EXPECT_EQ(t.distance(), 2);
    }

    {
        text::text t = "\xc3\xa2""aa";
        t.replace(first(t), s_circumflex.begin(), s_circumflex.end());
        EXPECT_EQ(t, text::text("\xcc\x82""aa")/*◌̂aa*/);
        EXPECT_EQ(t.distance(), 3);
    }
    {
        text::text t = "\xc3\xa2""aa";
        t.replace(second(t), s_circumflex.begin(), s_circumflex.end());
        EXPECT_EQ(t, text::text("\xc3\xa2\xcc\x82""a")/*â◌̂a*/);
        EXPECT_EQ(t.distance(), 2); // not 3 because â◌̂ is a single grapheme
    }
    {
        text::text t = "\xc3\xa2""aa";
        t.replace(third(t), s_circumflex.begin(), s_circumflex.end());
        EXPECT_EQ(t, text::text("\xc3\xa2\xc3\xa2")/*ââ*/);
        EXPECT_EQ(t.distance(), 2);
    }
}

TEST(text, test_sentinel_api)
{
    {
        char const * chars = "chars";
        text::text s(chars, text::utf8::null_sentinel{});
        EXPECT_EQ(s, text::text(chars));
    }
    {
        char const * chars = "chars";
        text::text s;
        s.insert(s.end(), chars, text::utf8::null_sentinel{});
        EXPECT_EQ(s, text::text(chars));
    }
    {
        char const * chars = "chars";
        text::text s;
        s.replace(
            text::text_view(s.begin(), s.begin()),
            chars,
            text::utf8::null_sentinel{});
        EXPECT_EQ(s, text::text(chars));
    }
}
