#pragma once

#include <emscripten/emscripten.h>

// Structure for a screenbuffer
typedef struct Screenbuffer {
	int w;
	int h;
	int byteW;
	unsigned char *buffer;
} Screenbuffer;

// Structure for an HTML5 ImageData color
typedef struct Color {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} Color;

void setPixelXY(Screenbuffer *s, int x, int y, Color *c);
void setPixel(Screenbuffer *s, int offset, Color *c);
EMSCRIPTEN_KEEPALIVE Screenbuffer* makeScreenbuffer(unsigned char buffer[], int w, int h);
EMSCRIPTEN_KEEPALIVE void killScreenbuffer(Screenbuffer *s);
void drawLine(Screenbuffer *s, int x1, int y1, int x2, int y2, Color *c);