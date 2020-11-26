#include <vector>
#include <set>

#include "regex.hpp"
#include "gtest/gtest.h"

using namespace std;
using namespace regex;

TEST(Regex, BasicOperations) {
    {
        auto re = Regex::from_string("a");
        ASSERT_TRUE(re.accept("a"));
        ASSERT_FALSE(re.accept("b"));
    }
    {
        auto re = Regex::from_string("ab+");
        ASSERT_TRUE(re.accept("a"));
        ASSERT_TRUE(re.accept("b"));
        ASSERT_FALSE(re.accept("c"));
        ASSERT_FALSE(re.accept("ab"));
    }
    {
        auto re = Regex::from_string("a*");
        ASSERT_TRUE(re.accept(""));
        ASSERT_TRUE(re.accept("a"));
        ASSERT_TRUE(re.accept("aaa"));
        ASSERT_FALSE(re.accept("b"));
        ASSERT_FALSE(re.accept("abc"));
    }
    {
        auto re = Regex::from_string("ab.");
        ASSERT_TRUE(re.accept("ab"));
        ASSERT_FALSE(re.accept("a"));
        ASSERT_FALSE(re.accept("ba"));
    }
}

TEST(Regex, ComplexRegex) {
    {
        auto re = Regex::from_string("abc++a.c*.");
        ASSERT_TRUE(re.accept("baccc"));
        ASSERT_TRUE(re.accept("ca"));
        ASSERT_TRUE(re.accept("aa"));
        ASSERT_FALSE(re.accept("aaa"));
        ASSERT_FALSE(re.accept("ccc"));
    }
    {
        auto re = Regex::from_string("c*b*.ab+.");
        ASSERT_TRUE(re.accept("a"));
        ASSERT_TRUE(re.accept("b"));
        ASSERT_TRUE(re.accept("cbb"));
        ASSERT_TRUE(re.accept("bb"));
        ASSERT_TRUE(re.accept("cccbbba"));
        ASSERT_FALSE(re.accept("bbbab"));
        ASSERT_FALSE(re.accept("ccbbac"));
        ASSERT_FALSE(re.accept("accbb"));
    }
}

TEST(Regex, LargeAcceptableSubstring) {
    {
        string regex = "ab+c.";
        string str = "baba";

        auto substr = Regex::large_acceptable_substring(regex, str);
        ASSERT_TRUE(substr.length() == 0);
    }
    {
        string regex = "ab+c.aba.*.bac.+.+*";
        string str = "babc";

        auto substr = Regex::large_acceptable_substring(regex, str);
        ASSERT_TRUE(substr.length() == 2);
        ASSERT_TRUE(str.find(substr) != string::npos);
    }
    {
        string regex = "acb..bab.c.*.ab.ba.+.+*a.";
        string str = "abbaa";

        auto substr = Regex::large_acceptable_substring(regex, str);
        ASSERT_TRUE(substr.length() == 4);
        ASSERT_TRUE(str.find(substr) != string::npos);
    }
}
