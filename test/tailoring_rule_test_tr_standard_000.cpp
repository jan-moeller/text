
// Warning! This file is autogenerated.
#include <boost/text/collation_table.hpp>
#include <boost/text/collate.hpp>
#include <boost/text/save_load_table.hpp>
#include <boost/text/data/all.hpp>

#include <boost/filesystem.hpp>

#include <gtest/gtest.h>

using namespace boost::text;

auto const error = [](string const & s) { std::cout << s; };
auto const warning = [](string const & s) {};

collation_table make_save_load_table()
{
    if (!exists(boost::filesystem::path("tr_standard.table"))) {
        collation_table table = tailored_collation_table(
            data::tr::standard_collation_tailoring(),
            "tr::standard_collation_tailoring()", error, warning);
        save_table(table, "tr_standard.table.0");
        boost::filesystem::rename("tr_standard.table.0", "tr_standard.table");
    }
    return load_table("tr_standard.table");
}
collation_table const & table()
{
    static collation_table retval = make_save_load_table();
    return retval;
}
TEST(tailoring, tr_standard_000_001)
{
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>(1, 0x0043),
        std::vector<uint32_t>{0x0063, 0x0327},
        table(), collation_strength::primary),
        -1);
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>{0x0063, 0x0327},
        std::vector<uint32_t>{0x0043, 0x0327},
        table(), collation_strength::tertiary),
        -1);
    // equal to preceeding cps at next-lower strength
    EXPECT_EQ(collate(
        std::vector<uint32_t>{0x0063, 0x0327},
        std::vector<uint32_t>{0x0043, 0x0327},
        table(), collation_strength::secondary),
        0);
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>(1, 0x0047),
        std::vector<uint32_t>{0x0067, 0x0306},
        table(), collation_strength::primary),
        -1);
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>{0x0067, 0x0306},
        std::vector<uint32_t>{0x0047, 0x0306},
        table(), collation_strength::tertiary),
        -1);
    // equal to preceeding cps at next-lower strength
    EXPECT_EQ(collate(
        std::vector<uint32_t>{0x0067, 0x0306},
        std::vector<uint32_t>{0x0047, 0x0306},
        table(), collation_strength::secondary),
        0);
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>(1, 0x0069),
        std::vector<uint32_t>(1, 0x0130),
        table(), collation_strength::tertiary),
        -1);
    // equal to preceeding cps at next-lower strength
    EXPECT_EQ(collate(
        std::vector<uint32_t>(1, 0x0069),
        std::vector<uint32_t>(1, 0x0130),
        table(), collation_strength::secondary),
        0);
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>(1, 0x004f),
        std::vector<uint32_t>{0x006f, 0x0308},
        table(), collation_strength::primary),
        -1);
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>{0x006f, 0x0308},
        std::vector<uint32_t>{0x004f, 0x0308},
        table(), collation_strength::tertiary),
        -1);
    // equal to preceeding cps at next-lower strength
    EXPECT_EQ(collate(
        std::vector<uint32_t>{0x006f, 0x0308},
        std::vector<uint32_t>{0x004f, 0x0308},
        table(), collation_strength::secondary),
        0);
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>(1, 0x0053),
        std::vector<uint32_t>{0x0073, 0x0327},
        table(), collation_strength::primary),
        -1);
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>{0x0073, 0x0327},
        std::vector<uint32_t>{0x0053, 0x0327},
        table(), collation_strength::tertiary),
        -1);
    // equal to preceeding cps at next-lower strength
    EXPECT_EQ(collate(
        std::vector<uint32_t>{0x0073, 0x0327},
        std::vector<uint32_t>{0x0053, 0x0327},
        table(), collation_strength::secondary),
        0);
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>(1, 0x0055),
        std::vector<uint32_t>{0x0075, 0x0308},
        table(), collation_strength::primary),
        -1);
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>{0x0075, 0x0308},
        std::vector<uint32_t>{0x0055, 0x0308},
        table(), collation_strength::tertiary),
        -1);
    // equal to preceeding cps at next-lower strength
    EXPECT_EQ(collate(
        std::vector<uint32_t>{0x0075, 0x0308},
        std::vector<uint32_t>{0x0055, 0x0308},
        table(), collation_strength::secondary),
        0);
}
