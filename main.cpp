/**
*	2-D Steady State Conduction without Heat Generation | main.cpp
*
*	@libs	[Nodes.a, NodesHelper.a]
*	@header	[Nodes.h, NodesHelper.h]
*
*	@author Samuel0Paul <paulsamuelvishesh@live.com>
**/

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstdint>
#include <chrono>
#include <thread>
#include <atomic>

#include "header/Nodes.h"
#include "header/NodesHelper.h"

#include "test/test.cpp"

using std::cout;	using std::endl;
using std::clog;
using std::cin;
using std::make_pair;

using prec_t = long double;

int main(int argc, char const *argv[])
{
	cout << std::nounitbuf;
	cout << std::setprecision(4) << std::fixed << std::boolalpha;
	cout << "############################ HMT Assignment ##########################" << endl
		 << " 2-D Steady State Conduction with and withour Heat Generation" << endl << endl
		 << " Author: Samuel Paul Vishesh [UR11ME145] <paulsamuelvishesh@live.com>" << endl
		 << "######################################################################" << endl << endl;

	/*HMT::NodesHelper<prec_t> nodes;
	nodes.init();
	nodes.canUseThreads(false);
	nodes.calculate();
	cout << nodes;
	cout << "Time taken: " << nodes.getDuration<std::chrono::nanoseconds>().count() << "ns" << endl;*/

	test::NodesWithoutHeatSrc<prec_t> testNodesWOHSrcTE{12, 12,
		500.0f, 100.0f, 100.0f, 100.0f, 0.0000001f, true};
	testNodesWOHSrcTE.test();
	test::NodesWithoutHeatSrc<prec_t> testNodesWOHSrcWTE{12, 12,
		500.0f, 100.0f, 100.0f, 100.0f, 0.0000001f, false};
	testNodesWOHSrcWTE.test();
	
	return 0;
}