#pragma once

static const float PI  = 3.141592654f;

inline float radians(float deg) { return deg * PI / 180.0f; }


//
// simple 4x4 matrix class for multiplication etc...
//
template<class T>
class mat4
{
public:
	mat4()
	{
		_m[0]  = 1.0f; _m[1]  = 0.0f; _m[2]  = 0.0f; _m[3]  = 0.0f;
		_m[4]  = 0.0f; _m[5]  = 1.0f; _m[6]  = 0.0f; _m[7]  = 0.0f;
		_m[8]  = 0.0f; _m[9]  = 0.0f; _m[10] = 1.0f; _m[11] = 0.0f;
		_m[12] = 0.0f; _m[13] = 0.0f; _m[14] = 0.0f; _m[15] = 1.0f;
	}

	mat4(T m0,T m1,T m2,T m3,T m4,T m5,T m6,T m7,T m8,T m9,T m10,T m11,T m12,T m13,T m14,T m15)
	{
		_m[0]  = m0;  _m[1]  = m1;  _m[2]  = m2;  _m[3]  = m3;
		_m[4]  = m4;  _m[5]  = m5;  _m[6]  = m6;  _m[7]  = m7;
		_m[8]  = m8;  _m[9]  = m9;  _m[10] = m10; _m[11] = m11;
		_m[12] = m12; _m[13] = m13; _m[14] = m14; _m[15] = m15;
	}

	mat4(const mat4<T>& rhs){ assign(rhs); }

	const T* get() const { return &_m[0]; }

	T get(uint32_t row, uint32_t col) const
	{
		return _m[row*4+col];
	}
	
	void set(uint32_t row, uint32_t col, T val) 
	{ 
		_m[row*4+col] = val;
	}

	mat4<T>& operator=(const mat4<T>& rhs)
	{
		if (this != &rhs){
			assign(rhs);
		}
		return *this;
	}

	const mat4<T> operator*(const mat4<T>& rhs)
	{
		return multiply(*this, rhs);
	}

	mat4<T>& operator*=(const mat4<T>& rhs)
	{
		*this = multiply(*this, rhs);
		return *this;
	}

	static mat4<T> identity()
	{
		mat4<T> m;
		return m;
	}

	static mat4<T> translate(T x, T y, T z)
	{
		mat4<T> m(
			1.0f, 0.0f, 0.0f, x,
			0.0f, 1.0f, 0.0f, y,
			0.0f, 0.0f, 1.0f, z,
			0.0f, 0.0f, 0.0f, 1.0f);

		return m;
	}

	static mat4<T> scale(T x, T y, T z)
	{
		mat4<T> m(
			x,    0.0f, 0.0f, 0.0f,
			0.0f, y,    0.0f, 0.0f,
			0.0f, 0.0f, z,    0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
		return m;
	}

	static mat4<T> rotateX(T angle)
	{
		const float c = cosf(radians(angle));
		const float s = sinf(radians(angle));

		mat4<T> m;
		m._m[0]  = 1.0f; m._m[1]  = 0.0f; m._m[2]  = 0.0f; m._m[3]  = 0.0f;
		m._m[4]  = 0.0f; m._m[5]  = c;    m._m[6]  = -s;   m._m[7]  = 0.0f;
		m._m[8]  = 0.0f; m._m[9]  = s;    m._m[10] = c;    m._m[11] = 0.0f;
		m._m[12] = 0.0f; m._m[13] = 0.0f; m._m[14] = 0.0f; m._m[15] = 1.0f;	
		return m;
	}

	static mat4<T> rotateY(T angle)
	{
		const float c = cosf(radians(angle));
		const float s = sinf(radians(angle));

		mat4<T> m;
		m._m[0]  = c;    m._m[1]  = 0.0f; m._m[2]  = s;    m._m[3]  = 0.0f;
		m._m[4]  = 0.0f; m._m[5]  = 1.0f; m._m[6]  = 0.0f; m._m[7]  = 0.0f;
		m._m[8]  = -s;   m._m[9]  = 0.0f; m._m[10] = c;    m._m[11] = 0.0f;
		m._m[12] = 0.0f; m._m[13] = 0.0f; m._m[14] = 0.0f; m._m[15] = 1.0f;	
		return m;
	}

	static mat4<T> rotateZ(T z)
	{
		const float c = cosf(radians(z));
		const float s = sinf(radians(z));

		mat4<T> m;
		m._m[0]  = c;    m._m[1]  = -s;   m._m[2]  = 0.0f; m._m[3]  = 0.0f;
		m._m[4]  = s;    m._m[5]  = c;    m._m[6]  = 0.0f; m._m[7]  = 0.0f;
		m._m[8]  = 0.0f; m._m[9]  = 0.0f; m._m[10] = 1.0f; m._m[11] = 0.0f;
		m._m[12] = 0.0f; m._m[13] = 0.0f; m._m[14] = 0.0f; m._m[15] = 1.0f;	
		return m;
	}

	static mat4<T> ortho(T left, T right, T bottom, T top, T znear, T zfar)
	{
		T tx = -(right + left) / (right - left);
		T ty = -(top + bottom) / (top - bottom);
		T tz = -(zfar + znear) / (zfar - znear);

		mat4<T> m(
			2.0f/(right-left), 0.0f,               0.0f,              tx,
			0.0f,              2.0f/(top-bottom),  0.0f,              ty,
			0.0f,              0.0f,               2.0f/(zfar-znear), tz,
			0.0f,              0.0f,               0.0f,              1.0f);

		return m;
	}

#if 0
        float r_l = right - left;
        float t_b = top - bottom;
        float f_n = far - near;
        float tx = - (right + left) / (right - left);
        float ty = - (top + bottom) / (top - bottom);
        float tz = - (far + near) / (far - near);
    
    matrix[0] = 2.0f / r_l;
    matrix[1] = 0.0f;
    matrix[2] = 0.0f;
    matrix[3] = tx;
    
    matrix[4] = 0.0f;
    matrix[5] = 2.0f / t_b;
    matrix[6] = 0.0f;
    matrix[7] = ty;
    
    matrix[8] = 0.0f;
    matrix[9] = 0.0f;
    matrix[10] = 2.0f / f_n;
    matrix[11] = tz;
    
    matrix[12] = 0.0f;
    matrix[13] = 0.0f;
    matrix[14] = 0.0f;
    matrix[15] = 1.0f;
}
#endif


	static mat4<T> multiply(const mat4<T>& a, const mat4<T>& b)
	{
		mat4<T> c;
		c._m[0]  = (a._m[0] * b._m[0])  + (a._m[1]  * b._m[4]) + (a._m[2]  * b._m[8])  + (a._m[3]  * b._m[12]);
		c._m[1]  = (a._m[0] * b._m[1])  + (a._m[1]  * b._m[5]) + (a._m[2]  * b._m[9])  + (a._m[3]  * b._m[13]);
		c._m[2]  = (a._m[0] * b._m[2])  + (a._m[1]  * b._m[6]) + (a._m[2]  * b._m[10]) + (a._m[3]  * b._m[14]);
		c._m[3]  = (a._m[0] * b._m[3])  + (a._m[1]  * b._m[7]) + (a._m[2]  * b._m[11]) + (a._m[3]  * b._m[15]);

		c._m[4]  = (a._m[4] * b._m[0])  + (a._m[5]  * b._m[4]) + (a._m[6]  * b._m[8])  + (a._m[7]  * b._m[12]);
		c._m[5]  = (a._m[4] * b._m[1])  + (a._m[5]  * b._m[5]) + (a._m[6]  * b._m[9])  + (a._m[7]  * b._m[13]);
		c._m[6]  = (a._m[4] * b._m[2])  + (a._m[5]  * b._m[6]) + (a._m[6]  * b._m[10]) + (a._m[7]  * b._m[14]);
		c._m[7]  = (a._m[4] * b._m[3])  + (a._m[5]  * b._m[7]) + (a._m[6]  * b._m[11]) + (a._m[7]  * b._m[15]);

		c._m[8]  = (a._m[8] * b._m[0])  + (a._m[9]  * b._m[4]) + (a._m[10] * b._m[8])  + (a._m[11] * b._m[12]);
		c._m[9]  = (a._m[8] * b._m[1])  + (a._m[9]  * b._m[5]) + (a._m[10] * b._m[9])  + (a._m[11] * b._m[13]);
		c._m[10] = (a._m[8] * b._m[2])  + (a._m[9]  * b._m[6]) + (a._m[10] * b._m[10]) + (a._m[11] * b._m[14]);
		c._m[11] = (a._m[8] * b._m[3])  + (a._m[9]  * b._m[7]) + (a._m[10] * b._m[11]) + (a._m[11] * b._m[15]);

		c._m[12] = (a._m[12] * b._m[0]) + (a._m[13] * b._m[4]) + (a._m[14] * b._m[8])  + (a._m[15] * b._m[12]);
		c._m[13] = (a._m[12] * b._m[1]) + (a._m[13] * b._m[5]) + (a._m[14] * b._m[9])  + (a._m[15] * b._m[13]);
		c._m[14] = (a._m[12] * b._m[2]) + (a._m[13] * b._m[6]) + (a._m[14] * b._m[10]) + (a._m[15] * b._m[14]);
		c._m[15] = (a._m[12] * b._m[3]) + (a._m[13] * b._m[7]) + (a._m[14] * b._m[11]) + (a._m[15] * b._m[15]);
		return c;
	}

private:

	void assign(const mat4<T>& rhs)
	{
		for (uint32_t i = 0; i < 16; ++i) { _m[i] = rhs._m[i]; }
	}

	T _m[16];
};
