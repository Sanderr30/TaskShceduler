#pragma once

#include "invoke.h"

namespace dts {


template <size_t Index, typename T>
class TupleElement {
public:
    TupleElement() = default;

    TupleElement(T&& val) 
        : value(std::forward<T>(val)) 
    {}

    T& get() { 
        return value; 
    }
    
    const T& get() const { 
        return value; 
    }
    
private:
    T value;
};


template <typename... Tail>
class Tuple;


template<>
class Tuple<> {
};

template <typename Head>
class Tuple<Head> {
public:
    Tuple() = default;

    Tuple(Head&& head) 
        : head(std::forward<Head>(head))
    {}

public:
    Tuple(const Tuple& other) 
        : head(other.head) 
    {}

    Tuple& operator=(const Tuple& other) {
        if (this == &other) {
            return *this;
        }
        head = other.head;
        return *this;
    }

    Tuple(Tuple&& other) noexcept 
        : head(std::move(other.head)) 
    {}

    Tuple& operator=(Tuple&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        head = std::move(other.head);
        return *this;
    }

    ~Tuple() = default;

public:
    Head head;
};


template <typename Head, typename... Tail>
class Tuple<Head, Tail...> {
public:
    Tuple() = default;

    Tuple(Head&& head, Tail... tail) 
        : tail(std::forward<Tail>(tail)...)
        , head(std::forward<Head>(head))
    {}

public:
    Tuple(const Tuple& other) 
        : tail(other.tuple)
        , head(other.head) 
    {}

    Tuple& operator=(const Tuple& other) {
        if (this == &other) {
            return *this;
        }
        tail = other.tail;
        head = other.head;
        return *this;
        
    }

    Tuple(Tuple&& other) noexcept 
        : tail(std::move(other.tail))
        , head(std::move(other.head)) 
    {}

    Tuple& operator=(Tuple&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        tail = std::move(other.tail);
        head = std::move(other.head);
        return *this;
    }

    ~Tuple() = default;

public:
    Head head;
    Tuple<Tail...> tail;
};


template <size_t Index, typename Head, typename... Tail>
auto& Get(Tuple<Head, Tail...>& tuple) {
    if constexpr (Index == 0) {
        return tuple.head;
    } else {
        return Get<Index - 1>(tuple.tail);
    }
}


template <size_t Index, typename Head, typename... Tail>
const auto& Get(const Tuple<Head, Tail...>& tuple) {
    if constexpr (Index == 0) {
        return tuple.head;
    } else {
        return Get<Index - 1>(tuple.tail);
    }
}


template <size_t Index, typename Head, typename... Tail>
auto&& Get(Tuple<Head, Tail...>&& tuple) {
    if constexpr (Index == 0) {
        return std::forward<Head>(tuple.head);
    } else {
        return Get<Index - 1>(std::forward<Tuple<Tail...>>(tuple.tail));
    }
}


template<typename... Args>
Tuple<Args...> MakeTuple(Args&&... args) {
    return Tuple<Args...>(std::forward<Args>(args)...);
}


template <typename>
struct TupleSize;


template <>
struct TupleSize<Tuple<>> {
    static constexpr size_t value = 0;
};

template <typename Head, typename... Tail>
struct TupleSize<Tuple<Head, Tail...>> {
    static constexpr size_t value = 1 + TupleSize<Tuple<Tail...>>::value;
};


template <size_t... Ind>
struct IndexSequence {};


template <size_t N, size_t... Ind>
struct MakeIndexSequenceImpl : MakeIndexSequenceImpl<N - 1, N - 1, Ind...> {};


template <size_t... Ind>
struct MakeIndexSequenceImpl<0, Ind...> {
    using type = IndexSequence<Ind...>;
};


template <size_t N>
using MakeIndexSequence = typename MakeIndexSequenceImpl<N>::type;


}