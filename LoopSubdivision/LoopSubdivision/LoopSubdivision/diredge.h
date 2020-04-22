#pragma once

#include <string>
#include <vector>
#include "Cartesian3.h"

// define a macro for "not used" flag
#define NO_SUCH_ELEMENT -1

// use macros for the "previous" and "next" IDs
#define PREVIOUS_EDGE(x) ((x) % 3) ? ((x) - 1) : ((x) + 2)
#define NEXT_EDGE(x) (((x) % 3) == 2) ? ((x) - 2) : ((x) + 1)

namespace diredge 
{
	// Structure of half edge data strucuture
    struct diredgeMesh
    {
		// Normal
        std::vector<Cartesian3> normal;

		// Position
        std::vector<Cartesian3> position;

		// Face indices
        std::vector<long> faceVertices;

		// Other halfs
        std::vector<long> otherHalf;

		// First Directed Edges
        std::vector<long> firstDirectedEdge;
    };

    // Makes a half edge mesh data structure from a triangle soup.
    diredgeMesh createMesh(std::vector<Cartesian3>);

    // Makes a triangle soup from the half edge mesh data structure.
    std::vector<Cartesian3> makeSoup(diredgeMesh);

    // Computes mesh.position and mesh.normal and mesh.faceVertices
    void makeFaceIndices(std::vector<Cartesian3> raw_vertices, diredgeMesh&);

    // Computes mesh.firstDirectedEdge and mesh.firstDirectedEdge, given mesh.position and mesh.normal and mesh.faceVertices
    void makeDirectedEdges(diredgeMesh&);

	// Updates the old vertex position
	Cartesian3 UpdateVertexPositions(diredgeMesh&, int);

	// Sub divides a face into 4 triangles
	void SubDivideFaces(diredgeMesh&);
}
