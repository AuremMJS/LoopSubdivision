// Include Statements
#include "./io.h"
#include <fstream>
#include <iostream>
#include <iomanip>
// Include Statements

// Namespaces
using namespace std;
using namespace diredge;

// Function to read the triangle soup and generates a vector of raw vertices
std::vector<Cartesian3> io::readTriangleSoup(string filename)
{
	// No of vertices
    long nVertices = 0;

	// Vector of raw vertices
    std::vector<Cartesian3> raw_vertices;

	// Stream to read file
    std::ifstream fin;

	// Open file
    fin.open(filename);

	// Check whether file is open
    if (true == fin.is_open())
    {
		// Read the number of faces
        fin >> nVertices;

		// Resize the vector to store the vertices of each face
        raw_vertices.resize(3*nVertices);

		// Loop through every vertex in the file
        for (long vertex = 0; vertex < 3*nVertices; vertex++)
        {
			// Read the x,y,z values of the current vertex
            fin >> raw_vertices[vertex].x >> raw_vertices[vertex].y >> raw_vertices[vertex].z;
        }

    }
	
	// Close the file
    fin.close();

    // Array is empty if file is not read.
    return raw_vertices;
}

// Function to write a triangle soup to a file
bool io::writeTriangleSoup(string filename, std::vector<Cartesian3> soup)
{
	// Strem to write to file
    std::ofstream fout;

	// Open file
    fout.open(filename);

	// Check whether file is open
    if (true == fout.is_open())
    {
		// Write the number of faces
        fout << soup.size()/3 << std::endl;

		// Read through every vertex in the soup
        for (auto position : soup)
        {
			// Write the x,y,z values of the current vertex to the file
            fout << 	std::setw(10) << std::setprecision(5) << position.x << " " <<
						std::setw(10) << std::setprecision(5) << position.y << " " <<
						std::setw(10) << std::setprecision(5) << position.z << std::endl;
        }
    }

	// Close the file
    fout.close();

	// Return whether the file is open
    return fout.is_open();
}
