#include "lib/AlgorithmsPY.h"
#include "lib/AlgorithmsSTL.h"
#include <iostream>
#include <list>
#include <vector>

bool is_positive(int i) {
  return i > 0;
}

bool descending(int a, int b) {
  return a >= b;
}

int main() {

  std::vector<int> v = {1, 2, 3, 4, 5};
  bool all_positive = MyStl::all_of(v.begin(), v.end(), is_positive);

  std::vector<int> vec = {5, 4, 3, 2, 1};
  bool is_descending = MyStl::is_sorted(v.begin(), v.end(), descending);

  if (is_descending)
    std::cout << "The vector is sorted in descending order." << std::endl;
  else
    std::cout << "The vector is not sorted in descending order." << std::endl;

  if (all_positive) {
    std::cout << "All elements are positive\n";
  } else {
    std::cout << "Not all elements are positive\n";
  }

  std::cout << "works\n";

  std::cout << MyPy::xrange(1,10,4) << '\n';
  std::cout << MyPy::xrange(1,10) << '\n';
  std::cout << MyPy::xrange(10) << '\n';

  auto a = MyPy::xrange(1.5,10.5);
  std::cout << a << '\n';

  for(auto i : MyPy::xrange(1, 6, 2)) {
    std::cout << i <<  " ";
  }

  std::cout << '\n';
//
  auto x = MyPy::xrange<float>('a', 'z');
//  std::cout << '\n' << *x.begin() << '\n';
  std::vector<float> v2{x.begin(), x.end()}; // 1.5 2.5 3.5 4.5
//  std::cout << *x.end();
  for(float i : v2) {
    std::cout << char(i) <<  " ";
  }
//
  std::cout << '\n';

  std::list l = {1, 2, 3, 4, 5};
  std::vector m = {'a', 'b', 'c', 'd'};
/*
1 a
2 b
3 c
4 d
*/

  for(auto value : MyPy::zip(l, m)) {
    std::cout << value.first << " " << value.second << std::endl;
  }



//  std::cout << '\n' << MyPy::zip(l,m);
//
//  auto x = MyPy::xrange(1.0, 2.0, 0.3);
//  std::cout << *(x.end()) << '\n';
//  std::vector<double> v(x.begin(), x.end());
//  for (auto i : v) {
//    std::cout << i << " ";
//  }

//  auto x = MyPy::xrange(1.0, 4.1);
//  std::cout << *(x.end());
//  std::vector<double> v(x.begin(), x.end());
//  for (auto i : v) {
//    std::cout << i << " ";
//  }

  return 0;
}