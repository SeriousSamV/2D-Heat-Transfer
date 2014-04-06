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