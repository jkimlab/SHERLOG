#ifndef GS_H
#define GS_H

#include <string>
#include <vector>
#include <map>

#include "seg.h"
#include "graph.h"

class Synteny_builder {
	map<string, vector<vector<Node*> > > synteny;

  public:
	void Find_conserved_link ( Graph& gr, string _grtype, string _out );
	void Build_synteny ( Graph& _igr, Graph& _ogr, int _level, int _alnminsize, Util& _util );
	void Update_link ( Graph& _gr, int _spccnt, string _out );
	void Find_loop ( Node* _curnode, vector<Node*>& _curpath, map<pair<Node*, Node*>, int>& _loop );
	void Remove_loop ( Graph& _gr );
	vector<vector<Node*> > Connect_links ( Node* _curnode );
};

#endif
