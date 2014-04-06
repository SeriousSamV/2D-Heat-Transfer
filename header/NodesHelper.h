#ifndef NODES_HELPER_H
#define NODES_HELPER_H

#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <chrono>

#include "Nodes.h"

namespace HMT
{

template<typename T>
class NodesHelper
{
public:
	NodesHelper(void);
	virtual ~NodesHelper(void) = default;

	void init(void);
	void addHeatSource(std::initializer_list<std::pair<std::pair<uint64_t, uint64_t>, T>>&);
	void canUseThreads(const bool choice) noexcept(true);
	bool canUseThreads(void) noexcept(true);
	void calculate(void);

	template<typename durationFormat> durationFormat getDuration() const;

	template<typename T1> friend std::ostream& operator<<(std::ostream&, const NodesHelper<T1>&);
	template<typename T1> friend std::istream& operator>>(std::istream&, const NodesHelper<T1>&);

private:
	Nodes<T> _nodes;
	bool _calculated, _hasHeatSource, _canUseThreads;
	T _tempNorth, _tempEast, _tempSouth, _tempWest;
	uint64_t _lenX, _lenY, _nodeX, _nodeY, _dX, _dY;
	std::list<std::pair<std::pair<uint64_t, uint64_t>, T>> _heatSources;
	
};

template<typename T1> std::ostream& operator<<(std::ostream&, const NodesHelper<T1>&);
template<typename T1> std::istream& operator>>(std::istream&, const NodesHelper<T1>&);

}

#include "../definition/NodesHelper.cxx"

#endif