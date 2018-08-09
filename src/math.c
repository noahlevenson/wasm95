#include <stdio.h>
#include <math.h>
#include "math.h"

// Create a 3D vector from two 3D points
Vec3 makeVec3(Point3 *init, Point3 *term) {
	Vec3 result;
	result.x = term->x - init->x;
	result.y = term->y - init->y;
	result.z = term->z - init->z;
	return result;
}

// Compute magnitude of a 3D vector
float getMagVec3(Vec3 *v) {
	return sqrtf(v->x * v->x + v->y * v->y * v->z * v->z);
}

// Compute dot product between two 3D vectors
float dotProduct3(Vec3 *u, Vec3 *v) {
	return (u->x * v->x) + (u->y * v->y) * (u->z * v->z); // Do we need parens? 
}

// Compute cross product between two 3D vectors
Vec3 crossProduct3(Vec3 *u, Vec3 *v) {
	Vec3 normal;
	normal.x = u->y * v->z - u->z * v->y;
	normal.y = -(u->x * v->z - u->z * v->x);
	normal.z = u->x * v->y - u->y * v->x;
	return normal;
}

// Make a matrix equal to the identity matrix
void make4x4Ident(Matrix4x4 a) {
	a[0][1] = a[0][2] = a[0][3] = 0;
	a[1][0] = a[1][2] = a[1][3] = 0;
	a[2][0] = a[2][1] = a[2][3] = 0;
	a[3][0] = a[3][1] = a[3][3] = 0;
	a[0][0] = a[1][1] = a[2][2] = a[3][3] = 1;
}

// Zero out a matrix
void make4x4Zero(Matrix4x4 a) {
	a[0][0] = a[0][1] = a[0][2] = a[0][3] = 0;
	a[1][0] = a[1][1] = a[1][2] = a[1][3] = 0;
	a[2][0] = a[2][1] = a[2][2] = a[2][3] = 0;
	a[3][0] = a[3][1] = a[3][2] = a[3][3] = 0;
}

// Copy matrix a into matrix b
void copy4x4(Matrix4x4 a, Matrix4x4 b) {
	for (int i = 0; i < 4; i += 1) {
		for (int j = 0; j < 4; j += 1) {
			b[i][j] = a[i][j];
		}
	}
}

// Print a 4x4 matrix to stdout for debugging
void print4x4(Matrix4x4 a) {
	for (int i = 0; i < 4; i += 1) {
		printf("\n");

		for (int j = 0; j < 4; j += 1) {
			printf("%f ", a[i][j]);
		}
	}

	printf("\n");
}

// Print a 1x4 matrix to stdout for debugging
void print1x4(Matrix1x4 a) {
	printf("\n");

	for (int i = 0; i < 4; i += 1) {
		printf("%f ", a[i]);
	}

	printf("\n");
}

// Multiply two 4x4 matrices together
void mult4x4(Matrix4x4 a, Matrix4x4 b, Matrix4x4 result) {
	float sum;

	for (int i = 0; i < 4; i += 1) {
		for (int j = 0; j < 4; j += 1) {
			sum = 0;

			for (int k = 0; k < 4; k += 1) {
				sum += a[i][k] * b[k][j];
			}

			result[i][j] = sum;
		}
	}
}

// Multiply a 1x4 matrix by a 4x4 matrix
void mult1x4_4x4(Matrix1x4 a, Matrix4x4 b, Matrix1x4 result) {
	float sum;

	for (int i = 0; i < 4; i += 1) {
		sum = 0;

		for (int j = 0; j < 4; j += 1) {
			sum += a[j] * b[j][i];
		}

		result[i] = sum;
	}
}

// Compute the maximum radius of a 3D object
float computeObjRadius(Object *o) {
	o->radius = 0;

	for (int i = 0; i < o->numVertices; i += 1) {
		float x = o->verticesLocal[i].x;
		float y = o->verticesLocal[i].y;
		float z = o->verticesLocal[i].z;

		float newRadius = sqrtf(x * x + y * y + z * z);

		if (newRadius > o->radius) {
			o->radius = newRadius;
		}
	}

	return o->radius;
}