#ifndef NODES_H
#define NODES_H

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <chrono>
#include <thread>

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

	void setWallTemp(const T& northTemp, const T& eastTemp, const T& southTemp, const T& westTemp);
	void setHeatSource(const uint64_t& posX, const uint64_t& posY, const T& temp);
	void canUseThreads(const bool choice) noexcept(true);
	bool canUseThreads(void) noexcept(true);
	void calculate(const prec_t epsilon);
	bool hasHeatSource(void) const noexcept(true);
	T getTemp(const uint64_t& posX, const uint64_t& posY) const;
	std::chrono::nanoseconds getDuration(void) const;

	template<typename T1> friend std::ostream& operator<<(std::ostream&, const Nodes<T1>&);

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

template<typename T1> std::ostream& operator<<(std::ostream&, const Nodes<T1>&);

}

#include "../definition/Nodes.cxx"

#endif