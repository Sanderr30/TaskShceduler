#pragma once

#include "tuple.h"
#include "invoke.h"
#include <functional>

namespace dts {


template <typename Func, typename Tuple, size_t... Indexes>
auto ApplyImpl(Func&& func, Tuple&& tuple, IndexSequence<Indexes...>) {
    return Invoke(std::forward<Func>(func),
                    Get<Indexes>(std::forward<Tuple>(tuple))...);
}


template <typename Func, typename Tuple>
auto Apply(Func&& func, Tuple&& tuple) {
    constexpr size_t size = TupleSize<std::decay_t<Tuple>>::value;
    using Indexes = MakeIndexSequence<size>;
    
    return ApplyImpl(std::forward<Func>(func),
                    std::forward<Tuple>(tuple),
                    Indexes{});
}


}