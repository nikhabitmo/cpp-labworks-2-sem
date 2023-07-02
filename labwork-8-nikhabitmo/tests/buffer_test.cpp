#include <lib/CCircularBufferExt.h>
#include <gtest/gtest.h>
#include <algorithm>
#include <numeric>

TEST(CCircularBufferTest, EmptyBufferTest) {
  CCircularBuffer<int> buffer(5);
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_TRUE(buffer.empty());
  EXPECT_FALSE(buffer.full());
}

TEST(CCircularBufferExtTest, EmptyBuffer) {
  CCircularBufferExt<int> buffer(10);
  EXPECT_TRUE(buffer.empty());
  EXPECT_EQ(buffer.size(), 0);
}

TEST(CCircularBufferExtTest, PushAndPop) {
  CCircularBufferExt<int> buffer(5);
  buffer.push(1);
  buffer.push(2);
  buffer.push(3);

  EXPECT_EQ(buffer.front(), 1);
  EXPECT_EQ(buffer.back(), 3);
  EXPECT_EQ(buffer.size(), 3);

  buffer.pop_front();
  EXPECT_EQ(buffer.front(), 2);
  EXPECT_EQ(buffer.size(), 2);

  buffer.pop_back();
  EXPECT_EQ(buffer.back(), 2);
  EXPECT_EQ(buffer.size(), 1);
}

TEST(CCircularBufferExtTest, Iterator) {
  CCircularBufferExt<int> buffer(5);
  buffer.push(1);
  buffer.push(2);
  buffer.push(3);
  buffer.push(4);
  int sum = 0;
  for (auto it = buffer.begin(); it != buffer.end(); ++it) {
    sum += *it;
  }
  EXPECT_EQ(sum, 10);
}

TEST(CCircularBufferExtTest, InsertAndErase) {
  CCircularBufferExt<int> buffer(4);
  buffer.push(1);
  buffer.push(4);
  buffer.push(4);
  EXPECT_EQ(buffer.size(), 3);
  EXPECT_EQ(buffer.front(), 1);
  EXPECT_EQ(buffer.back(), 4);
}

TEST(CCircularBufferExtTest, Resize) {
  CCircularBufferExt<int> buffer(4);
  buffer.push_back(1);
  buffer.push_back(2);
  buffer.push_back(3);
  buffer.resize(6);
  EXPECT_EQ(buffer.size(), 6);
  EXPECT_EQ(buffer.front(), 0);
  EXPECT_EQ(buffer.back(), 0);
}

TEST(CCircularBufferExtTest, Assign) {
  CCircularBufferExt<int> buffer(5);
  buffer.assign(3, 1);
  EXPECT_EQ(buffer.size(), 3);
  CCircularBufferExt<int> otherBuffer(5);
  otherBuffer.push(2);
  otherBuffer.push(3);
  otherBuffer.push(4);
  buffer.assign(otherBuffer.begin(), otherBuffer.end());
  EXPECT_EQ(buffer.size(), 3);
  EXPECT_EQ(buffer.front(), 2);
  EXPECT_EQ(buffer.back(), 4);
}

TEST(CCircularBufferTest, STLAlgorithms) {
  CCircularBufferExt<int> buffer({5,3,2,1,4});

  auto it = std::find(buffer.begin(), buffer.end(), 3);
  EXPECT_EQ(*it, 3);

  int count = std::count(buffer.begin(), buffer.end(), 2);
  EXPECT_EQ(count, 1);

  std::sort(buffer.begin(), buffer.end());
  for (auto it = buffer.begin(); it!= buffer.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << " ********************************************** \n";
  std::reverse(buffer.begin(), buffer.end());

  for (auto it = buffer.begin(); it!= buffer.end(); ++it) {
    std::cout << *it << " ";
  }

  std::cout << " ********************************************** \n";
  CCircularBuffer<int> buff(6);
  buff.push(100);
  buff.push(21600);
  buff.push(1900);
  buff.push(1000);
  buff.push(5000);
  int sum = std::accumulate(buff.begin(), buff.end(),0);
  int sum2 = 100 + 21600 + 1900 + 1000 + 5000;
  std::cout << sum << " == " << sum2 << '\n';
  std::sort(buff.begin(), buff.end());
  for (auto it = buff.begin(); it!= buff.end(); ++it) {
    std::cout << *it << " ";
  }

}

TEST(CCircularBufferExtTest, ClearTest) {
  CCircularBufferExt<int> buffer(5);
  buffer.push(10);
  buffer.push(20);
  buffer.push(30);
  buffer.push(60);
  buffer.push(50);
  buffer.clear();
  EXPECT_EQ(buffer.size(), 0);
  EXPECT_TRUE(buffer.empty());
  EXPECT_FALSE(buffer.full());
}