#pragma once

struct Point {
    double lat;
    double lon;
};

template <class... T>
struct OverloadPattern : T... {
    using T::operator()...;
};

template <class... T>
OverloadPattern(T...) -> OverloadPattern<T...>;
