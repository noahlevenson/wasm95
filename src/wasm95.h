#pragma once

typedef struct {
	int w;
	int h;
	unsigned char *buffer;
} Framebuffer;

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
} Pixel;

void putPixel(Framebuffer *f, int x, int y, Pixel *p);
EMSCRIPTEN_KEEPALIVE Framebuffer* makeFramebuffer(unsigned char buffer[], int w, int h);
EMSCRIPTEN_KEEPALIVE void delFramebuffer(Framebuffer *f);
EMSCRIPTEN_KEEPALIVE void run(Framebuffer *f);