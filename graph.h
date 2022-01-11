#ifndef BUILD_GR_H
#define BUILD_GR_H

#include <string>
#include <vector>

#include "seg.h"

class Graph {
	vector<Node> nodes;

  public:
	vector<Node>& Return_nodes (){ return nodes; }
	
	friend class Graph_builder;
	friend class Synteny_builder;
};

#endif
