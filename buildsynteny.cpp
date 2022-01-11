#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>

using namespace std;
#define CONS -1

#include "buildsynteny.h"

void Synteny_builder::Find_conserved_link ( Graph& _gr, string _grtype, string _out ){
	string _file = _out + "/edge_" + _grtype + ".conserve.txt";
	ofstream WF ( _file.data() );

	for( int _n = 0; _n < _gr.nodes.size(); ++_n ){
		Node* _cnode = &( _gr.nodes[_n] );
		char _dir = _cnode->Return_dir();
		
		vector<Node*> _qlinks = _cnode->Return_links( 0 );
		vector<Node*> _tlinks = _cnode->Return_links( 1 );
		
		for( int _l = 0; _l < _qlinks.size(); ++_l ){
			if( _dir != _qlinks[_l]->Return_dir() ){ continue; }

			if( _dir == '+' ){
				string _qid = _qlinks[_l]->Return_id();
				if( find_if( _tlinks.begin(), _tlinks.end(), [_qid] ( Node* _tlink ){ return _tlink->Return_id() == _qid; } ) != _tlinks.end() ){
					_cnode->Add_link( CONS, _qlinks[_l] ); 
					WF << _cnode->Return_id() << "\t" << _qlinks[_l]->Return_id() << " 0" << endl;
					WF << _cnode->Return_id() << "\t" << _qlinks[_l]->Return_id() << " 1" << endl;
				}
			} else {
				string _cid = _cnode->Return_id();

				vector<Node*> _tlinks_nxt = _qlinks[_l]->Return_links( 1 );
				if( find_if( _tlinks_nxt.begin(), _tlinks_nxt.end(), [_cid] ( Node* _tlink ){ return _tlink->Return_id() == _cid; } ) != _tlinks_nxt.end() ){
					_qlinks[_l]->Add_link( CONS, _cnode ); 
					WF << _cnode->Return_id() << "\t" << _qlinks[_l]->Return_id() << " 0" << endl;
					WF << _qlinks[_l]->Return_id() << "\t" << _cnode->Return_id() << " 1" << endl;
				}
			}
		}
	}

	WF.close();
}

vector<vector<Node*> > Synteny_builder::Connect_links ( Node* _cnode ){
	vector<vector<Node*> > _allpaths;
	
	vector<Node*> _conserved_links = _cnode->Return_links( CONS );
	for( int _n = 0; _n < _conserved_links.size(); ++_n ){
		if( synteny.find( _conserved_links[_n]->Return_id() ) != synteny.end() ){
			_allpaths.insert( _allpaths.end(), synteny[ _conserved_links[_n]->Return_id() ].begin(),  synteny[ _conserved_links[_n]->Return_id() ].end() );
			synteny.erase( _conserved_links[_n]->Return_id() );
		}else{
			_conserved_links[_n]->Update_process( true );
			vector<vector<Node*> > _nxt = Connect_links( _conserved_links[_n] );
			_allpaths.insert( _allpaths.end(), _nxt.begin(), _nxt.end() );
		}
	}
	
	if( _allpaths.size() == 0 ){
		vector<Node*> _newpath { _cnode };
		_allpaths.push_back( _newpath );
	} else {
		for( int _p = 0; _p < _allpaths.size(); ++_p ){
			_allpaths[_p].insert( _allpaths[_p].begin(), _cnode );
		}
	}
	return _allpaths;
}

void Synteny_builder::Build_synteny ( Graph& _igr, Graph& _ogr, int _level , int _alnminsize, Util& _util ){
	string _out = _util.Return_outdir();
	synteny.clear();

	for( int _n = 0; _n < _igr.nodes.size(); ++_n ){
		Node* _cnode = &( _igr.nodes[_n] );

		if( ! _cnode->Return_proc() ){
			vector<vector<Node*> > _paths = Connect_links ( _cnode );
			synteny[ _cnode->Return_id() ] = _paths;
		}
	}
	
	int _sid = 0;
	map<string, vector<vector<Node*> > >::iterator mit;
	for( mit = synteny.begin(); mit != synteny.end(); ++mit ){
		vector<vector<Node*> > _paths = mit->second;

		for( int _i = 0; _i < _paths.size(); ++_i ){
			Node* _member = _paths[_i][0] ;
			Node* _newsf = new Node ( to_string( ++_sid ), *_member );

			for( int _j = 1; _j < _paths[_i].size(); ++_j ){
				_member = _paths[_i][_j];
				_newsf->Add_member( _member );
			}
			
			if( _newsf->Return_length() >= _alnminsize ){
				_ogr.nodes.push_back( *(_newsf) );
			} else {
				delete _newsf;
			}
		}
	}
	
	if( _level == 1 ){
		string _file = _out + "/large_scale_synteny.txt";
		string _subfile = _out + "/small_scale_synteny.txt";
		int _newid = 1 ;

		ofstream WF ( _file.data() );
		ofstream WSF ( _subfile.data() );
		int _refspc = 0;
		sort( _ogr.nodes.begin(), _ogr.nodes.end(), [_refspc]( Node& _anode, Node& _bnode ){ return Sort_seg ( _anode, _bnode, _refspc ); } );
		for( int _n = 0; _n < _ogr.nodes.size(); ++_n ){
			_ogr.nodes[_n].Update_id( to_string( _newid++ ) );
			_ogr.nodes[_n].Print_seg( WF, _util );

			int _mid = 1;
			vector<Node*> _newsf_member = _ogr.nodes[_n].Return_member();
			string _newsf_id = _ogr.nodes[_n].Return_id();
			for( int _m = 0; _m < _newsf_member.size(); ++_m ){
				WF << "\t";
				_newsf_member[_m]->Update_id( _newsf_id + "." + to_string( _mid++ ) );
				_newsf_member[_m]->Print_seg( WF, _util );
				_newsf_member[_m]->Print_seg( WSF, _util );

				vector<Node*> _subsf_member = _newsf_member[_m]->Return_member();
				for( int _sm = 0; _sm < _subsf_member.size(); ++_sm ){
					WSF << "\t";
					_subsf_member[_sm]->Print_seg( WSF, _util );
				}
				WSF << endl;
			}
			WF << endl;
		}
		WF.close();
	}
}

void Synteny_builder::Update_link ( Graph& _gr, int _spccnt, string _out ){
	map< string, string > _idmap;
	map< string, Node* > _id2ptr;

	for( int _n = 0; _n < _gr.nodes.size(); ++_n ){
		string _nodeid = _gr.nodes[_n].Return_id();
		_id2ptr[_nodeid] = &( _gr.nodes[_n] );

		vector<Node*> _nodemember = _gr.nodes[_n].Return_member();

		for( int _m = 0; _m < _nodemember.size(); ++_m ){
			string _memberid = _nodemember[_m]->Return_id();
			_idmap[_memberid] = _nodeid;
		}
	}
	
	string _file = _out + "/edge_sf_lv2.txt";
	ofstream WF ( _file.data() );

	for( int _n = 0; _n < _gr.nodes.size(); ++_n ){
		string _nodeid = _gr.nodes[_n].Return_id();
		vector<Node*> _nodemember = _gr.nodes[_n].Return_member();
		
		for( int _m = 0; _m < _nodemember.size(); ++_m ){
			string _memberid = _nodemember[_m]->Return_id();
			for( int _curspc = 0; _curspc < _spccnt; ++_curspc ){
				vector<Node*> _memberlink = _nodemember[_m]->Return_links( _curspc );
				for(int _l = 0; _l < _memberlink.size(); ++_l ){
					string _linkid = _memberlink[_l]->Return_id();
					string _updatelinkid = _idmap[_linkid];
					
					vector<Node*> _curlinks = _gr.nodes[_n].Return_links( _curspc );
					if( _nodeid != _updatelinkid && find_if( _curlinks.begin(), _curlinks.end(), [_updatelinkid] ( Node* _curlink ){ return _curlink->Return_id() == _updatelinkid; } ) == _curlinks.end() ){
						_gr.nodes[_n].Add_link( _curspc, _id2ptr[_updatelinkid] );
						
						WF << _nodeid << "(" << _memberid << ")\t" << _updatelinkid << "(" << _linkid << ")\t" << _curspc << endl;
					}
				}
			}
		}
	}

	WF.close();
}

void Synteny_builder::Find_loop ( Node* _curnode, vector<Node*>& _curpath, map<pair<Node*, Node*>, int>& _loop ){
	_curnode->Update_process( true );

	vector<Node*> _links = _curnode->Return_links( CONS );
	for( int _l = 0; _l < _links.size(); ++_l ){
		bool _alreadyused = false;
		string _nxtnodeid = _links[_l]->Return_id();
	
		for( int _p = 0; _p < _curpath.size(); ++_p ){
			if( _curpath[_p]->Return_id() == _nxtnodeid ){
				_loop[make_pair( _curnode,  _links[_l] )] = 1;
				_alreadyused = true;	
				break;
			}
		}

		if( ! _alreadyused ){
			_curpath.push_back( _curnode );
			Find_loop( _links[_l], _curpath, _loop );
		}
	}
}

void Synteny_builder::Remove_loop ( Graph& _gr ){
	map<pair<Node*, Node*>, int> _loop;

	for( int _n = 0; _n < _gr.nodes.size(); ++_n ){
		Node* _cnode = &( _gr.nodes[_n] );
		if( ! _cnode->Return_proc() ){
			vector<Node*> _newpath;
			Find_loop( _cnode, _newpath, _loop );
		}
	}

	for( map<pair<Node*, Node*>, int>::iterator _mit = _loop.begin(); _mit != _loop.end(); ++_mit ){
		_mit->first.first->Remove_link( CONS, _mit->first.second );
		_mit->first.second->Remove_link( CONS, _mit->first.first );
	}

	for( int _n = 0; _n < _gr.nodes.size(); ++_n ){
		_gr.nodes[_n].Update_process( false );
	}
}

