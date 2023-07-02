#pragma once

namespace MyStl {

template<typename InputIterator, typename Predicate>
bool all_of(InputIterator first, const InputIterator& last, Predicate predicate) {
  for (; first != last; ++first) {
    if (!predicate(*first)) {
      return false;
    }
  }
  return true;
}

template<typename InputIterator, typename Predicate>
bool any_of(InputIterator first, const InputIterator& last, Predicate predicate) {
  for (; first != last; ++first) {
    if (predicate(*first)) {
      return true;
    }
  }
  return false;
}

template<typename InputIterator, typename Predicate>
bool none_of(InputIterator first, const InputIterator& last, Predicate predicate) {
  for (; first != last; ++first) {
    if (predicate(*first)) {
      return false;
    }
  }
  return true;
}

template<typename InputIterator, typename Predicate>
bool one_of(InputIterator first, const InputIterator& last, Predicate predicate) {
  bool flag = false;
  for (; first != last; ++first) {
    if (flag) {
      return false;
    }
    if (predicate(*first)) {
      flag = true;
    }
  }
  return false;
}

template<typename ForwardIterator, typename Predicate>
bool is_sorted(ForwardIterator first, const ForwardIterator& last, Predicate predicate) {
  for (ForwardIterator it = first + 1; it != last; ++it) {
    if (!predicate(*it, *(it - 1))) {
      return false;
    }
  }
  return true;
}

template<typename InputIterator, typename Predicate>
bool is_partitioned(InputIterator first, const InputIterator& last, Predicate predicate) {
  for (; first != last; ++first) {
    if (!predicate(*first)) {
      break;
    }
  }
  for (; first != last; ++first) {
    if (predicate(*first)) {
      return false;
    }
  }
  return true;
}

template<typename InputIterator, typename T>
InputIterator find_not(InputIterator first, const InputIterator& last, const T& value) {
  for (InputIterator it = first; it != last; ++it) {
    if (*it != value) {
      return it;
    }
  }
  return last;
}

template<typename BidirectionalIterator, typename T>
BidirectionalIterator find_backward(BidirectionalIterator first, BidirectionalIterator last, const T& value) {
  auto it = last - 1;
  for (; it != first; --it)
    if (*it == value) {
      return it;
    }
  if (*first == value) {
    return first;
  }
  return last;
}

template<typename RandomAccessIterator, typename Predicate>
bool is_palindrome(RandomAccessIterator first, RandomAccessIterator last, Predicate predicate) {
  for (; first <= last;) {
    if (!predicate(*(first++), *(--last))) {
      return false;
    }
  }
  return true;
}

};