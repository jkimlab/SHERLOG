#ifndef SEG_H
#define SEG_H

#include <string>
#include <vector>
#include <map>

#include "util.h"

struct Aln {
	vector<int> matchlen;
	map<int, vector<int> > gaplen;
};
	
class Seg {
	char segdir;

  protected: 
	string id;
	bool proc;
	int spc_comb;
	map <int, string> chr;
	map <int, int> beg;
	map <int, int> end;
  
  public:
	Seg ( const string _id, const int _aspc, const string _achr, const int _abeg, const int _aend, const int _bspc, const string _bchr, const int _bbeg, const int _bend, const char _dir );
	Seg ( const string _id, const Seg& _oriseg );

	void Update_beg ( const int _spc, const int _beg ) { beg[ _spc ] = _beg; }
	void Update_end ( const int _spc, const int _end ) { end[ _spc ] = _end; }
	void Update_process ( const bool _proc ) { proc = _proc; }
	void Update_id ( const string _id ){ this->id = _id; }

	string Return_chr ( int _spc ){ return chr[ _spc ]; }
	char Return_dir (){ return segdir; }
	string Return_id (){ return id; }
	int Return_beg ( int _spc ){ return beg[ _spc ]; }
	int Return_end ( int _spc ){ return end[ _spc ]; }
	int Return_comb () { return spc_comb; }
	bool Return_proc () { return proc; }
	int Return_length ();
	vector<int> Return_spc ();

	bool Contain_spc ( int _spc ){ return ( this->beg.find(_spc) == this->beg.end() )? "false" : "true"; }
	void Print_seg ( ofstream& _WF, Util& _util );
};

inline bool operator == ( Seg& _aaln, Seg& _baln ){
	return _aaln.Return_id() == _baln.Return_id();
}

bool Sort_seg ( Seg& _aseg, Seg& _bseg, const int _spc );

bool Sort_end ( string& _apos, string& _bpos );

struct End_inTree {
	string chr;
	int pos;
	End_inTree* left;
	End_inTree* right;
};

class Node : public Seg {
	map <int, vector<Node*>> next;
	vector<Node*> member;

  public:
	Node ( const string _id, Seg& _seg );
	void Add_link ( const int _spc, Node* _nxt ){ this->next[_spc].push_back( _nxt ); }
	void Remove_link( const int _spc, Node* _nxt );
	void Add_member ( Node* _newmem );
	vector<Node*> Return_member () { return this->member; }

	vector<Node*> Return_links ( const int _spc ){ return this->next[_spc]; }
	
	void Print_node ( ofstream& _WF );
};

inline bool operator == ( Node& _anode, Node& _bnode ){
	return _anode.Return_id() == _bnode.Return_id();
}

#endif
