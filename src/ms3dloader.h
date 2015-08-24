#ifndef __MS3D_LOADER_H__
#define __MS3D_LOADER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//typedef bool BOOL;
//typedef true TRUE;
//typedef false FALSE;

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x)	                                   if (x) { (x)->Release(); (x) = NULL; }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x)	                                   if (x) { delete (x); (x) = NULL; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x)	                           if (x) { delete [] (x); (x) = NULL; }
#endif

#define MAX(a, b)                                          ( (a) > (b) ? (a) : (b) )
#define MIN(a, b)                                          ( (a) < (b) ? (a) : (b) )

struct Vector2
{
	float x, y;
};

bool operator==(const Vector2 &left, const Vector2 &right);

inline bool operator==(const Vector2 &left, const Vector2 &right)
{
	return (left.x == right.x && left.y == right.y);
}

#include <math.h>

/**
 * Represents a 3D vector and provides common methods/operators
 * for vector math
 */
class Vector3
{
public:
	Vector3()                                              {}
	Vector3(float vx, float vy, float vz)                  { x = vx; y = vy; z = vz; }
	Vector3(const float *v)                                { x = v[0]; y = v[1]; z = v[2]; }
	~Vector3()                                             {}

	static Vector3 Cross(const Vector3 &a, const Vector3 &b);
	static float Dot(const Vector3 &a, const Vector3 &b);
	static Vector3 Normalize(const Vector3 &a);
	static Vector3 SurfaceNormal(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3);
	static float Magnitude(const Vector3 &a);
	static float SquaredLength(const Vector3 &a);
	static Vector3 SetLength(const Vector3 &v, float length);
	static float Distance(const Vector3 &a, const Vector3 &b);
	static bool IsPointInTriangle(const Vector3 &point, const Vector3 &pa, const Vector3 &pb, const Vector3 &pc);

	float x;
	float y;
	float z;
};

bool operator==(const Vector3 &left, const Vector3 &right);
Vector3 operator-(const Vector3 &left);
Vector3 operator+(const Vector3 &left, const Vector3 &right);
Vector3 &operator+=(Vector3 &left, const Vector3 &right);
Vector3 operator-(const Vector3 &left, const Vector3 &right);
Vector3 &operator-=(Vector3 &left, const Vector3 &right);
Vector3 operator*(const Vector3 &left, float right);
Vector3 &operator*=(Vector3 &left, float right);
Vector3 operator/(const Vector3 &left, float right);
Vector3 &operator/=(Vector3 &left, float right);
Vector3 operator*(const Vector3 &left, const Vector3 &right);
Vector3 &operator*=(Vector3 &left, const Vector3 &right);
Vector3 operator/(const Vector3 &left, const Vector3 &right);
Vector3 &operator/=(Vector3 &left, const Vector3 &right);

#define ZERO_VECTOR Vector3(0.0f, 0.0f, 0.0f)

#define X_AXIS Vector3(1.0f, 0.0f, 0.0f)
#define Y_AXIS Vector3(0.0f, 1.0f, 0.0f)
#define Z_AXIS Vector3(0.0f, 0.0f, 1.0f)

#define UP_VECTOR Vector3(0.0f, 1.0f, 0.0f)

/**
 * Computes the cross product of 2 vectors.
 * x = a.y * b.z - b.y * a.z
 * y = a.z * b.x - b.z * a.x
 * z = a.x * b.y - b.x * a.y
 * @param a first vector
 * @param b second vector
 *
 * @return Vector3 the cross product
 */
inline Vector3 Vector3::Cross(const Vector3 &a, const Vector3 &b)
{
	return Vector3(
		(a.y * b.z) - (b.y * a.z),
		(a.z * b.x) - (b.z * a.x),
		(a.x * b.y) - (b.x * a.y)
		);
}

/**
 * Computes the dot product of 2 vectors.
 * dot = (a.x * b.x) + (a.y * b.y) + (a.z * b.z)
 * @param a first vector
 * @param b second vector
 *
 * @return float the dot product
 */
inline float Vector3::Dot(const Vector3 &a, const Vector3 &b)
{
	return (a.x * b.x) +
		(a.y * b.y) +
		(a.z * b.z);
}

/**
 * Normalizes a vector
 * x = a.x / ||a||
 * y = a.y / ||a||
 * z = a.z / ||a||
 * @param a vector to normalize
 *
 * @return Vector3 the normalized vector
 */
inline Vector3 Vector3::Normalize(const Vector3 &a)
{
	float magnitudeSquared = (a.x * a.x) + (a.y * a.y) + (a.z * a.z);
	if (magnitudeSquared > 0.0f)
	{
		float inverseMagnitude = 1.0f / sqrtf(magnitudeSquared);
		return Vector3(
			a.x * inverseMagnitude,
			a.y * inverseMagnitude,
			a.z * inverseMagnitude
		);
	}
	else
		return a;
}

/**
 * Calculates a normal vector for the given 3 vectors making up
 * a triangle (counter-clockwise order)
 * @param v1 first vertex
 * @param v2 second vertex
 * @param v3 third vertex
 *
 * @return Vector3 normal vector for the triangle
 */
inline Vector3 Vector3::SurfaceNormal(const Vector3 &v1, const Vector3 &v2, const Vector3 &v3)
{
	return Vector3::Normalize(Vector3::Cross(v2 - v1, v3 - v1));
}

/**
 * Returns magnitude of a vector.
 * ||a|| = sqrt((a.x * a.x) + (a.y * a.y) + (a.z * a.z))
 * @param a vector to calculate the magnitude of
 *
 * @return float vector magnitude
 */
inline float Vector3::Magnitude(const Vector3 &a)
{
	return sqrtf(
		(a.x * a.x) +
		(a.y * a.y) +
		(a.z * a.z)
		);
}

/**
 * Returns the squared length of a vector (the magnitude minus
 * the sqrt call)
 * @param a vector to calculate the squared length of
 *
 * @return float squared length of the vector
 */
inline float Vector3::SquaredLength(const Vector3 &a)
{
	return
		(a.x * a.x) +
		(a.y * a.y) +
		(a.z * a.z);
}

/**
 * Adjusts a vector so that it's magnitude is equal to the given
 * length
 * @param v the original vector to be adjusted
 * @param length desired vector magnitude
 *
 * @return Vector3 the resulting vector after it's length has
 *         been converted to the desired amount
 */
inline Vector3 Vector3::SetLength(const Vector3 &v, float length)
{
	float magnitude = Vector3::Magnitude(v);
	return v * (length / magnitude);
}

/**
 * Calculates the distance between two points
 * @param a the first point
 * @param b the second point
 *
 * @return float the distance between both points
 */
inline float Vector3::Distance(const Vector3 &a, const Vector3 &b)
{
	return sqrtf(
		((b.x - a.x) * (b.x - a.x)) +
		((b.y - a.y) * (b.y - a.y)) +
		((b.z - a.z) * (b.z - a.z))
		);
}

/**
 * Checks if a given point lies inside a triangle or not
 * @param point point to test
 * @param a first vector of the triangle
 * @param b second vector of the triangle
 * @param c third vector of the triangle
 *
 * @return BOOL TRUE if the point lies inside the triangle,
 *         FALSE if it doesn't
 */
inline bool Vector3::IsPointInTriangle(const Vector3 &point, const Vector3 &pa, const Vector3 &pb, const Vector3 &pc)
{
	Vector3 edge1 = pb - pa;
	Vector3 edge2 = pc - pa;

	float a = Vector3::Dot(edge1, edge1);
	float b = Vector3::Dot(edge1, edge2);
	float c = Vector3::Dot(edge2, edge2);
	float ac_bb = (a * c) - (b * b);
	Vector3 vp(point.x - pa.x, point.y - pa.y, point.z - pa.z);

	float d = Vector3::Dot(vp, edge1);
	float e = Vector3::Dot(vp, edge2);
	float x = (d * c) - (e * b);
	float y = (e * a) - (d * b);
	float z = x + y - ac_bb;

	int result = (( ((unsigned int&) z)& ~(((unsigned int&) x)|((unsigned int&) y)) ) & 0x80000000);
	if (result == 0)
		return false;
	else
		return true;
}

inline bool operator==(const Vector3 &left, const Vector3 &right)
{
	return (left.x == right.x && left.y == right.y && left.z == right.z);
}

inline Vector3 operator-(const Vector3 &left)
{
	return Vector3(-left.x, -left.y, -left.z);
}

inline Vector3 operator+(const Vector3 &left, const Vector3 &right)
{
	return Vector3(left.x + right.x, left.y + right.y, left.z + right.z);
}

inline Vector3 &operator+=(Vector3 &left, const Vector3 &right)
{
	left.x += right.x;
	left.y += right.y;
	left.z += right.z;

	return left;
}

inline Vector3 operator-(const Vector3 &left, const Vector3 &right)
{
	return Vector3(left.x - right.x, left.y - right.y, left.z - right.z);
}

inline Vector3 &operator-=(Vector3 &left, const Vector3 &right)
{
	left.x -= right.x;
	left.y -= right.y;
	left.z -= right.z;

	return left;
}

inline Vector3 operator*(const Vector3 &left, float right)
{
	return Vector3(left.x * right, left.y * right, left.z * right);
}

inline Vector3 &operator*=(Vector3 &left, float right)
{
	left.x *= right;
	left.y *= right;
	left.z *= right;

	return left;
}

inline Vector3 operator/(const Vector3 &left, float right)
{
	return Vector3(left.x / right, left.y / right, left.z / right);
}

inline Vector3 &operator/=(Vector3 &left, float right)
{
	left.x /= right;
	left.y /= right;
	left.z /= right;

	return left;
}

inline Vector3 operator*(const Vector3 &left, const Vector3 &right)
{
	return Vector3(left.x * right.x, left.y * right.y, left.z * right.z);
}

inline Vector3 &operator*=(Vector3 &left, const Vector3 &right)
{
	left.x *= right.x;
	left.y *= right.y;
	left.z *= right.z;

	return left;
}

inline Vector3 operator/(const Vector3 &left, const Vector3 &right)
{
	return Vector3(left.x / right.x, left.y / right.y, left.z / right.z);
}

inline Vector3 &operator/=(Vector3 &left, const Vector3 &right)
{
	left.x /= right.x;
	left.y /= right.y;
	left.z /= right.z;

	return left;
}


//==============================================================================
#include <string>

struct Ms3dHeader
{
	char id[10];
	int32_t version;
};

struct Ms3dVertex
{
	uint8_t editorFlags;
	Vector3 vertex;
	int8_t jointIndex;
	uint8_t unused;
};

struct Ms3dTriangle
{
	uint16_t editorFlags;
	uint16_t vertices[3];
	Vector3 normals[3];
	Vector2 texCoords[3];
	uint8_t smoothingGroup;
	uint8_t meshIndex;
};

struct Ms3dMesh
{
	uint8_t editorFlags;
	std::string name;
	uint16_t numTriangles;
	uint16_t *triangles;
	int8_t materialIndex;

	Ms3dMesh()
	{
		triangles = NULL;
	}

	~Ms3dMesh()
	{
		SAFE_DELETE_ARRAY(triangles);
	}
};

struct Ms3dMaterial
{
	std::string name;
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emissive[4];
	float shininess;
	float transparency;
	int8_t mode;
	std::string texture;
	std::string alpha;
};

struct Ms3dKeyFrame
{
	float time;
	Vector3 param;
};

struct Ms3dJoint
{
	uint8_t editorFlags;
	std::string name;
	std::string parentName;
	Vector3 rotation;
	Vector3 position;
	uint16_t numRotationFrames;
	uint16_t numTranslationFrames;
	Ms3dKeyFrame *rotationFrames;
	Ms3dKeyFrame *translationFrames;

	Ms3dJoint()
	{
		rotationFrames = NULL;
		translationFrames = NULL;
	}

	~Ms3dJoint()
	{
		SAFE_DELETE_ARRAY(rotationFrames);
		SAFE_DELETE_ARRAY(translationFrames);
	}
};

class Ms3d
{
public:
	Ms3d();
	virtual ~Ms3d()                                        { Release(); }

	void Release();
	bool Load(const std::string &file);

	uint16_t GetNumVertices() const                        { return m_numVertices; }
	uint16_t GetNumTriangles() const                       { return m_numTriangles; }
	uint16_t GetNumMeshes() const                          { return m_numMeshes; }
	uint16_t GetNumMaterials() const                       { return m_numMaterials; }
	uint16_t GetNumJoints() const                          { return m_numJoints; }
	float GetAnimationFps() const                          { return m_animationFps; }
	uint32_t GetNumFrames() const                          { return m_numFrames; }
	Ms3dVertex* GetVertices() const                        { return m_vertices; }
	Ms3dTriangle* GetTriangles() const                     { return m_triangles; }
	Ms3dMesh* GetMeshes() const                            { return m_meshes; }
	Ms3dMaterial* GetMaterials() const                     { return m_materials; }
	Ms3dJoint* GetJoints() const                           { return m_joints; }

	int32_t FindIndexOfMesh(const std::string &meshName) const;
	int32_t FindIndexOfJoint(const std::string &jointName) const;

private:

	uint16_t m_numVertices;
	uint16_t m_numTriangles;
	uint16_t m_numMeshes;
	uint16_t m_numMaterials;
	uint16_t m_numJoints;
	float m_animationFps;
	float m_editorAnimationTime;
	uint32_t m_numFrames;
	Ms3dVertex *m_vertices;
	Ms3dTriangle *m_triangles;
	Ms3dMesh *m_meshes;
	Ms3dMaterial *m_materials;
	Ms3dJoint *m_joints;
};


void ms3d_draw(Ms3d *m, float dt);


#endif // __MS3D_LOADER_H__
