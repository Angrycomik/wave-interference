/**

    @file      Projection.hpp
    @brief Projection class handles 2D to 3D conversion.  

**/
#pragma once

#ifndef Projection_hpp
#define Projection_hpp

#include <SFML/Graphics.hpp>
#include <cmath>

#define M_PI 3.14159265358979323846
/**
    @struct Matrix
    @brief  Class that represents 4 by 4 square matrix
**/
struct Matrix {
	float data[4][4]; //!< matrix
    /**
        @brief  operator [] overload for non const matrix
        @param  i -  index
        @return   - pointer to i-row of the matrix
    **/
    float* operator[](int i) { return data[i]; }
    /**
    @brief  operator [] overload for const matrix
    @param  i  index
    @return    pointer to i-row of the matrix
    **/
    const float* operator[](int i) const { return data[i]; }
};
/**

    @class   Projection
    @brief  Class that has mathematical functions for 2D to 3D conversion 

**/
class Projection {
private:
    /**
        @brief  function that evaluates dot product of two vectors
        @param  a - vector a 
        @param  b - vector b
        @return   - dot product
    **/
    static float dot(const sf::Vector3f& a, const sf::Vector3f& b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    /**
        @brief  function that normalizes vector
        @param  v - vector
        @return   - normalized vector
    **/
    static sf::Vector3f normalize(const sf::Vector3f& v) {
        float length = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        return { v.x / length, v.y / length, v.z / length };
    }
    /**
        @brief function that evaluates cross product 
        @param  a - vector a 
        @param  b - vector b
        @return   - cross product
    **/
    static sf::Vector3f cross(const sf::Vector3f& a, const sf::Vector3f& b) {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
    }
public:
 /**
     @brief function that creates projection matrix used for further calculations
     @param angleOfView - camera's FOV
     @param near        - near clipping  
     @param far         - far clipping
     @param matrix      - the result projection matrix
 **/
	static void createProjectionMatrix(const float& angleOfView, const float& near, const float& far, Matrix& matrix) {
        float scale = 1 / tan(angleOfView * 0.5 * M_PI / 180);
        matrix[0][0] = scale;
        matrix[1][1] = scale;
        matrix[2][2] = -far / (far - near);
        matrix[3][2] = -far * near / (far - near);
        matrix[2][3] = -1;
        matrix[3][3] = 0;
	}
    /**
        @brief function that creates lookAt matrix
        @param eye    - camera's position
        @param target - camera's target 
        @param matrix - the result lookAt matrix 
    **/
    static void createLookAtMatrix(const sf::Vector3f& eye, const sf::Vector3f& target, Matrix& matrix) {
        sf::Vector3f up = { 0, 0, 1 };
        sf::Vector3f forward = normalize(eye - target);
        sf::Vector3f right = normalize(cross(up, forward));
        sf::Vector3f upVector = cross(forward, right);

        matrix[0][0] = right.x;  matrix[1][0] = right.y;  matrix[2][0] = right.z;
        matrix[3][0] = -dot(right, eye);

        matrix[0][1] = upVector.x; matrix[1][1] = upVector.y; matrix[2][1] = upVector.z;
        matrix[3][1] = -dot(upVector, eye);

        matrix[0][2] = forward.x; matrix[1][2] = forward.y; matrix[2][2] = forward.z;
        matrix[3][2] = -dot(forward, eye);

        matrix[3][3] = 1.0f;
    }
    /**
        @brief function that creates translation matrix 
        @param tx     - X translation
        @param ty     - Y translation
        @param tz     - Z translation
        @param matrix - the result translation matrix
    **/
    static void createTranslationMatrix(float tx, float ty, float tz, Matrix& matrix) {
        matrix = {
            1, 0, 0, tx,
            0, 1, 0, ty,
            0, 0, 1, tz,
            0, 0, 0, 1
        };
    }
    /**
        @brief function to multiply vector by matrix
        @param in     - input vector
        @param out    -  output vector
        @param matrix - matrix to multiply the vector by
    **/
    static void multiply(sf::Vector3f in, sf::Vector3f& out, Matrix& matrix) {
        out.x = in.x * matrix[0][0] + in.y * matrix[1][0] + in.z * matrix[2][0] + matrix[3][0];
        out.y = in.x * matrix[0][1] + in.y * matrix[1][1] + in.z * matrix[2][1] + matrix[3][1];
        out.z = in.x * matrix[0][2] + in.y * matrix[1][2] + in.z * matrix[2][2] + matrix[3][2];
        float w = in.x * matrix[0][3] + in.y * matrix[1][3] + in.z * matrix[2][3] + matrix[3][3];

        if (w != 1) {
            out.x /= w;
            out.y /= w;
            out.z /= w;
        }
    }
};

#endif
