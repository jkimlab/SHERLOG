#include <iostream>
#include <string>
#include <cstdlib>
#include <dirent.h>

using namespace std;

#include "util.h"
#include "buildgraph.h"
#include "buildsynteny.h"

int main ( int argc, char* argv[] ){

	if ( argc < 2 ){
		cerr << "You must specify a proper config file." << endl;
		cerr << "See the online documentation at http://github.com/jkimlab/SHERLOG for the up-to-date information." << endl;
		cerr << "\nusage: sherlog [config]\n" << endl;
		return -1;
	}
	
	Util utils;

	Graph aln_gr;
	Graph sf_gr, sf_gr2;
	Graph_builder builder;

	cerr << "[1] Read config file" << endl;
	string config_f = argv[1];
	utils.Set_param ( config_f );

	string cmd_create_dir = "mkdir -p " + utils.Return_outdir();
	system( cmd_create_dir.c_str() );

	cerr << "\n[2] Read alignnment file(s)" << endl;
	int spccnt = utils.Return_spccnt();
	int aln_min_size = utils.Return_alnminsize();
	cerr << "    - Minimum size of alignment: " << aln_min_size << endl;
	
	map <pair<string, string>, string> inpfile = utils.Return_ipfle();
	for ( map<pair<string, string>, string>::iterator mit = inpfile.begin(); mit != inpfile.end(); ++mit ){
		string inp_file = mit->first.first;
		string spc_comb_str = mit->first.second;
		string inp_format = mit->second;
		
		vector<string> spc_comb;
		Split_by_space( spc_comb_str, spc_comb );
		
		if( spc_comb[0] == spc_comb[1] ){
			cerr << "\n[Error] Name of query and target genome is same.\n" << endl;
			return -1;
		}

		if( inp_format == "chain" ){
			cerr << "    - Read chain file: " << inp_file << endl;
			builder.Read_chain ( inp_file, atoi( spc_comb[0].c_str() ), atoi( spc_comb[1].c_str() ), aln_min_size );
		} else if ( inp_format == "delta" ){
			cerr << "    - Read delta file: " << inp_file << endl;
			builder.Read_delta ( inp_file, atoi( spc_comb[0].c_str() ), atoi( spc_comb[1].c_str() ), aln_min_size );
		} else if ( inp_format == "paf" ){
			cerr << "    - Read paf file: " << inp_file << endl;
			builder.Read_paf ( inp_file, atoi( spc_comb[0].c_str() ), atoi( spc_comb[1].c_str() ), aln_min_size );
		} else {
			cerr << "[Error] Unknown format " << inp_format << endl;
			cerr << "\tCannot parse " << inp_file << endl;
		}
	}

	if(! builder.Return_segcnt()){
		cerr << "\n[Error] Cannot find any proper alignment information." << endl;
		return 1;
	}
	
	cerr << "\n[3] Merging Pairwise alignment for multiple alignment" << endl;
	
	cerr << "\n[4] Building graph" << endl;
	cerr << "    - Build node" << endl;
	builder.Build_node ( aln_gr, utils.Return_outdir() );
	cerr << "    - Build edge" << endl;
	builder.Build_link ( aln_gr, spccnt, "seg", utils.Return_outdir() );

	Synteny_builder traveler;
	cerr << "\n[5] Find conserved links" << endl;
	traveler.Find_conserved_link ( aln_gr, "seg", utils.Return_outdir() );
	
	cerr << "\n[6] Construct synteny in hierarchy structure" << endl;
	cerr << "    - Construct small-scale synteny" << endl;
	traveler.Build_synteny ( aln_gr, sf_gr, 2, aln_min_size, utils );
	builder.Build_link ( sf_gr, spccnt, "seg", utils.Return_outdir() );

	cerr << "    - Construct large-scale synteny" << endl;
	traveler.Find_conserved_link ( sf_gr, "sf_lv2", utils.Return_outdir() );
	traveler.Build_synteny ( sf_gr, sf_gr2, 1, aln_min_size, utils );
	
	return 1;
}
