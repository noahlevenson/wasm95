// emcc wasm95.c -s TOTAL_MEMORY=1024MB -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap', 'allocate']" -s WASM=1 -O3 -o ../demo/wasm95.js

#include <stdlib.h>
#include <stdio.h>
#include <emscripten/emscripten.h>
#include "wasm95.h"

EMSCRIPTEN_KEEPALIVE Framebuffer* makeFramebuffer(unsigned char buffer[], int w, int h) {
	Framebuffer *f = malloc(sizeof(Framebuffer));
	f->w = w;
	f->h = h;
	f->buffer = buffer;
	return f; 
}

EMSCRIPTEN_KEEPALIVE void delFramebuffer(Framebuffer *f) {
	free(f);
}

void putPixel(Framebuffer *f, int x, int y, Pixel *p) {
	int offset = (y * f->w * 4) + (x * 4);
	f->buffer[offset] = p->r;
	f->buffer[offset + 1] = p->g;
	f->buffer[offset + 2] = p->b;
	f->buffer[offset + 3] = p->a;
}

EMSCRIPTEN_KEEPALIVE void run(Framebuffer *f) {
	Pixel green = {0, 255, 0, 255};

	for (int i = 0; i < 10000; i += 1) {
		int x = rand() % 640;
		int y = rand() % 480;
		putPixel(f, x, y, &green);
	}
}

int main() {
	printf("Made with wasm95\n");
	return 0;
}