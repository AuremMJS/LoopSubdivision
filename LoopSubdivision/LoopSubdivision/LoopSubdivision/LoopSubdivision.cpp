// Include Statements
#include <vector>
#include <cmath>
#include <iostream>
#include "io.h"
#include "diredge.h"
#include "Cartesian3.h"
// Include Statements

// Namespaces
using namespace std;
using namespace diredge;


// Main function
int main(int argc, char **argv)
{ // main()
	// Input File name
	string inputFilename;

	// Output File name
	string outputFilename;

	// no of iterations
	int noIterations;

	if (argc != 4)
	{ // wrong # of arguments
		std::cout << "Usage: %s infile outfile iterations" << std::endl;
		return 0;
	} // wrong # of arguments
	else
	{
		// Set up input parameters
		inputFilename = argv[1];
		outputFilename = argv[2];
		noIterations = atoi(argv[3]);
	}
	// Read in the triangle soup to a vector for processing
	vector<Cartesian3> soup = io::readTriangleSoup(inputFilename);

	// Create mesh from the triangle soup
	diredge::diredgeMesh mesh = diredge::createMesh(soup);

	// Sub division
	for (int i = 0; i < noIterations; i++)
	{
		diredge::SubDivideFaces(mesh);
	}
	
	// Make soup from the data structure
	soup = diredge::makeSoup(mesh);

	// write the triangle soup back out
	io::writeTriangleSoup(outputFilename, soup);

	return 0;
} // main()
