#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>

using namespace std;

#include "util.h"
#include "buildgraph.h"

void Graph_builder::Read_chain ( const string _file, const int _tspc, const int _qspc, const int _alnminsize ){
	ifstream _openFile ( _file.data() );
	if ( _openFile.is_open() ){
		string _ln, _tchr, _qchr;

		char _dir = '\0';
		int _tbeg = 0, _tend = 0, _qbeg = 0, _qend = 0, _cid = -1;
		int _tbeg_sub = 0, _qbeg_sub = 0, _tend_sub = 0, _qend_sub = 0;
		int _qbeg_sub_5 = 0, _qend_sub_5 = 0, _qchr_len = 0;

		vector <int> _matches;
		vector <int> _qgaps;
		vector <int> _tgaps;

		string _spcpair = to_string ( _tspc ) + ":" + to_string ( _qspc );

		int _oid = 1;
		while ( getline ( _openFile, _ln ) ){
			if( ! _ln.length() || _ln[0] == '#' ){ continue; }
			
			vector <string> _col;
			Split_by_space ( _ln, _col );

			if( _ln.find("chain") != string::npos ){
				_cid = 0, _oid = 1;

				_tbeg = atoi( _col[5].c_str() ), _tend = atoi( _col[6].c_str() ), _qbeg = atoi( _col[10].c_str() ), _qend = atoi( _col[11].c_str() );

				int _tsize = _tend - _tbeg, _qsize = _qend - _qbeg;
				int _size = ( _tsize < _qsize )? _tsize : _qsize;
				if( _size < _alnminsize ){ continue; } 
				
				_tchr = _col[2], _qchr = _col[7];
				_dir= _col[9][0];
				_cid = atoi( _col[12].c_str() );

				_tbeg_sub = _tend_sub = _tbeg;
				_qbeg_sub = _qend_sub = _qbeg;

				_qchr_len = atoi( _col[8].c_str() );
				
				_matches.clear();
				_tgaps.clear();
				_qgaps.clear();
			} else {
				if( _cid ){
					int _match = atoi ( _col[0].c_str() );
					
					_tend_sub += _match;
					_qend_sub += _match;

					_matches.push_back( _match );

					int _tgap = 0, _qgap = 0;
					if( _col.size() != 1 ){
						_tgap = atoi( _col[1].c_str() );
						_qgap = atoi( _col[2].c_str() );

						if( _tgap == 0 || _qgap == 0 ){
							_tend_sub += _tgap;
							_qend_sub += _qgap;

							_tgaps.push_back( _tgap );
							_qgaps.push_back( _qgap );

							continue;
						}
					}else{
						if( _tend_sub != _tend || _qend_sub != _qend ){
							cerr << "[Error] Chain" << _cid << " seems to be not parsed well" << endl;
						}
					}
					
					_qbeg_sub_5 = _qbeg_sub, _qend_sub_5 = _qend_sub;

					if(_dir == '-'){
						_qbeg_sub_5 = _qchr_len - _qend_sub;
						_qend_sub_5 = _qchr_len - _qbeg_sub;
					}
						
					string _sid = to_string( _cid ) + "." + to_string( _oid++ );

					Seg _newseg( _sid, _tspc, _tchr, _tbeg_sub, _tend_sub, _qspc, _qchr, _qbeg_sub_5, _qend_sub_5, _dir );
					this->segs.push_back( _newseg );

					_matches.clear();
					_tgaps.clear();
					_qgaps.clear();

					_tbeg_sub = _tend_sub + _tgap;
					_qbeg_sub = _qend_sub + _qgap;

					_tend_sub = _tbeg_sub;
					_qend_sub = _qbeg_sub;
				}
			}
		}

		_matches.clear();
		_qgaps.clear();
		_tgaps.clear();

	} else {
		cerr << "[Error] Cannot open " << _file << endl;
	}

	_openFile.close();
}

void Graph_builder::Read_delta ( const string _file, const int _tspc, const int _qspc, const int _alnminsize ){
	ifstream _openFile ( _file.data() );
	if ( _openFile.is_open() ){
		bool header = true;
		string _ln, _tchr, _qchr;
		int _tbeg, _tend, _qbeg, _qend;
		char _dir = '\0';

		string _spcpair = to_string ( _tspc ) + ":" + to_string ( _qspc );

		int _oid = 1;
		while ( getline ( _openFile, _ln ) ){
			if( ! _ln.length() ){ continue; }
			
			if( _ln[0] == '>' ){
				header = false;
			}
			if( header ){ continue; }
			
			vector <string> _col;
			Split_by_space ( _ln, _col );
			
			if( _col.size() == 7 ){
				_tbeg = atoi( _col[0].c_str() ), _tend = atoi( _col[1].c_str() ), _qbeg = atoi( _col[2].c_str() ), _qend = atoi( _col[3].c_str() );
				
				_tbeg -= 1;
				if( _qbeg > _qend ){
					int	_tmp = _qbeg;
					_qbeg = _qend - 1;
					_qend = _tmp;
					_dir = '-';
				}else{
					_qbeg -= 1;
					_dir = '+';
				}

				int _tsize = _tend - _tbeg, _qsize = _qend - _qbeg;
				int _size = ( _tsize < _qsize )? _tsize : _qsize;
				if( _size < _alnminsize ){ continue; } 
				
				string _sid = to_string( _oid++ );
				Seg _newseg( _sid, _tspc, _tchr, _tbeg, _tend, _qspc, _qchr, _qbeg, _qend, _dir );
				this->segs.push_back( _newseg );
			}else if( _ln[0] == '>' ){
				_tchr = _col[0].substr( 1 ), _qchr = _col[1];
			}
		}
	} else {
		cerr << "[Error] Cannot open " << _file << endl;
	}

	_openFile.close();
}

void Graph_builder::Read_paf ( const string _file, const int _tspc, const int _qspc, const int _alnminsize ){
	ifstream _openFile ( _file.data() );
	if ( _openFile.is_open() ){
		string _ln, _tchr, _qchr;
		int _tbeg, _tend, _qbeg, _qend;
		char _dir = '\0';

		string _spcpair = to_string ( _tspc ) + ":" + to_string ( _qspc );

		int _oid = 1;
		while ( getline ( _openFile, _ln ) ){
			if( ! _ln.length() ){ continue; }
			
			vector <string> _col;
			Split_by_space ( _ln, _col );

			_tchr = _col[0], _qchr = _col[5];

			_tbeg = atoi( _col[2].c_str() ), _tend = atoi( _col[3].c_str() ), _qbeg = atoi( _col[7].c_str() ), _qend = atoi( _col[8].c_str() );
			_dir = _col[4][0];

			int _tsize = _tend - _tbeg, _qsize = _qend - _qbeg;
			int _size = ( _tsize < _qsize )? _tsize : _qsize;
			if( _size < _alnminsize ){ continue; } 
			
			string _sid = to_string( _oid++ );
			Seg _newseg( _sid, _tspc, _tchr, _tbeg, _tend, _qspc, _qchr, _qbeg, _qend, _dir );
			this->segs.push_back( _newseg );
		}
	} else {
		cerr << "[Error] Cannot open " << _file << endl;
	}

	_openFile.close();
}

void Graph_builder::Build_endTree ( vector<pair<string, int> >& _sortend, int _beg, int _end, End_inTree* _anc ){
	int _mid = int( ( _end + _beg ) / 2 );

	_anc->chr = *(&_sortend[_mid].first);
	_anc->pos = *(&_sortend[_mid].second);

	_anc->left = NULL, _anc->right = NULL;
	if( _beg != _mid ){ 
		End_inTree* _lt_seg = new End_inTree();
		Build_endTree ( _sortend, _beg, _mid - 1, _lt_seg );
		_anc->left = _lt_seg;
	}
	if( _end != _mid ){
		End_inTree* _rt_seg = new End_inTree();
		Build_endTree ( _sortend, _mid + 1, _end, _rt_seg );
		_anc->right = _rt_seg;
	}
}

void Graph_builder::Remove_endTree ( End_inTree* _anc ){
	if( _anc->left != NULL ){
		Remove_endTree( _anc->left );
	}
	if( _anc->right != NULL ){
		Remove_endTree( _anc->right );
	}

	delete _anc;
}

void Graph_builder::Find_overlapEnd ( End_inTree* _anc, Seg& _query, const int _curspc, vector<int>& _overlap ){
	string _qchr = _query.Return_chr( _curspc );
	int _qbeg = _query.Return_beg( _curspc );
	int _qend = _query.Return_end( _curspc );
	
	string _dbchr = _anc->chr;
	int _dbpos = _anc->pos;

	if( _qchr == _dbchr && _qbeg < _dbpos && _dbpos < _qend ){
		_overlap.push_back( _dbpos );
		
		if( _anc->left != nullptr ){
			Find_overlapEnd ( _anc->left, _query, _curspc, _overlap );
		}
	
		if( _anc->right != nullptr ){
			Find_overlapEnd ( _anc->right, _query, _curspc, _overlap );
		}
	} else {	
		bool _small_q = ( _qchr < _dbchr || ( _qchr == _dbchr && _qbeg < _dbpos ) );

		if( _small_q ){
			if( _anc->left != nullptr ){
				Find_overlapEnd ( _anc->left, _query, _curspc, _overlap );
			}
		}else{
			if( _anc->right != nullptr ){
				Find_overlapEnd ( _anc->right, _query, _curspc, _overlap );
			}
		}
	}
}

void Graph_builder::Build_node( Graph& _gr, string _out ){
	int _nid = 0,  _refspc = 0 ;
	sort ( this->segs.begin(), this->segs.end(), [_refspc]( Seg _anode, Seg _bnode ){ return Sort_seg ( _anode, _bnode, _refspc ); } );

	for( int _i = 0; (unsigned int)_i < this->segs.size(); ++_i ){
		if( _i > 0 ){ 
			int _lastnode = _gr.nodes.size() - 1;
			
			char _predir = _gr.nodes[_lastnode].Return_dir(), _curdir = this->segs[_i].Return_dir();
			string _qprechr = _gr.nodes[_lastnode].Return_chr( 0 ), _qcurchr = this->segs[_i].Return_chr( 0 );
			string _tprechr = _gr.nodes[_lastnode].Return_chr( 1 ), _tcurchr = this->segs[_i].Return_chr( 1 );

			int _qprebeg = _gr.nodes[_lastnode].Return_beg( 0 ), _qpreend = _gr.nodes[_lastnode].Return_end( 0 );
			int _qcurbeg = this->segs[_i].Return_beg( 0 ), _qcurend = this->segs[_i].Return_end( 0 );
			int _tprebeg = _gr.nodes[_lastnode].Return_beg( 1 ), _tpreend = _gr.nodes[_lastnode].Return_end( 1 );
			int _tcurbeg = this->segs[_i].Return_beg( 1 ), _tcurend = this->segs[_i].Return_end( 1 );
			
			if( _predir == _curdir && _qprechr == _qcurchr && _tprechr == _tcurchr && ( ( _qprebeg <= _qcurbeg && _qcurend <= _qpreend && _tprebeg <= _tcurbeg && _tcurend <= _tpreend ) || ( _qcurbeg <= _qprebeg && _qpreend <= _qcurend && _tcurbeg <= _tprebeg && _tpreend <= _tcurend ) ) ){
			
				_gr.nodes[_lastnode].Add_member( (Node*)&( this->segs[_i ] ) );

				continue;
			}
		}

		Node _newnode( to_string( _nid ), segs[_i] );
		_nid++;
		_gr.nodes.push_back( _newnode );
	}

	string _file = _out + "/segment.txt";
	ofstream WF ( _file.data() );
	
	for( int _i = 0; (unsigned int)_i < _gr.nodes.size(); ++_i ){
		_gr.nodes[_i].Print_node( WF );
	}

	WF.close();
}

void Graph_builder::Build_link( Graph& _gr, const int _spccnt, string _nodetype, string _out ){
	string path = _out + "/edge_" + _nodetype + ".txt";
	ofstream WF( path.data() );
	
	for( int _curspc = 0; _curspc < _spccnt; ++_curspc ){
		vector<Node> _sorter;
		map<string, Node* > _id2ptr;

		for( int _n = 0; (unsigned int)_n < _gr.nodes.size(); ++_n ){
			if( _gr.nodes[_n].Contain_spc( _curspc ) ){
				_sorter.push_back( _gr.nodes[_n] );
				
				_id2ptr[ _gr.nodes[_n].Return_id() ] = &( _gr.nodes[_n] );
			}
		}

		sort( _sorter.begin(), _sorter.end(), [_curspc]( Node _anode, Node _bnode ){ return Sort_seg ( _anode, _bnode, _curspc ); } ); 
		
		vector<Node*> _procnode, _ongnode, _curnode;
		_curnode.push_back( _id2ptr[_sorter[0].Return_id()] );
		for( int _n = 1; (unsigned int)_n < _sorter.size(); ++_n ){
			Node* _newnode = _id2ptr[_sorter[_n].Return_id()];
			if( ! _newnode->Contain_spc( _curspc ) ){ break; }

			string _nchr = _newnode->Return_chr( _curspc );
			int _nbeg = _newnode->Return_beg( _curspc ), _nend = _newnode->Return_end( _curspc );
			string _nid = _newnode->Return_id();

			string _cchr = _curnode[0]->Return_chr( _curspc );

			if( _cchr != _nchr ){
				_procnode.clear();
				_curnode.clear();
			}else{
				int _cnt_ovl_cnode = 0;

				for( int _c = 0; (unsigned int)_c < _curnode.size(); ){
					Node* _cnode = _curnode[_c];
					int _cbeg = _cnode->Return_beg( _curspc ), _cend = _cnode->Return_end( _curspc );

					if( _nbeg > _cbeg && _nend > _cend ){
						_cnode->Add_link( _curspc, _newnode );
						
						string _cid = _cnode->Return_id();
						WF << _cid << "\t" << _nid << "\t" << _curspc << endl;

						_ongnode.push_back( _cnode );
						_curnode.erase( _curnode.begin() + _c );
					}else{
						_cnt_ovl_cnode++;
						++_c;
					}
				}

				if( _ongnode.size() == 0 ){
					if( _cnt_ovl_cnode != 0 ){
						for( int _p = 0; (unsigned int)_p < _procnode.size(); ++_p ){
							Node* _pnode = _procnode[_p];
							_pnode->Add_link( _curspc, _newnode );
							
							string _pid = _pnode->Return_id();
							WF << _pid << "\t" << _nid << "\t" << _curspc << endl;
						}
					}

				} else {
					for ( int _p = 0; (unsigned int)_p < _procnode.size(); ++_p ){
						Node* _pnode = _procnode[_p];

						int _pbeg = _pnode->Return_beg( _curspc );
						int _pend = _pnode->Return_end( _curspc );

						bool _pass = false;
						for( int _o = 0; (unsigned int)_o < _ongnode.size(); ++_o ){
							Node* _onode = _ongnode[_o];
							int _obeg = _onode->Return_beg( _curspc );
							int _oend = _onode->Return_end( _curspc );

							if( _pbeg < _obeg && _pend < _oend ){
								_pass = true;
								break;
							}
						}

						if(! _pass){
							_pnode->Add_link( _curspc, _newnode );
							
							string _pid = _pnode->Return_id();
							WF << _pid << "\t" << _nid << "\t" << _curspc << endl;
						}

					}
					
					_procnode.clear();
					_procnode = _ongnode;
					_ongnode.clear();
				}
			}
			_curnode.push_back( _newnode );
		}
	}
	WF.close();
}
