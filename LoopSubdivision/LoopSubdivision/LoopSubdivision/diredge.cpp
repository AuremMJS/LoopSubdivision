// Include Statements
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include "diredge.h"
// Include Statements

// Namespaces
using namespace std;
using namespace diredge;


// Function to create directed edge data structure from raw vertices
diredgeMesh diredge::createMesh(std::vector<Cartesian3> raw_vertices)
{
	// mesh data structure to return
	diredgeMesh mesh;

	// Resize the vectors of face indices and normals
	mesh.faceVertices.resize(raw_vertices.size(), -1);
	mesh.normal.resize(raw_vertices.size() / 3, Cartesian3(0.0, 0.0, 0.0));

	// Generate the face indices
	makeFaceIndices(raw_vertices, mesh);

	// Resize the vectors of other halfs and first directed edges
	mesh.otherHalf.resize(mesh.faceVertices.size(), NO_SUCH_ELEMENT);

	// Determine the other halfs and first directed edges and normals
	mesh.firstDirectedEdge.resize(mesh.position.size(), NO_SUCH_ELEMENT);
	makeDirectedEdges(mesh);

	// Return the data structure
	return mesh;
}

// Function to create the face indices
void diredge::makeFaceIndices(std::vector<Cartesian3> raw_vertices, diredgeMesh &mesh)
{
	// set the initial vertex ID
	long nextVertexID = 0;

	// loop through the vertices
	for (unsigned long vertex = 0; vertex < raw_vertices.size(); vertex++)
	{ // vertex loop
		// first see if the vertex already exists
		for (unsigned long other = 0; other < vertex; other++)
		{ // per other
			if (raw_vertices[vertex] == raw_vertices[other])
				mesh.faceVertices[vertex] = mesh.faceVertices[other];
		} // per other

		// if not set, set to next available
		if (mesh.faceVertices[vertex] == -1)
			mesh.faceVertices[vertex] = nextVertexID++;
	} // vertex loop

	// id of next vertex to write
	long writeID = 0;

	for (long vertex = 0; vertex < raw_vertices.size(); vertex++)
	{
		// if it's the first time found
		if (writeID == mesh.faceVertices[vertex])
		{
			// Push the vertex to the position vector
			mesh.position.push_back(raw_vertices[vertex]);
			writeID++;
		}
	}
}

// Function to determine the other halfs and first directed edges
void diredge::makeDirectedEdges(diredgeMesh &mesh)
{
	// we will also want a temporary variable for the degree of each vertex
	std::vector<long> vertexDegree(mesh.position.size(), 0);

	// 2.	now loop through the directed edges
	for (long dirEdge = 0; dirEdge < (long)mesh.faceVertices.size(); dirEdge++)
	{ // for each directed edge
		// a. retrieve to and from vertices
		long from = mesh.faceVertices[dirEdge];
		long to = mesh.faceVertices[NEXT_EDGE(dirEdge)];

		// aa. Error check for duplicated vertices on faces
		if (from == to)
		{ // error: duplicate vertex on face
			printf("Error: Directed Edge %ld has matching ends %ld %ld\n", dirEdge, from, to);
			exit(0);
		} // error: duplicate vertex on face

		// b. if from vertex has no "first", set it
		if (mesh.firstDirectedEdge[from] == NO_SUCH_ELEMENT)
			mesh.firstDirectedEdge[from] = dirEdge;

		// increment the vertex degree
		vertexDegree[from]++;

		// c. if the other half is already set, we can skip this edge
		if (mesh.otherHalf[dirEdge] != NO_SUCH_ELEMENT)
			continue;

		// d. set a counter for how many matching edges
		long nMatches = 0;

		// e. now search all directed edges on higher index faces
		long face = dirEdge / 3;
		for (long otherEdge = 3 * (face + 1); otherEdge < (long)mesh.faceVertices.size(); otherEdge++)
		{ // for each higher face edge
			// i. retrieve other's to and from
			long otherFrom = mesh.faceVertices[otherEdge];
			long otherTo = mesh.faceVertices[NEXT_EDGE(otherEdge)];

			// ii. test for match
			if ((from == otherTo) && (to == otherFrom))
			{ // match
				// if it's not the first match, we have a non-manifold edge
				if (nMatches > 0)
				{ // non-manifold edge
					printf("Error: Directed Edge %ld matched more than one other edge (%ld, %ld)\n", dirEdge, mesh.otherHalf[dirEdge], otherEdge);
					exit(0);
				} // non-manifold edge

				// otherwise we set the two edges to point to each other
				mesh.otherHalf[dirEdge] = otherEdge;
				mesh.otherHalf[otherEdge] = dirEdge;

				// increment the counter
				nMatches++;
			} // match

		} // for each higher face edge

		// f. if it falls through here with no matches, we know it is non-manifold
		if (nMatches == 0)
		{ // non-manifold edge
			printf("Error: Directed Edge %ld (%ld, %ld) matched no other edge\n", dirEdge, from, to);
			exit(0);
		} // non-manifold edge

	} // for each other directed edge

	// 3.	now we assume that the data structure is correctly set, and test whether all neighbours are on a single cycle
	for (long vertex = 0; vertex < (long)mesh.position.size(); vertex++)
	{ // for each vertex
		// start a counter for cycle length
		long cycleLength = 0;

		// loop control is the neighbouring edge
		long outEdge = mesh.firstDirectedEdge[vertex];

		// could happen in a malformed input
		if (outEdge == NO_SUCH_ELEMENT)
		{ // no first edge
			printf("Error: Vertex %ld had not incident edges\n", vertex);
		} // no first edge

		// do loop to iterate correctly
		do
		{ // do loop
			// while we are at it, we can set the normal
			long face = outEdge / 3;
			Cartesian3 *v0 = &(mesh.position[mesh.faceVertices[3 * face]]);
			Cartesian3 *v1 = &(mesh.position[mesh.faceVertices[3 * face + 1]]);
			Cartesian3 *v2 = &(mesh.position[mesh.faceVertices[3 * face + 2]]);
			// now compute the normal vector
			Cartesian3 uVec = *v2 - *v0;
			Cartesian3 vVec = *v1 - *v0;
			Cartesian3 faceNormal = uVec.cross(vVec);
			mesh.normal[vertex] = mesh.normal[vertex] + faceNormal;

			// flip to the other half
			long edgeFlip = mesh.otherHalf[outEdge];
			// take the next edge on its face
			outEdge = NEXT_EDGE(edgeFlip);
			// increment the cycle length
			cycleLength++;

		} // do loop
		while (outEdge != mesh.firstDirectedEdge[vertex]);

		// now check the length against the vertex degree
		if (cycleLength != vertexDegree[vertex])
		{ // wrong cycle length
			printf("Error: vertex %ld has edge cycle of length %ld but degree of %ld\n", vertex, cycleLength, vertexDegree[vertex]);
			exit(0);
		} // wrong cycle length

		// normalise the vertex normal
		mesh.normal[vertex] = mesh.normal[vertex].normalise();
	} // for each vertex

}

// Function to sub divide a face into 4 triangles
void diredge::SubDivideFaces(diredgeMesh &mesh)
{
	// Vector of new face indices
	vector<long> newFaceVertices;
	
	// Vector of new positions
	vector<Cartesian3> newPositions;

	int newIndexVal = 0;
	// Loop through faces
	for (int i = 0; i < mesh.faceVertices.size() / 3; i++)
	{
		// Vector of newly created edge points
		vector<int> newlyCreatedVertices;

		// Loop through the vertices in the face
		for (int j = 0; j < 3; j++)
		{
			// Fetch the current and other two adjacent vertices in this face
			int index0 = (i * 3) + (j % 3);
			int index1 = (i * 3) + (j + 1) % 3;
			int index2 = (i * 3) + (j + 2) % 3;

			// Fetch the other two neighbouring vertices (v3 for edge v0v1 and edge v0v2) in adjacent face to the current vertex
			int index3 = (mesh.otherHalf[index0] / 3) * 3 + ((mesh.otherHalf[index0] + 2) % 3); // v3 for edge v0v1
			int index4 = (mesh.otherHalf[index2] / 3) * 3 + ((mesh.otherHalf[index2] + 2) % 3); // v3 for edge v0v2

			// Array of vertices for new face
			Cartesian3 vertices[3];

			// Positions of old vertices
			Cartesian3 oldvertex0 = mesh.position[mesh.faceVertices[index0]];
			Cartesian3 oldvertex1 = mesh.position[mesh.faceVertices[index1]];
			Cartesian3 oldvertex2 = mesh.position[mesh.faceVertices[index2]];

			// Calculate the new position for vertex 0
			vertices[0] = UpdateVertexPositions(mesh, mesh.faceVertices[index0]);

			// Generate new vertex in between old vertex 0 and old vertex 1. Note: v4 = old vertex 2
			vertices[1] = (oldvertex0 + oldvertex1) * (3 / 8.0) + (mesh.position[mesh.faceVertices[index3]] + oldvertex2) * (1 / 8.0);

			// Generate new vertex in between old vertex 0 and old vertex 2. Note: v4 = old vertex 1
			vertices[2] = (oldvertex0 + oldvertex2) * (3 / 8.0) + (mesh.position[mesh.faceVertices[index4]] + oldvertex1)* (1 / 8.0);

			// Check whether the vertices of the new face exists already and add if not
			for (int j = 0; j < 3; j++)
			{
				int index = -1;
				for (int i = 0; i < newPositions.size(); i++)
				{
					if (vertices[j] == newPositions[i])
					{
						index = i;
						break;
					}
				}
				if (index == -1)
				{
					newPositions.push_back(vertices[j]);
					index = newIndexVal;
					newIndexVal++;
				}

				if (j != 0)
					newlyCreatedVertices.push_back(index);
				newFaceVertices.push_back(index);
			}
		}

		// Generate the face made by three new edge points
		for (int i = 2; i >= 0; i--)
		{
			newFaceVertices.push_back(newlyCreatedVertices.at(i));
		}
	}

	// Update the face indices
	mesh.faceVertices = newFaceVertices;

	// Update the new positions
	mesh.position = newPositions;

	// Clear first directed edges and other halfs
	mesh.firstDirectedEdge.clear();
	mesh.otherHalf.clear();

	// Resize first directed edges and other halfs
	mesh.otherHalf.resize(mesh.faceVertices.size(), NO_SUCH_ELEMENT);
	mesh.firstDirectedEdge.resize(mesh.position.size(), NO_SUCH_ELEMENT);

	// Clear the normals and resize
	mesh.normal.clear();
	mesh.normal.resize(newPositions.size(), Cartesian3(0.0, 0.0, 0.0));

	// Make the first directed edges, normals and other halfs again
	diredge::makeDirectedEdges(mesh);
}

// Function to update an old vertex position
Cartesian3 diredge::UpdateVertexPositions(diredgeMesh &mesh, int index)
{
	// Beta
	double beta = 0;

	// New position calculated
	Cartesian3 newPosition;
	// Valence of the vertex
	double n = 0;

	// Sum of adjacent vertices
	Cartesian3 sumVertex;

	// Loop through incident vertices to calculate degree and sum of positions
	long outEdge = mesh.firstDirectedEdge[index];

	// could happen in a malformed input
	if (outEdge == NO_SUCH_ELEMENT)
	{ // no first edge
		printf("Error: Vertex %ld had not incident edges\n", index);
	} // no first edge

	do
	{ // do loop

		// Find the incident vertex
		long nextEdge = NEXT_EDGE(outEdge);
		Cartesian3 *v1 = &(mesh.position[mesh.faceVertices[nextEdge]]);

		// Add the incident vertex to the sum
		sumVertex = sumVertex + *v1;

		// flip to the other half
		long edgeFlip = mesh.otherHalf[outEdge];
		// take the next edge on its face
		outEdge = NEXT_EDGE(edgeFlip);
		// increment the cycle length
		n++;

	} // do loop
	while (outEdge != mesh.firstDirectedEdge[index]);

	// Calculate Beta
	beta = (1 / n) * ((5 / 8.0) - (powf((3 + 2 * cos((2 * 3.14) / n)), 2) / 64.0));

	// Calculate new position
	newPosition = mesh.position.at(index) * (float)(1 - n * beta) + sumVertex * beta;

	// Update the new position
	return newPosition;

}

// Function to make triangle soup from data structure
std::vector<Cartesian3> diredge::makeSoup(diredgeMesh mesh)
{
	// Soup to return
	vector<Cartesian3> soup;

	// Loop through the face
	for (long face = 0; face < (long)mesh.faceVertices.size() / 3; face++)
	{
		for (int i = 0; i < 3; i++)
		{
			// Fetch the vertex and add to soup
			Cartesian3 position = mesh.position[mesh.faceVertices[3 * face + i]];
			soup.push_back(position);
		}
	}

	// Return the soup
	return soup;
}
