#ifndef GR_H
#define GR_H

#include <string>
#include <vector>
#include <map>

#include "seg.h"
#include "graph.h"

class Graph_builder {
	vector<Seg> segs;

  public:
	void Read_chain ( const string _file, const int _tspc, const int _qspc, const int _alnminsize);
	void Read_delta ( const string _file, const int _tspc, const int _qspc, const int _alnminsize );
	void Read_paf ( const string _file, const int _tspc, const int _qspc, const int _alnminsize );

	int Return_segcnt () { return segs.size(); }

	void Build_endTree ( vector<pair<string, int>>& _sortend, int _beg, int _end,  End_inTree* _anc );
	void Remove_endTree ( End_inTree* _anc );
	void Find_overlapEnd ( End_inTree* _anc, Seg& _query, const int _curspc, vector<int>& _overlap );
	vector<Seg> Split_seg ( const int _spccnt, const int _alnminsize );

	vector<int> Get_boundary ( Seg& _splitseg, const int _spc, const int _pos, vector<int>& _updatematch, map<int, vector<int> >& _updategap );
	void Build_node ( Graph& _gr, string _out );
	void Build_link ( Graph& _gr, const int _spccnt, string _nodetype, string _out );

	void Print_seg ();
};

#endif
