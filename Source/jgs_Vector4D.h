/*
  ==============================================================================

   This file is part of the JUCE framework.
   Copyright (c) Raw Material Software Limited

   JUCE is an open source framework subject to commercial or open source
   licensing.

   By downloading, installing, or using the JUCE framework, or combining the
   JUCE framework with any other source code, object code, content or any other
   copyrightable work, you agree to the terms of the JUCE End User Licence
   Agreement, and all incorporated terms including the JUCE Privacy Policy and
   the JUCE Website Terms of Service, as applicable, which will bind you. If you
   do not agree to the terms of these agreements, we will not license the JUCE
   framework to you, and you must discontinue the installation or download
   process and cease use of the JUCE framework.

   JUCE End User Licence Agreement: https://juce.com/legal/juce-8-licence/
   JUCE Privacy Policy: https://juce.com/juce-privacy-policy
   JUCE Website Terms of Service: https://juce.com/juce-website-terms-of-service/

   Or:

   You may also use this code under the terms of the AGPLv3:
   https://www.gnu.org/licenses/agpl-3.0.en.html

   THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL
   WARRANTIES, WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF
   MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

  ==============================================================================
*/
#include <JuceHeader.h>

namespace jgs
{

//==============================================================================
/**
    A four-coordinate vector.

    @tags{OpenGL}
*/
template <typename Type>
class Vector4D
{
public:
    Vector4D() noexcept                                                     : x(), y(), z(), w() {}
    Vector4D (Type xValue, Type yValue, Type zValue, Type wValue) noexcept  : x (xValue), y (yValue), z (zValue), w(wValue) {}
    Vector4D (const Vector4D& other) noexcept                               : x (other.x), y (other.y), z (other.z), w(other.w) {}
    Vector4D& operator= (Vector4D other) noexcept                           { x = other.x;  y = other.y;  z = other.z;  w = other.w; return *this; }

    /** Returns a vector that lies along the X axis. */
    static Vector4D xAxis() noexcept                        { return { (Type) 1, 0, 0, 0 }; }
    /** Returns a vector that lies along the Y axis. */
    static Vector4D yAxis() noexcept                        { return { 0, (Type) 1, 0, 0 }; }
    /** Returns a vector that lies along the Z axis. */
    static Vector4D zAxis() noexcept                        { return { 0, 0, (Type)1, 0 }; }
    /** Returns a vector that lies along the Z axis. */
    static Vector4D wAxis() noexcept                        { return { 0, 0, 0, (Type)1 }; }

    Vector4D& operator+= (Vector4D other) noexcept          { x += other.x;  y += other.y;  z += other.z;  w += other.w;  return *this; }
    Vector4D& operator-= (Vector4D other) noexcept          { x -= other.x;  y -= other.y;  z -= other.z;  w -= other.w;  return *this; }
    Vector4D& operator*= (Type scaleFactor) noexcept        { x *= scaleFactor;  y *= scaleFactor;  z *= scaleFactor;  w *= scaleFactor;  return *this; }
    Vector4D& operator/= (Type scaleFactor) noexcept        { x /= scaleFactor;  y /= scaleFactor;  z /= scaleFactor;  w /= scaleFactor;  return *this; }

    Vector4D operator+ (Vector4D other) const noexcept      { return { x + other.x, y + other.y, z + other.z, w + other.w }; }
    Vector4D operator- (Vector4D other) const noexcept      { return { x - other.x, y - other.y, z - other.z, w - other.w }; }
    Vector4D operator* (Type scaleFactor) const noexcept    { return { x * scaleFactor, y * scaleFactor, z * scaleFactor, w * scaleFactor }; }
    Vector4D operator/ (Type scaleFactor) const noexcept    { return { x / scaleFactor, y / scaleFactor, z / scaleFactor, w / scaleFactor }; }
    Vector4D operator-() const noexcept                     { return { -x, -y, -z, -w }; }

    /** Returns the dot-product of these two vectors. */
    Type operator* (Vector4D other) const noexcept          { return x * other.x + y * other.y + z * other.z + w * other.w; }

    /** Returns the cross-product of these two vectors. */
    //Vector3D operator^ (Vector3D other) const noexcept      { return { y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x }; }

    Type length() const noexcept                            { return std::sqrt (lengthSquared()); }
    Type lengthSquared() const noexcept                     { return x * x + y * y + z * z + w * w; }

    Vector4D normalised() const noexcept                    { return *this / length(); }

    /** Returns true if the vector is practically equal to the origin. */
    bool lengthIsBelowEpsilon() const noexcept
    {
        auto epsilon = std::numeric_limits<Type>::epsilon();
        return ! (x < -epsilon || x > epsilon || y < -epsilon || y > epsilon || z < -epsilon || z > epsilon || w < -epsilon || w > epsilon);
    }

    Vector4D transformed(const Vector4D<Type> other, juce::Matrix3D<Type>& m) noexcept
    {
        auto&& m2 = m.mat;

        //v'_x = M[0][0]*v_x + M[0][1]*v_y + M[0][2]*v_z + M[0][3]*v_w
        //    v'_y = M[1][0]*v_x + M[1][1]*v_y + M[1][2]*v_z + M[1][3]*v_w
        //    v'_z = M[2][0]*v_x + M[2][1]*v_y + M[2][2]*v_z + M[2][3]*v_w
        //    v'_w = M[3][0]*v_x + M[3][1]*v_y + M[3][2]*v_z + M[3][3]*v_w
        return { m2[0] * other.x + m2[1] * other.y + m2[2] * other.z + m2[3] * other.w,
                 m2[4] * other.x + m2[5] * other.y + m2[6] * other.z + m2[7] * other.w,
                 m2[8] * other.x + m2[9] * other.y + m2[10] * other.z + m2[11] * other.w,
                 m2[12] * other.x + m2[13] * other.y + m2[14] * other.z + m2[15] * other.w };
    }

    Type x, y, z, w;
};

} // namespace jgs
