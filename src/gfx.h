#pragma once

#include <emscripten/emscripten.h>

// #define SCREEN_WIDTH 640
// #define SCREEN_HEIGHT 480
#define HALF_SCREEN_WIDTH 320
#define HALF_SCREEN_HEIGHT 240
#define VIEWING_DISTANCE 200
#define ASPECT_RATIO 1.33f

// Structure for a screenbuffer
typedef struct Screenbuffer {
	int w;
	int h;
	int byteW;
	int byteSize;
	unsigned char *buffer;
} Screenbuffer;

// Structure for an HTML5 ImageData color
typedef struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} Color;

#define MAX_POINTS_PER_POLY 4
#define MAX_VERTICES_PER_OBJECT 10000
#define MAX_POLYS_PER_OBJECT 30000
#define POLY_CLIP_MIN_X 0
#define POLY_CLIP_MAX_X 639
#define POLY_CLIP_MIN_Y 0
#define POLY_CLIP_MAX_Y 479

// Structures for matrices
typedef float Matrix4x4[4][4];
typedef float Matrix1x4[4];

// Structure for 3D vector or 3D point
typedef struct Vec3 {
	float x;
	float y;
	float z;
	float w;
} Vec3, Point3;

// Structure for angular orientation
typedef struct Dir3 {
	int angX;
	int angY;
	int angZ;
} Dir3;

// Structure for a polygon
typedef struct Poly {
	int numPoints;
	int vertexList[MAX_POINTS_PER_POLY];
	Color color;
	Color shade;
	int shading;
	int twoSided;
	int visible;
	int active;
	int clipped;
	float normalLength;
} Poly;

// Structure for a facet (a polygon that holds actual vertices instead of references to vertices)
typedef struct Facet {
	int numPoints;
	Color color;
	Color shade;
	int shading;
	int twoSided;
	int visible;
	int clipped;
	int active;
	Point3 vertexList[MAX_POINTS_PER_POLY];
} Facet;

// Structure for a 3D object
typedef struct Object {
	int id;
	int numVertices;
	Point3 verticesLocal[MAX_VERTICES_PER_OBJECT];
	Point3 verticesWorld[MAX_VERTICES_PER_OBJECT];
	Point3 verticesCamera[MAX_VERTICES_PER_OBJECT];
	int numPolys;
	Poly polys[MAX_POLYS_PER_OBJECT];
	float radius;
	int state;
	Point3 worldPos;
} Object;

Vec3 makeVector3(Point3 *init, Point3 *term);
float getMagVec3(Vec3 *v);
float dotProduct3(Vec3 *u, Vec3 *v);
Vec3 crossProduct3(Vec3 *u, Vec3 *v);
void make4x4Ident(Matrix4x4 a);
void make4x4Zero(Matrix4x4 a);
void copy4x4(Matrix4x4 a, Matrix4x4 b);
void print4x4(Matrix4x4 a);
void print1x4(Matrix1x4 a);
void mult4x4(Matrix4x4 a, Matrix4x4 b, Matrix4x4 result);
void mult1x4_4x4(Matrix1x4 a, Matrix4x4 b, Matrix1x4 result);
float getObjectRadius(Object *o);
void translateObject(Object *o, float x, float y, float z);
void positionObject(Object *o, float x, float y, float z);
void rotateObject(Object *o, float x, float y, float z);

void setPixelXY(Screenbuffer *s, int x, int y, Color *c);
void setPixel(Screenbuffer *s, int offset, Color *c);
EMSCRIPTEN_KEEPALIVE Screenbuffer* makeScreenbuffer(unsigned char buffer[], int w, int h);
EMSCRIPTEN_KEEPALIVE void killScreenbuffer(Screenbuffer *s);
EMSCRIPTEN_KEEPALIVE void clearScreenbuffer(Screenbuffer *s);
void drawLine(Screenbuffer *s, int x1, int y1, int x2, int y2, Color *c);
int clipLine(int *x1, int *y1, int *x2, int *y2);
void drawObjectWire(Screenbuffer *s, Object *o);