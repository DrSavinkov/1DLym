//#include "Globals.cpp"
//#include "TDGlobals.cpp"
//#include "TreeDataIndepend.cpp"
//#include "TreeDataDepend.cpp"
//#include "TaskData.h"
#include "TDVesselWrite.h"
#include <string>

namespace TreeInitTD {

	//using namespace std;
	//using namespace Globals;
	//using namespace TreeDataIndepend;
	//using namespace TreeDataDepend;
	using namespace TDVesselWrite;

	vector<string> SetExtraName(long TreeID) {
		if (TreeID == LYMPHATIC) {
			return {"q"};  //{"q", "Qave", "Qmax"};	
		} else {
			return { "Pmax" , "Pave" , "Qave" , "MDP" , "MSP" , "EE" };
		}	
	}
	//«агрузка задачно-зависимых параметров
	inline void TDLoadExternalConnections( Zadacha& Z , Derevo& Tr );
	void TDLoadGasExchKnots(Zadacha& Z, Derevo& Tr);

	inline void TDLoadBranch( Zadacha& Z , Derevo& Tr , Vetv& br , ifstream& fin ) {

		long brID;
		// added by Rufina
		if ( br.myTreeID == LYMPHATIC ) {
			//read parameters 
			return;
		}

		fin >> brID;
		fin >> br.TD.c;
		fin >> br.TD.R;
		fin >> br.TD.Ps;
		fin >> br.TD.Smin;
		fin >> br.TD.Smax;
		fin >> br.TD.d_start;
		br.TD.Ps = 0.0;

		// вычисление абсолютных значений
		br.TD.Smin = br.TD.Smin * PI * br.width * br.width / 4.;
		br.TD.Smax = br.TD.Smax * PI * br.width * br.width / 4.;
		br.TD.S0 = PI * br.width * br.width / 4.;
		br.TD.Par = 0.;
		br.TD.Pgr = 0.;
		br.TD.delta = 0.;

		br.TD.conc.resize( Z.Nmatter );
		br.TD.conc_old.resize( Z.Nmatter );
		for ( long i = 0; i < Z.Nmatter; i++ ) {
			br.TD.conc [ i ].resize( br.pts );
			br.TD.conc_old [ i ].resize( br.pts );
			for ( long j = 0; j < br.pts; j++ ) {
				br.TD.conc [ i ] [ j ] = 0.0;
			}
		}
		//  онцентрации O2 и CO2
		if ( Z.Nmatter == 2 ) {
			if ( br.myTreeID == PULMVEN || br.myTreeID == SYSART ) {
				for ( long j = 0; j < br.TD.conc [ 0 ].size( ); j++ ) {
					br.TD.conc [ 0 ] [ j ] = 0.2; // кислород
					br.TD.conc [ 1 ] [ j ] = 0.48; // углекислый газ
				}
			}
			if ( br.myTreeID == PULMART || br.myTreeID == SYSVEN ) {
				for ( long j = 0; j < br.TD.conc [ 0 ].size( ); j++ ) {
					br.TD.conc [ 0 ] [ j ] = 0.15; // кислород
					br.TD.conc [ 1 ] [ j ] = 0.52; // углекислый газ
				}
			}
			if ( br.myTreeID == BRONCHIAL ) {
				for ( long j = 0; j < br.TD.conc [ 0 ].size( ); j++ ) {
					br.TD.conc [ 0 ] [ j ] = 0.209; // кислород
					br.TD.conc [ 1 ] [ j ] = 3.e-4; // углекислый газ
				}
			}
		}
		br.TD.conc_old = br.TD.conc;

		br.TD.Qave_curr = 0.;
		br.TD.Rave_curr = 0.;
		br.TD.Pave_curr = 0.;
		br.TD.RadiusAve_curr = 0.;
		br.TD.Qave_prev = 0.;
		br.TD.Rave_prev = 0.;
		br.TD.Pave_prev = 0.;
		br.TD.RadiusAve_prev = 0.;

	}
	
	inline void TreeInitializationTD( Derevo& Tr , Zadacha& Z ) {
		long i , j , k , idx;
		string fname_base;
		vector<string> var_name , extra_name;
		string str_idx;
		int LResult;

		//-------------------- «агрузка соединений с узлами других сетей и деревьев ------------------
		TDLoadExternalConnections( Z , Tr );
		if ( Tr.ID == BRONCHIAL ) {
			TDLoadGasExchKnots( Z , Tr );
		}
		//-------------------------------- «агрузка параметров ребер --------------------------------
		ifstream fin( Tr.TDbranchfilename , ifstream::in );
		if ( fin.good() ) {
			for ( long i = 0; i < Tr.Nbr; i++ ) {
				TDLoadBranch( Z , Tr , Tr.B [ i ] , fin );
			}
		}
		fin.close( );
		//-------------------------- «агрузка параметров внешних воздействий -------------------------
		fin.open( Tr.TDImpactfilename , ifstream::in );
		if ( fin.good() ) {
			fin >> Tr.Nimp;	// количество внешних воздействий
			Tr.I.resize( Tr.Nimp );
			for ( i = 0; i < Tr.Nimp; i++ ) {
				fin >> Tr.I [ i ].BrID;
				fin >> Tr.I [ i ].IType;
				fin >> Tr.I [ i ].GType;
				fin >> Tr.I [ i ].len;
				fin >> Tr.I [ i ].SubstID;
				fin >> Tr.I [ i ].alfa;
				fin >> Tr.I [ i ].Tstart;
				fin >> Tr.I [ i ].Tend;
			}
		}
		fin.close( );
		
		//------------------------- ѕодготовка файлов дл€ записи результатов -------------------------
		var_name = {"s", "u", "p"};
		extra_name = SetExtraName(Tr.ID);
		fname_base = trim( TDGlobals::root_out ) + trim( Tr.dirname ) + slash + "result" + slash;
		cout << "NAMEBASE " << fname_base << endl;
		LResult = system( ( string( "mkdir " ) + trim( TDGlobals::root_out ) + trim( Tr.dirname ) ).c_str( ) );
		LResult = system( ( string( "mkdir " ) + fname_base ).c_str( ) );
		if ( Z.Nmatter > 0 ) LResult = system( ( "mkdir " + trim( fname_base ) + "Concentration" ).c_str( ) );
		for (int i = 0; i < var_name.size(); ++i) {
			var_name[i] =  trim( fname_base ) + var_name[i] + slash;
			//cout << var_name[i] << " ";
			LResult = system( ( "mkdir " + var_name[i] ).c_str( ) );
		}
		for (int i = 0; i < extra_name.size(); ++i) {
			extra_name[i] =  trim( fname_base ) + extra_name[i] + slash;
			//cout << extra_name[i] << " ";
			LResult = system( ( "mkdir " + extra_name[i] ).c_str( ) );
		}

		for ( long i = 0; i < Tr.Nbr; i++ ) {
			str_idx = "";
			k = Tr.B [ i ].ID;
			for (int i = 0; i < BRANCH_ORDER; ++i) {
				str_idx = Achar( Ichar( "0" )  + k % 10) + str_idx ;
				k /= 10;
			}
			Tr.B [ i ].TD.strID = trim( str_idx );
			if ( Z.Nmatter > 0 ) {
				Tr.B [ i ].TD.fnameC.resize( Z.Nmatter );
				for ( long j = 0; j < Z.Nmatter; j++ ) {
					Tr.B [ i ].TD.fnameC [ j ] = trim( fname_base ) + "c" + Achar( Ichar( "0" ) + j ) + "_" + trim( str_idx ) + ".tres";;
				}
			}
			Tr.B [ i ].TD.fnameVar.resize(var_name.size());
			for (int j = 0; j < var_name.size(); ++j) { 
				Tr.B [ i ].TD.fnameVar[j] = var_name[j] + trim( str_idx ) + ".tres";
				if ( use_MATLAB_out ) {
					ofstream fout( Tr.B [ i ].TD.fnameVar[j] , ofstream::out );
					fout << Tr.B [ i ].pts << endl;
					fout.close( );
				}
				if ( use_GNUPLOT_out ) { }
			}
			Tr.B [ i ].TD.fnameTD.resize(extra_name.size());
			for (int j = 0; j < extra_name.size(); ++j) { 
				Tr.B [ i ].TD.fnameTD[j] = extra_name[j] + trim( str_idx ) + ".tres";
				if ( use_MATLAB_out ) {
					ofstream fout( Tr.B [ i ].TD.fnameTD[j] , ofstream::out );
					fout << Tr.B [ i ].pts << endl;
					fout.close( );
				}
				if ( use_GNUPLOT_out ) { }
			}
		}
	}

//=====================================================================//
	inline void TDLoadExternalConnections( Zadacha& Z , Derevo& Tr ) {
		long j , TextID , kid , Knum;
		// Default values
		for ( long i = 0; i < Tr.Nkn; i++ ) {
			Tr.K [ i ].TD.TextID = 0;
			Tr.K [ i ].TD.Kext_sz = 0;
		}
		if ( Tr.ID == LYMPHATIC ) {
			return;
		}

		ifstream fin( Tr.TDknotExternalFilename , ifstream::in );
		cout << Tr.TDknotExternalFilename << endl;
		fin >> TextID;
		if ( TextID > 0 ) {
			fin >> Knum;										// количество внешних соединений
			for ( long i = 0; i < Knum; i++ ) {
				fin >> kid;										// номер узла в текущей сети
				fin >> Tr.K [ kid ].TD.Kext_sz;	// количество внешних узлов соединенных с данным узлом
				Tr.K [ kid ].TD.TextID = TextID;		// ID сети из которой берутс€ ¬—≈ внешние узлы
				Tr.K [ kid ].TD.KextID.resize( Tr.K [ kid ].TD.Kext_sz );
				for ( j = 0; j < Tr.K [ kid ].TD.Kext_sz; j++ ) {
					fin >> Tr.K [ kid ].TD.KextID [ j ];
				}
			}
		}
		fin.close( );
	}
	
	void TDLoadGasExchKnots(Zadacha& Z, Derevo& Tr){
		long i, j, N, kid;
		ifstream fin( trim("treeBronch/tdgasexchangeknots.tre") , ifstream::in );
			fin >> N;
			for ( i = 0; i < N; ++i ) {
				fin >> kid;
				fin >> Tr.K[kid].TD.NgasExch;
				Tr.K[kid].TD.gasExchKnt.resize(Tr.K[kid].TD.NgasExch);
				for ( j = 0; j < Tr.K[kid].TD.NgasExch; ++j ) {
					fin >> Tr.K [ kid ].TD.gasExchKnt [ j ];
				}
			}
		fin.close( );
	}

	inline void InitWithDefaults( Derevo& Tr , Zadacha& Z ) {
		for ( long i = 0; i < Tr.Nbr; i++ ) {
			for ( long j = 0; j < Tr.B [ i ].VB [ 0 ].size( ); j++ ) {
				Tr.B [ i ].VB [ 0 ][ j ] = PI * Tr.B [ i ].width*Tr.B [ i ].width / 4.;	// S = S0
				Tr.B [ i ].VB [ 1 ][ j ] = 0.0;
			}
			Tr.B [ i ].VBO = Tr.B [ i ].VB;
		}
	}

	inline void InitDataTD( Derevo& Tr , Zadacha& Z ) {
		using namespace Globals;
		using namespace TDGlobals;

		string fname;
		long id , npt , isGood , ioerr;

		isGood = Z.useDump ? 1 : 0;

		if ( isGood ) {
			fname = trim( Tr.dirname ) + "/result/dump" + Achar( Ichar( "0" ) + Tr.ID ) + ".dmp";
			ifstream fin( fname , ifstream::in );
			ioerr = fin.is_open( );

			if ( ioerr ) {
				isGood = 0;
			}
			else {
				fin >> id;
			}

			if ( id == Tr.ID && isGood ) {		// проверка номера дерева
				for ( long i = 0; i < Tr.Nbr; i++ ) {
					fin >> id;															// номер ветви
					if ( id == Tr.B [ i ].ID && isGood ) {
						fin >> npt;														// количество точек разбиени€
						if ( npt == Tr.B [ i ].pts && isGood ) {
							for ( long it1 = 0; it1 < Tr.B [ i ].VB.size( ); it1++ ) {
								for ( long it2 = 0; it2 < Tr.B [ i ].VB [ it1 ].size( ); it2++ ) {
									fin >> Tr.B [ i ].VB [ it1 ] [ it2 ];
								}
							}
							for ( long it1 = 0; it1 < Tr.B [ i ].VBO.size( ); it1++ ) {
								for ( long it2 = 0; it2 < Tr.B [ i ].VBO [ it1 ].size( ); it2++ ) {
									fin >> Tr.B [ i ].VBO [ it1 ] [ it2 ];
								}
							}
						}
						else {
							isGood = 0;
							cout << "Warning! Inconsistent dump data. Initializing with defaults values." << endl;
							break;
						}
					}
					else {
						isGood = 0;
						cout << "Warning! Inconsistent dump data. Initializing with defaults values." << endl;
						break;
					}
				}
			}
			fin.close( );
		}
		if ( isGood == 0 ) { // Default initialization
			InitWithDefaults( Tr , Z );
		}
		return;
	}

};