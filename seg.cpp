#include <iostream>
#include <fstream>

using namespace std;

#include <string>
#include <vector>
#include <map>
#include "seg.h"

Seg::Seg ( const string _id, const int _aspc, const string _achr, const int _abeg, const int _aend, const int _bspc, const string _bchr, const int _bbeg, const int _bend, const char _dir ){
	this->id = _id;
	this->spc_comb = _aspc + _bspc;
	this->segdir = _dir;
	chr[_aspc] = _achr, chr[_bspc] = _bchr;
	beg[_aspc] = _abeg, beg[_bspc] = _bbeg;
	end[_aspc] = _aend, end[_bspc] = _bend;

	this->proc = false;
}

Seg::Seg ( const string _id, const Seg& _oriseg ){
	this->id = _id;
	this->segdir = _oriseg.segdir;
	this->spc_comb = _oriseg.spc_comb;
	this->chr.insert( _oriseg.chr.begin(),  _oriseg.chr.end() );
	this->beg.insert( _oriseg.beg.begin(),  _oriseg.beg.end() );
	this->end.insert( _oriseg.end.begin(),  _oriseg.end.end() );

	this->proc = false;
}

int Seg::Return_length (){
	int _len = -1;
		
	for( map<int, int>::iterator _it=this->beg.begin(); _it != this->beg.end(); ++_it ){
		int _spc = _it->first;
		if( _len < 0 ){
			_len = this->end[_spc] - this->beg[_spc];
		}else{
			_len = ( ( this->end[_spc] - this->beg[_spc] ) < _len )? ( this->end[_spc] - this->beg[_spc] ) : _len ;
		}
	}

	return _len;
}

vector<int> Seg::Return_spc (){
	vector<int> _spclist;

	for( map<int, int>::iterator _it=this->beg.begin(); _it != this->beg.end(); ++_it ){
		_spclist.push_back(_it->first);
	}

	return _spclist;
}

void Seg::Print_seg ( ofstream& WF, Util& _util ){
	WF << this->id;
	for( map<int, string>::iterator _it=this->chr.begin(); _it != this->chr.end(); ++_it ){
		int spc = _it->first;
		WF << "\t" << _util.Return_spcname( spc ) << "." << this->chr[spc] << ":" << this->beg[spc] << "-" << this->end[spc];
	}
	WF << "\t" << segdir << endl;
}

bool Sort_seg ( Seg& _aseg, Seg& _bseg, const int _spc ){
	string _achr, _bchr;
	int _aocp, _bocp, _abeg, _bbeg, _aend, _bend;

	_aocp = _aseg.Contain_spc( _spc );
	_bocp = _bseg.Contain_spc( _spc );

	_achr = _aseg.Return_chr( _spc ), _bchr = _bseg.Return_chr( _spc );
	_abeg = _aseg.Return_beg( _spc ), _bbeg = _bseg.Return_beg( _spc );
	_aend = _aseg.Return_end( _spc ), _bend = _bseg.Return_end( _spc );

	return ( _aocp > _bocp || _achr < _bchr || ( _achr == _bchr && _abeg < _bbeg ) || ( _achr == _bchr && _abeg == _bbeg && _aend < _bend ) );
}

bool Sort_end ( string& _apos, string& _bpos ){
	vector<string> _apos_col, _bpos_col;

	Split_by_space ( _apos, _apos_col );
	Split_by_space ( _bpos, _bpos_col );
	
	string _achr = _apos_col[0];
	string _bchr = _bpos_col[0];

	int _aloc = atoi( _apos_col[1].c_str() ); 
	int _bloc = atoi( _bpos_col[1].c_str() ); 

	return ( _achr < _bchr || _aloc < _bloc ); 
}

Node::Node ( const string _id, Seg& _seg ) : Seg ( _id, _seg ){
	Node* _ptr = (Node*)(&_seg);
	Add_member( _ptr );
}

void Node::Add_member ( Node* _newmem ){
	if( this->Return_dir() != _newmem->Return_dir() ){
		cerr << "\n[Error]! The alignments with differnt Orientaion are contained in a synteny" << endl;
		cerr << " - Check synteny " << this->Return_id() << endl;
	}
	this->member.push_back( _newmem );

	vector<int> _spclist = this->Return_spc();
	for(int _i = 0; _i < _spclist.size(); ++_i){
		int _curspc = _spclist[_i];

		if( this->beg[ _curspc ] > _newmem->beg[ _curspc ] ){
			this->beg[ _curspc ] = _newmem->beg[ _curspc ];
		}

		if( this->end[ _curspc ] < _newmem->end[ _curspc ] ){
			this->end[ _curspc ] = _newmem->end[ _curspc ];
		}
	}
}

void Node::Remove_link( const int _spc, Node* _nxt ){
	for( int _i = 0; _i < this->next[_spc].size(); ++_i ){
		if( this->next[_spc][_i]->Return_id() == _nxt->Return_id() ){
			this->next[_spc].erase(this->next[_spc].begin() + _i );
			break;
		}
	}
}

void Node::Print_node ( ofstream& WF ) {
	WF << "#" << this->id;
	for( map<int, string>::iterator _it=this->chr.begin(); _it != this->chr.end(); ++_it ){
		int spc = _it->first;
		WF << "\t" << spc << "." << this->chr[spc] << ":" << this->beg[spc] << "-" << this->end[spc];
	}

	WF << "\t" << this->Return_dir() << endl;
	WF << " - member :"; 
	for( int _m = 0; _m < member.size(); ++_m ){
		WF << " " << member[_m]->Return_id();
	}
	WF << endl;
}

