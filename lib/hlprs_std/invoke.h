#pragma once

#include <utility>


namespace dts {


template <typename Func, typename... Args>
auto Invoke(Func&& func, Args&&... args) 
    -> decltype(func(std::forward<Args>(args)...)) {
    return std::forward<Func>(func)(std::forward<Args>(args)...);
}


template <typename T, typename Func, typename... Args>
auto Invoke(Func&& func, T&& obj, Args&&... args) 
    -> decltype((std::forward<T>(obj).*func)(std::forward<Args>(args)...)) {
    return (std::forward<T>(obj).*func)(std::forward<Args>(args)...);
}


template <typename T, typename Func, typename... Args>
auto Invoke(Func&& func, T* obj, Args&&... args)
    -> decltype((obj->*func)(std::forward<Args>(args)...)) {
    return (obj->*func)(std::forward<Args>(args)...);
}


template <typename T, typename M>
auto Invoke(M T::*member, T&& obj)
    -> decltype(std::forward<T>(obj).*member) {
    return std::forward<T>(obj).*member;
}


template <typename T, typename Func, typename... Args>
auto Invoke(Func&& func, std::reference_wrapper<T> obj, Args&&... args)
    -> decltype((obj.get().*func)(std::forward<Args>(args)...)) {
    return (obj.get().*func)(std::forward<Args>(args)...);
}


}