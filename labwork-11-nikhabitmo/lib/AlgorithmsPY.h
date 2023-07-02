#pragma once
#include <iterator>

template<typename T1, typename T2>
std::ostream& operator<<(std::ostream& ostream, const std::pair<T1, T2>& p) {
  ostream << p.first << " " << p.second;
  return ostream;
}

namespace MyPy {

template<typename T>
class xrange {
 public:
  xrange(const T& begin, const T& end, const T& step = 1)
      : begin_(begin), end_(end), step_(step) {};

  explicit xrange(const T& end)
      : begin_(0), end_(end), step_(1) {};

  class iterator : public std::iterator<std::input_iterator_tag, T> {
   public:
    iterator(const T& value, const T& step, const T& end)
        : current(value), step_(step), end_(end) {}

    const T& operator*() const {
      return current;
    }

    iterator& operator++() {
      current += step_;
      return *this;
    }

    bool operator!=(const iterator& other) const {
      return step_ > 0 ? current < other.end_ : current > other.end_;
    }

    bool operator==(const iterator& other) const {
      return !(*this != other);
    }

   private:
    T current;
    T step_;
    T end_;
  };

  iterator begin() const {
    return iterator(begin_, step_, end_);
  }

  iterator end() const {
    return iterator(end_, step_, end_);
  }

  friend std::ostream& operator<<(std::ostream& ostream, const xrange& _xrange) {
    for (auto& it : _xrange) {
      ostream << it << ' ';
    }
    return ostream;
  }

 private:
  T begin_;
  T end_;
  T step_;
};

template<typename Container1, typename Container2>
class Zip {
  template<typename InputIt1, typename InputIt2>
  class ZipIterator : public std::iterator<std::input_iterator_tag,
                                           std::pair<typename InputIt1::value_type, typename InputIt2::value_type>> {
   public:
    using value_type = std::pair<typename InputIt1::value_type, typename InputIt2::value_type>;
    using pointer = value_type*;
    using reference = value_type&;

    ZipIterator(InputIt1 it1, InputIt2 it2)
        : it1_(it1), it2_(it2) {}

    reference operator*() {
      current_pair_.first = *it1_;
      current_pair_.second = *it2_;
      return current_pair_;
    }

    pointer operator->() const {
      return &current_pair_;
    }

    const ZipIterator& operator++() {
      ++it1_;
      ++it2_;
      return *this;
    }

    const ZipIterator operator++(int) {
      ZipIterator it = *this;
      ++(*this);
      return it;
    }

    bool operator==(const ZipIterator& other) const {
      return it1_ == other.it1_ && it2_ == other.it2_;
    }

    bool operator!=(const ZipIterator& other) const {
      return !(*this == other);
    }

   private:
    InputIt1 it1_;
    InputIt2 it2_;
    value_type current_pair_;
  };

 public:
  using iterator = ZipIterator<typename Container1::const_iterator, typename Container2::const_iterator>;

  Zip(const Container1& c1, const Container2& c2) : c1_(c1), c2_(c2) {}

  [[nodiscard]] iterator begin() const {
    return iterator(c1_.cbegin(), c2_.cbegin());
  }

  [[nodiscard]] iterator end() const {
    size_t size1 = std::distance(c1_.cbegin(), c1_.cend());
    size_t size2 = std::distance(c2_.cbegin(), c2_.cend());
    size_t min_size = std::min(size1, size2);
    return iterator(std::next(c1_.cbegin(), min_size), std::next(c2_.cbegin(), min_size));
  }

  friend std::ostream& operator<<(std::ostream& ostream, const Zip<Container1, Container2>& zip) {
    for (auto& it : zip) {
      ostream << it << '\n';
    }
    return ostream;
  }

 private:
  const Container1& c1_;
  const Container2& c2_;
};

template<typename Container1, typename Container2>
Zip<Container1, Container2> zip(const Container1& c1, const Container2& c2) {
  return Zip<Container1, Container2>(c1, c2);
}

}
