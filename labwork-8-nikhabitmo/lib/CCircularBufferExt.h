#include <algorithm>
#include <iostream>
#include <memory>
#include <stdexcept>

template<typename T, typename Allocator = std::allocator<T>>
class CCircularBufferExt {
 public:
  using value_type = T;
  using size_type = std::size_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using allocator_type = Allocator;

  class Iterator : public std::random_access_iterator_tag {
   public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using reference = T&;

    Iterator(CCircularBufferExt* buffer, size_t index)
        : buffer_(buffer), index_(index) {}

    reference operator*() const {
      return buffer_->buffer_[index_];
    }

    pointer operator->() const {
      return &(buffer_->buffer_[index_]);
    }

    Iterator& operator++() {
      ++index_;
      return *this;
    }

    Iterator operator++(int) {
      Iterator temp(*this);
      ++index_;
      return temp;
    }

    Iterator& operator--() {
      --index_;
      return *this;
    }

    Iterator operator--(int) {
      Iterator temp(*this);
      --index_;
      return temp;
    }

    Iterator& operator+=(difference_type n) {
      index_ += n;
      return *this;
    }

    Iterator& operator-=(difference_type n) {
      index_ -= n;
      return *this;
    }

    Iterator operator+(difference_type n) const {
      Iterator result(*this);
      result += n;
      return result;
    }

    Iterator operator-(difference_type n) const {
      Iterator result(*this);
      result -= n;
      return result;
    }

    difference_type operator-(const Iterator& other) const {
      return index_ - other.index_;
    }

    reference operator[](difference_type n) const {
      return buffer_->buffer_[(index_ + n) % buffer_->capacity_];
    }

    bool operator==(const Iterator& other) const {
      return buffer_ == other.buffer_ && index_ == other.index_;
    }

    bool operator!=(const Iterator& other) const {
      return !(*this == other);
    }

    bool operator<(const Iterator& other) const {
      return buffer_ == other.buffer_ && index_ < other.index_;
    }

    bool operator<=(const Iterator& other) const {
      return *this < other || *this == other;
    }

    bool operator>(const Iterator& other) const {
      return buffer_ == other.buffer_ && index_ > other.index_;
    }

    bool operator>=(const Iterator& other) const {
      return *this > other || *this == other;
    }

    CCircularBufferExt* buffer_;
    size_t index_;
  };

  class ConstIterator : public std::random_access_iterator_tag {
   public:
    using iterator_category = std::random_access_iterator_tag;
    using value_type = const T;
    using difference_type = std::ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;

    ConstIterator(const CCircularBufferExt* buffer, size_t index)
        : buffer_(buffer), index_(index) {}

    reference operator*() const {
      return buffer_->buffer_[index_];
    }

    pointer operator->() const {
      return &(buffer_->buffer_[index_]);
    }

    ConstIterator& operator++() {
      ++index_;
      return *this;
    }

    ConstIterator operator++(int) {
      ConstIterator temp(*this);
      ++index_;
      return temp;
    }

    ConstIterator& operator--() {
      --index_;
      return *this;
    }

    ConstIterator operator--(int) {
      ConstIterator temp(*this);
      --index_;
      return temp;
    }

    ConstIterator& operator+=(difference_type n) {
      index_ += n;
      return *this;
    }

    ConstIterator& operator-=(difference_type n) {
      index_ -= n;
      return *this;
    }

    ConstIterator operator+(difference_type n) const {
      ConstIterator result(*this);
      result += n;
      return result;
    }

    ConstIterator operator-(difference_type n) const {
      ConstIterator result(*this);
      result -= n;
      return result;
    }

    difference_type operator-(const ConstIterator& other) const {
      return index_ - other.index_;
    }

    reference operator[](difference_type n) const {
      return buffer_->buffer_[(index_ + n) % buffer_->capacity_];
    }

    bool operator==(const ConstIterator& other) const {
      return buffer_ == other.buffer_ && index_ == other.index_;
    }

    bool operator!=(const ConstIterator& other) const {
      return !(*this == other);
    }

    bool operator<(const ConstIterator& other) const {
      return buffer_ == other.buffer_ && index_ < other.index_;
    }

    bool operator<=(const ConstIterator& other) const {
      return *this < other || *this == other;
    }

    bool operator>(const ConstIterator& other) const {
      return buffer_ == other.buffer_ && index_ > other.index_;
    }

    bool operator>=(const ConstIterator& other) const {
      return *this > other || *this == other;
    }

   private:
    const CCircularBufferExt* buffer_;
    size_t index_;
  };

  explicit CCircularBufferExt(size_t capacity)
      : capacity_(capacity),
        buffer_(std::make_unique<T[]>(capacity)),
        buffer_start_(0),
        buffer_end_(0),
        size_(0) {}

  CCircularBufferExt(const CCircularBufferExt& other)
      : capacity_(other.capacity_),
        buffer_(std::make_unique<T[]>(other.capacity_)),
        buffer_start_(other.buffer_start_),
        buffer_end_(other.buffer_end_),
        size_(other.size_) {
    std::copy(other.buffer_.get(), other.buffer_.get() + capacity_, buffer_.get());
  }

  CCircularBufferExt(CCircularBufferExt&& other) noexcept
      : capacity_(other.capacity_),
        buffer_(std::move(other.buffer_)),
        buffer_start_(other.buffer_start_),
        buffer_end_(other.buffer_end_),
        size_(other.size_) {
    other.capacity_ = 0;
    other.buffer_start_ = 0;
    other.buffer_end_ = 0;
    other.size_ = 0;
  }

  CCircularBufferExt& operator=(const CCircularBufferExt& other) {
    if (this == &other) {
      return *this;
    }

    if (capacity_ != other.capacity_) {
      buffer_ = std::make_unique<T[]>(other.capacity_);
      capacity_ = other.capacity_;
    }

    buffer_start_ = other.buffer_start_;
    buffer_end_ = other.buffer_end_;
    size_ = other.size_;

    std::copy(other.buffer_.get(), other.buffer_.get() + capacity_, buffer_.get());

    return *this;
  }

  CCircularBufferExt(const std::initializer_list<T>& initList) : capacity_(initList.size()), size_(0) {
    buffer_ = std::make_unique<T[]>(capacity_);
    for (auto i = 0; i < capacity_; ++i) {
      buffer_[i] = *(initList.begin()+i);
    }
    buffer_start_ = 0;
    buffer_end_ = capacity_;
  }

  CCircularBufferExt& operator=(CCircularBufferExt&& other) noexcept {
    if (this == &other) {
      return *this;
    }

    capacity_ = other.capacity_;
    buffer_ = std::move(other.buffer_);
    buffer_start_ = other.buffer_start_;
    buffer_end_ = other.buffer_end_;
    size_ = other.size_;

    other.capacity_ = 0;
    other.buffer_start_ = 0;
    other.buffer_end_ = 0;
    other.size_ = 0;

    return *this;
  }

  ~CCircularBufferExt() = default;

  void push(const T& value) {
    if (size_ == capacity_) {
      reserve(capacity_ * 2);
      capacity_ = capacity_ * 2;
    }

    buffer_[buffer_end_] = value;
    buffer_end_ = (buffer_end_ + 1) % capacity_;
    ++size_;
  }

  Iterator insert(const Iterator& pos, const T& value) {
    if (pos.buffer_ != this) {
      throw std::invalid_argument("Iterator does not belong to this buffer");
    }

    ptrdiff_t index_diff = pos.index_ - buffer_start_;
    if (index_diff < 0 || index_diff > size_) {
      throw std::out_of_range("Iterator is out of range");
    }

    if (full()) {
      reserve(capacity_ * 2);
    }

    size_t insert_index = (buffer_start_ + index_diff) % capacity_;
    size_t new_buffer_end = (buffer_end_ + 1) % capacity_;

    if (new_buffer_end > insert_index) {
      std::rotate(buffer_.get() + insert_index, buffer_.get() + buffer_end_, buffer_.get() + new_buffer_end);
    } else {
      std::rotate(buffer_.get(), buffer_.get() + buffer_end_, buffer_.get() + capacity_);
      std::rotate(buffer_.get(), buffer_.get() + insert_index, buffer_.get() + capacity_);
    }

    buffer_[insert_index] = value;
    buffer_end_ = new_buffer_end;
    ++size_;

    return Iterator(this, insert_index);
  }

  Iterator erase(const Iterator& pos) {
    if (pos.buffer_ != this) {
      throw std::invalid_argument("Iterator does not belong to this buffer");
    }

    ptrdiff_t index_diff = pos.index_ - buffer_start_;
    if (index_diff < 0 || index_diff >= size_) {
      throw std::out_of_range("Iterator is out of range");
    }

    size_t erase_index = (buffer_start_ + index_diff) % capacity_;
    size_t new_buffer_end = (buffer_end_ - 1 + capacity_) % capacity_;

    if (new_buffer_end > erase_index) {
      std::rotate(buffer_.get() + erase_index, buffer_.get() + erase_index + 1, buffer_.get() + new_buffer_end + 1);
    } else {
      std::rotate(buffer_.get() + erase_index, buffer_.get() + erase_index + 1, buffer_.get() + capacity_);
      std::rotate(buffer_.get(), buffer_.get() + 1, buffer_.get() + new_buffer_end + 1);
    }

    buffer_end_ = new_buffer_end;
    --size_;

    return Iterator(this, (buffer_start_ + index_diff) % capacity_);
  }

  bool empty() const {
    return size_ == 0;
  }

  bool full() const {
    return size_ == capacity_;
  }

  void clear() {
    buffer_start_ = 0;
    buffer_end_ = 0;
    size_ = 0;
  }

  const T& front() const {
    if (size_ == 0) {
      throw std::out_of_range("Buffer is empty");
    }
    return buffer_[buffer_start_];
  }

  T& front() {
    return const_cast<T&>(static_cast<const CCircularBufferExt*>(this)->front());
  }

  const T& back() const {
    if (size_ == 0) {
      throw std::out_of_range("Buffer is empty");
    }
    return buffer_[(buffer_end_ - 1 + capacity_) % capacity_];
  }

  T& back() {
    return const_cast<T&>(static_cast<const CCircularBufferExt*>(this)->back());
  }

  void push_back(const T& value) {
    if (size_ == capacity_ - 1) {
      reserve(capacity_ * 2);
    }
    buffer_end_ = (buffer_end_ + 1) % capacity_;
    buffer_[buffer_end_] = value;
    ++size_;
  }

  void pop_back() {
    if (size_ == 0) {
      throw std::out_of_range("Buffer is empty");
    }
    buffer_end_ = (buffer_end_ - 1 + capacity_) % capacity_;
    --size_;
  }

  void pop_front() {
    if (size_ == 0) {
      throw std::out_of_range("Buffer is empty");
    }

    buffer_start_ = (buffer_start_ + 1) % capacity_;
    --size_;
  }

  void resize(size_t new_size) {
    if (new_size > capacity_) {
      reserve(new_size);
    }

    if (new_size > size_) {
      std::fill(buffer_.get() + buffer_end_, buffer_.get() + buffer_end_ + (new_size - size_), T());
    }

    buffer_end_ = (buffer_end_ + (new_size - size_)) % capacity_;
    size_ = new_size;
  }

  void reserve(size_t new_capacity) {
    if (new_capacity <= capacity_) {
      return;
    }

    std::unique_ptr<T[]> new_buffer = std::make_unique<T[]>(new_capacity);
    if (size_ > 0) {
      if (buffer_start_ < buffer_end_) {
        std::copy(buffer_.get() + buffer_start_, buffer_.get() + buffer_end_, new_buffer.get());
      } else {
        std::copy(buffer_.get() + buffer_start_, buffer_.get() + capacity_, new_buffer.get());
        std::copy(buffer_.get(), buffer_.get() + buffer_end_, new_buffer.get() + capacity_ - buffer_start_);
      }
    }
    capacity_ = new_capacity;
    buffer_ = std::move(new_buffer);
    buffer_start_ = 0;
    buffer_end_ = size_;
  }

  void shrink_to_fit() {
    if (size_ < capacity_) {
      reserve(size_);
    }
  }

  size_t size() const {
    return size_;
  }

  size_t capacity() const {
    return capacity_;
  }

  const T& at(size_t index) const {
    if (index >= size_) {
      throw std::out_of_range("Index out of range");
    }

    return buffer_[(buffer_start_ + index) % capacity_];
  }

  T& at(size_t index) {
    return const_cast<T&>(static_cast<const CCircularBufferExt*>(this)->at(index));
  }

  template<typename... Args>
  void emplace(const T& value, Args&& ... args) {
    if (size_ == capacity_) {
      reserve(capacity_ * 2);
      capacity_ = capacity_ * 2;
    }

    buffer_end_ = (buffer_end_ + 1) % capacity_;
    buffer_[buffer_end_] = T(std::forward<Args>(args)...);
    ++size_;
  }

  void assign(size_t count, const T& value) {
    clear();
    resize(count);
    std::fill_n(begin(), count, value);
  }

  void assign(Iterator first, Iterator last) {
    clear();
    for (auto it = first; it != last; ++it) {
      push(*it);
    }
  }

  void assign(std::initializer_list<T> init_list) {
    assign(init_list.begin(), init_list.end());
  }

  using iterator = Iterator;
  using const_iterator = ConstIterator;

  iterator begin() {
    return iterator(this, buffer_start_);
  }

  iterator end() {
    return iterator(this, buffer_end_);
  }

  const_iterator cbegin() const {
    return ConstIterator(const_cast<CCircularBufferExt*>(this), buffer_start_);
  }

  const_iterator cend() const {
    return const_iterator(const_cast<CCircularBufferExt*>(this), buffer_end_);
  }

 private:
  size_t capacity_;
  std::unique_ptr<T[]> buffer_;
  size_t buffer_start_;
  size_t buffer_end_;
  size_t size_;
  Allocator allocator_;
};

template<typename T, typename Allocator = std::allocator<T>>
class CCircularBuffer : public CCircularBufferExt<T, Allocator> {
 public:
  using Base = CCircularBufferExt<T, Allocator>;
  using typename Base::value_type;
  using typename Base::size_type;
  using typename Base::reference;
  using typename Base::const_reference;
  using typename Base::allocator_type;
  using typename Base::Iterator;

  explicit CCircularBuffer(size_type capacity)
      : Base(capacity) {}

  void push_back(const_reference value) {
    if (Base::size() == Base::capacity()) {
      Base::erase(Base::begin());
    }
    Base::push_back(value);
  }

  Iterator insert(Iterator pos, const_reference value) {
    if (Base::size() == Base::capacity()) {
      Base::erase(Base::begin());
    }
    return Base::insert(pos, value);
  }

  template<typename... Args>
  Iterator emplace(Iterator pos, Args&& ... args) {
    if (Base::size() == Base::capacity()) {
      Base::erase(Base::begin());
    }
    return Base::emplace(pos, std::forward<Args>(args)...);
  }
};
