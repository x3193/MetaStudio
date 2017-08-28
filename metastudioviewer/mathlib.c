#include "mathlib.h"
#include <memory.h>


void VectorClear( vec3_t dest )
{
	dest[0] = 0.0;
	dest[1] = 0.0;
	dest[2] = 0.0;
}

void VectorCopy( vec3_t src, vec3_t dest )
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
}

void VectorCopy4( vec4_t src, vec4_t dest )
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
}

void VectorMA( vec3_t vec1, float scale, vec3_t vec2, vec3_t dest )
{
	int		i;

	for ( i = 0; i < 3; i++ )
	{
		dest[i] = vec1[i] + vec2[i] * scale;
	}
}

void GL_PositionSlerp( vec3_t vec1, vec3_t vec2, float t, vec3_t dest )
{
	int		i;

	for ( i = 0; i < 3; i++ )
	{
		dest[i] = vec1[i] + t * ( vec2[i] - vec1[i] );
	}
}

void GL_QuaternionSlerp( vec4_t quat1, vec4_t quat2, float t, vec4_t dest )
{
	float	rot1q[4];
	float	omega, cosom, oosinom;
	float	scalerot0, scalerot1;

	cosom = quat1[0]*quat2[0] + quat1[1]*quat2[1] + quat1[2]*quat2[2] + quat1[3]*quat2[3];

	if( cosom < 0.0f )
	{
		cosom = -cosom;
		rot1q[0] = -quat2[0];
		rot1q[1] = -quat2[1];
		rot1q[2] = -quat2[2];
		rot1q[3] = -quat2[3];
	}
	else  
	{
		rot1q[0] = quat2[0];
		rot1q[1] = quat2[1];
		rot1q[2] = quat2[2];
		rot1q[3] = quat2[3];
	}

	if ( ( 1.0f - cosom ) > 0.0001f ) 
	{ 
		omega = acos( cosom );
		oosinom = 1.0f / sin( omega );
		scalerot0 = sin( ( 1.0f - t ) * omega ) * oosinom;
		scalerot1 = sin( t * omega ) * oosinom;
	}
	else
	{ 
		scalerot0 = 1.0f - t;
		scalerot1 = t;
	}

	dest[0] = ( scalerot0 * quat1[0] + scalerot1 * rot1q[0] );
	dest[1] = ( scalerot0 * quat1[1] + scalerot1 * rot1q[1] );
	dest[2] = ( scalerot0 * quat1[2] + scalerot1 * rot1q[2] );
	dest[3] = ( scalerot0 * quat1[3] + scalerot1 * rot1q[3] );
}

/***************************

 The GL matrix

 | Xx Yx Zx Tx |
 | Xy Yy Zy Ty |
 | Xz Yz Zz Tz |
 |  0  0  0  1 |

 This matrix store by column :

  m[0] = [ Xx Xy Xz 0 ];
  m[1] = [ Yx Yy Yz 0 ];
  m[2] = [ Zx Zy Zz 0 ];
  m[3] = [ Tx Ty Tz 1 ];

***************************/

void MatrixIdentity( mat4_t dest )
{
	dest[0][0] = 1.0;  dest[1][0] = 0.0;  dest[2][0] = 0.0;  dest[3][0] = 0.0;
	dest[0][1] = 0.0;  dest[1][1] = 1.0;  dest[2][1] = 0.0;  dest[3][1] = 0.0;
	dest[0][2] = 0.0;  dest[1][2] = 0.0;  dest[2][2] = 1.0;  dest[3][2] = 0.0;
	dest[0][3] = 0.0;  dest[1][3] = 0.0;  dest[2][3] = 0.0;  dest[3][3] = 1.0;
}

void MatrixCopy( mat4_t src, mat4_t dest )
{
	memcpy( dest, src, sizeof( mat4_t ) );
}

void GL_QuaternionMatrix( vec4_t quat, mat4_t mat )
{
	float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz;

	s = 2.0 / ( quat[0]*quat[0] + quat[1]*quat[1] + quat[2]*quat[2] + quat[3]*quat[3] );

	xs = quat[0] * s;
	ys = quat[1] * s;
	zs = quat[2] * s;

	wx = quat[3] * xs;
	wy = quat[3] * ys;
	wz = quat[3] * zs;

	xx = quat[0] * xs;
	xy = quat[0] * ys;
	xz = quat[0] * zs;

	yy = quat[1] * ys;
	yz = quat[1] * zs;

	zz = quat[2] * zs;

	mat[0][0] = 1.0 - ( yy + zz );
	mat[1][0] = xy - wz;
	mat[2][0] = xz + wy;
	
	mat[0][1] = xy + wz;
	mat[1][1] = 1.0 - ( xx + zz );
	mat[2][1] = yz - wx;

	mat[0][2] = xz - wy;
	mat[1][2] = yz + wx;
	mat[2][2] = 1.0 - ( xx + yy );

	mat[3][0] = mat[3][1] = mat[3][2] = mat[0][3] = mat[1][3] = mat[2][3] = 0.0;
	mat[3][3] = 1.0;
}

void GL_ConcatTransforms( mat4_t mat1, mat4_t mat2, mat4_t dest )
{
	dest[0][0] = mat1[0][0] * mat2[0][0] + mat1[1][0] * mat2[0][1] + mat1[2][0] * mat2[0][2] + mat1[3][0] * mat2[0][3];
	dest[0][1] = mat1[0][1] * mat2[0][0] + mat1[1][1] * mat2[0][1] + mat1[2][1] * mat2[0][2] + mat1[3][1] * mat2[0][3];
	dest[0][2] = mat1[0][2] * mat2[0][0] + mat1[1][2] * mat2[0][1] + mat1[2][2] * mat2[0][2] + mat1[3][2] * mat2[0][3];
	dest[0][3] = mat1[0][3] * mat2[0][0] + mat1[1][3] * mat2[0][1] + mat1[2][3] * mat2[0][2] + mat1[3][3] * mat2[0][3];

	dest[1][0] = mat1[0][0] * mat2[1][0] + mat1[1][0] * mat2[1][1] + mat1[2][0] * mat2[1][2] + mat1[3][0] * mat2[1][3];
	dest[1][1] = mat1[0][1] * mat2[1][0] + mat1[1][1] * mat2[1][1] + mat1[2][1] * mat2[1][2] + mat1[3][1] * mat2[1][3];
	dest[1][2] = mat1[0][2] * mat2[1][0] + mat1[1][2] * mat2[1][1] + mat1[2][2] * mat2[1][2] + mat1[3][2] * mat2[1][3];
	dest[1][3] = mat1[0][3] * mat2[1][0] + mat1[1][3] * mat2[1][1] + mat1[2][3] * mat2[1][2] + mat1[3][3] * mat2[1][3];

	dest[2][0] = mat1[0][0] * mat2[2][0] + mat1[1][0] * mat2[2][1] + mat1[2][0] * mat2[2][2] + mat1[3][0] * mat2[2][3];
	dest[2][1] = mat1[0][1] * mat2[2][0] + mat1[1][1] * mat2[2][1] + mat1[2][1] * mat2[2][2] + mat1[3][1] * mat2[2][3];
	dest[2][2] = mat1[0][2] * mat2[2][0] + mat1[1][2] * mat2[2][1] + mat1[2][2] * mat2[2][2] + mat1[3][2] * mat2[2][3];
	dest[2][3] = mat1[0][3] * mat2[2][0] + mat1[1][3] * mat2[2][1] + mat1[2][3] * mat2[2][2] + mat1[3][3] * mat2[2][3];

	dest[3][0] = mat1[0][0] * mat2[3][0] + mat1[1][0] * mat2[3][1] + mat1[2][0] * mat2[3][2] + mat1[3][0] * mat2[3][3];
	dest[3][1] = mat1[0][1] * mat2[3][0] + mat1[1][1] * mat2[3][1] + mat1[2][1] * mat2[3][2] + mat1[3][1] * mat2[3][3];
	dest[3][2] = mat1[0][2] * mat2[3][0] + mat1[1][2] * mat2[3][1] + mat1[2][2] * mat2[3][2] + mat1[3][2] * mat2[3][3];
	dest[3][3] = mat1[0][3] * mat2[3][0] + mat1[1][3] * mat2[3][1] + mat1[2][3] * mat2[3][2] + mat1[3][3] * mat2[3][3];
}

void GL_VectorRotate( vec3_t src, mat4_t mat, vec3_t dest )
{
	dest[0] = mat[0][0] * src[0] + mat[1][0] * src[1] + mat[2][0] * src[2];
	dest[1] = mat[0][1] * src[0] + mat[1][1] * src[1] + mat[2][1] * src[2];
	dest[2] = mat[0][2] * src[0] + mat[1][2] * src[1] + mat[2][2] * src[2];
}

void GL_VectorTransform( vec3_t src, mat4_t mat, vec3_t dest )
{
	dest[0] = mat[0][0] * src[0] + mat[1][0] * src[1] + mat[2][0] * src[2] + mat[3][0];
	dest[1] = mat[0][1] * src[0] + mat[1][1] * src[1] + mat[2][1] * src[2] + mat[3][1];
	dest[2] = mat[0][2] * src[0] + mat[1][2] * src[1] + mat[2][2] * src[2] + mat[3][2];
}