#ifndef NODES_HELPER_CXX
#define NODES_HELPER_CXX

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <chrono>
#include <mutex>

#include "../header/NodesHelper.h"

using std::cout;	using std::endl;
using std::cin;
using std::clog;
using std::make_pair;

using prec_t = long double;

namespace HMT
{

template<typename T>
NodesHelper<T>::NodesHelper(void): _calculated(false), _canUseThreads(false),
	_tempNorth(0), _tempEast(0), _tempSouth(0), _tempWest(0),
	_lenX(0), _lenY(0), _nodeX(0), _nodeY(0), _dX(0), _dY(0)
{ }

template<typename T>
void NodesHelper<T>::init(void)
{
	char heatSourceChoice = 'n';

	cout << endl << "Input Section START from NodesHelper ##########" << endl;
	cout << "Enter lenght in x-axis: ";
	cin >> this->_lenX;
	cout << "Enter length in y-axis: ";
	cin >> this->_lenY;
	cout << "Enter the dist. b/t nodes in x-axis: ";
	cin >> this->_dX;
	cout << "Enter the dist. b/t nodes in y-axis: ";
	cin >> this->_dY;
	cout << "Enter temp. in northern wall: ";
	cin >> this->_tempNorth;
	cout << "Enter temp. in eastern wall: ";
	cin >> this->_tempEast;
	cout << "Enter temp. in southern wall: ";
	cin >> this->_tempSouth;
	cout << "Enter temp. in western wall: ";
	cin >> this->_tempWest;

	this->_nodeX = this->_lenX / this->_dX;
	this->_nodeY = this->_lenY / this->_dX;
	clog << "[nodeX: " << this->_nodeX << "]" << endl;
	clog << "[nodeY: " << this->_nodeY << "]" << endl;

	this->_nodes = Nodes<T>(this->_nodeX, this->_nodeY);
	this->_nodes.canUseThreads(this->_canUseThreads);
	this->_nodes.setWallTemp(this->_tempNorth, this->_tempEast, this->_tempSouth, this->_tempWest);

	{
		uint64_t posX = 0, posY = 0;
		T temp = 0;
		cout << "Does this surface have any Heat Source? (y/n): ";
		cin >> heatSourceChoice;
		while (true) {
			if (!(heatSourceChoice == 'y' || heatSourceChoice == 'Y')) {
				break;
			}
			cout << "posX: ";
			cin >> posX;
			cout << "posY: ";
			cin >> posY;
			if (posX < this->_nodeX && posY < this->_nodeY) {
				cout << "temp @(" << posX << ", " << posY << "): ";
				cin >> temp;
				this->_heatSources.push_back(make_pair(make_pair(posX, posY), temp));
			} else {
				cout << "pos outOfRange error (please refer documentation for further reference)..." << endl;
			}
			cout << "Still have Heat Sources to enter? (y/n): ";
			cin >> heatSourceChoice;
		}
	}

	for (const std::pair<std::pair<uint64_t, uint64_t>, T>& i : this->_heatSources) {
		this->_nodes.setHeatSource(
			i.first.first,
			i.first.second,
			i.second);
	}

	cout << "Input Section END from NodesHelper  ##########" << endl << endl;
}

template<typename T>
void NodesHelper<T>::addHeatSource(std::initializer_list<std::pair<std::pair<uint64_t, uint64_t>, T>>& lst)
{
	for (const std::pair<std::pair<uint64_t, uint64_t>, T>& i : lst) {
		this->_heatSources.push_back(i);
		this->_nodes.setHeatSource(
			i.first.first,
			i.first.second,
			i.second);
	}
}

template<typename T>
void NodesHelper<T>::canUseThreads(const bool choice) noexcept(true)
{
	this->_canUseThreads = choice;
	this->_nodes.canUseThreads(this->_canUseThreads);
}

template<typename T>
bool NodesHelper<T>::canUseThreads(void) noexcept(true)
{
	return this->_canUseThreads;
}

template<typename T>
void NodesHelper<T>::calculate(void)
{
	prec_t epsilon = 0;
	cout << "Enter value of Epsilon: ";
	cin >> epsilon;

	this->_nodes.calculate(epsilon);
	this->_calculated = true;
}

template<typename T> template<typename durationFormat>
durationFormat NodesHelper<T>::getDuration() const
{
	return std::chrono::duration_cast<durationFormat>(this->_nodes.getDuration());
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const NodesHelper<T>& obj)
{
	for (uint64_t i = 0; i < obj._nodeX; ++i) {
		for (uint64_t j = 0; j < obj._nodeY; ++j) {
			cout << obj._nodes.getTemp(i, j) << ", ";
		}
		cout << endl;
	}

	return os;
}

}

#endif