// @ 2019 Oleg Popov 
#include "pch.h"
#include <iostream>
#include <vector>
#include <list>
#include <string>

#include "crackeralgorithm.h"

// The graph sample was taken from 
// http://for.unipi.it/alessandro_lulli/files/2015/07/J002_FastConnectedComponentsComputationInLargeGraphsByVertexPruning.pdf

int main()
{
	std::vector<Matrix<unsigned>> res;

	Matrix<unsigned> M(9);

	std::cout << "Empty graph (with 0 vertices):" << std::endl;

	res = all_connected_components(M, 0);

	for (auto mx : res)
		MatrixTest(mx);

	M[0][1] = M[1][0] = M[1][2] = M[2][1] = M[2][4] = M[4][2] = M[4][3] = M[3][4] = M[2][5] = M[5][2] =
		M[3][6] = M[6][3] = M[7][3] = M[3][7] = M[7][8] = M[8][7] = 
		M[5][8] = M[8][5] = true;
	
	res= all_connected_components(M, 0);

	std::cout << "Full graph:" << std::endl;

	for (auto mx : res)
		MatrixTest(mx); 
			
	std::cout << "Press enter to continue...";
	std::getchar();
	std::cout << std::endl << std::endl;
	
	//We will disconnect (2, 4) and (5, 8) pairs of vertices
	//the graph will fall to two components

	M[4][2] = M[2][4] = M[5][8] = M[8][5] = false;

	res = all_connected_components(M,0);

	std::cout << "Splitted graph:" << std::endl;

	for (auto mx : res)
		MatrixTest(mx);

	std::cout << "Press enter to continue...";
	std::getchar();
	std::cout << std::endl << std::endl;

	// Disconnect (3, 7)

	M[3][7] = M[7][3] = false;

	std::cout << "3 component graph:" << std::endl;
	res = all_connected_components(M, 0);

	for (auto mx : res)
		MatrixTest(mx);

	std::cout << "Press enter to continue...";
	std::getchar();
	std::cout << std::endl << std::endl;

	//// Disconnect (7, 8)
	std::cout << "4 component graph (with 2 null graphs (with 1 vertex)):" << std::endl;
	M[7][8] = M[8][7] = false;
	res = all_connected_components(M, 0);

	for (auto mx : res)
		MatrixTest(mx);

	std::cout << "Press enter to continue...";
	std::getchar();
	std::cout << std::endl << std::endl;
}

