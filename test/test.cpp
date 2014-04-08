/**
The MIT License (MIT)

Copyright (c) 2014 Samuel Vishesh Paul

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
**/

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdint>
#include <chrono>
#include <thread>
#include <atomic>

#include "../header/Nodes.h"

using std::cout;	using std::endl;
using std::clog;
using std::cin;
using std::make_pair;

using prec_t = long double;

namespace test
{

class IUnitTest
{
public:
	virtual void test(void) = 0;
};

template<typename T>
class NodesWithoutHeatSrc: public IUnitTest
{
public:
	NodesWithoutHeatSrc(void) = default;
	NodesWithoutHeatSrc(uint64_t nodeX, uint64_t nodeY,
			T tempNorth, T tempEast, T tempSouth, T tempWest,
			T epsilon, bool canUseThreadsChoice): _epsilon(epsilon), _nodeX(nodeX), _nodeY(nodeY)
	{
		this->_nodes = HMT::Nodes<prec_t>(nodeX, nodeY);
		this->_nodes.setWallTemp(tempNorth, tempEast, tempSouth, tempWest);
		this->_nodes.canUseThreads(canUseThreadsChoice);
		clog << "############### test::NodesWithoutHeatSrc [" << typeid(*this).name() << "] ########" << endl;
		clog << "HMT::Nodes obj created..." << endl;
	}
	virtual ~NodesWithoutHeatSrc() = default;

	virtual void test(void) override
	{
		this->_nodes.calculate(this->_epsilon);

		clog << "After calculation: " << endl;
		for (uint64_t i = 0; i < this->_nodeX; ++i) {
			for (uint64_t j = 0; j < this->_nodeY; ++j) {
				clog << this->_nodes.getTemp(i, j) << ", ";
			}
			cout << endl;
		}

		clog << endl
			 << "using threads: " << this->_nodes.canUseThreads() << endl
			 << "time taken for [" << typeid(*this).name() << "]: " << this->_nodes.getDuration().count() << "ns"
			 << endl
			 << "################################################################################" << endl
			 << endl;
	}
	
private:
	HMT::Nodes<T> _nodes;
	prec_t _epsilon;
	uint64_t _nodeX, _nodeY;
};

template<typename T>
class NodesWithHeatSrc: public NodesWithoutHeatSrc<T>
{
public:
	NodesWithHeatSrc(uint64_t nodeX, uint64_t nodeY,
			T tempNorth, T tempEast, T tempSouth, T tempWest,
			T epsilon, bool canUseThreadsChoice,
			const std::vector<std::pair<std::pair<uint64_t, uint64_t>, T>>& tempHeatSrc): _epsilon(epsilon),
				_nodeX(nodeX), _nodeY(nodeY)
	{
		this->_nodes = HMT::Nodes<prec_t>(nodeX, nodeY);
		this->_nodes.setWallTemp(tempNorth, tempEast, tempSouth, tempWest);
		this->_nodes.canUseThreads(canUseThreadsChoice);
		for (const auto& i : tempHeatSrc) {
			this->_nodes.setHeatSource(i.first.first, i.first.second, i.second);
		}
		clog << "############### test::NodesWithHeatSrc [" << typeid(*this).name() << "] ########" << endl;
		clog << "HMT::Nodes obj created..." << endl;
	}
	virtual ~NodesWithHeatSrc() = default;

	void test(void) override
	{
		this->_nodes.calculate(this->_epsilon);

		clog << "After calculation: " << endl;
		for (uint64_t i = 0; i < this->_nodeX; ++i) {
			for (uint64_t j = 0; j < this->_nodeY; ++j) {
				clog << this->_nodes.getTemp(i, j) << ", ";
			}
			cout << endl;
		}

		clog << endl
			 << "using threads: " << this->_nodes.canUseThreads() << endl
			 << "time taken for [" << typeid(*this).name() << "]: " << this->_nodes.getDuration().count() << "ns"
			 << endl
			 << "################################################################################" << endl
			 << endl;
	}
	
private:
	HMT::Nodes<T> _nodes;
	prec_t _epsilon;
	uint64_t _nodeX, _nodeY;
};

}