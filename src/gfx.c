#include <stdlib.h>
#include <emscripten/emscripten.h>
#include "gfx.h"

EMSCRIPTEN_KEEPALIVE Screenbuffer* makeScreenbuffer(unsigned char buffer[], int w, int h) {
	Screenbuffer *s = malloc(sizeof(Screenbuffer));
	s->w = w;
	s->h = h;
	s->byteW = w * 4;
	s->buffer = buffer;
	return s; 
}

EMSCRIPTEN_KEEPALIVE void killScreenbuffer(Screenbuffer *s) {
	free(s);
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