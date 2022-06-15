#pragma once
#include <math.h>

namespace Arriba::Maths
{
    template<typename T = float>
    struct vec2
    {
        //Vars
        union {T x, r;};
        union {T y, g;};

        //Operators
        vec2<T> operator+(vec2<T> right)
        {
            return {x + right.x, y + right.y};
        }

        vec2<T> operator+=(vec2<T> right)
        {
            return {x += right.x, y += right.y};
        }

        //Define multiplication as the dot product
        T operator*(vec2<T> right)
        {
            return {x * right.x + y * right.y};
        }

        vec2<T> operator*(T right)
        {
            return {x * right, y * right};
        }
    };

    template<typename T = float>
    struct vec3
    {
        //Vars
        union {T x, r;};
        union {T y, g;};
        union {T z, b;};

        //Operators
        vec3<T> operator+(vec3<T> right)
        {
            return {x + right.x, y + right.y, z + right.z};
        }

        vec3<T> operator+=(vec3<T> right)
        {
            return {x += right.x, y += right.y, z += right.z};
        }

        //Define multiplication as the dot product
        T operator*(vec3<T> right)
        {
            return {x * right.x + y * right.y + z * right.z};
        }

        vec3<T> operator*(T right)
        {
            return {x * right, y * right, z * right};
        }

        operator vec2<T>()
        {
            return vec2<T>{x,y};
        }
    };

    template<typename T = float>
    struct vec4
    {
        //Vars
        union {T x, r;};
        union {T y, g;};
        union {T z, b;};
        union {T w, a;};

        //Operators
        vec4<T> operator+(vec4<T> right)
        {
            return {x + right.x, y + right.y, z + right.z, w + right.w};
        }

        vec4<T> operator+=(vec4<T> right)
        {
            return {x += right.x, y += right.y, z += right.z, w += right.w};
        }

        //Define multiplication as the dot product
        T operator*(vec4<T> right)
        {
            return {x * right.x + y * right.y + z * right.z + w * right.w};
        }

        vec4<T> operator*(T right)
        {
            return {x * right, y * right, z * right, w * right};
        }

        operator vec3<T>()
        {
            return vec3<T>{x,y,z};
        }

        operator vec2<T>()
        {
            return vec2<T>{x,y};
        }
    };

    template<typename T = float>
    struct mat4
    {
        //vars
        vec4<T> col1 = {1,0,0,0};
        vec4<T> col2 = {0,1,0,0};
        vec4<T> col3 = {0,0,1,0};
        vec4<T> col4 = {0,0,0,1};

        //Operators
        mat4<T> operator*(mat4<T> right)
        {
            //mat4<T> transposed = transpose();
            mat4<T> newMatrix = mat4<T>();
            //newMatrix.col1 = {transposed.col1 * right.col1, transposed.col1 * right.col2, transposed.col1 * right.col3, transposed.col1 * right.col4};
            //newMatrix.col2 = {transposed.col2 * right.col1, transposed.col2 * right.col2, transposed.col2 * right.col3, transposed.col2 * right.col4};
            //newMatrix.col3 = {transposed.col3 * right.col1, transposed.col3 * right.col2, transposed.col3 * right.col3, transposed.col3 * right.col4};
            //newMatrix.col4 = {transposed.col4 * right.col1, transposed.col4 * right.col2, transposed.col4 * right.col3, transposed.col4 * right.col4};
            //Is this faster than the above? Needs benchmarked.
            //Row 1 of new matrix
            newMatrix.col1.x = {vec4<T>{col1.x, col2.x, col3.x, col4.x} * right.col1};
            newMatrix.col2.x = {vec4<T>{col1.x, col2.x, col3.x, col4.x} * right.col2};
            newMatrix.col3.x = {vec4<T>{col1.x, col2.x, col3.x, col4.x} * right.col3};
            newMatrix.col4.x = {vec4<T>{col1.x, col2.x, col3.x, col4.x} * right.col4};
            //Row 2 of new matrix
            newMatrix.col1.y = {vec4<T>{col1.y, col2.y, col3.y, col4.y} * right.col1};
            newMatrix.col2.y = {vec4<T>{col1.y, col2.y, col3.y, col4.y} * right.col2};
            newMatrix.col3.y = {vec4<T>{col1.y, col2.y, col3.y, col4.y} * right.col3};
            newMatrix.col4.y = {vec4<T>{col1.y, col2.y, col3.y, col4.y} * right.col4};
            //Row 3 of new matrix
            newMatrix.col1.z = {vec4<T>{col1.z, col2.z, col3.z, col4.z} * right.col1};
            newMatrix.col2.z = {vec4<T>{col1.z, col2.z, col3.z, col4.z} * right.col2};
            newMatrix.col3.z = {vec4<T>{col1.z, col2.z, col3.z, col4.z} * right.col3};
            newMatrix.col4.z = {vec4<T>{col1.z, col2.z, col3.z, col4.z} * right.col4};
            //Row 4 of new matrix
            newMatrix.col1.w = {vec4<T>{col1.w, col2.w, col3.w, col4.w} * right.col1};
            newMatrix.col2.w = {vec4<T>{col1.w, col2.w, col3.w, col4.w} * right.col2};
            newMatrix.col3.w = {vec4<T>{col1.w, col2.w, col3.w, col4.w} * right.col3};
            newMatrix.col4.w = {vec4<T>{col1.w, col2.w, col3.w, col4.w} * right.col4};
            return newMatrix;
        }
    };

    struct Transform
    {
        vec3 <float>position = {0,0,0};
        vec3 <float>scale = {1,1,1};
        vec3 <float>rotation = {0,0,0};
    };

    template<typename T>
    mat4<T> makeOrtho(T left, T right, T bottom, T top, T near, T far)
    {
        //Create an affine matrix
        //https://en.wikipedia.org/wiki/Orthographic_projection#Geometry
        mat4<T> scaleMatrix = mat4<T>();
        scaleMatrix.col1.x = 2/(right-left);
        scaleMatrix.col2.y = 2/(top-bottom);
        scaleMatrix.col3.z = 2/(far-near);
        mat4<T> translationMatrix = mat4<T>();
        translationMatrix.col4.x = -(left+right)/2;
        translationMatrix.col4.y = -(top+bottom)/2;
        translationMatrix.col4.z = -(far+near)/2;
        translationMatrix.col3.z = -1;
        return scaleMatrix * translationMatrix;
    }

    //Taken from https://stackoverflow.com/a/59036873
    template<typename T>
    mat4<T> rotate(mat4<T> originalMatrix, T radians, vec3<T> rotation)
    {
        rotation = rotation * radians;
        //Rotate around X
        mat4<T> rotationX = mat4<T>();
        rotationX.col2 = vec4<T>{0,cos(rotation.x),sin(rotation.x), 0};
        rotationX.col3 = vec4<T>{0,-sin(rotation.x),cos(rotation.x), 0};
        //Rotate around Y
        mat4<T> rotationY = mat4<T>();
        rotationY.col1 = vec4<T>{cos(rotation.y),0,-sin(rotation.y), 0};
        rotationY.col3 = vec4<T>{sin(rotation.y),0,cos(rotation.y), 0};
        //Rotate around Z
        mat4<T> rotationZ = mat4<T>();
        rotationZ.col1 = vec4<T>{cos(rotation.z),sin(rotation.z),0, 0};
        rotationZ.col2 = vec4<T>{-sin(rotation.z),cos(rotation.z),0, 0};
        return originalMatrix * rotationX * rotationY * rotationZ;
    }

    //Does this work? I know this is going to have 1 dimension cut off but I feel like this shouldn't work without editing col4
    template<typename T>
    mat4<T> scale(mat4<T> originalMatrix, vec3<T> scalars)
    {
        mat4<T> scaleMatrix = mat4<T>();
        scaleMatrix.col1 = scaleMatrix.col1 * scalars.x;
        scaleMatrix.col2 = scaleMatrix.col2 * scalars.y;
        scaleMatrix.col3 = scaleMatrix.col3 * scalars.z;
        return originalMatrix * scaleMatrix;
    }

    template<typename T>
    mat4<T> translate(mat4<T> originalMatrix, vec3<T> translation)
    {
        mat4<T> translationMatrix = mat4<T>();
        translationMatrix.col4.x = translation.x;
        translationMatrix.col4.y = translation.y;
        translationMatrix.col4.z = translation.z;
        return originalMatrix * translationMatrix;
    }

    template<typename T>
    T radians(T degree)
    {
        const float pi = 3.14159265359;
        const float degreeInRad = pi/180;
        return degree * degreeInRad;
    }

    template<typename T>
    T lerp(T p1, T p2, float t)
    {
        return p1*(1-t) + p2*t;
    }
}