#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>
#include <map>

class Util {
	int aln_min_size;
	map <pair<string, string>, string> ip_fle;
	map <string, int> spc_id;
	string chain_fle, out_dir;
	
  public:
	Util(){ aln_min_size = 1000; }

	void Set_param ( const string _f_config );
	
	int Return_spccnt (){ return spc_id.size(); }	
	string Return_spcname ( const int _id );		

	int Return_alnminsize (){ return aln_min_size; }
	map <pair<string, string>, string> Return_ipfle (){ return ip_fle; }
	string Return_outdir (){ return out_dir; }
};
	
void Split_by_space (const string& _str, vector<string>& _splitstr);

#endif
