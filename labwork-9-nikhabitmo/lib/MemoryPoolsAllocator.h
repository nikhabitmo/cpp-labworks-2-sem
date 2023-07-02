#include <iostream>
#include <limits>
#include <vector>

struct Chunk {
  static constexpr int kBasicSize = 4;
  static constexpr int kBasicNumber = 16;
  Chunk* next = nullptr;
  Chunk* prev = nullptr;
};

class Pool {
 public:
  static constexpr int cChunkStructSize = sizeof(Chunk);

  [[nodiscard]] size_t size() const {
    return accessible_size_;
  }

  [[nodiscard]] size_t size_of_chunk() const {
    return size_of_chunk_;
  }

  [[nodiscard]] size_t number_of_chunks() const {
    return number_of_chunks_;
  }

  [[nodiscard]] Chunk* allocated_chunk_begin() const {
    return allocated_chunk_begin_;
  }

  [[nodiscard]] Chunk* allocated_chunk_end() const {
    return allocated_chunk_end_;
  }

  [[nodiscard]] Chunk* free_chunks_begin() const {
    return free_chunks_begin_;
  }

  [[nodiscard]] Chunk* free_chunks_end() const {
    return free_chunks_end_;
  }

  explicit Pool(size_t number_of_chunks = Chunk::kBasicNumber, size_t size_of_chunk = Chunk::kBasicSize)
      : number_of_chunks_(number_of_chunks),
        size_of_chunk_(size_of_chunk),
        accessible_size_(number_of_chunks * size_of_chunk) {
    void* mem = ::operator new(number_of_chunks * (size_of_chunk_ + cChunkStructSize));
    for (size_t i = 0; i < number_of_chunks; ++i) {
      auto* current_chunk =
          static_cast<Chunk*>(static_cast<void*>(static_cast<char*>(mem) + i * (size_of_chunk_ + cChunkStructSize)));
      current_chunk->prev = free_chunks_end_;
      current_chunk->next = nullptr;

      if (free_chunks_begin_ == nullptr) {
        free_chunks_begin_ = current_chunk;
      }

      if (free_chunks_end_ != nullptr) {
        free_chunks_end_->next = current_chunk;
      }

      free_chunks_end_ = current_chunk;
    }
  }

  ~Pool() = default;

  void* allocate(size_t size) {
    size_t number = (size + size_of_chunk_ - 1) / size_of_chunk_;
    Chunk* current_chunk = search(number);
    if (current_chunk != nullptr) {
      Chunk* temp = current_chunk;
      for (size_t i = 0; i < number; ++i) {
        if (temp->prev != nullptr) {
          temp->prev->next = temp->next;
        }
        if (current_chunk->next != nullptr) {
          temp->next->prev = temp->prev;
        }
        if (temp == free_chunks_begin_) {
          free_chunks_begin_ = free_chunks_begin_->next;
        }
        temp = temp->next;
      }
      current_chunk->next = nullptr;
      current_chunk->prev = allocated_chunk_end_;
      if (allocated_chunk_begin_ == nullptr) {
        allocated_chunk_begin_ = current_chunk;
      }
      if (allocated_chunk_end_ != nullptr) {
        allocated_chunk_end_->next = current_chunk;
      }
      allocated_chunk_end_ = current_chunk;
    }
    return current_chunk;
  }

  void deallocate(Chunk* ptr, size_t size) {
    size_t number = (size + size_of_chunk_ - 1) / size_of_chunk_;
    auto* current_chunk = static_cast<Chunk*>(static_cast<void*>(ptr));
    Chunk* temp_end = nullptr;
    Chunk* temp_begin = nullptr;
    Chunk* it = allocated_chunk_begin_;
    for (size_t i = 0; i < number_of_chunks_; ++i) {
      if (current_chunk == it) {
        temp_end = it->prev;
        temp_begin = it->next;
        if (it == allocated_chunk_begin_) {
          allocated_chunk_begin_ = temp_begin;
        }
        if (it == allocated_chunk_end_) {
          allocated_chunk_end_ = temp_end;
        }
        if (temp_end != nullptr) {
          temp_end->next = temp_begin;
        }
        if (temp_begin != nullptr) {
          temp_begin->prev = temp_end;
        }
        break;
      }
      it = it->next;
    }
    if (temp_end != nullptr) {
      temp_end->next = nullptr;
    }
    if (temp_begin != nullptr) {
      temp_begin->prev = nullptr;
    }
    if (free_chunks_begin_ == nullptr) {
      free_chunks_begin_ = current_chunk;
    }
    if (free_chunks_end_ != nullptr) {
      free_chunks_end_->next = current_chunk;
    }
    current_chunk->prev = free_chunks_end_;
    current_chunk->next = nullptr;
    free_chunks_end_ = current_chunk;
  }

 private:
  [[nodiscard]] Chunk* search(size_t number) {
    Chunk* current_chunk = free_chunks_begin_;
    while (current_chunk != nullptr) {
      size_t count = 0;
      Chunk* it = current_chunk;
      while (it != nullptr && count < number) {
        ++count;
        it = it->next;
      }
      if (count == number) {
        return current_chunk;
      }
      current_chunk = current_chunk->next;
    }
    return nullptr;
  }

  size_t number_of_chunks_;
  size_t size_of_chunk_;
  size_t accessible_size_;
  Chunk* free_chunks_begin_ = nullptr;
  Chunk* free_chunks_end_ = nullptr;
  Chunk* allocated_chunk_begin_ = nullptr;
  Chunk* allocated_chunk_end_ = nullptr;
};

template<typename T>
class PoolAllocator {
 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;

  template<typename U>
  struct rebind {
    using other = PoolAllocator<U>;
  };

  PoolAllocator() noexcept = default;

  template<typename U>
  explicit PoolAllocator(const PoolAllocator<U>&) noexcept {}

  [[nodiscard]] T* allocate(size_t n) {
    if (n > std::numeric_limits<size_type>::max() / sizeof(T))
      throw std::bad_alloc();
    if (auto p = static_cast<T*>(get_pool().allocate(n * sizeof(T))))
      return p;
    throw std::bad_alloc();
  }

  void deallocate(T* p, size_t n) noexcept {
    get_pool().deallocate(reinterpret_cast<Chunk*>(static_cast<void*>(p)), n * sizeof(T));
  }

  template<typename U, typename... Args>
  void construct(U* p, Args&& ... args) {
    ::new(static_cast<void*>(p)) U(std::forward<Args>(args)...);
  }

  template<typename U>
  void destroy(U* p) noexcept {
    p->~U();
  }

  friend bool operator==(const PoolAllocator<value_type>& lhs, const PoolAllocator<value_type>& rhs) {
    if (lhs.current_number_of_pools_ == rhs.current_number_of_pools_) {
      for (size_t i = 0; i < lhs.current_number_of_pools_; ++i) {
        if (lhs.all_pools_[i] != rhs.all_pools_[i]) {
          return false;
        }
      }
      return true;
    }
    return false;
  }

  friend bool operator!=(const PoolAllocator<value_type>& lhs, const PoolAllocator<value_type>& rhs) {
    return !(lhs == rhs);
  }

 private:
  Pool& get_pool() {
    if (pools_.empty()) {
      pools_.emplace_back();
    }
    return pools_.back();
  }

  std::vector<Pool> pools_;
};
