#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <emscripten/emscripten.h>
#include "gfx.h"
#include "wasm95.h"

EMSCRIPTEN_KEEPALIVE Screenbuffer* makeScreenbuffer(unsigned char buffer[], int w, int h) {
	Screenbuffer *s = malloc(sizeof(Screenbuffer));
	s->w = w;
	s->h = h;
	s->byteW = w * 4;
	s->byteSize = w * h * 4;
	s->buffer = buffer;
	return s; 
}

EMSCRIPTEN_KEEPALIVE void killScreenbuffer(Screenbuffer *s) {
	free(s);
}

EMSCRIPTEN_KEEPALIVE void clearScreenbuffer(Screenbuffer *s) {
	memset(s->buffer, 0, s->byteSize);
}

// Write a pixel of a specified color to a specified screenbuffer at a specified offset
void setPixel(Screenbuffer *s, int offset, Color *c) {
	s->buffer[offset] = c->r;
	s->buffer[offset + 1] = c->g;
	s->buffer[offset + 2] = c->b;
	s->buffer[offset + 3] = c->a; // You should be able to write RGBA in one pass by combining all 4 bytes and addressing the buffer as a 32-bit integer
}

// Write a pixel of a specified color to a specified screenbuffer at a specified X/Y coordinate
void setPixelXY(Screenbuffer *s, int x, int y, Color *c) {
	int offset = (y * s->byteW) + (x << 2); // There's a better bitwise hack for this
	s->buffer[offset] = c->r;
	s->buffer[offset + 1] = c->g;
	s->buffer[offset + 2] = c->b;
	s->buffer[offset + 3] = c->a; // You should be able to write RGBA in one pass by combining all 4 bytes and addressing the buffer as a 32-bit integer
}

// Draw a line using Bresenham's algorithm in a specified color to a specified screenbuffer
void drawLine(Screenbuffer *s, int x1, int y1, int x2, int y2, Color *c) {
	int dx = x2 - x1;
	int dy = y2 - y1;

	int xinc;
	if (dx >= 0) {
		xinc = 4;
	} else {
		xinc = -4;
		dx = -dx;
	}

	int yinc;
	if (dy >= 0) {
		yinc = s->byteW;
	} else {
		yinc = -s->byteW;
		dy = -dy;
	}

	int error = 0;
	int offset = (y1 * s->byteW) + (x1 << 2); // There's a better bitwise hack for this

	if (dx > dy) {
		for (int i = 0; i <=dx; i += 1) {
			setPixel(s, offset, c);
			error += dy;

			if (error > dx) {
				error -= dx;
				offset += yinc;
			}

			offset += xinc;
		}
	} else {
		for (int i = 0; i <= dy; i += 1) {
			setPixel(s, offset, c);
			error += dx;

			if (error > 0) {
				error -= dy;
				offset += xinc;
			}

			offset += yinc;
		}
	}
}

// Clip a line using the globally defined clipping region
int clipLine(int *x1, int *y1, int *x2, int *y2) {
	int point1 = 0;
	int point2 = 0;

	// Test if line is completely visible
	if (*x1 >= POLY_CLIP_MIN_X && *x1 <= POLY_CLIP_MAX_X && *y1 >= POLY_CLIP_MIN_Y && *y1 <= POLY_CLIP_MAX_Y) {
		point1 = 1;
	}

	if (*x2 >= POLY_CLIP_MIN_X && *x2 <= POLY_CLIP_MAX_X && *y2 >= POLY_CLIP_MIN_Y && *y2 <= POLY_CLIP_MAX_Y) {
		point2 = 1;
	}

	if (point1 == 1 && point2 == 1) {
		return 1;
	}

	// int clipAlways = 0;

	// Test if line is completely invisible
	if (point1 == 0 && point2 == 0 ) {
		if ((*x1 < POLY_CLIP_MIN_X && *x2 < POLY_CLIP_MIN_X) || (*x1 > POLY_CLIP_MAX_X && *x2 > POLY_CLIP_MAX_X) || 
			(*y1 < POLY_CLIP_MIN_Y && *y2 < POLY_CLIP_MIN_Y) || (*y1 > POLY_CLIP_MAX_Y && *y2 > POLY_CLIP_MAX_Y)) {
			return 0;
		}	

		// clipAlways = 1;
	}

	float dx;
	float dy;
	int rightEdge = 0;
	int leftEdge = 0;
	int bottomEdge = 0;
	int topEdge = 0;
	int xintersect;
	int yintersect;
	int success = 0;

	// Handle case where either endpoint is in clipping region
	if (point1 == 1 || (point1 == 0 && point2 == 0)) {
		dx = *x2 - *x1;
		dy = *y2 - *y1;

		// Compute what boundary lines need to be clipped against
		if (*x2 > POLY_CLIP_MAX_X) {
			rightEdge = 1;

			// Compute intersection with right edge
			if (dx != 0) {
				yintersect = 0.5f + (dy / dx) * (POLY_CLIP_MAX_X - *x1) + *y1;
			} else {
				yintersect = -1;
			}
		} else if (*x2 < POLY_CLIP_MIN_X) {
			leftEdge = 1;

			// Compute intersection with left edge
			if (dx != 0) {
				yintersect = 0.5f + (dy / dx) * (POLY_CLIP_MIN_X - *x1) + *y1;
			} else {
				yintersect = -1;
			}
		}

		if (*y2 > POLY_CLIP_MAX_Y) {
			bottomEdge = 1;

			// Compute intersection with bottom edge
			if (dy != 0) {
				xintersect = 0.5f + (dx / dy) * (POLY_CLIP_MAX_Y - *y1) + *x1;
			} else {
				xintersect = -1;
			}
		} else if (*y2 < POLY_CLIP_MIN_Y) {
			topEdge = 1;

			// Compute intersection with top edge
			if (dy != 0) {
				xintersect = 0.5f + (dx / dy) * (POLY_CLIP_MIN_Y - *y1) + *x1;
			} else {
				xintersect = -1;
			}
		}

		// Now we know where the line passed through
		// Compute which edge is the proper intersection
		if (rightEdge == 1 && (yintersect >= POLY_CLIP_MIN_Y && yintersect <= POLY_CLIP_MAX_Y)) {
			*x2 = POLY_CLIP_MAX_X;
			*y2 = yintersect;

			success = 1;
		} else if (leftEdge == 1 && (yintersect >= POLY_CLIP_MIN_Y && yintersect <= POLY_CLIP_MAX_Y)) {
			*x2 = POLY_CLIP_MIN_X;
			*y2 = yintersect;

			success = 1;
		} 

		if (bottomEdge == 1 && (xintersect >= POLY_CLIP_MIN_X && xintersect <= POLY_CLIP_MAX_X)) {
			*x2 = xintersect;
			*y2 = POLY_CLIP_MAX_Y;

			success = 1;
		} else if (topEdge == 1 && (xintersect >= POLY_CLIP_MIN_X && xintersect <= POLY_CLIP_MAX_X)) {
			*x2 = xintersect;
			*y2 = POLY_CLIP_MIN_Y;

			success = 1;
		}
	}

	rightEdge = 0;
	leftEdge = 0;
	topEdge = 0;
	bottomEdge = 0;

	// Test second endpoint
	if (point2 == 1 || (point1 == 0 && point2 == 0)) {
		// Compute deltas
		dx = *x1 - *x2;
		dy = *y1 - *y2;

		// Compute what boundary line needs to be clipped against
		if (*x1 > POLY_CLIP_MAX_X) {
			rightEdge = 1;

			// Compute intersection with right edge
			if (dx != 0) {
				yintersect = 05.f + (dy / dx) * (POLY_CLIP_MAX_X - *x2) + *y2;
			} else {
				yintersect = -1;
			}
		} else if (*x1 < POLY_CLIP_MIN_X) {
			leftEdge = 1;

			// Compute intersection with left edge
			if (dx != 0) {
				yintersect = 0.5f + (dy / dx) * (POLY_CLIP_MIN_X - *x2) + *y2;
			} else {
				yintersect = -1;
			}
		}

		if (*y1 > POLY_CLIP_MAX_Y) {
			bottomEdge = 1;

			// Compute intersection with bottom edge
			if (dy != 0) {
				xintersect = 0.5f + (dx / dy) * (POLY_CLIP_MAX_Y - *y2) + *x2;
			} else {
				xintersect = -1;
			}
		} else if (*y1 < POLY_CLIP_MIN_Y) {
			topEdge = 1;

			// Compute intersection with top edge
			if (dy != 0) {
				xintersect = 0.5f + (dx / dy) * (POLY_CLIP_MIN_Y - *y2) + *x2;
			} else {
				xintersect = -1;
			}
		}

		// Now we know where the line passed through
		// Compute which edge is the proper intersection
		if (rightEdge == 1 && (yintersect >= POLY_CLIP_MIN_Y && yintersect <= POLY_CLIP_MAX_Y)) {
			*x1 = POLY_CLIP_MAX_X;
			*y1 = yintersect;

			success = 1;
		} else if (leftEdge == 1 && (yintersect >= POLY_CLIP_MIN_Y && yintersect <= POLY_CLIP_MAX_Y)) {
			*x1 = POLY_CLIP_MIN_X;
			*y1 = yintersect;

			success = 1;
		}

		if (bottomEdge == 1 && (xintersect >= POLY_CLIP_MIN_X && xintersect <= POLY_CLIP_MAX_X)) {
			*x1 = xintersect;
			*y1 = POLY_CLIP_MAX_Y;

			success = 1;
		} else if (topEdge == 1 && (xintersect >= POLY_CLIP_MIN_X && xintersect <= POLY_CLIP_MAX_X)) {
			*x1 = xintersect;
			*y1 = POLY_CLIP_MIN_Y;

			success = 1;
		}
	}

	return success;
}

// Draw an object wireframe
void drawObjectWire(Screenbuffer *s, Object *o) {
	int ix1, iy1, ix2, iy2;
	float x1c, y1c, x2c, y2c;

	// Loop through each poly in the object
	for (int i = 0; i < o->numPolys; i += 1) {
		// Visibility check
		if (o->polys[i].visible && !o->polys[i].clipped) {
			// Loop through each vertex in the poly
			for (int j = 0; j < o->polys[i].numPoints - 1; j += 1) {
				// Extract two endpoints for a line
				int vertex = o->polys[i].vertexList[j];
				float x1 = o->verticesCamera[vertex].x;
				float y1 = o->verticesCamera[vertex].y;
				float z1 = o->verticesCamera[vertex].z;

				vertex = o->polys[i].vertexList[j + 1];
				float x2 = o->verticesCamera[vertex].x;
				float y2 = o->verticesCamera[vertex].y;
				float z2 = o->verticesCamera[vertex].z;

				// Convert to screen coords
				x1c = HALF_SCREEN_WIDTH + x1 * VIEWING_DISTANCE / z1;
				y1c = HALF_SCREEN_HEIGHT - ASPECT_RATIO * y1 * VIEWING_DISTANCE / z1;

				x2c = HALF_SCREEN_WIDTH + x2 * VIEWING_DISTANCE / z2;
				y2c = HALF_SCREEN_HEIGHT - ASPECT_RATIO * y2 * VIEWING_DISTANCE / z2;

				// Cast for line clipper
				ix1 = (int)x1c;
				iy1 = (int)y1c;
				ix2 = (int)x2c;
				iy2 = (int)y2c;

				// Draw clipped lines!
				if (clipLine(&ix1, &iy1, &ix2, &iy2)) {
					drawLine(s, ix1, iy1, ix2, iy2, &o->polys[i].color);
				}
			}
		}
		
		// Now we close the poly
		ix1 = (int)x2c;
		iy1 = (int)y2c;

		// Grab the starting point again
		int vertex = o->polys[i].vertexList[0];

		float x2 = o->verticesCamera[vertex].x;
		float y2 = o->verticesCamera[vertex].y;
		float z2 = o->verticesCamera[vertex].z;

		// Convert to screen coords
		x2c = HALF_SCREEN_WIDTH + x2 * VIEWING_DISTANCE / z2;
		y2c = HALF_SCREEN_HEIGHT - ASPECT_RATIO * y2 * VIEWING_DISTANCE / z2;

		// Cast for line clipper
		ix2 = (int)x2c;
		iy2 = (int)y2c;

		// Draw clipped lines!
		if (clipLine(&ix1, &iy1, &ix2, &iy2)) {
			drawLine(s, ix1, iy1, ix2, iy2, &o->polys[i].color);
		}
	}
}


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
float getObjectRadius(Object *o) {
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

// Translate an object 
void translateObject(Object *o, float x, float y, float z) {
	o->worldPos.x += x;
	o->worldPos.y += y;
	o->worldPos.z += z;
}

// Position an object
void positionObject(Object *o, float x, float y, float z) {
	o->worldPos.x = x;
	o->worldPos.y = y;
	o->worldPos.z = z;
}

// Rotate an object
void rotateObject(Object *o, float x, float y, float z) {
	if (x == 0 && y == 0 & z == 0) {
		return;
	}

	Matrix4x4 rotx;
	Matrix4x4 roty;
	Matrix4x4 rotz;

	if (x) {
		make4x4Ident(rotx);
		rotx[1][1] = cos(x);
		rotx[1][2] = sin(x);
		rotx[2][1] = -sin(x);
		rotx[2][2] = cos(x);
	}

	if (y) {
		make4x4Ident(roty);
		roty[0][0] = cos(y);
		roty[0][2] = -sin(y);
		roty[2][0] = sin(y);
		roty[2][2] = cos(y);
	}

	if (z) {
		make4x4Ident(rotz);
		rotz[0][0] = cos(z);
		rotz[0][1] = sin(z);
		rotz[1][0] = -sin(z);
		rotz[1][1] = cos(z);
	}

	// We can make this more efficient by only concatenating the matrices that
	// will have an effect - see p. 690
	Matrix4x4 temp;
	Matrix4x4 finalRotation;
	make4x4Ident(finalRotation);

	mult4x4(rotx, roty, temp);
	mult4x4(temp, rotz, finalRotation);

	float xval, yval, zval;

	for (int i = 0; i < o->numVertices; i += 1) {
		xval = o->verticesLocal[i].x * finalRotation[0][0] + o->verticesLocal[i].y * finalRotation[1][0] + o->verticesLocal[i].z * finalRotation[2][0];
		yval = o->verticesLocal[i].x * finalRotation[0][1] + o->verticesLocal[i].y * finalRotation[1][1] + o->verticesLocal[i].z * finalRotation[2][1];
		zval = o->verticesLocal[i].x * finalRotation[0][2] + o->verticesLocal[i].y * finalRotation[1][2] + o->verticesLocal[i].z * finalRotation[2][2];

		o->verticesLocal[i].x = xval;
		o->verticesLocal[i].y = yval;
		o->verticesLocal[i].z = zval;
	}
}