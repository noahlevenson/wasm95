// Serialize an obj file and print to stdout

const fs = require("fs");

const fileName = process.argv[2]

if (fileName) {
	fs.readFile(fileName, "utf8", (err, data) => {
		if (err) {
			console.log("ERROR: " + err);
		} else {
			const serialized = data.split("\r\n").join("\\n");
			
			fs.writeFile(fileName + "-serialized.txt", serialized, (err) => {
				if (err) {
					throw err;
				}
			});
		}
	});
} else {
	console.log("ERROR: You must specify a file!\n");
}