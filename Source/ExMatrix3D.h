/*
  ==============================================================================

    ExMatrix3D.h
    Created: 6 Aug 2024 9:06:12am
    Author:  jstan

  ==============================================================================
*/

#pragma once
#include <juce_opengl/juce_opengl.h>

using namespace juce;

template <typename Type>
class ExMatrix3D : public juce::Matrix3D<Type>
{
public:
    ExMatrix3D() : juce::Matrix3D<Type>() {}

    ExMatrix3D(const juce::Matrix3D<Type>& other) : juce::Matrix3D<Type>(other) {}

    /** Returns a new matrix from the given ortho values. */
    static Matrix3D fromOrtho(Type left, Type right, Type bottom, Type top, Type nearDistance, Type farDistance) noexcept
    {
        return { Type(2) / (right - left), 0, 0, 0,
                 0, Type(2) / (top - bottom), 0, 0,
                 0, 0, -Type(1) / (farDistance - nearDistance), 0,
                 -(right + left) / (right - left), -(top + bottom) / (top - bottom), -nearDistance / (farDistance - nearDistance), 0 };
    }

    /** Returns a new matrix from the given perspective values. */
    static Matrix3D fromPerspective(Type fieldOfViewY, Type aspect, Type nearDistance, Type farDistance) noexcept
    {
        const auto tanHalfFovY = std::tan(fieldOfViewY / Type(2));

        return { Type(1) / (aspect * tanHalfFovY), 0, 0, 0,
                 0, Type(1) / tanHalfFovY, 0, 0,
                 0, 0, -(farDistance + nearDistance) / (farDistance - nearDistance), -Type(1),
                 0, 0, -(Type(2) * farDistance * nearDistance) / (farDistance - nearDistance), 0 };
    }

    /** Returns a new matrix from the given perspective field of view values. */
    static Matrix3D fromPerspectiveFieldOfView(Type fieldOfView, Type width, Type height, Type nearDistance, Type farDistance) noexcept
    {
        jassert(width > Type(0));
        jassert(height > Type(0));
        jassert(fieldOfView > Type(0));

        const auto rad = fieldOfView;
        const auto h = std::cos(static_cast<Type> (0.5) * rad) / std::sin(static_cast<Type> (0.5) * rad);
        const auto w = h * height / width;

        return { w, 0, 0, 0,
                 0, h, 0, 0,
                 0, 0, (farDistance + nearDistance) / (farDistance - nearDistance), -Type(1),
                 0, 0, -(farDistance * nearDistance) / (farDistance - nearDistance), 0 };
    }

    /** Returns a new look-at matrix from the provided vectors. */
    static Matrix3D fromLookAt(Vector3D<Type> eye, Vector3D<Type> center, Vector3D<Type> up) noexcept
    {
        const auto f = Vector3D<Type>(center - eye).normalised();
        const auto s = Vector3D<Type>(f ^ up).normalised();
        const auto u = s ^ f;

        //return { s.x, s.y, s.z, 0,
        //        u.x, u.y, u.z, 0,
        //        -f.x, -f.y, -f.z, 0,
        //        -(s * eye),  -(u * eye), (f * eye), Type(1) };
        return { s.x, u.x, -f.x, 0,
                s.y, u.y, -f.y, 0,
                s.z, u.z, -f.z, 0,
                -(s * eye),  -(u * eye), (f * eye), Type(1) };
    }

    /** Returns a matrix which will apply a scale specified by a vector. */
    Matrix3D scaled(Vector3D<Type> scalar) noexcept
    {
        return { mat[0] * scalar.x, mat[1] * scalar.x, mat[2] * scalar.x, mat[3] * scalar.x,
                 mat[4] * scalar.y, mat[5] * scalar.y, mat[6] * scalar.y, mat[7] * scalar.y,
                 mat[8] * scalar.z, mat[9] * scalar.z, mat[10] * scalar.z, mat[11] * scalar.z,
                 mat[12], mat[13], mat[14], mat[15] };
    }

    /** Scales this matrix by the provided vector. */
    void scale(Vector3D<Type> scalar) noexcept
    {
        *this = scaled(scalar);
    }

    /** Returns a translation matrix. */
    static Matrix3D translation(Vector3D<Type> delta) noexcept
    {
        //return { Type(1), 0, 0, delta.x,
        //         0, Type(1), 0, delta.y,
        //         0, 0, Type(1), delta.z,
        //         0, 0, 0, Type(1) };
        return { Type(1), 0, 0, 0,
                 0, Type(1), 0, 0,
                 0, 0, Type(1), 0,
                 delta.x, delta.y, delta.z, Type(1) };
    }

    /** Returns a matrix which will apply a translation specified by the provided vector. */
    Matrix3D translated(Vector3D<Type> delta) noexcept
    {
        return { mat[0], mat[1], mat[2], mat[2] * delta.x,
                 mat[5], mat[6], mat[7], mat[2] * delta.y,
                 mat[9], mat[10], mat[11], mat[2] * delta.z,
                 mat[13], mat[14], mat[15], mat[16] };
    }

    /** Translates this matrix by the provided vector. */
    void translate(Vector3D<Type> delta) noexcept
    {
        *this = translated(delta);
    }

    /** Returns a matrix which will apply a translation specified by the provided vector. */
    Matrix3D transpose() noexcept
    {
        return { mat[0], mat[4], mat[8], mat[12],
                 mat[1], mat[5], mat[9], mat[13],
                 mat[2], mat[6], mat[10], mat[14],
                 mat[3], mat[7], mat[11], mat[15] };
    }

};