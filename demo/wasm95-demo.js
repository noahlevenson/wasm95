let screenPtr, screenbufferPtr;
let framebuffer = new ImageData(640, 480);

const screen = document.getElementById("screen")

screen.width = 640;
screen.height = 480;

const screenCtx = screen.getContext("2d");

function start() {
	screenPtr = Module._malloc(640 * 480 * 4);
	screenbufferPtr = Module.ccall("makeScreenbuffer", "number", ["number", "number", "number"], [screenPtr, 640, 480]);
	requestAnimationFrame(jsupdate);
}

function jsupdate() {
	Module.ccall("update", null, ["number"], [screenbufferPtr]);

	for (let i = 0, len = framebuffer.data.length; i < len; i += 1) {
		framebuffer.data[i] = Module.HEAPU8[screenPtr + i];
	}

	screenCtx.putImageData(framebuffer, 0, 0);

	requestAnimationFrame(jsupdate);
}

