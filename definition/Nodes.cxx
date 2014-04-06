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

#ifndef NODES_CXX
#define NODES_CXX

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <chrono>
#include <thread>
#include <mutex>

#include "../header/Nodes.h"

using std::cout;	using std::endl;
using std::clog;
using std::cin;
using std::make_pair;

using prec_t = long double;

namespace HMT
{

template<typename T>
std::chrono::nanoseconds Nodes<T>::getDuration(void) const
{
	if (this->_hasCalculated) {
		return std::chrono::duration_cast<std::chrono::nanoseconds>(this->_endTime - this->_startTime);
	} else {
		//!TODO	implement error handling and notification
		return std::chrono::nanoseconds(0);
	}
}

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
	for (uint64_t i = 0; i < this->_nodeY; i++) {
		this->_nodes[i].shrink_to_fit();
		this->_nodesOld[i].shrink_to_fit();
	}
	this->_nodes.shrink_to_fit();
	this->_nodesOld.shrink_to_fit();
}

template<typename T>
void Nodes<T>::setWallTemp(const T& northTemp, const T& eastTemp, const T& southTemp, const T& westTemp)
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
	for (uint64_t i = 1; i < this->_nodeY - 1; ++i) {
		for (uint64_t j = 1; j < this->_nodeX - 1; ++j) {
			this->_nodes[i][j].first = (northTemp + eastTemp + southTemp + westTemp) / 4;
		}
	}
}

template<typename T>
void Nodes<T>::setHeatSource(const uint64_t& posX, const uint64_t& posY, const T& temp)
{
	this->_hasHeatSource = true;
	this->_hasCalculated = false;
	this->_nodes[posY][posX].first = temp;
	this->_nodes[posY][posX].second = true;
}

template<typename T>
void Nodes<T>::canUseThreads(const bool choice) noexcept(true)
{
	this->_canUseThreads = choice;
}

template<typename T>
bool Nodes<T>::canUseThreads(void) noexcept(true)
{
	return this->_canUseThreads;
}

template<typename T>
void Nodes<T>::calculate(const prec_t epsilon)
{
	if (!this->_hasCalculated) {
		this->_startTime = std::chrono::high_resolution_clock::now();
		if (this->_canUseThreads &&
				(this->_nodeY > this->_nodeX ? this->_nodeY : this->_nodeX) > std::thread::hardware_concurrency() * 4) {
			this->calculateWThread(epsilon);
		} else {
			this->_itterCnt = 0;
			prec_t diff = epsilon;
			while (epsilon <= diff) {
				++(this->_itterCnt);
				for (uint64_t i = 0; i < this->_nodeY; ++i)
					for (uint64_t j = 0; j < this->_nodeX; ++j)
						this->_nodesOld[i][j] = this->_nodes[i][j];

				diff = 0.0f;
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
		this->_hasCalculated = true;
	}
}

template<typename T>
void Nodes<T>::calculateWThread(const prec_t& epsilon)
{
	unsigned int nofCore = std::thread::hardware_concurrency();
	std::thread threads[nofCore];
	auto calc = [=] (uint64_t nodeX, uint64_t nodeY, const prec_t epsilon) -> void {
		std::mutex myMutex;
		clog << "thread [id: " << std::this_thread::get_id() << "]" << endl;
		prec_t diff = epsilon;
		while (epsilon <= diff) {
			++(this->_itterCnt);
			for (uint64_t i = 0; i < nodeY; ++i)
				for (uint64_t j = 0; j < nodeX; ++j) {
					std::lock_guard<std::mutex> guard(myMutex);
					this->_nodesOld[i][j] = this->_nodes[i][j];
				}

			diff = 0.0f;
			for (uint64_t i = 1; i < nodeY - 1; ++i) {
				for (uint64_t j = 1; j < nodeX - 1; ++j) {
					std::lock_guard<std::mutex> guard(myMutex);
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
	};

	clog << "main tread [id: " << std::this_thread::get_id() << "]" << endl; 
	for (unsigned int i = 0; i < nofCore; ++i) {
		if (this->_nodeY > this->_nodeX) {
			threads[i] = std::thread(calc, this->_nodeX, (this->_nodeY / nofCore) * (i + 1), epsilon);
		} else {
			threads[i] = std::thread(calc, (this->_nodeX / nofCore) * (i + 1), this->_nodeY, epsilon);
		}
	}
	for (unsigned int i = 0; i < nofCore; ++i)
		threads[i].join();
}

template<typename T>
T Nodes<T>::getTemp(const uint64_t& posX, const uint64_t& posY) const
{
	if (this->_hasCalculated)
		return this->_nodes[posY][posX].first;
	else {
		//!TODO implement error handling or error throw mechanism
		return 0;
	}
}

template<typename T>
std::ostream& operator<<(std::ostream& os, const Nodes<T>& obj)
{
	os << endl;
	for (const auto& i : obj._nodes) {
		for (const auto& node : i)
			os << node.first << ", ";
		os << endl;
	}
	return os;
}

}

#endif