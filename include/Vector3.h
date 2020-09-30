#ifndef VECTOR3_H
#define VECTOR3_H

#include <TMath.h>
#include <cmath>
#include <iostream>

//______________________________________________________________________________
class Vector3
{
private:
  Double_t x, y, z;

public:
  // 构造函数与析构函数
  Vector3();
  Vector3(Double_t x, Double_t y, Double_t z);
  ~Vector3();

  // 定义取值操作
  Double_t X();
  Double_t Y();
  Double_t Z();

  // 定义运算
  Double_t& operator [] (Int_t i);
  Bool_t operator == (const Vector3& v) const;
  Bool_t operator != (const Vector3& v) const;
  Vector3 operator - ();
  Vector3& operator += (const Vector3& v);
  Vector3& operator -= (const Vector3& v);
  Vector3& operator *= (const Double_t scalar);
  Vector3& operator /= (const Double_t scalar);
  friend Vector3 operator + (const Vector3& v1, const Vector3& v2);
  friend Vector3 operator - (const Vector3& v1, const Vector3& v2);
  friend Vector3 operator * (const Vector3& v, const Double_t scalar);
  friend Vector3 operator * (const Double_t scalar, const Vector3& v);
  friend Vector3 operator / (const Vector3& v, const Double_t scalar);

  // 简单的线性代数计算
  Double_t Norm() const;
  void Normalize(Double_t GoalNorm = 1.0);
  Double_t Dot(const Vector3& v1, const Vector3& v2);
  Vector3  Cross(const Vector3& v1, const Vector3& v2);
  Double_t AngleBetween(const Vector3& v1, const Vector3& v2);
  Double_t AngleDegBetween(const Vector3& v1, const Vector3& v2);
  Double_t DistanceBetween(const Vector3& v1, const Vector3& v2);

  // 球坐标表征
  Double_t R();
  Double_t Theta();
  Double_t ThetaDeg();
  Double_t Phi();
  Double_t PhiDeg();
  Double_t DistToZAxis();

};

#endif
