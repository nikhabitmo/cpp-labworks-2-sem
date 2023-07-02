# Лабораторная работа 8

STL - совместимый контейнер "Циклический буфер", с поддержкой возможности расширения и фиксированным размером.

[Циклический буфер](https://en.wikipedia.org/wiki/Circular_buffer) - структура данных, которая часто применяется для буферизации в задачах по передачи и обработки потока данных.

## Задача

Реализовать два класса:
CCirtucalBuffer и CCircularBufferExt - для циклического буфера и циклического буфера с возможностью расширения (см ниже).

Реализовать циклический буфер для хранения данных произвольного типа в виде stl-совместимого контейнера.
Шаблон класс(ы) должен параметризироваться типом хранимого значения и  аллокатором.

## Требования

Контейнер должен удовлетворять [следующим требованиям](https://en.cppreference.com/w/cpp/named_req/Container) для stl-контейнера.
А также [требования для последовательного контейнера](https://en.cppreference.com/w/cpp/named_req/SequenceContainer)

Исключая rvalue и move-семантику.

## Итератор

Класс должен предоставлять итератор произвольного доступа.

С требования для подобного итератора можно ознакомиться [здесь](https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator).

## Кольцевой буфер с расширением максимального размера.

В учебных целях, класс CCircularBufferExt должен обладать функциональностью для расширения свой максимального размера.
Должно быть реализовано след поведение: в случае достижения размера кольцевого буфера максимального возможного своего размера, значение максимального размера должно удваиваться.

## Тесты

Вашу реализацию Кольцевого Буфера требуется покрыть тестами, с помощью фреймворка Google Test.
Документация и примеры можно найти вот [тут](http://google.github.io/googletest).

Способ подключения и запуска тестов можно посмотреть в предыдущих лабораторных.

Тесты также являются частью задания, поэтому покрытие будет влиять на максимальный балл.


## Ограничения

* Запрещено использовать стандартные контейнеры

## Deadline

1. 20.03.23. 0.85
2. 27.03.23. 0.65
3. 03.04.23. 0.5


Максимальное количество баллов - 15










template <typename T, typename Alloc = std::allocator<T>>
class CCircularBufferExt {
public:
using value_type = T;
using allocator_type = Alloc;
using size_type = std::size_t;
using reference = value_type&;
using const_reference = const value_type&;
using pointer = typename std::allocator_traits<allocator_type>::pointer;
using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
class Iterator;

    CCircularBufferExt(size_type capacity = 0, const allocator_type& alloc = allocator_type());
    explicit CCircularBufferExt(const CCircularBufferExt& other);
    CCircularBufferExt(CCircularBufferExt&& other) noexcept;
    ~CCircularBufferExt() noexcept;

    CCircularBufferExt& operator=(const CCircularBufferExt& other);
    CCircularBufferExt& operator=(CCircularBufferExt&& other) noexcept;

    template <typename InputIt>
    void assign(InputIt first, InputIt last);

    void assign(size_type count, const T& value);

    void swap(CCircularBufferExt& other) noexcept;

    void resize(size_type count);

    void reserve(size_type new_capacity);

    void clear() noexcept;

    bool empty() const noexcept;

    size_type size() const noexcept;

    size_type capacity() const noexcept;

    reference front();

    const_reference front() const;

    reference back();

    const_reference back() const;

    void push_back(const T& value);

    void push_back(T&& value);

    template <typename... Args>
    reference emplace_back(Args&&... args);

    void pop_back();

    Iterator begin() noexcept;

    Iterator end() noexcept;

    const Iterator begin() const noexcept;

    const Iterator end() const noexcept;

    const Iterator cbegin() const noexcept;

    const Iterator cend() const noexcept;

private:
pointer m_buffer;
size_type m_size;
size_type m_capacity;
size_type m_head;
size_type m_tail;
allocator_type m_alloc;

    void copy(const CCircularBufferExt& other);
    void deallocate() noexcept;
    void reallocate(size_type new_capacity);
    size_type get_next_capacity(size_type new_capacity) const noexcept;
};

template <typename T, typename Alloc>
class CCircularBufferExt<T, Alloc>::Iterator {
public:
using value_type = T;
using reference = value_type&;
using pointer = value_type*;
using difference_type = std::ptrdiff_t;
using iterator_category = std::random_access_iterator_tag;

    Iterator(pointer ptr, size_type head, size_type tail, size_type capacity)
        : m_ptr(ptr)
        , m_head(head)
        , m_tail(tail)
        , m_capacity(capacity)
    {
    }

    reference operator*() const
    {
        return *m_ptr;
    }

    pointer operator->() const
    {
        return m_ptr;
    }

    Iterator& operator++()
    {
        m_ptr++;
        if (m_ptr == (m_head == 0 ? (m_tail == 0 ? m_ptr + m_capacity : m_tail) : m_head)) {
            m_ptr = m_ptr - m_capacity;
        }
        return *this;
    }

    Iterator operator++(int)
    {
        Iterator tmp(*this);
        operator++();
        return tmp;
    }

    Iterator& operator--()
    {
        if (m_ptr == (m_head == 0 ? (m_tail == 0 ? m_ptr : m_tail - 1) : m_head - 1)) {
            m_ptr = m_ptr
