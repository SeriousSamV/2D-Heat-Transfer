/**
*	2-D Steady State Conduction without Heat Generation | main.cpp
*
*	@author Samuel0Paul <paulsamuelvishesh@live.com>
**/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <chrono>

using std::cout;	using std::endl;
using std::clog;
using std::cin;
using std::make_pair;

using time_ms = std::chrono::milliseconds;
using prec_t = long double;

namespace HMT
{
	template<typename T>
	class Nodes
	{
	public:
		Nodes() = default;
		Nodes(const uint64_t& nodeX, const uint64_t& nodeY);
		virtual ~Nodes() = default;

		void putWallTemp(const T& northTemp, const T& eastTemp, const T& southTemp, const T& westTemp);
		void putHeatSource(const uint64_t& posX, const uint64_t& posY, const T& temp);
		void calculate(const prec_t& epsilon);
		bool hasHeatSource(void) const noexcept(true);
		T getTemp(const uint64_t& posX, const uint64_t& posY) const;

		void testBuffers(void) const;

	protected:
		void initBuffer(void);
		void calculateWThread(const prec_t& epsilon);
		
	private:
		bool _hasHeatSource, _hasCalculated, _canUseThreads;
		uint64_t _nodeX, _nodeY, _itterCnt;
		std::vector<std::vector<std::pair<T, bool>>> _nodes, _nodesOld;
		std::chrono::time_point<std::chrono::high_resolution_clock> _startTime, _endTime;
	};

	template<typename T>
	void Nodes<T>::testBuffers(void) const
	{
		for (uint64_t i = 0; i < this->_nodeY; ++i) {
			for (uint64_t j = 0; j < this->_nodeX; ++j) {
				cout << this->_nodes[i][j].first << ", ";
			}
			cout << endl;
		}
		for (uint64_t i = 0; i < this->_nodeY; ++i) {
			for (uint64_t j = 0; j < this->_nodeX; ++j) {
				cout << this->_nodesOld[i][j].first << ", ";
			}
			cout << endl;
		}
	}

	template<typename T>
	Nodes<T>::Nodes(const uint64_t& nodeX, const uint64_t& nodeY)
	{
		this->_nodeX = nodeX;
		this->_nodeY = nodeY;
		this->initBuffer();
		this->_hasHeatSource = false;
		this->_hasCalculated = false;
		this->_canUseThreads = false;
	}

	template<typename T>
	void Nodes<T>::initBuffer(void)
	{
		this->_nodes.reserve(this->_nodeY);
		this->_nodesOld.reserve(this->_nodeY);
		for (uint64_t i = 0; i < this->_nodeY; ++i) {
			std::vector<std::pair<T, bool>> tmp;
			tmp.reserve(this->_nodeX);
			for (uint64_t j = 0; j < this->_nodeX; ++j) {
				tmp.push_back(make_pair(static_cast<T>(0), false));
			}
			this->_nodes.push_back(tmp);
			this->_nodesOld.push_back(std::move(tmp));
		}
	}

	template<typename T>
	void Nodes<T>::putWallTemp(const T& northTemp, const T& eastTemp, const T& southTemp, const T& westTemp)
	{
		this->_hasCalculated = false;
		for (uint64_t i = 0; i < this->_nodeY; ++i) {
			this->_nodes[i][0].first = westTemp;
			this->_nodes[i][this->_nodeX - 1].first = eastTemp;
		}
		for (uint64_t i = 0; i < this->_nodeX; ++i) {
			this->_nodes[0][i].first = northTemp;
			this->_nodes[this->_nodeY - 1][i].first = southTemp;
		}
		for (uint64_t i = 0; i < this->_nodeY; i++) {
			this->_nodes[i].shrink_to_fit();
			this->_nodesOld[i].shrink_to_fit();
		}
		this->_nodes.shrink_to_fit();
		this->_nodesOld.shrink_to_fit();
		for (uint64_t i = 1; i < this->_nodeY - 1; ++i) {
			for (uint64_t j = 1; j < this->_nodeX - 1; ++j) {
				this->_nodes[i][j].first = (northTemp + eastTemp + southTemp + westTemp) / 4;
			}
		}
	}

	template<typename T>
	void Nodes<T>::putHeatSource(const uint64_t& posX, const uint64_t& posY, const T& temp)
	{
		this->_hasHeatSource = true;
		this->_hasCalculated = false;
		this->_nodes[posY][posX].first = temp;
		this->_nodes[posY][posX].second = true;
	}

	template<typename T>
	void Nodes<T>::calculate(const prec_t& epsilon)
	{
		if (!this->_hasCalculated) {
			this->_startTime = std::chrono::high_resolution_clock::now();
			if (this->_canUseThreads) {
				this->calculateWThread(epsilon);
			} else {
				this->_itterCnt = 0;
				prec_t diff = epsilon;
				while (epsilon <= diff) {
					++(this->_itterCnt);
					for (uint64_t i = 0; i < this->_nodeY; ++i)
						for (uint64_t j = 0; j < this->_nodeX; ++j)
							this->_nodesOld[i][j] = this->_nodes[i][j];

					for (uint64_t i = 1; i < this->_nodeY - 1; ++i) {
						for (uint64_t j = 1; j < this->_nodeX - 1; ++j) {
							if (this->_nodes[i][j].second != true) {
								this->_nodes[i][j].first = (this->_nodesOld[i - 1][j].first +
															this->_nodesOld[i + 1][j].first +
													 		this->_nodesOld[i][j - 1].first + 
													 		this->_nodesOld[i][j + 1].first) / 4;
								if (diff < std::fabs(this->_nodesOld[i][j].first - this->_nodes[i][j].first)) {
									diff = std::fabs(this->_nodesOld[i][j].first - this->_nodes[i][j].first);
								}
							}
						}
					}
				}
			}
			this->_endTime = std::chrono::high_resolution_clock::now();
			clog << "[noi: " << this->_itterCnt << "]" << endl;
			this->_hasCalculated = true;
		}
	}

	template<typename T>
	void Nodes<T>::calculateWThread(const prec_t& epsilon)
	{
		//!TODO implementation with std::thread
	}

	template<typename T>
	T Nodes<T>::getTemp(const uint64_t& posX, const uint64_t& posY) const
	{
		if (this->_hasCalculated)
			return this->_nodes[posY][posX].first;
		else return NULL;
	}
}

int main(int argc, char const *argv[])
{
	cout << std::nounitbuf;
	cout << std::setprecision(4) << std::fixed;
	cout << "############################ HMT Assignment ##########################" << endl
		 << " 2-D Steady State Conduction with Heat Generation" << endl << endl
		 << " Author: Samuel Paul Vishesh [UR11ME145] <paulsamuelvishesh@live.com>" << endl 
		 << "######################################################################" << endl << endl;

	std::chrono::time_point<std::chrono::high_resolution_clock> startTime, endTime;
	prec_t	lengthX = 0, lengthY = 0, dx = 0, dy = 0,
			epsilon = 0, diff = 0,
			tempNorth = 0, tempSouth = 0, tempEast = 0, tempWest = 0, tempAvg = 0;
	uint64_t nodeX = 0, nodeY = 0;
	bool hasHeatSource = false;

	cout << "epsilon(give a smaller value for high precision): ";
	cin >> epsilon;
	diff = epsilon;
	cout << "Enter length in x-axix: ";
	cin >> lengthX;
	cout << "Enter length in y-axis: ";
	cin >> lengthY;
	cout << "Enter dist. b/t nodes in x-axis: ";
	cin >> dx;
	cout << "Enter dist. b/t nodes in y-axis: ";
	cin >> dy;

	if (lengthX != 0 && lengthY != 0 && dx != 0 && dy != 0 && epsilon != 0)	{
		nodeX = static_cast<uint64_t>(lengthX / dx);
		nodeY = static_cast<uint64_t>(lengthY / dy);
	} else {
		clog << "Error in intial values entered... (please refer documentation for correct usage)" << endl;
		std::exit(EXIT_FAILURE);
	}
	clog << endl
		 << "[nodeX: " << nodeX << "]" << endl
		 << "[nodeY: " << nodeY << "]" << endl;
/**
	// init cal buffers
	std::vector<std::vector<std::pair<prec_t, bool>>> nodes, nodesOld;
	nodes.reserve(nodeY);
	nodesOld.reserve(nodeY);
	for (uint64_t i = 0; i < nodeY; ++i) {
		std::vector<std::pair<prec_t, bool>> tmp;
		for (uint64_t j = 0; j < nodeX; ++j) {
			tmp.push_back(make_pair(0.0f, false));
		}
		nodes.push_back(tmp);
		nodesOld.push_back(tmp);
	}

	cout << "temp. of northern wall: ";
	cin >> tempNorth;
	cout << "temp. of southern wall: ";
	cin >> tempSouth;
	cout << "temp. of eastern wall: ";
	cin >> tempEast;
	cout << "temp. of western wall: ";
	cin >> tempWest;

	for (uint64_t i = 0; i < nodeY; ++i) {
		nodes[i][0] = make_pair(tempWest, true);
		nodes[i][nodeY - 1] = make_pair(tempEast, true);
	}
	for (uint64_t i = 0; i < nodeX; ++i) {
		nodes[0][i] = make_pair(tempNorth, true);
		nodes[nodeX - 1][i] = make_pair(tempSouth, true);
	}
	tempAvg = (tempWest + tempEast + tempNorth + tempSouth) / 4;

	for (uint64_t i = 1; i < nodeY - 1; ++i) {
		for (uint64_t j = 1; j < nodeX - 1; ++j) {
			nodes[i][j] = make_pair(tempAvg, false);
		}
	}

	for (uint64_t i = 0; i < nodeY; ++i) {
		for (uint64_t j = 0; j < nodeX; ++j)
			cout << nodes[i][j].first << ", ";
		cout << endl;
	}

	{
		char choice = 'y';
		uint64_t posX = 0, posY = 0;
		prec_t sourceTemp = 0.0f;
		while (true) {
			 cout << "Do you want to mark any node(s) as heat source [y/n]: ";
			 cin >> choice;
			 if (!(choice == 'y' || choice == 'Y')) {
			 	hasHeatSource = true;
			 	break;
			 }

			 cout << "posX: ";
			 cin >> posX;
			 cout << "posY: ";
			 cin >> posY;
			 cout << "heat source temp @(" << posX << ", " << posY << "): ";
			 cin >> sourceTemp;

			 nodes[posX - 1][posY - 1].first = sourceTemp;
			 nodes[posX - 1][posY - 1].second = true;
		}
	}

	if (hasHeatSource)
		clog << endl << "[hasHeatSource: " << hasHeatSource << "]" << endl;
	for (auto const &i : nodes) {
		for (auto const &node : i)
			if (node.second == true)
				cout << node.first << ", ";
			else
				cout << "\t, ";
		cout << endl;
	}

	startTime = std::chrono::high_resolution_clock::now();
	size_t iter = 0;
	while (epsilon <= diff) {
		iter++;
		for (uint64_t i = 0; i < nodeY; ++i)
			for (uint64_t j = 0; j < nodeX; ++j)
				nodesOld[i][j] = nodes[i][j];

		diff = 0.0f;
		for (uint64_t i = 1; i < nodeY - 1; ++i)
			for (uint64_t j = 1; j < nodeX - 1; ++j) {
				if (nodes[i][j].second != true) {
					nodes[i][j].first = (nodesOld[i - 1][j].first + nodesOld[i + 1][j].first +
											 nodesOld[i][j - 1].first + nodesOld[i][j + 1].first) / 4;
				}

				if (diff < std::fabs(nodesOld[i][j].first - nodes[i][j].first))
					diff = std::fabs(nodesOld[i][j].first - nodes[i][j].first);
			}
	}
	endTime = std::chrono::high_resolution_clock::now();
	auto elapsedTime = endTime - startTime;

	cout << endl << "######## After iter ..." << endl;

	for (auto const &i : nodes) {
		for (auto const &node : i)
			cout << node.first << ", ";
		cout << endl;
	}

	clog << endl
		 << "[iter: " << iter << "]" << endl
		 << "[time for iter: " << std::chrono::duration_cast<time_ms>(elapsedTime).count() << "ms]" << endl;

	{
		char choice = 'y';
		uint64_t posX = 0, posY = 0;
		while (true)
		{
			cout << "get temp any pos? (n/y): ";
			cin >> choice;
			if (!(choice == 'y' || choice == 'Y'))
				break;

			cout << "posX: ";
			cin >> posX;
			cout << "posY: ";
			cin >> posY;
			cout << "temp @(" << posX << ", " << posY << "): " << nodes[posY - 1][posX - 1].first << endl;
		}
	}**/

	HMT::Nodes<prec_t> nodes{nodeX, nodeY};
	nodes.putWallTemp(500.0f, 100.0f, 100.0f, 100.0f);
	nodes.testBuffers();
	nodes.calculate(0.1f);
	nodes.testBuffers();
	
	return 0;
}