#include "TaskData.h"

namespace TDVesselWrite {
	using namespace TaskData;
	
	inline int TestWriteData ( Zadacha& Z, long skip) {
		if (skip * Globals::dt >= Globals::write_delta) return 1;
		return 0;
	}
	void TDWriteCurrentTime(string filename) {
		ofstream fout;
		fout.open (filename, ofstream::app);
		fout << Globals::T << endl;
		fout.close();
	}
	void TDWriteCurrentTimeSteps(string filename) {
		ofstream fout;
		fout.open (filename, ofstream::out);
		fout << Globals::Nz << endl;
		fout.close();
	}
//------- Функции записи в файл задачно-зависимых переменных -------------------------
	void WriteResultLymph ( Derevo& Tr , Zadacha& Z ) { 
		double q;
		ofstream fout;
		for ( int i = 0; i < Tr.Nbr; ++i ) {
			fout.open (Tr.B[i].TD.fnameTD[0], ofstream::app); 
			for (int k = 0; k < Tr.B[i].pts; ++k) {
				fout << Tr.B[i].VB[0][k] * Tr.B[i].VB[1][k] << " ";
			}
			fout << endl;
			fout.close();
		}
	}
	void WriteResultBronch ( Derevo& Tr , Zadacha& Z ) { }
	void WriteResultArtery ( Derevo& Tr , Zadacha& Z ) { }
	void WriteResultVein ( Derevo& Tr , Zadacha& Z ) { }
	
	void WriteResultTD ( Derevo& Tr , Zadacha& Z ) { 
		if ( Tr.ID == LYMPHATIC ) WriteResultLymph(Tr, Z);
		if ( ( Tr.ID == PULMART ) || ( Tr.ID == SYSART ) ) WriteResultArtery(Tr, Z);
		if ( ( Tr.ID == PULMVEN ) || ( Tr.ID == SYSVEN ) ) WriteResultVein(Tr, Z);
		if ( Tr.ID == BRONCHIAL ) WriteResultBronch(Tr, Z);
	}

//-------Подпрограмма записи в файл переменных s,u,p и прочих------------------------------
// может работать долго, потому что вычисляет давление 
	void WriteResultV(Derevo& Tr , Zadacha& Z) {
		double p;
		ofstream fout;
		int cur_track = 0; // если пишутся не все ветви
		for ( int i = 0; i < Tr.Nbr; ++i ) {
			if (Z.N_towrite) {
				if ( i == Z.Num_towrite[cur_track] ) {
					cur_track++;
				} else {
					continue;
				}
			}
			for ( int j = 0; j < Z.Cor; ++j ) { 				// s, u
				fout.open (Tr.B[i].TD.fnameVar[j], ofstream::app); 
				for (int k = 0; k < Tr.B[i].pts; ++k) {
					fout << Tr.B[i].VB[j][k] << " ";
				}
				fout << endl;
				fout.close();
			}
			fout.open (Tr.B[i].TD.fnameVar[Z.Cor], ofstream::app);
			for (int k = 0; k < Tr.B[i].pts; ++k) { 			// p
				p = TDGlobals::LymPofS (Tr.B[i].VB[0][k], Tr.B[i].TD.S0, k*Tr.B[i].dx, Globals::T); //LymPofS( S, S0,  x,  t )
				fout << p << " ";
			}
			fout << endl;
			fout.close();
		}
		WriteResultTD(Tr , Z);
		TDGlobals::isFirstTime = 0;
	}
	
	void WriteBranchResultForGNUPLOT (Derevo& Tr , Zadacha& Z) {}
	
	void TDWrite(Zadacha& Z) {
	//-------------------------- MATLAB output ---------------------------------
		if ( Globals::use_MATLAB_out ) {
			for ( int i = 0; i < Z.Ntr; ++i ) {
				WriteResultV(TreeLst[i], Z);
			}
			if ( Z.useLungs ) {
				WriteResultV(BronchTree, Z);
			}
			TDWriteCurrentTimeSteps(trim(TDGlobals::root_out) + "tsteps.tres");
			TDWriteCurrentTime(trim(TDGlobals::root_out) + "time.tres");
		}
	//-------------------------- GNUPLOT output ---------------------------------
		if ( Globals::use_GNUPLOT_out ) {
			for ( int i = 0; i < Z.Ntr; ++i ) {
				WriteBranchResultForGNUPLOT (TreeLst[i] , Z );
			}
			if ( Z.useLungs ) {
				WriteBranchResultForGNUPLOT ( BronchTree, Z );
			}
		}
	}
//===========================================================================================
//  сохранение текущих значений для использования в дальнейших вычислениях
//===========================================================================================
	void SaveDump(Derevo& Tr , Zadacha& Z) {
		string fnDump;
		int LResult;

		LResult = MAKEDIRQQ( trim( SharedDirectory ) + trim(Tr.dirname) );
		LResult = MAKEDIRQQ( trim( SharedDirectory ) + trim(Tr.dirname) + slash + "result" );
		fnDump = trim(SharedDirectory) + trim(Tr.dirname) + slash + "result" + slash + "tree" + trim( Achar( Ichar( "0" ) + Tr.ID ) )  +  ".dmp";
		cout << "Dump file : " << fnDump << endl;
		ofstream fout( fnDump , ofstream::out );

		fout <<  Tr.ID << endl;
		for ( int i = 0; i < Tr.Nbr; ++i ) { 
			fout <<  Tr.B[ i ].ID << " " <<  Tr.B[ i ].pts << endl;;		// номер ветви, количество точек разбиения
			for ( int j = 0; j < Z.Cor; ++j ) {
				for (int k = 0; k < Tr.B[i].pts; ++k) {
					fout << Tr.B[i].VB[j][k] << " ";
				}
				fout << endl;
			}
			for ( int j = 0; j < Z.Cor; ++j ) {
				for (int k = 0; k < Tr.B[i].pts; ++k) {
					fout << Tr.B[i].VBO[j][k] << " ";
				}
				fout << endl;
			}
		}
		fout.close();
	}
	
	void ConvertResults(Derevo& Tr , Zadacha& Z) {}
};