
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
    if (!exists(boost::filesystem::path("ta_standard.table"))) {
        collation_table table = tailored_collation_table(
            data::ta::standard_collation_tailoring(),
            "ta::standard_collation_tailoring()", error, warning);
        save_table(table, "ta_standard.table.0");
        boost::filesystem::rename("ta_standard.table.0", "ta_standard.table");
    }
    return load_table("ta_standard.table");
}
collation_table const & table()
{
    static collation_table retval = make_save_load_table();
    return retval;
}
TEST(tailoring, ta_standard_000_001)
{
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>(1, 0x0b94),
        std::vector<uint32_t>(1, 0x0b82),
        table(), collation_strength::primary),
        -1);
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>(1, 0x0b82),
        std::vector<uint32_t>(1, 0x0b83),
        table(), collation_strength::primary),
        -1);
    // greater than (or equal to, for =) preceeding cps
    EXPECT_EQ(collate(
        std::vector<uint32_t>(1, 0x0bb9),
        std::vector<uint32_t>{0x0b95, 0x0bcd, 0x0bb7},
        table(), collation_strength::primary),
        -1);
}

TEST(tailoring, ta_standard_000_002)
{
}
