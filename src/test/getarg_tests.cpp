#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-PLB");
    BOOST_CHECK(GetBoolArg("-PLB"));
    BOOST_CHECK(GetBoolArg("-PLB", false));
    BOOST_CHECK(GetBoolArg("-PLB", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-PLBo"));
    BOOST_CHECK(!GetBoolArg("-PLBo", false));
    BOOST_CHECK(GetBoolArg("-PLBo", true));

    ResetArgs("-PLB=0");
    BOOST_CHECK(!GetBoolArg("-PLB"));
    BOOST_CHECK(!GetBoolArg("-PLB", false));
    BOOST_CHECK(!GetBoolArg("-PLB", true));

    ResetArgs("-PLB=1");
    BOOST_CHECK(GetBoolArg("-PLB"));
    BOOST_CHECK(GetBoolArg("-PLB", false));
    BOOST_CHECK(GetBoolArg("-PLB", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noPLB");
    BOOST_CHECK(!GetBoolArg("-PLB"));
    BOOST_CHECK(!GetBoolArg("-PLB", false));
    BOOST_CHECK(!GetBoolArg("-PLB", true));

    ResetArgs("-noPLB=1");
    BOOST_CHECK(!GetBoolArg("-PLB"));
    BOOST_CHECK(!GetBoolArg("-PLB", false));
    BOOST_CHECK(!GetBoolArg("-PLB", true));

    ResetArgs("-PLB -noPLB");  // -PLB should win
    BOOST_CHECK(GetBoolArg("-PLB"));
    BOOST_CHECK(GetBoolArg("-PLB", false));
    BOOST_CHECK(GetBoolArg("-PLB", true));

    ResetArgs("-PLB=1 -noPLB=1");  // -PLB should win
    BOOST_CHECK(GetBoolArg("-PLB"));
    BOOST_CHECK(GetBoolArg("-PLB", false));
    BOOST_CHECK(GetBoolArg("-PLB", true));

    ResetArgs("-PLB=0 -noPLB=0");  // -PLB should win
    BOOST_CHECK(!GetBoolArg("-PLB"));
    BOOST_CHECK(!GetBoolArg("-PLB", false));
    BOOST_CHECK(!GetBoolArg("-PLB", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--PLB=1");
    BOOST_CHECK(GetBoolArg("-PLB"));
    BOOST_CHECK(GetBoolArg("-PLB", false));
    BOOST_CHECK(GetBoolArg("-PLB", true));

    ResetArgs("--noPLB=1");
    BOOST_CHECK(!GetBoolArg("-PLB"));
    BOOST_CHECK(!GetBoolArg("-PLB", false));
    BOOST_CHECK(!GetBoolArg("-PLB", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-PLB", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PLB", "eleven"), "eleven");

    ResetArgs("-PLB -bar");
    BOOST_CHECK_EQUAL(GetArg("-PLB", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PLB", "eleven"), "");

    ResetArgs("-PLB=");
    BOOST_CHECK_EQUAL(GetArg("-PLB", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-PLB", "eleven"), "");

    ResetArgs("-PLB=11");
    BOOST_CHECK_EQUAL(GetArg("-PLB", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-PLB", "eleven"), "11");

    ResetArgs("-PLB=eleven");
    BOOST_CHECK_EQUAL(GetArg("-PLB", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-PLB", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-PLB", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-PLB", 0), 0);

    ResetArgs("-PLB -bar");
    BOOST_CHECK_EQUAL(GetArg("-PLB", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-PLB=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-PLB", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-PLB=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-PLB", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--PLB");
    BOOST_CHECK_EQUAL(GetBoolArg("-PLB"), true);

    ResetArgs("--PLB=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-PLB", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noPLB");
    BOOST_CHECK(!GetBoolArg("-PLB"));
    BOOST_CHECK(!GetBoolArg("-PLB", true));
    BOOST_CHECK(!GetBoolArg("-PLB", false));

    ResetArgs("-noPLB=1");
    BOOST_CHECK(!GetBoolArg("-PLB"));
    BOOST_CHECK(!GetBoolArg("-PLB", true));
    BOOST_CHECK(!GetBoolArg("-PLB", false));

    ResetArgs("-noPLB=0");
    BOOST_CHECK(GetBoolArg("-PLB"));
    BOOST_CHECK(GetBoolArg("-PLB", true));
    BOOST_CHECK(GetBoolArg("-PLB", false));

    ResetArgs("-PLB --noPLB");
    BOOST_CHECK(GetBoolArg("-PLB"));

    ResetArgs("-noPLB -PLB"); // PLB always wins:
    BOOST_CHECK(GetBoolArg("-PLB"));
}

BOOST_AUTO_TEST_SUITE_END()
