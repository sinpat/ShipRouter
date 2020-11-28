#pragma once

#include <functional>

template<class T>
using Ref = std::reference_wrapper<T>;

template<class T>
using CRef = std::reference_wrapper<const T>;

using Tags = std::unordered_map<std::string, std::string>;
