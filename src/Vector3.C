#include "../include/Vector3.h"
using namespace std;


//______________________________________________________________________________
Vector3::Vector3()
{
  x = 0.0; y = 0.0; z = 0.0;
}

//______________________________________________________________________________
Vector3::Vector3(Double_t x, Double_t y, Double_t z)
{
  this->x = x;
  this->y = y;
  this->z = z;
}

//______________________________________________________________________________
Vector3::~Vector3()
{
  //cout<<"Exit Class Vecto3, Bye!"<<endl;
}

//______________________________________________________________________________
Double_t Vector3::X()
{
  return this->x;
}

//______________________________________________________________________________
Double_t Vector3::Y()
{
  return this->y;
}

//______________________________________________________________________________
Double_t Vector3::Z()
{
  return this->z;
}


//______________________________________________________________________________
Double_t& Vector3::operator [] (Int_t i)
{
  switch(i) {
    case 0: return this->x;
    case 1: return this->y;
    case 2: return this->z;
    default: cout<<"Error: index invalid in Double_t Vector3::operator [] (Int_t i)"<<endl;
  }
  return this->x;
}

//_____________________________________________________________________________
Bool_t Vector3::operator == (const Vector3& v) const
{
  return (v.x == this->x && v.y == this->y && v.z == this->z);
}


//_____________________________________________________________________________
Bool_t Vector3::operator != (const Vector3& v) const
{
  return !(*this == v);
}

//______________________________________________________________________________
Vector3 Vector3::operator - ()
{
  return Vector3(-this->x,-this->y, -this->z);
}

//______________________________________________________________________________
Vector3& Vector3::operator += (const Vector3& v)
{
  this->x += v.x;
  this->y += v.y;
  this->z += v.z;
  return *this;
}

//______________________________________________________________________________
Vector3& Vector3::operator -= (const Vector3& v)
{
  this->x -= v.x;
  this->y -= v.y;
  this->z -= v.z;
  return *this;
}

//______________________________________________________________________________
Vector3& Vector3::operator *= (const Double_t scalar)
{
  this->x *= scalar;
  this->y *= scalar;
  this->z *= scalar;
  return *this;
}

//______________________________________________________________________________
Vector3& Vector3::operator /= (const Double_t scalar)
{
  this->x /= scalar;
  this->y /= scalar;
  this->z /= scalar;
  return *this;
}

//______________________________________________________________________________
Vector3 operator + (const Vector3& v1, const Vector3& v2)
{
  return Vector3(v1.x+v2.x, v1.y+v2.y, v1.z+v2.z);
}

//______________________________________________________________________________
Vector3 operator - (const Vector3& v1, const Vector3& v2)
{
  return Vector3(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
}

//______________________________________________________________________________
Vector3 operator * (const Vector3& v, const Double_t scalar)
{
  return Vector3(v.x*scalar, v.y*scalar, v.z*scalar);
}

//______________________________________________________________________________
Vector3 operator * (const Double_t scalar, const Vector3& v)
{
  return v * scalar;
}

//______________________________________________________________________________
Vector3 operator / (const Vector3& v, const Double_t scalar)
{
  return Vector3(v.x/scalar, v.y/scalar, v.z/scalar);
}


//______________________________________________________________________________
// 简单的线性代数计算
Double_t Vector3::Norm() const
{
  return TMath::Sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
}

//______________________________________________________________________________
void Vector3::Normalize(Double_t GoalNorm)
{
  if (Norm() != 0.0) *this *= GoalNorm / Norm();
}

//______________________________________________________________________________
Double_t Vector3::Dot(const Vector3& v1, const Vector3& v2)
{
  return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
}

//______________________________________________________________________________
Vector3 Vector3::Cross(const Vector3& v1, const Vector3& v2)
{
  Vector3 vec3(v1.y*v2.z - v1.z*v2.y,
               v1.z*v2.x - v1.x*v2.z,
               v1.x*v2.y - v1.y*v2.x);
  return vec3;
}

//______________________________________________________________________________
Double_t Vector3::AngleBetween(const Vector3& v1, const Vector3& v2)
{
  Double_t normproduct = v1.Norm() * v2.Norm();
  return (normproduct == 0.0) ? 0.0 : TMath::ACos(Dot(v1,v2)/normproduct);
}

//______________________________________________________________________________
Double_t Vector3::AngleDegBetween(const Vector3& v1, const Vector3& v2)
{
  return AngleBetween(v1, v2) * 180.0 / TMath::Pi();
}

//______________________________________________________________________________
Double_t Vector3::DistanceBetween(const Vector3& v1, const Vector3& v2)
{
  return TMath::Sqrt(pow(v1.x-v2.x, 2) + pow(v1.y-v2.y, 2) + pow(v1.z-v2.z, 2));
}


//______________________________________________________________________________
// 球坐标参数
Double_t Vector3::R()
{
  return TMath::Sqrt(pow(this->x, 2) + pow(this->y, 2) + pow(this->z, 2));
}

//______________________________________________________________________________
Double_t Vector3::Theta()
{
  return (R() == 0.0) ? 0.0 : TMath::ACos(this->z/R());
}

//______________________________________________________________________________
Double_t Vector3::ThetaDeg()
{
  return Theta() * 180.0 / TMath::Pi();
}

//______________________________________________________________________________
Double_t Vector3::Phi()
{
  if (this->x == 0) {
    return (this->y >= 0) ? TMath::Pi()*1./2 : TMath::Pi()*3./2;
  } else {
    return TMath::ATan(this->y/this->x) + TMath::Pi() * 1./2 *
           (2 - (this->y)/TMath::Abs(this->y) - (this->x)*(this->y)/
           TMath::Abs((this->x)*(this->y)));

  // 公式与一下情况等效:
  // for x > 0, y > 0 : phi = atan(y/x)            (SSD4)
  // for x < 0, y > 0 : phi = atan(y/x) + pi       (SSD3)
  // for x < 0, y < 0 : phi = atan(y/x) + pi       (SSD2)
  // for x > 0, y < 0 : phi = atan(y/x) + 2 * pi   (SSD1)
  }
}

//______________________________________________________________________________
Double_t Vector3::PhiDeg()
{
  return Phi() * 180.0 / TMath::Pi();
}

//______________________________________________________________________________
Double_t Vector3::DistToZAxis()
{
  return TMath::Sqrt(pow(this->x, 2) + pow(this->y, 2));
}
