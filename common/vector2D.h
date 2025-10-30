#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <cmath>

template <typename T>
class Vector2D {
public:
    T x;
    T y;

    /*
    * Constructores
    *
    * */
    Vector2D() : x(0), y(0) {}
    Vector2D(T x, T y) : x(x), y(y) {}

    /*
    * Operador suma de vectores
    *
    * */
    Vector2D<T> operator+(const Vector2D<T>& other) const {
        return Vector2D<T>(x + other.x, y + other.y);
    }

    /*
    * Operador resta de vectores
    *
    * */
    Vector2D<T> operator-(const Vector2D<T>& other) const {
        return Vector2D<T>(x - other.x, y - other.y);
    }

    /*
    * Operador multiplicación por escalar
    *
    * */
    Vector2D<T> operator*(T scalar) const {
        return Vector2D<T>(x * scalar, y * scalar);
    }

   
    /*
    * Cálculo de la longitud del vector
    *
    * */
    T length() const {
        return std::sqrt(x * x + y * y);
    }

    /*
    * Normalización del vector
    *
    * */
    // Vector2D<T> normalized() const {
    //     T len = length();
    //     if (len == 0) return Vector2D<T>(0, 0);
    //     return Vector2D<T>(x / len, y / len);
    // }

    /*
    * Cálculo de la distancia entre dos vectores
    *
    * */
    static T distance(const Vector2D<T>& a, const Vector2D<T>& b) {
        return std::sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));
    }

    /*
    * Operador de comparación igualdad
    *
    * */
    bool operator==(const Vector2D<T>& other) const {
        return x == other.x && y == other.y;
    }

    /*
    * Operador de comparación desigualdad
    *
    * */
    bool operator!=(const Vector2D<T>& other) const {
        return !(*this == other);
    }
};
#endif // VECTOR2D_H
