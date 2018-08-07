let screenPtr;

const screen = document.getElementById("screen")

screen.width = 640;
screen.height = 480;

const screenCtx = screen.getContext("2d");

function start() {
	screenPtr = Module._malloc(640 * 480 * 4);
	const screenFramebufferPtr = Module.ccall("makeFramebuffer", "number", ["number", "number", "number"], [screenPtr, 640, 480]);
	requestAnimationFrame(update);
	Module.ccall("run", null, ["number"], [screenFramebufferPtr]);
}

function update() {
	const imgData = new ImageData(640, 480);
	
	for (let i = 0, len = imgData.data.length; i < len; i += 1) {
		imgData.data[i] = Module.HEAPU8[screenPtr + i];
	}

	screenCtx.putImageData(imgData, 0, 0);
	requestAnimationFrame(update);
}