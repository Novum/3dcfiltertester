#pragma once

#include <cmath>
#include <string>

namespace engine
{
	// Matrix
	template <unsigned int dimx,unsigned int dimy,class type=float> class Matrix 
	{
	public:
		template <unsigned int dimx2, unsigned int dimy2, class type2> class Accessor
		{
			friend class Matrix<dimx2,dimy2,type2>;
		public:
			type &operator[] (unsigned int y) { return matrix.matrix[y+dimy2*x]; }
			operator type2() { return matrix.matrix[y+dimy2*x]; }	
			type operator =(type value) { return(matrix.matrix[y+dimy2*x] = value); }
		private:
			explicit Accessor(unsigned int x,Matrix<dimx2,dimy2,type2> &matrix) : matrix(matrix), x(x) {}
			unsigned int x;
			Matrix<dimx2,dimy2,type2> &matrix;
		};

		friend class Matrix::Accessor<dimx,dimy,type>;

		Accessor<dimx,dimy,type> operator [] (unsigned int x) { return Accessor<dimx,dimy,type>(x,*this); }	

		Matrix<dimx,dimy,type> operator *(type scalar) {
			Matrix<dimx,dimy,type> ret;
			for(unsigned int i=0;i<dimx*dimy;++i) ret.matrix[i] = matrix[i] * scalar; 
			return ret;
		}

		void operator *=(type scalar) { 
			for(unsigned int i=0;i<dimx*dimy;++i) matrix[i]*=scalar; 
		}

		template <unsigned int columns> Matrix<dimx,dimy,type> &operator *=(Matrix<columns,dimx,type> &mat);

		Matrix<dimx,dimy,type> operator /(type scalar) {
			Matrix<dimx,dimy,type> ret;
			type rec = 1.0/scalar;
			for(unsigned int i=0;i<dimx*dimy;++i) ret.matrix[i] = matrix[i] * rec; 
			return ret;
		}

		void operator /=(type scalar) {
			type rec = 1.0/scalar;
			for(unsigned int i=0;i<dimx*dimy;++i) matrix[i] *= rec; 
		}

		friend std::ostream &operator <<(std::ostream& os, const Matrix<4,4,type> &matrix) 
		{
			for(unsigned int y=0;y<4;++y) {
				for(unsigned int x=0;x<4;++x) {
					os << matrix.matrix[y+dimy*x] << " ";
				}
				if(y < 4) os << std::endl;
			}

			return os;
		}
	private:
		type matrix[dimx*dimy];
	};

	template <class type,class a_type,class b_type,unsigned int a_rows,unsigned int a_columns_b_rows,unsigned int b_columns> 
	inline Matrix<b_columns,a_rows,type> matrix_mult(Matrix<a_columns_b_rows,a_rows,a_type> &a, Matrix<b_columns,a_columns_b_rows,b_type> &b) { 
		Matrix<b_columns,a_rows,type> result;

		for(unsigned int x=0;x<b_columns;++x)	
			for(unsigned int y=0;y<a_rows;++y) {
				result[x][y] = (type)0;		
				for(unsigned int i=0;i<a_columns_b_rows;++i)
					result[x][y] += a[i][y] * b[x][i];
			}	

		return result;
	} 
	
	template <class type,unsigned int a_rows,unsigned int a_columns_b_rows,unsigned int b_columns>
	inline Matrix<b_columns,a_rows,type> operator *(Matrix<a_columns_b_rows,a_rows,type> &a, Matrix<b_columns,a_columns_b_rows,type> &b) {
		return matrix_mult<type>(a,b);
	}

	template <unsigned int dimx,unsigned int dimy,class type> template <unsigned int columns> Matrix<dimx,dimy,type> 
	&Matrix<dimx,dimy,type>::operator *=(Matrix<columns,dimx,type> &mat) {
		*this = matrix_mult<type>(*this,mat);		
		return *this;
	}
	
	// Vector
	template <unsigned int dim,class type=float> class Vector
	{
	public:
		Vector(type arg1=(type)0,type arg2=(type)0,type arg3=(type)0,
			   type arg4=(type)0,type arg5=(type)0,type arg6=(type)0,
			   type arg7=(type)0,type arg8=(type)0,type arg9=(type)0) 
		{
			if(dim>=1) vector[0] = arg1;	if(dim>=2) vector[1] = arg2;	if(dim>=3) vector[2] = arg3;
			if(dim>=4) vector[3] = arg4;	if(dim>=5) vector[4] = arg5;	if(dim>=6) vector[5] = arg6;
			if(dim>=7) vector[6] = arg7;	if(dim>=8) vector[7] = arg8;	if(dim>=9) vector[8] = arg9;			
		}

		template<unsigned int dim2> operator Vector<dim2> () {
			Vector<dim2> ret;
			for(unsigned int i=0;i<dim2;++i) {
				ret[i] = i < dim ? vector[i] : (type)0;
			}
			return ret;
		}

		type Length() { 
			type sqr_sum = (type)0;
			for(unsigned int i=0;i<dim;++i) sqr_sum += vector[i] * vector[i];
			return sqrt(sqr_sum);
		}

		void Normalize() {
			*this /= Length();
		}

		type &operator[] (unsigned int i) { return vector[i]; }		
		const type &operator[] (unsigned int i) const { return vector[i]; }

		bool operator ==(Vector<dim,type> &val) {
			for(unsigned int i=0;i<dim;++i)
				if(vector[i] != val.vector[i]) return false;
			return true;
		}

		bool operator !=(Vector<dim,type> &val) {
			for(unsigned int i=0;i<dim;++i)
				if(vector[i] != val.vector[i]) return true;
			return false;
		}

		Vector<dim,type> operator *(type scalar) {
			Vector<dim,type> ret;
			for(unsigned int i=0;i<dim;++i) ret.vector[i] = vector[i] * scalar;
			return ret;
		}		

		void operator *=(type scalar) {
			for(unsigned int i=0;i<dim;++i) vector[i] *= scalar;
		}

		Vector<dim,type> operator +=(Vector<dim,type> op) {
			for(unsigned int i=0;i<dim;++i) vector[i] += op.vector[i];
			return *this;
		}

		Vector<dim,type> operator -=(Vector<dim,type> op) {
			for(unsigned int i=0;i<dim;++i) vector[i] -= op.vector[i];
			return *this;
		}

		Vector<dim,type> operator +() {
			return *this;
		}

		Vector<dim,type> operator -() {
			return *this * -1.0f;
		}

		Vector<dim,type> operator /(type scalar) {
			Vector<dim,type> ret;
			type rec = (type)1.0 / (type)scalar;
			for(unsigned int i=0;i<dim;++i) ret.vector[i] = vector[i] * rec;
			return ret;
		}

		void operator /=(type scalar) {
			type rec = (type)1.0 / (type)scalar;
			for(unsigned int i=0;i<dim;++i) vector[i] *= rec;
		}		
	private:
		type vector[dim];
	};

	template <class type,class type_a,class type_b>
	inline Vector<3,type> cross_product(Vector<3,type_a> &a,Vector<3,type_b> &b) {
		Vector<3,type> ret;
		ret[0] = a[1] * b[2] - a[2] * b[1];
		ret[1] = a[2] * b[0] - a[0] * b[2];
		ret[2] = a[0] * b[1] - a[1] * b[0];
		return ret;
	}

	template <class type>
	inline Vector<3,type> cross(Vector<3,type> &a,Vector<3,type> &b) {
		return cross_product<type>(a,b);
	}

	template <class type,class type_a,class type_b,unsigned int dim>
	inline type dot_product(Vector<dim,type_a> &a,Vector<dim,type_b> &b) {
		type ret = (type)0;
		for(unsigned int i=0;i<dim;++i) ret += (type)a[i] * (type)b[i];
		return ret;
	}

	template <class type,unsigned int dim>
	inline type operator *(Vector<dim,type> &a,Vector<dim,type> &b) {
		return dot_product<type>(a,b);
	}

	template <class type,class type_a,class type_b,unsigned int dim>
	inline Vector<dim,type> add(Vector<dim,type_a> &a,Vector<dim,type_b> &b) {
		Vector<dim,type> ret;
		for(unsigned int i=0;i<dim;++i) ret[i] = a[i] + b[i];
		return ret;
	}

	template <class type,unsigned int dim>
	inline Vector<dim,type> operator +(Vector<dim,type> &a,Vector<dim,type> &b) {
		return add<type>(a,b);
	}	

	template <class type,class type_a,class type_b,unsigned int dim>
	inline Vector<dim,type> sub(Vector<dim,type_a> &a,Vector<dim,type_b> &b) {
		Vector<dim,type> ret;
		for(unsigned int i=0;i<dim;++i) ret[i] = a[i] - b[i];
		return ret;
	}

	template <class type,unsigned int dim>
	inline Vector<dim,type> operator -(Vector<dim,type> &a,Vector<dim,type> &b) {
		return sub<type>(a,b);
	}

	template <class type,class a_type,class b_type,unsigned int a_columns,unsigned int a_rows,unsigned int dim>
	inline Vector<a_columns,type> matrix_mult(Matrix<a_columns,a_rows,a_type> &a, Vector<dim,b_type> &b)
	{
		Vector<a_columns,type> result;		

		for(unsigned int y=0;y<a_rows;++y) {
			result[y] = (type)0;		
			for(unsigned int i=0;i<a_columns;++i) {
				if(i < dim) result[y] += a[i][y] * b[i];
				else result[y] += a[i][y];
			}
		}

		return result;
	}

	template <class type,unsigned int a_columns,unsigned int a_rows,unsigned int dim>
	inline Vector<a_columns,type> operator *(Matrix<a_columns,a_rows,type> &a, Vector<dim,type> &b) {
		return matrix_mult<type>(a,b);		
	}

	// Identity
	template <unsigned int size,class type> Matrix<size,size,type> MatrixIdentity() {
		Matrix<size,size,type> identitiy;
		for(unsigned int x=0;x<size;++x)
			for(unsigned int y=0;y<size;++y)
				identitiy[x][y] = (x==y) ? (type)1 : (type)0;
		return identitiy;
	}

	// Transpose
	template <unsigned int columns,unsigned int rows,class type> Matrix<rows,columns,type> MatrixTranspose(Matrix<columns,rows,type> &matrix) {
		Matrix<rows,columns,type> transpose;
		for(unsigned int x=0;x<columns;++x)
			for(unsigned int y=0;y<rows;++y)
				transpose[y][x] = matrix[x][y];
		return transpose;
	}

	// X Rotation Matrix
	template <class type> Matrix<4,4,type> XRotationMatrix44(type angle) {
		Matrix<4,4,type> matrix;
		matrix[0][0] = 1;	matrix[1][0] = 0;			matrix[2][0] = 0;			matrix[3][0] = 0;
		matrix[0][1] = 0;	matrix[1][1] = cos(angle);	matrix[2][1] = -sin(angle);	matrix[3][1] = 0;
		matrix[0][2] = 0;	matrix[1][2] = sin(angle);	matrix[2][2] = cos(angle);	matrix[3][2] = 0;
		matrix[0][3] = 0;	matrix[1][3] = 0;			matrix[2][3] = 0;			matrix[3][3] = 1;
		return matrix;
	}

	template <class type> Matrix<3,3,type> XRotationMatrix33(type angle) {
		Matrix<3,3,type> matrix;
		matrix[0][0] = 1;	matrix[1][0] = 0;			matrix[2][0] = 0;
		matrix[0][1] = 0;	matrix[1][1] = cos(angle);	matrix[2][1] = -sin(angle);
		matrix[0][2] = 0;	matrix[1][2] = sin(angle);	matrix[2][2] = cos(angle);
		return matrix;
	}	

	// Y Rotation Matrix
	template <class type> Matrix<4,4,type> YRotationMatrix44(type angle) {
		Matrix<4,4,type> matrix;
		matrix[0][0] = cos(angle);	matrix[1][0] = 0;	matrix[2][0] = sin(angle);	matrix[3][0] = 0;
		matrix[0][1] = 0;			matrix[1][1] = 1;	matrix[2][1] = 0;			matrix[3][1] = 0;
		matrix[0][2] = -sin(angle);	matrix[1][2] = 0;	matrix[2][2] = cos(angle);	matrix[3][2] = 0;
		matrix[0][3] = 0;			matrix[1][3] = 0;	matrix[2][3] = 0;			matrix[3][3] = 1;
		return matrix;
	}

	template <class type> Matrix<3,3,type> YRotationMatrix33(type angle) {
		Matrix<3,3,type> matrix;
		matrix[0][0] = cos(angle);	matrix[1][0] = 0;	matrix[2][0] = sin(angle);
		matrix[0][1] = 0;			matrix[1][1] = 1;	matrix[2][1] = 0;
		matrix[0][2] = -sin(angle);	matrix[1][2] = 0;	matrix[2][2] = cos(angle);
		return matrix;
	}

	// Z Rotation Matrix
	template <class type> Matrix<4,4,type> ZRotationMatrix44(type angle) {
		Matrix<4,4,type> matrix;
		matrix[0][0] = cos(angle);	matrix[1][0] = -sin(angle);		matrix[2][0] = (type)0;		matrix[3][0] = (type)0;
		matrix[0][1] = sin(angle);	matrix[1][1] = cos(angle);		matrix[2][1] = (type)0;		matrix[3][1] = (type)0;
		matrix[0][2] = (type)0;		matrix[1][2] = (type)0;			matrix[2][2] = (type)1;		matrix[3][2] = (type)0;
		matrix[0][3] = (type)0;		matrix[1][3] = (type)0;			matrix[2][3] = (type)0;		matrix[3][3] = (type)1;
		return matrix;
	}

	template <class type> Matrix<3,3,type> ZRotationMatrix33(type angle) {
		Matrix<3,3,type> matrix;
		matrix[0][0] = cos(angle);	matrix[1][0] = -sin(angle);		matrix[2][0] = (type)0;
		matrix[0][1] = sin(angle);	matrix[1][1] = cos(angle);		matrix[2][1] = (type)0;
		matrix[0][2] = (type)0;		matrix[1][2] = (type)0;			matrix[2][2] = (type)1;	
		return matrix;
	}	

	// Arbitary Rotation Matrix
	template <class type> Matrix<4,4,type> ArbitaryRotationMatrix44(type angle,Vector<3,type> axis) {
		Matrix<4,4,type> matrix;

		const type c = cos(angle);
		const type s = sin(angle);

		matrix[0][0] = c+axis[0]*axis[0]*(1-c);				
		matrix[1][0] = axis[0]*axis[1]*(1-c)-axis[2]*s;		
		matrix[2][0] = axis[0]*axis[2]*(1-c)+axis[1]*s;
		matrix[3][0] = (type)0;

		matrix[0][1] = axis[0]*axis[1]*(1-c)+axis[2]*s;
		matrix[1][1] = c+axis[1]*axis[1]*(1-c);
		matrix[2][1] = axis[1]*axis[2]*(1-c)-axis[0]*s;
		matrix[3][1] = (type)0;

		matrix[0][2] = axis[0]*axis[2]*(1-c)-axis[1]*s;
		matrix[1][2] = axis[1]*axis[2]*(1-c)+axis[0]*s;
		matrix[2][2] = c+axis[2]*axis[2]*(1-c);
		matrix[3][2] = (type)0;

		matrix[0][3] = (type)0;
		matrix[1][3] = (type)0;
		matrix[2][3] = (type)0;
		matrix[3][3] = (type)1;		

		return matrix;
	}

	template <class type> Matrix<3,3,type> ArbitaryRotationMatrix33(type angle,Vector<3,type> axis) {
		Matrix<3,3,type> matrix;
		
		const type c = cos(angle);
		const type s = sin(angle);

		matrix[0][0] = c+axis[0]*axis[0]*(1-c);				
		matrix[1][0] = axis[0]*axis[1]*(1-c)-axis[2]*s;		
		matrix[2][0] = axis[0]*axis[2]*(1-c)+axis[1]*s;

		matrix[0][1] = axis[0]*axis[1]*(1-c)+axis[2]*s;
		matrix[1][1] = c+axis[1]*axis[1]*(1-c);
		matrix[2][1] = axis[1]*axis[2]*(1-c)-axis[0]*s;

		matrix[0][2] = axis[0]*axis[2]*(1-c)-axis[1]*s;
		matrix[1][2] = axis[1]*axis[2]*(1-c)+axis[0]*s;
		matrix[2][2] = c+axis[2]*axis[2]*(1-c);

		return matrix;
	}

	// Quaternion
	template<class type=float> class Quaternion
	{
	public:
		Quaternion() {}
		Quaternion(type w,type x,type y,type z) : w(w),x(x),y(y),z(z) {}

		Quaternion(Vector<3,type> axis,type angle) {
			w = cos(angle * (type)0.5);
			const type s = sin(angle * (type)0.5);			
			x = axis[0] * s; y = axis[1] * s; z = axis[2] * s;
		}
		
		Quaternion<type> operator *(Quaternion<type> &a) {
			Quaternion<type> ret;
			ret.w = w*a.w - x*a.x - y*a.y - z*a.z;
			ret.x = w*a.x + x*a.w + y*a.z - z*a.y;
			ret.y = w*a.y - x*a.z + y*a.w + z*a.x;
			ret.z = w*a.z + x*a.y - y*a.x + z*a.w;
			return ret;
		}

		Vector<3,type> operator *(Vector<3,type> &a) {
			Vector3 uv, uuv; 
			Vector3 qvec(x,y,z);
			uv = cross(qvec,a);
			uuv = cross(qvec,uv);
			uv *= ((type)2 * w); 
			uuv *= (type)2;
			return a + uv + uuv;
		}

		void operator *=(Quaternion<type> &a) {
			*this = *this * a;
		}

		Quaternion<type> inverse() {
			Quaternion<type> ret;
			type one_over_sqr = (type)1 / (w*w + x*x + y*y + z*z);
			ret.w = w * one_over_sqr; ret.x = -x * one_over_sqr;
			ret.y = -y * one_over_sqr; ret.z = -z * one_over_sqr;
			return ret;
		}

		operator std::wstring() {
			std::wstring ret = L"Quaternion(" 
				+ boost::lexical_cast<std::wstring>(w) + L","
				+ boost::lexical_cast<std::wstring>(x) + L","
				+ boost::lexical_cast<std::wstring>(y) + L","
				+ boost::lexical_cast<std::wstring>(z) + L")";
			return ret;
		}
	private:
		type w,x,y,z;
	};

	template<class type> Quaternion<type> QuaternionIdentity()
	{
		return(Quaternion<type>((type)1,(type)0,(type)0,(type)0));
	}

	typedef Matrix<4,4> Matrix44;	
	typedef Matrix<3,3> Matrix33;

	typedef Vector<2> Vector2;
	typedef Vector<3> Vector3;
	typedef Vector<4> Vector4;	

	typedef Quaternion<> Quat;
}