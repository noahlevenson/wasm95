#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gfx.h"

// Parse a serialized ASCII .obj file and return an Object
Object loadObject(char *ascii) {
	Object o;
	int ptr = 0;
	int index = 0;

	int numVertices = 0;
	int numPolys = 0;
	
	while (ascii[ptr] != '\0') {
		char c = 1;
		while (c != '\n' && c != '\0') {
			c = ascii[index];
			index += 1;
		}

		int len = index - ptr - 1;
		char *line = malloc(len + 1);

		for (int i = 0; i < len; i += 1) {
			line[i] = ascii[ptr + i];
		}

		line[len] = '\0';

		char lineHeader[64]; // Hard limit of 64 characters per line header
		sscanf(line, "%s", lineHeader);
		
		if (strcmp(lineHeader, "v") == 0) {
			Point3 p = {0, 0, 0, 1};
			sscanf(line, "%*s %f %f %f", &p.x, &p.y, &p.z);
			o.verticesLocal[numVertices] = p;
			numVertices += 1;
		} else if (strcmp(lineHeader, "f") == 0) {
			Poly p;
			int vertexList[3];
			sscanf(line, "%*s %d/%*d/%*d %d/%*d/%*d %d/%*d/%*d", &vertexList[0], &vertexList[1], &vertexList[2]);
			//sscanf(line, "%*s %d %d %d", &vertexList[0], &vertexList[1], &vertexList[2]);
			p.vertexList[0] = vertexList[0] - 1;
			p.vertexList[1] = vertexList[1] - 1;
			p.vertexList[2] = vertexList[2] - 1;
			p.numPoints = 3;
			p.visible = 1;
			p.active = 1;
			p.clipped = 0;

			Color green = {143, 201, 157, 255};
			p.color = green;

			// Still need to properly set color, shade, shading, twoSided, normalLength
			o.polys[numPolys] = p;
			numPolys += 1;
		}

		free(line);
		ptr += len + 1;
	}
	
	Point3 worldPos = {0, 0, 0};
	o.worldPos = worldPos;
	o.numVertices = numVertices;
	o.numPolys = numPolys;
	// Still need to set id, radius, state
	return o;
}