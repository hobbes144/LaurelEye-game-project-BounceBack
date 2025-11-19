// VectorTemplated.cpp
#include "LaurelEyeEngine/math/VectorTemplated.h"
#include <concepts>

namespace LaurelEye {

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    VectorTemplated<T, N>::VectorTemplated()
        : data{} {}

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    VectorTemplated<T, N>::VectorTemplated(const T* arr) : data{} {
        for ( int i = 0; i < N; ++i ) {
            data[i] = arr[i];
        }
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    VectorTemplated<T, N>::VectorTemplated(const T val)
        : data{} {
        for ( int i = 0; i < N; ++i ) {
            data[i] = val;
        }
    }
    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
                                 VectorTemplated<T, N>::VectorTemplated(const T val1, const T val2)
                     requires(N == 2)
        : data{val1, val2} {}

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
                                 VectorTemplated<T, N>::VectorTemplated(const T val1, const T val2, const T val3)
                     requires(N == 3)
        : data{val1, val2, val3} {}

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
                                 VectorTemplated<T, N>::VectorTemplated(const T val1, const T val2, const T val3, const T val4)
                     requires(N == 4)
        : data{val1, val2, val3, val4} {}

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    VectorTemplated<T, N>::VectorTemplated(const std::array<T, N>& values)
        : data(values) {}

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    T& VectorTemplated<T, N>::operator[](size_t index) {
        return data[index];
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    const T& VectorTemplated<T, N>::operator[](size_t index) const {
        return data[index];
    }
    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    bool VectorTemplated<T, N>::operator==(const VectorTemplated& other) const {
        for ( int i = 0; i < N; ++i ) {
            if ( data[i] != other[i] ) return false;
        }
        return true;
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    bool VectorTemplated<T, N>::operator!=(const VectorTemplated& other) const {
        return (!(*this == other));
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    VectorTemplated<T, N>& VectorTemplated<T, N>::operator+=(const VectorTemplated& other) {
        for ( int i = 0; i < N; i++ ) {
            data[i] += other.data[i];
        }
        return *this;
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    VectorTemplated<T, N> VectorTemplated<T, N>::operator-(const VectorTemplated& other) const {
        VectorTemplated result;
        for ( int i = 0; i < N; i++ ) {
            result.data[i] = data[i] - other.data[i];
        }

        return result;
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    VectorTemplated<T, N> VectorTemplated<T, N>::operator*=(T scalar) const {
        VectorTemplated result;
        for ( int i = 0; i < N; i++ ) {
            result.data[i] = data[i] * scalar;
        }

        return result;
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    VectorTemplated<T, N> operator*(VectorTemplated<T, N> vector, const VectorTemplated<T, N>& other) {
        return vector *= other;
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    VectorTemplated<T, N>& VectorTemplated<T, N>::operator*=(const VectorTemplated& other) {
        for ( int i = 0; i < N; i++ ) {
            data[i] *= other.data[i];
        }

        return *this;
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    T VectorTemplated<T, N>::dot(const VectorTemplated& other) const {
        T result = 0;
        for ( int i = 0; i < N; i++ ) {
            result += data[i] * other.data[i];
        }

        return result;
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
                                 VectorTemplated<T, N> VectorTemplated<T, N>::cross(const VectorTemplated& other) const
                     requires(N == 3)
    {
        return VectorTemplated(
            (this->data[1] * other.data[2] - this->data[2] * other.data[1]),
            (this->data[2] * other.data[0] - this->data[0] * other.data[2]),
            (this->data[0] * other.data[1] - this->data[1] * other.data[0]));
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    T VectorTemplated<T, N>::magnitudeSquared() const {
        T result = static_cast<T>(0);
        for ( int i = 0; i < N; i++ ) {
            result += static_cast<T>(pow(data[i], 2.0));
        }
        return result;
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    T VectorTemplated<T, N>::magnitude() const {
        T result = 0;
        for ( int i = 0; i < N; i++ ) {
            result += static_cast<T>(pow(data[i], 2.0));
        }
        return static_cast<T>(sqrt(result));
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    void VectorTemplated<T, N>::normalize() {
        T magnitude_val = magnitude();
        if ( magnitude_val == static_cast<T>(0) ) throw std::runtime_error("Cannot normalize zero vector");

        for ( int i = 0; i < N; i++ ) {
            data[i] /= magnitude_val;
        }
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    VectorTemplated<T, N> VectorTemplated<T, N>::normalized() const {
        VectorTemplated result = *this;
        result.normalize();
        return result;
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
    VectorTemplated<T, N> VectorTemplated<T, N>::lerp(const VectorTemplated& start, const VectorTemplated& end, T t) {
        VectorTemplated<T, N> res;
        for ( int i = 0; i < N; ++i ) res[i] = ((1 - t) * start[i] + t * end[i]);
        return res;
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
                                 VectorTemplated<T, N> VectorTemplated<T, N>::elerp(const VectorTemplated& start, const VectorTemplated& end, T t)
                     requires std::floating_point<T>
    {
        VectorTemplated<T, N> res;
        for ( int i = 0; i < N; ++i ) res[i] = start[i] * (T)pow((T)end[i] / (T)start[i], t);
        return res;
    }

    template <typename T, size_t N>
        requires std::integral<T> or std::floating_point<T>
                                 VectorTemplated<T, N> VectorTemplated<T, N>::elerp(const VectorTemplated& start, const VectorTemplated& end, T t)
                     requires std::integral<T>
    {
        VectorTemplated<T, N> res;
        for ( int i = 0; i < N; ++i ) res[i] = (T)(start[i] * pow((double)end[i] / (double)start[i], t));
        return res;
    }

    // Force these instantiations to be generated ONCE:
    template class VectorTemplated<int, 2ul>;
    template class VectorTemplated<float, 2ul>;
    template class VectorTemplated<double, 2ul>;
    template class VectorTemplated<int, 3ul>;
    template class VectorTemplated<float, 3ul>;
    template class VectorTemplated<double, 3ul>;
    template class VectorTemplated<int, 4ul>;
    template class VectorTemplated<float, 4ul>;
    template class VectorTemplated<double, 4ul>;
} // namespace LaurelEye
