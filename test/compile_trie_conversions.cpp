#include <boost/text/trie.hpp>

#include <string>


void bool_conversions()
{
    boost::trie::trie<std::string, bool> t;

    auto nf = t["not-found"];

    if (nf) {
        bool const b = *nf;
        (void)b;
    }
}

void convertible_to_bool_conversions()
{
    boost::trie::trie<std::string, int> t;

    auto nf = t["not-found"];

    if (nf) {
        bool const b = nf;
        (void)b;
    }
}

void bool_assignment()
{
    boost::trie::trie<std::string, bool> t = {{std::string("found"), false}};

    auto f = t["found"];

    if (f) {
        f = true;
    }

    t["found"] = true;
}

void non_bool_assignment()
{
    boost::trie::trie<std::string, int> t = {{std::string("found"), 0}};

    auto f = t["found"];

    if (f) {
        f = 1;
    }

    t["found"] = 1;
}
