#include <gtest/gtest.h>
#include <lib/AlgorithmsPY.h>
#include <lib/AlgorithmsSTL.h>
#include <vector>

bool is_even(int i) {
  return i % 2 == 0;
}

bool is_negative(int i) {
  return i < 0;
}

bool is_positive(int i) {
  return i > 0;
}

bool descending(int a, int b) {
  return a >= b;
}

class AllAnyNoneOneOfTest : public ::testing::Test {
 protected:
  std::vector<int> v1{1, 2, 3, 4, 5};
  std::vector<int> v2{2, 4, 6, 8, 10};
  std::vector<int> v3{1, 3, 5, 7, 9};
  std::vector<int> v4{-10, 216, -2, 12, 0};
};

TEST_F(AllAnyNoneOneOfTest, AllOfBasicTests) {
  EXPECT_FALSE(MyStl::all_of(v1.begin(), v1.end(), is_even));
  EXPECT_TRUE(MyStl::all_of(v2.begin(), v2.end(), is_even));
  EXPECT_FALSE(MyStl::all_of(v3.begin(), v3.end(), is_even));
  EXPECT_TRUE(MyStl::all_of(v1.begin(), v1.end(), is_positive));
  EXPECT_TRUE(MyStl::all_of(v2.begin(), v2.end(), is_positive));
  EXPECT_TRUE(MyStl::all_of(v3.begin(), v3.end(), is_positive));
}

TEST_F(AllAnyNoneOneOfTest, AnyOfBasicTests) {
  EXPECT_TRUE(MyStl::any_of(v1.begin(), v1.end(), is_even));
  EXPECT_TRUE(MyStl::any_of(v2.begin(), v2.end(), is_even));
  EXPECT_FALSE(MyStl::any_of(v3.begin(), v3.end(), is_even));

  EXPECT_TRUE(MyStl::any_of(v1.begin(), v1.end(), is_positive));
  EXPECT_TRUE(MyStl::any_of(v2.begin(), v2.end(), is_positive));
  EXPECT_TRUE(MyStl::any_of(v3.begin(), v3.end(), is_positive));

  EXPECT_FALSE(MyStl::any_of(v1.begin(), v1.end(), is_negative));
  EXPECT_FALSE(MyStl::any_of(v2.begin(), v2.end(), is_negative));
  EXPECT_TRUE(MyStl::any_of(v4.begin(), v4.end(), is_negative));
}

TEST_F(AllAnyNoneOneOfTest, NoneOfBasicTests) {
  EXPECT_FALSE(MyStl::none_of(v1.begin(), v1.end(), is_even));
  EXPECT_FALSE(MyStl::none_of(v2.begin(), v2.end(), is_even));
  EXPECT_TRUE(MyStl::none_of(v3.begin(), v3.end(), is_even));

  EXPECT_FALSE(MyStl::none_of(v1.begin(), v1.end(), is_positive));
  EXPECT_FALSE(MyStl::none_of(v2.begin(), v2.end(), is_positive));
  EXPECT_FALSE(MyStl::none_of(v3.begin(), v3.end(), is_positive));

  EXPECT_TRUE(MyStl::none_of(v1.begin(), v1.end(), is_negative));
  EXPECT_TRUE(MyStl::none_of(v2.begin(), v2.end(), is_negative));
  EXPECT_FALSE(MyStl::none_of(v4.begin(), v4.end(), is_negative));
}

TEST_F(AllAnyNoneOneOfTest, OneOfBasicTests) {
  EXPECT_FALSE(MyStl::one_of(v1.begin(), v1.end(), is_even));
  EXPECT_FALSE(MyStl::one_of(v2.begin(), v2.end(), is_even));
  EXPECT_FALSE(MyStl::one_of(v3.begin(), v3.end(), is_even));

  EXPECT_FALSE(MyStl::one_of(v1.begin(), v1.end(), is_positive));
  EXPECT_FALSE(MyStl::one_of(v3.begin(), v3.end(), is_positive));
  EXPECT_FALSE(MyStl::one_of(v2.begin(), v2.end(), is_positive));

  EXPECT_FALSE(MyStl::one_of(v1.begin(), v1.end(), is_negative));
  EXPECT_FALSE(MyStl::one_of(v4.begin(), v4.end(), is_negative));
  EXPECT_FALSE(MyStl::one_of(v2.begin(), v2.end(), is_negative));
}

TEST(IsSortedTest, BasicTests) {
  std::vector<int> v1{1, 2, 3, 4, 5};
  std::vector<int> v2{5, 4, 3, 2, 1};
  std::vector<int> v3{-3, -2, -1, 0, 1, 2, 3};

  EXPECT_TRUE(MyStl::is_sorted(v1.begin(), v1.end(), descending));
  EXPECT_FALSE(MyStl::is_sorted(v2.begin(), v2.end(), std::greater<int>()));
  EXPECT_TRUE(MyStl::is_sorted(v3.begin(), v3.end(), descending));
  EXPECT_FALSE(MyStl::is_sorted(v3.begin(), v3.end(), std::less<int>()));
}

TEST(IsPartitionedTest, BasicTests) {
  std::vector<int> v1{2, 4, 6, 1, 3, 5};
  std::vector<int> v2{1, 3, 5, 2, 4, 6};
  std::vector<int> v3{-5, -4, 0, 1, 2, 3};
  std::vector<int> v4{1, 2, 3, 4, 5};

  EXPECT_TRUE(MyStl::is_partitioned(v1.begin(), v1.end(), is_even));
  EXPECT_FALSE(MyStl::is_partitioned(v2.begin(), v2.end(), is_even));
  EXPECT_TRUE(MyStl::is_partitioned(v3.begin(), v3.end(), is_negative));
  EXPECT_TRUE(MyStl::is_partitioned(v4.begin(), v4.end(), is_positive));
}

TEST(FindNotTest, BasicTests) {
  std::vector<int> v1{2, 4, 6, 8, 10};
  std::vector<int> v2{1, 3, 5, 7, 9};
  std::vector<int> v3{1, 2, 3, 2, 1};
  std::vector<int> v4{2, 2, 2, 2, 2};

  EXPECT_EQ(MyStl::find_not(v1.begin(), v1.end(), 2), v1.begin() + 1);
  EXPECT_EQ(MyStl::find_not(v2.begin(), v2.end(), 1), v2.begin() + 1);
  EXPECT_EQ(MyStl::find_not(v3.begin(), v3.end(), 2), v3.begin());
  EXPECT_EQ(MyStl::find_not(v3.begin(), v3.end(), 3), v3.begin());
  EXPECT_EQ(MyStl::find_not(v3.begin(), v3.end(), 1), v3.begin() + 1);
  EXPECT_EQ(MyStl::find_not(v4.begin(), v4.end(), 2), v4.end());
}

TEST(FindBackwardTests, BasicTests) {
  std::vector<int> v1{1, 2, 3, 4, 5, 6};
  std::vector<int> v2{6, 5, 4, 3, 2, 1};
  std::vector<int> v3{1, 2, 3, 4, 5, 6};
  std::vector<int> v4{1, 2, 3, 4, 5, 6, 3, 7, 8, 3};
  std::vector<int> v5{3};

  ASSERT_EQ(*MyStl::find_backward(v1.begin(), v1.end(), 3), 3);
  ASSERT_EQ(*MyStl::find_backward(v2.begin(), v2.end(), 3), 3);
  ASSERT_EQ(MyStl::find_backward(v3.begin(), v3.end(), 7), v3.end());
  ASSERT_EQ(*MyStl::find_backward(v4.begin(), v4.end(), 7), 7);
  ASSERT_EQ(*MyStl::find_backward(v5.begin(), v5.end(), 3), 3);
}

TEST(IsPalindromeTest, BasicTests) {
  std::vector<int> v1{1, 2, 3, 2, 1};
  std::string s1{"abcba"};
  std::vector<double> v2{1.0, 2.0, 1.0};
  std::vector<std::string> v3{"ITMO", "is the best", "is the best", "ITMO"};
  EXPECT_TRUE(MyStl::is_palindrome(v1.begin(), v1.end(), std::equal_to<int>()));
  EXPECT_TRUE(MyStl::is_palindrome(s1.begin(), s1.end(), std::equal_to<char>()));
  EXPECT_TRUE(MyStl::is_palindrome(v2.begin(), v2.end(), [](double a, double b) { return a == b; }));
  EXPECT_TRUE(MyStl::is_palindrome(v3.begin(), v3.end(), std::equal_to<std::string>()));

  std::vector<int> n1{1, 2, 3, 2, 5};
  std::string n_s{"abcde"};
  std::vector<double> n2{1.0, 2.0, 3.0};
  std::vector<std::string> n3{"ITMO", "is the best", "it's not a joke"};
  EXPECT_FALSE(MyStl::is_palindrome(n1.begin(), n1.begin(), std::equal_to<int>()));
  EXPECT_FALSE(MyStl::is_palindrome(n_s.begin(), n_s.begin(), std::equal_to<char>()));
  EXPECT_FALSE(MyStl::is_palindrome(n2.begin(), n2.end(), [](double a, double b) { return a == b; }));
  EXPECT_FALSE(MyStl::is_palindrome(n3.begin(), n3.end(), std::equal_to<std::string>()));
}

TEST(XRangeTest, BasicIntTest) {
  std::vector<int> result{1, 2, 3, 4, 5};
  std::vector<int> current;
  for (auto& i : MyPy::xrange(1, 6)) {
    current.push_back(i);
  }
  ASSERT_EQ(result, current
  );
}

TEST(XRangeTest, NegativeFloatStep) {
  std::vector<float> result{5.5, 3.5, 1.5, -0.5, -2.5};
  std::vector<float> current;
  for (auto& i : MyPy::xrange(5.5, -4.0, -2.0)) {
    current.push_back(i);
  }
  ASSERT_EQ(result, current);
}

TEST(XRangeTest, SingleXRangeargument) {
  std::vector<int> result{0, 1, 2, 3, 4};
  std::vector<int> current;
  for (auto i : MyPy::xrange(5)) {
    current.push_back(i);
  }
  ASSERT_EQ(result, current);
}

TEST(ZipTest, BasicTest) {
  std::vector<int> v1{1, 2, 3, 4, 5};
  std::vector<std::string> v2{"one", "two", "three", "four", "five"};
  std::vector<std::pair<int, std::string>> result{{1, "one"}, {2, "two"}, {3, "three"}, {4, "four"}, {5, "five"}};
  std::vector<std::pair<int, std::string>> current;
  for (auto& value : MyPy::zip(v1, v2)) {
    current.push_back({value.first, value.second});
  }
  ASSERT_EQ(result, current);
}

TEST(ZipTest, DifferentSizes) {
  std::vector<int> v1{1, 2, 3, 4, 5};
  std::vector<std::string> v2{"ITMO", "SE", "y26"};
  std::vector<std::pair<int, std::string>> result{{1, "ITMO"}, {2, "SE"}, {3, "y26"}};
  std::vector<std::pair<int, std::string>> current;
  for (auto& value : MyPy::zip(v1, v2)) {
    current.push_back({value.first, value.second});
  }
  ASSERT_EQ(result, current);
}