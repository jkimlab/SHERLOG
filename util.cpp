#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <map>

using namespace std;

#include "util.h"

void Util::Set_param ( const string _f_config ){
	string _curline;
	smatch _name;
	regex _reg ( "^>(\\w+)" );
	regex _reg_fle ( "^(\\S+),(\\S+)\\s+(\\w+)\\s+(\\S+)" );

	ifstream _openfile ( _f_config.data() );

	if( _openfile.is_open() ){
		int _cur_spc_id = 0;
		while ( getline ( _openfile, _curline )){
			if ( regex_search ( _curline, _name, _reg )){
				if ( _name[1] == "outdir" ){
					getline ( _openfile, this->out_dir );
				} else if ( _name[1] == "resolution" ){
					string _resol;
					getline ( _openfile, _resol );
					this->aln_min_size = atoi( _resol.c_str() );
				}
			} else if ( regex_search ( _curline, _name, _reg_fle )){
				if( spc_id.find( _name[1] ) == spc_id.end() ){
					this->spc_id[_name[1]] = _cur_spc_id++;
				}
				if( spc_id.find( _name[2] ) == spc_id.end() ){
					this->spc_id[_name[2]] = _cur_spc_id++;
				}

				string _spc1 = to_string( this->spc_id[_name[1]] ), _spc2 = to_string( this->spc_id[_name[2]] );
				this->ip_fle[ make_pair( _name[4], _spc1 + " " + _spc2 ) ] = _name[3];
			}
		}
	} else {
		cerr << "[Error] Cannot open " << _f_config << endl;
		exit(0);
	}
}

string Util::Return_spcname ( const int _id ){
	for(map<string, int>::iterator _it = this->spc_id.begin(); _it != this->spc_id.end(); ++_it){
		if( _it->second == _id ) return _it->first;
	}

	return "Unknown";
}

void Split_by_space ( const string& _str, vector<string>& _splitstr ){
	stringstream _sst;
	_sst.str(_str);

	string _token;
	while ( _sst >> _token ){
		_splitstr.push_back ( _token );
	}
}
