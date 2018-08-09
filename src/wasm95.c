// source ./emsdk_env.sh --build=Release
// emcc wasm95.c math.c gfx.c -s TOTAL_MEMORY=1024MB -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall', 'cwrap', 'allocate']" -s WASM=1 -O3 -o ../demo/wasm95.js

#include <stdlib.h>
#include <stdio.h>
#include <emscripten/emscripten.h>
#include "wasm95.h"
#include "math.h"
#include "gfx.h"

EMSCRIPTEN_KEEPALIVE void run(Screenbuffer *s) {
	Color blue = {0, 0, 255, 255};
	drawLine(s, 20, 20, 400, 50, &blue);
	drawLine(s, 400, 50, 200, 100, &blue);
	drawLine(s, 200, 100, 20, 20, &blue);
}

int main(int argc, char* argv[]) {
	printf("Made with wasm95\n");
	return 0;
}