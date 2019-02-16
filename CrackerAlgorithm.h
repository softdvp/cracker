// @ 2019 Oleg Popov 
#pragma once
#include <vector>

template <typename CellElement>
class Matrix {
	std::vector<std::vector<CellElement>> mx;

public:
	Matrix() {}

	Matrix(size_t size) : mx(size, std::vector<CellElement>(size)){

	}

	size_t size() const {
		return mx.size();
	}

	std::vector<CellElement>& operator[](const size_t row) {
		return mx[row];
	}

	std::vector<CellElement> operator[](const size_t row) const{
		return mx[row];
	}
	

	void reset() {
		for (auto& e : mx)
			e.assign(size(), 0);
	}

	void resize(size_t size) {
		std::vector<CellElement> v(size);
		mx.resize(size, v);
	}
};

template<typename Ty>
class Cracker {
	Matrix<Ty> PropagationTrees;
	std::list<std::vector<size_t>> Components;
	std::vector<size_t> Seeds;
	size_t ActiveNum;
	std::vector<bool> ActiveVertices;

	void ProcessGraph(Matrix<Ty> &tempGraph);
	void PropogateTrees();
	void ProcessTreeNode(const size_t node, std::vector<size_t> &Nodevector);
	size_t neihbourCount(const size_t n, const std::vector<Ty> &neihbours) const;
	size_t incomingCount(const size_t n, Matrix<Ty> &mx) const;
	void ConvertVertexvectorToMatrix(const std::vector<size_t> &vertices, const Matrix<Ty> &input, Matrix<Ty> &output);

public:
	std::vector<Matrix<Ty>> GetAllComponents(const Matrix<Ty> &input, const size_t Count);
};

template<typename Ty>
void Cracker<Ty>::ProcessGraph(Matrix<Ty> &tempGraph) {

	//Min Selection
	Matrix<Ty> directH(tempGraph.size());

	for (size_t i = 0; i != tempGraph.size(); ++i) {
		if (ActiveVertices[i]) {
			// An active vertex
			std::vector<Ty> &neihbours = tempGraph[i];

			for (size_t j = 0; j != neihbours.size(); ++j) {
				if ((ActiveVertices[j] && neihbours[j]) || (j == i)) {
					size_t minVertex = j;
					directH[i][minVertex] = true;

					for (size_t k = 0; k != neihbours.size(); ++k)
						if (ActiveVertices[k] && neihbours[k])
							//Add edge
							directH[k][minVertex] = true;
					break;
				}
			}
		}
	}

	//Pruning 
	tempGraph.reset();

	//Min selection
	for (size_t i = 0; i != directH.size(); ++i) {
		size_t minVertex = 0;
		if (ActiveVertices[i]) {
			// An active vertex
			std::vector<Ty> &neihbours = directH[i];
			size_t nbCount = neihbourCount(i, neihbours);

			for (size_t j = 0; j != neihbours.size(); ++j) {
				if (ActiveVertices[j] && neihbours[j] && (j != i)) {
					minVertex = j;

					if (nbCount > 1) {
						//Add an edge
						for (size_t k = j + 1; k != neihbours.size(); ++k)
							if (ActiveVertices[k] && neihbours[k] && (k != minVertex)) {

								//Undirect graph, symmetrize
								tempGraph[k][minVertex] = true;
								tempGraph[minVertex][k] = true;
							}
					}
					break;
				}
			}

			size_t cv = incomingCount(i, directH);

			if (!directH[i][i]) {
				//Not connected to itself. Disable the vertex
				ActiveVertices[i] = false;
				--ActiveNum;
				PropagationTrees[minVertex][i] = true;
			}
			else
				if (cv == 1 && nbCount == 1) {

					//IsSeed: Connected to itself only, it has not any connected (upcoming and outcoming) vertices 
					Seeds.push_back(i);
					//Disable the vertex
					ActiveVertices[i] = false;
					--ActiveNum;
				}
		}
	}
}

template<typename Ty>
void Cracker<Ty>::PropogateTrees() {
	size_t sz = Seeds.size();
	Components.resize(sz);

	std::vector<size_t>::iterator seedit = Seeds.begin();
	size_t n = 0;

	for (auto &l : Components) {
		size_t seed = *seedit;
		ProcessTreeNode(seed, l);
		++seedit;
		++n;
	}

	Components.remove_if([](auto &el) {return el.size() == 1; });
};

template<typename Ty>
void Cracker<Ty>::ProcessTreeNode(const size_t node, std::vector<size_t> &Nodevector) {
	Nodevector.push_back(node);
	std::vector<Ty> &children = PropagationTrees[node];

	for (size_t i = 0; i != children.size(); ++i)
		if (children[i])
			ProcessTreeNode(i, Nodevector);
}

template<typename Ty>
size_t Cracker<Ty>::neihbourCount(const size_t n, const std::vector<Ty> &neihbours) const {
	size_t count = 0;
	for (size_t i = 0; i != neihbours.size(); ++i)
		if (ActiveVertices[i] && neihbours[i]) {
			count++;

			// We compare with 1 only
			if (count == 2) return 2;
		}

	return count;
}

template<typename Ty>
size_t Cracker<Ty>::incomingCount(const size_t n, Matrix<Ty> &mx) const {
	size_t count = 0;

	for (size_t i = 0; i != mx.size(); i++)
		if (ActiveVertices[i] && mx[i][n]) {
			++count;
			// We compare with 1 only
			if (count == 2) return 2;
		}

	return count;
}

template<typename Ty>
void Cracker<Ty>::ConvertVertexvectorToMatrix(const std::vector<size_t> &vertices, const Matrix<Ty> &input, Matrix<Ty> &output)
{
	std::vector<bool>NodesExist(input.size());

	for (size_t node : vertices)
		NodesExist[node] = true;

	for (size_t i = 0; i < input.size(); ++i)
		if (NodesExist[i])
			for (size_t j = i + 1; j < input.size(); ++j)
				if (NodesExist[j]) {
					output[i][j] = input[i][j];
					output[j][i] = input[j][i];
				}
}

template<typename Ty>
std::vector<Matrix<Ty>> Cracker<Ty>::GetAllComponents(const Matrix<Ty> &input, const size_t Count) {
	ActiveVertices.assign(input.size(), true);

	ActiveNum = input.size();
	PropagationTrees.resize(input.size());

	Matrix<Ty> tempG = input;

	do {
		ProcessGraph(tempG);
	} while (ActiveNum > 0);

	PropogateTrees();
	Components.sort([](const auto &x, const auto &y) {return x.size() > y.size(); });

	//if Count = 0 - get all components, 1 - largest
	size_t sz, compsz = Components.size();
	sz = Count ? (Count < compsz ? Count : compsz) : compsz;
	Components.resize(sz);

	std::vector<Matrix<Ty>> matrices(Components.size(), Matrix<Ty>(input.size()));

	size_t k = 0;

	for (auto &lst : Components) {
		ConvertVertexvectorToMatrix(lst, input, matrices[k]);
		++k;
	}

	return matrices;
}

// if Count=0 - get all components, 1 - largest
template<typename Ty>
std::vector<Matrix<Ty>> all_connected_components(const Matrix<Ty> &input, const size_t Count) {
	Cracker<Ty> CA;

	return CA.GetAllComponents(input, Count);
}

template<typename Ty>
void MatrixTest(Matrix<Ty> M) {
	for (size_t i = 0; i < M.size(); ++i) {
		for (size_t j = 0; j < M.size(); ++j)
			if (M[i][j])
				std::cout << i << "," << j << " ";
			else
				std::cout << "    ";

		std::cout << std::endl;
	}

	std::cout << std::endl << "===========================================" << std::endl;
}