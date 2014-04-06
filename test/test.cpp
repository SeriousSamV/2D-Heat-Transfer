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
	NodesWithoutHeatSrc(uint64_t nodeX, uint64_t nodeY,
			T tempNorth, T tempEast, T tempSouth, T tempWest,
			T epsilon, bool canUseThreadsChoice): _epsilon(epsilon), _nodeY(nodeY), _nodeX(nodeX)
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
			 << "time taken for [" << typeid(*this).name() << "]: " << this->_nodes.getDuration().count() << "ns"
			 << endl;
	}
	
private:
	HMT::Nodes<T> _nodes;
	prec_t _epsilon;
	uint64_t _nodeX, _nodeY;
};

}