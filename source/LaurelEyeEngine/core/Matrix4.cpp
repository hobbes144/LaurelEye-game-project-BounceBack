/*!****************************************************************************
 * \file   Matrix4.cpp
 * \author Anish Murthy (anish.murthy.dev@gmail.com)
 * \par    **DigiPen Email**
 *    anish.murthy@digipen.edu
 * \author Mariojulio Osmin Zaldivar Alvarez (ex-Adjunct Professor)
 *   (zaldivaralvarez@gmail.com) (Advising and Base Code)
 * \par    **Course**
 *    CS529
 * \date   10-05-2024
 *
 * Copyright © 2025 DIGIPEN Institute of Technology. All rights reserved.
 *
 *****************************************************************************/
#include "LaurelEyeEngine/core/Matrix4.h"

namespace LaurelEye {

/*!****************************************************************************
 * \brief Construct a new Matrix 4:: Matrix 4 object
 *
 *****************************************************************************/
Matrix4::Matrix4() {
    data[0][0] = 1.0f;
    data[0][1] = 0.0f;
    data[0][2] = 0.0f;
    data[0][3] = 0.0f;
    data[1][0] = 0.0f;
    data[1][1] = 1.0f;
    data[1][2] = 0.0f;
    data[1][3] = 0.0f;
    data[2][0] = 0.0f;
    data[2][1] = 0.0f;
    data[2][2] = 1.0f;
    data[2][3] = 0.0f;
    data[3][0] = 0.0f;
    data[3][1] = 0.0f;
    data[3][2] = 0.0f;
    data[3][3] = 1.0f;
}

/*!****************************************************************************
 * \brief Construct a new Matrix 4:: Matrix 4 object
 *
 * \param x0
 * \param y0
 * \param z0
 * \param w0
 * \param x1
 * \param y1
 * \param z1
 * \param w1
 * \param x2
 * \param y2
 * \param z2
 * \param w2
 * \param x3
 * \param y3
 * \param z3
 * \param w3
 *****************************************************************************/
Matrix4::Matrix4(
    float x0, float y0, float z0, float w0,
    float x1, float y1, float z1, float w1,
    float x2, float y2, float z2, float w2,
    float x3, float y3, float z3, float w3) {
    /*data = [
      [x0, y0, z0, w0],
      [x1, y1, z1, w1],
      [x2, y2, z2, w2],
      [x3, y3, z3, w3],
    ];*/

    data[0][0] = x0;
    data[0][1] = y0;
    data[0][2] = z0;
    data[0][3] = w0;
    data[1][0] = x1;
    data[1][1] = y1;
    data[1][2] = z1;
    data[1][3] = w1;
    data[2][0] = x2;
    data[2][1] = y2;
    data[2][2] = z2;
    data[2][3] = w2;
    data[3][0] = x3;
    data[3][1] = y3;
    data[3][2] = z3;
    data[3][3] = w3;
}

/*!****************************************************************************
 * \brief Getter for the address to the Matrix's data for use with GL code
 *
 * \return \b float* Address of the Matrix data.
 *****************************************************************************/
const float* Matrix4::getData() const {
    return &data[0][0];
}

/*!****************************************************************************
 * \brief Setter for specific elements in the Matrix
 *
 * \param row Row of the element.
 * \param col Column of the element.
 * \param value Value of the element.
 *****************************************************************************/
void Matrix4::updateElement(int row, int col, float value) {
    if ( row < 0 || row >= 4 || col < 0 || col >= 4 ) {
        throw std::out_of_range("Matrix indices out of range");
    }
    // We swap column and row since we store the matrix in column major
    data[col][row] = value;
}

/*!****************************************************************************
 * \brief Getter for specific elements in the Matrix
 *
 * \param row Row of the element.
 * \param col Column of the element.
 * \return \b float Value of the element.
 *****************************************************************************/
float Matrix4::getElement(int row, int col) const {
    if ( row < 0 || row >= 4 || col < 0 || col >= 4 ) {
        throw std::out_of_range("Matrix indices out of range");
    }
    // We swap column and row since we store the matrix in column major
    return data[col][row];
}

Matrix4 Matrix4::operator*(const float scalar) const {
    Matrix4 result;
    for ( int i = 0; i < 4; ++i ) {
        for ( int j = 0; j < 4; ++j ) {
            result.data[i][j] = data[i][j] * scalar;
        }
    }
    return result;
}

/*!****************************************************************************
 * \brief * Operator overloader for Matrix4 multiplication
 *
 * \param other Matrix4 object to be multiplied with.
 * \return \b Matrix4 The result of the multiplication.
 *****************************************************************************/
Matrix4 Matrix4::operator*(const Matrix4& other) {
    Matrix4 result;
    for ( int i = 0; i < 4; i++ ) {
        for ( int j = 0; j < 4; j++ ) {
            result.data[j][i] = 0.0f;
            for ( int k = 0; k < 4; k++ ) {
                result.data[j][i] += data[k][i] * other.data[j][k];
            }
        }
    }

    return result;
}

/*!****************************************************************************
 * \brief * Operator overloader for Vector3 multiplication
 *
 * \param vec Vector3 object to be multiplied with.
 * \return \b Vector3 The result of the multiplication.
 *****************************************************************************/
Vector3 Matrix4::operator*(const Vector3& vec) const {
    float x, y, z, w;
    x = vec.x * data[0][0] + vec.y * data[1][0] + vec.z * data[2][0] + 1 * data[3][0];
    y = vec.x * data[0][1] + vec.y * data[1][1] + vec.z * data[2][1] + 1 * data[3][1];
    z = vec.x * data[0][2] + vec.y * data[1][2] + vec.z * data[2][2] + 1 * data[3][2];
    w = vec.x * data[0][3] + vec.y * data[1][3] + vec.z * data[2][3] + 1 * data[3][3];

    if ( w != 0 ) {
        x /= w;
        y /= w;
        z /= w;
    }

    return Vector3(x, y, z);
}

VectorTemplated<float, 4> Matrix4::operator*(const VectorTemplated<float, 4>& vec) const {
    float vectorData[4] = {
        vec[0] * data[0][0] + vec[1] * data[1][0] + vec[2] * data[2][0] + vec[3] * data[3][0],
        vec[0] * data[0][1] + vec[1] * data[1][1] + vec[2] * data[2][1] + vec[3] * data[3][1],
        vec[0] * data[0][2] + vec[1] * data[1][2] + vec[2] * data[2][2] + vec[3] * data[3][2],
        vec[0] * data[0][3] + vec[1] * data[1][3] + vec[2] * data[2][3] + vec[3] * data[3][3]
    };

    return VectorTemplated<float, 4>(vectorData);
}

float* Matrix4::operator[](int row) {
    return data[row];
}

const float* Matrix4::operator[](int row) const {
    return data[row];
}

/*!****************************************************************************
 * \brief Static function to create a Translation matrix
 *
 * \param tx Translation in the x axis.
 * \param ty Translation in the y axis.
 * \param tz Translation in the z axis.
 * \return \b Matrix4 The resulting Translation matrix.
 *****************************************************************************/
Matrix4 Matrix4::translation(float tx, float ty, float tz) {
    Matrix4 result = Matrix4(
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      tx, ty, tz, 1.0f);

    return result;
}

/*!****************************************************************************
 * \brief Static function to create a Translation matrix
 *
 * \param translationVec Vector3 of Translation in each axis.
 * \return \b Matrix4 The resulting Translation matrix.
 *****************************************************************************/
Matrix4 Matrix4::translation(const Vector3& translationVec) {
    Matrix4 result = Matrix4(
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      translationVec.x, translationVec.y, translationVec.z, 1.0f);

    return result;
}


/*!****************************************************************************
 * \brief Static function to create a Scaling matrix
 *
 * \param sx Scaling in the x axis.
 * \param sy Scaling in the y axis.
 * \param sz Scaling in the z axis.
 * \return \b Matrix4 The resulting Scaling matrix.
 *****************************************************************************/
Matrix4 Matrix4::scale(float sx, float sy, float sz) {
    Matrix4 result = Matrix4(
      sx, 0.0f, 0.0f, 0.0f,
      0.0f, sy, 0.0f, 0.0f,
      0.0f, 0.0f, sz, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);

    return result;
}

/*!****************************************************************************
 * \brief Static function to create a Scaling matrix
 *
 * \param scaling Vector3 of Scaling in each axis.
 * \return \b Matrix4 The resulting Scaling matrix.
 *****************************************************************************/
Matrix4 Matrix4::scale(const Vector3& scaling) {
    Matrix4 result = Matrix4(
      scaling.x, 0.0f, 0.0f, 0.0f,
      0.0f, scaling.y, 0.0f, 0.0f,
      0.0f, 0.0f, scaling.z, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);

    return result;
}

/*!****************************************************************************
 * \brief Static function to create a Rotation matrix in the x axis
 *
 * \param angle Angle of rotation in the x axis.
 * \return \b Matrix4 The resulting Rotation matrix.
 *****************************************************************************/
Matrix4 Matrix4::rotationX(float angle) {
    Matrix4 result = Matrix4(
      1.0f, 0.0f, 0.0f, 0.0f,
      0.0f, cos(angle), sin(angle), 0.0f,
      0.0f, -sin(angle), cos(angle), 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);

    return result;
}

/*!****************************************************************************
 * \brief Static function to create a Rotation matrix in the y axis
 *
 * \param angle Angle of rotation in the y axis.
 * \return \b Matrix4 The resulting Rotation matrix.
 *****************************************************************************/
Matrix4 Matrix4::rotationY(float angle) {
    Matrix4 result = Matrix4(
      cos(angle), 0.0f, -sin(angle), 0.0f,
      0.0f, 1.0f, 0.0f, 0.0f,
      sin(angle), 0.0f, cos(angle), 0.0f,
      0.0f, 0.0f, 0.0f, 1.0);

    return result;
}

/*!****************************************************************************
 * \brief Static function to create a Rotation matrix in the z axis
 *
 * \param angle Angle of rotation in the z axis.
 * \return \b Matrix4 The resulting Rotation matrix.
 *****************************************************************************/
Matrix4 Matrix4::rotationZ(float angle) {
    Matrix4 result = Matrix4(
      cos(angle), sin(angle), 0.0f, 0.0f,
     -sin(angle), cos(angle), 0.0f, 0.0f,
      0.0f, 0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f);

    return result;
}

/*!****************************************************************************
 * \brief Static function to create a Rotation matrix in all 3 axis
 *
 * \param angleX Angle of rotation in the x axis.
 * \param angleY Angle of rotation in the y axis.
 * \param angleZ Angle of rotation in the z axis.
 * \return \b Matrix4 The resulting Rotation matrix.
 *****************************************************************************/
Matrix4 Matrix4::rotationXYZ(float angleX, float angleY, float angleZ) {
    Matrix4 result =
        rotationX(angleX) *
        rotationY(angleY) *
        rotationZ(angleZ);

    return result;
}

/*!****************************************************************************
 * \brief Static function to create a Rotation matrix in all 3 axis
 *
 * \param rotation Vector3 of Rotation in each axis.
 * \return \b Matrix4 The resulting Rotation matrix.
 *****************************************************************************/
Matrix4 Matrix4::rotationXYZ(const Vector3 rotation) {
    Matrix4 result =
        rotationX(rotation.x) *
        rotationY(rotation.y) *
        rotationZ(rotation.z);

    return result;
}

/*!****************************************************************************
 * \brief Static function to create a Rotation matrix in all 3 axis in reverse
 * for inverse calc
 *
 * \param angleX Angle of rotation in the x axis.
 * \param angleY Angle of rotation in the y axis.
 * \param angleZ Angle of rotation in the z axis.
 * \return \b Matrix4 The resulting Rotation matrix.
 *****************************************************************************/
Matrix4 Matrix4::rotationZYX(float angleX, float angleY, float angleZ) {
    Matrix4 result =
        rotationZ(angleX) *
        rotationY(angleY) *
        rotationX(angleZ);

    return result;
}

/*!****************************************************************************
 * \brief Static function to create a Rotation matrix in all 3 axis in reverse
 * for inverse calc
 *
 * \param rotation Vector3 of Rotation in each axis.
 * \return \b Matrix4 The resulting Rotation matrix.
 *****************************************************************************/
Matrix4 Matrix4::rotationZYX(const Vector3 rotation) {
    Matrix4 result =
        rotationZ(rotation.z) *
        rotationY(rotation.y) *
        rotationX(rotation.x);

    return result;
}

/*!****************************************************************************
 * \brief Static function to create a Rotation matrix in all 3 axis
 *
 * \param rotation Vector3 of Rotation in each axis.
 * \return \b Matrix4 The resulting Rotation matrix.
 *****************************************************************************/
Matrix4 Matrix4::rotation(Quaternion rotation) {
    Quaternion nQuaternion = rotation.normalized();
    float qw = nQuaternion.w();
    float qx = nQuaternion.x();
    float qy = nQuaternion.y();
    float qz = nQuaternion.z();

    Matrix4 result(
      1.0f - 2.0f * qy * qy - 2.0f * qz * qz, 2.0f * qx * qy + 2.0f * qz * qw, 2.0f * qx * qz - 2.0f * qy * qw, 0.0f,
      2.0f * qx * qy - 2.0f * qz * qw, 1.0f - 2.0f * qx * qx - 2.0f * qz * qz, 2.0f * qy * qz + 2.0f * qx * qw, 0.0f,
      2.0f * qx * qz + 2.0f * qy * qw, 2.0f * qy * qz - 2.0f * qx * qw, 1.0f - 2.0f * qx * qx - 2.0f * qy * qy, 0.0f,
      0.0f, 0.0f, 0.0f, 1.0f
    );

    return result;
}

/*!****************************************************************************
 * \brief Static function to create an Orthographic projection matrix
 *
 * \param left Leftmost position in the world to be projected.
 * \param right Rightmost position in the world to be projected.
 * \param bottom Bottommost position in the world to be projected.
 * \param top Topmost position in the world to be projected.
 * \param near Near plane of the projection.
 * \param far Far plane of the projection.
 * \return \b Matrix4 The resulting Orthographic projection matrix.
 *****************************************************************************/
Matrix4 Matrix4::orthographic(
    const float left,
    const float right,
    const float bottom,
    const float top,
    const float near,
    const float far) {
    Matrix4 result;
    result.data[0][0] = 2.0f / ( right - left );
    result.data[1][1] = 2.0f / ( top - bottom );
    result.data[2][2] = -2.0f / ( far - near );
    result.data[3][0] = -( right + left ) / ( right - left );
    result.data[3][1] = -( top + bottom ) / ( top - bottom );
    result.data[3][2] = -( far + near ) / ( far - near );
    result.data[3][3] = 1.0f;
    return result;
}

/*!****************************************************************************
 * \brief Static function to create a Perspective projection matrix
 *
 * \param fov FOV of the camera.
 * \param aspectRatio Aspect ratio of the screen to be projected on.
 * \param near Near plane of the projection.
 * \param far Far plane of the projection.
 * \return \b Matrix4 The resulting Perspective projection matrix.
 *****************************************************************************/
Matrix4 Matrix4::perspective(const float fov, const float aspectRatio,
                             const float near, const float far) {
    float tanHalfFov = tan(fov / 2.0f);

    Matrix4 result = Matrix4(
      1.0f / ( aspectRatio * tanHalfFov ), 0.0f, 0.0f, 0.0f,
      0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
      0.0f, 0.0f, -( far + near ) / ( far - near ), -1.0f,
      0.0f, 0.0f, -( 2.0f * far * near ) / ( far - near ), 0.0f);

    return result;
}

/*!****************************************************************************
 * \brief Static function to generate a Viewing matrix to setup the eye
 *
 * \param eye Eye position.
 * \param center Position in the world to be centered on.
 * \param up Up vector to help determine camera orientation.
 * \return \b Matrix4 The resulting Viewing matrix.
 *****************************************************************************/
Matrix4 Matrix4::lookAt(const Vector3& eye, const Vector3& center,
                        const Vector3& up) {
    Vector3 f = ( center - eye ).normalized();
    Vector3 s = f.cross(up).normalized();
    Vector3 u = s.cross(f).normalized();

    Matrix4 result = Matrix4(
      s.x, u.x, -f.x, 0.0f,
      s.y, u.y, -f.y, 0.0f,
      s.z, u.z, -f.z, 0.0f,
     -s.dot(eye), -u.dot(eye), f.dot(eye), 1.0f);

    return result;
}

Matrix4 Matrix4::inverse(const Matrix4& m) {
    float Coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
    float Coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
    float Coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

    float Coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
    float Coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
    float Coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

    float Coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
    float Coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
    float Coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

    float Coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
    float Coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
    float Coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

    float Coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
    float Coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
    float Coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

    float Coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
    float Coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
    float Coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

    VectorTemplated<float, 4> Fac0(std::array<float, 4>{Coef00, Coef00, Coef02, Coef03});
    VectorTemplated<float, 4> Fac1(std::array<float, 4>{Coef04, Coef04, Coef06, Coef07});
    VectorTemplated<float, 4> Fac2(std::array<float, 4>{Coef08, Coef08, Coef10, Coef11});
    VectorTemplated<float, 4> Fac3(std::array<float, 4>{Coef12, Coef12, Coef14, Coef15});
    VectorTemplated<float, 4> Fac4(std::array<float, 4>{Coef16, Coef16, Coef18, Coef19});
    VectorTemplated<float, 4> Fac5(std::array<float, 4>{Coef20, Coef20, Coef22, Coef23});

    VectorTemplated<float, 4> Vec0(std::array<float, 4>{m[1][0], m[0][0], m[0][0], m[0][0]});
    VectorTemplated<float, 4> Vec1(std::array<float, 4>{m[1][1], m[0][1], m[0][1], m[0][1]});
    VectorTemplated<float, 4> Vec2(std::array<float, 4>{m[1][2], m[0][2], m[0][2], m[0][2]});
    VectorTemplated<float, 4> Vec3(std::array<float, 4>{m[1][3], m[0][3], m[0][3], m[0][3]});

    VectorTemplated<float, 4> Inv0(Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2);
    VectorTemplated<float, 4> Inv1(Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4);
    VectorTemplated<float, 4> Inv2(Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5);
    VectorTemplated<float, 4> Inv3(Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5);

    VectorTemplated<float, 4> SignA(std::array<float, 4>{+1, -1, +1, -1});
    VectorTemplated<float, 4> SignB(std::array<float, 4>{-1, +1, -1, +1});
    VectorTemplated<float, 4> InverseA(Inv0 * SignA);
    VectorTemplated<float, 4> InverseB(Inv1 * SignB);
    VectorTemplated<float, 4> InverseC(Inv2 * SignA);
    VectorTemplated<float, 4> InverseD(Inv3 * SignB);
    Matrix4 Inverse(InverseA[0], InverseA[1], InverseA[2], InverseA[3],
      InverseB[0], InverseB[1], InverseB[2], InverseB[3],
      InverseC[0], InverseC[1], InverseC[2], InverseC[3],
      InverseD[0], InverseD[1], InverseD[2], InverseD[3]);

    VectorTemplated<float, 4> Row0(std::array<float, 4>{Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]});

    VectorTemplated<float, 4> Dot0(VectorTemplated<float, 4>(m[0]) * Row0);
    float Dot1 = ( Dot0[0] + Dot0[1] ) + ( Dot0[2] + Dot0[3] );

    float OneOverDeterminant = static_cast<float>( 1 ) / Dot1;

    return Inverse * OneOverDeterminant;
}

/*!****************************************************************************
 * \brief
 *
 * \param direction - the direction to be transformed
 * \return \b Vector3 the tranformed direction
 *****************************************************************************/
Vector3 Matrix4::transformDirection(const Vector3& direction) const {
    return Vector3(
    data[0][0] * direction.x + data[1][0] * direction.y + data[2][0] * direction.z,
    data[0][1] * direction.x + data[1][1] * direction.y + data[2][1] * direction.z,
    data[0][2] * direction.x + data[1][2] * direction.y + data[2][2] * direction.z
    ).normalized();
}

Matrix4 Matrix4::rotationAxis(const Vector3& axis, float radians) {
    Vector3 n = axis.normalized();
    float cosTheta = std::cos(radians);
    float sinTheta = std::sin(radians);
    float oneMinusCos = 1.0f - cosTheta;

    float x = n.x, y = n.y, z = n.z;

    return Matrix4(
        cosTheta + x * x * oneMinusCos, x * y * oneMinusCos - z * sinTheta, x * z * oneMinusCos + y * sinTheta, 0.0f,
        y * x * oneMinusCos + z * sinTheta, cosTheta + y * y * oneMinusCos, y * z * oneMinusCos - x * sinTheta, 0.0f,
        z * x * oneMinusCos - y * sinTheta, z * y * oneMinusCos + x * sinTheta, cosTheta + z * z * oneMinusCos, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

Quaternion Matrix4::toQuaternion() const {
    float trace = data[0][0] + data[1][1] + data[2][2]; // Sum of diagonal elements
    Quaternion q;

    if ( trace > 0.0f ) {
        float s = sqrt(trace + 1.0f) * 2.0f;
        q.setW(0.25f * s);
        q.setX(( data[2][1] - data[1][2] ) / s);
        q.setY(( data[0][2] - data[2][0] ) / s);
        q.setZ(( data[1][0] - data[0][1] ) / s);
    }
    else {
        if ( data[0][0] > data[1][1] && data[0][0] > data[2][2] ) {
            float s = sqrt(1.0f + data[0][0] - data[1][1] - data[2][2]) * 2.0f;
            q.setW(( data[2][1] - data[1][2] ) / s);
            q.setX(0.25f * s);
            q.setY(( data[0][1] + data[1][0] ) / s);
            q.setZ(( data[0][2] + data[2][0] ) / s);
        }
        else if ( data[1][1] > data[2][2] ) {
            float s = sqrt(1.0f + data[1][1] - data[0][0] - data[2][2]) * 2.0f;
            q.setW(( data[0][2] - data[2][0] ) / s);
            q.setX(( data[0][1] + data[1][0] ) / s);
            q.setY(0.25f * s);
            q.setZ(( data[1][2] + data[2][1] ) / s);
        }
        else {
            float s = sqrt(1.0f + data[2][2] - data[0][0] - data[1][1]) * 2.0f;
            q.setW(( data[1][0] - data[0][1] ) / s);
            q.setX(( data[0][2] + data[2][0] ) / s);
            q.setY(( data[1][2] + data[2][1] ) / s);
            q.setZ(0.25f * s);
        }
    }


    return q.normalized();

}


std::ostream& operator<<(std::ostream& os, const Matrix4& m) {
    const float* data = m.getData();

    os << "[ ";

    for ( int i = 0; i < 4; ++i ) {
        if ( i > 0 ) os << "  ";
        os << "[ ";
        for ( int j = 0; j < 4; ++j ) {
            os << data[i * 4 + j] << " ";
        }
        os << "]";
        if ( i < 3 ) os << "\n";
    }
    os << " ]";

    return os;
}

}
