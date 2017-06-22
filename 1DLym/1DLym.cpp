// 1DLym.cpp: определяет точку входа для консольного приложения.

//#pragma once
#include "TreeInit.h"
using namespace TreeInit;

int main( ) {	
	long steps_skipped;
	//string protocol_str , SChar , STemp , LString;

	Zadacha Z;
	MultiKnotTreeGroup LKnotTreeGroup;
	MultiKnot MKnot;
	Vetv Branch;
	Uzel Knot;

	LoadGlobals( );
	timeForTrack = 0.;    //для отслеживания времени работы
	dt = 0.1;				// Начальный шаг по времени
	T = 0.;						// Начальное время
	steps_skipped = 0;	// кол-во шагов по времени без записи
	Nz = 0;						// кол-во записанных шагов по времени
	last_write = 0.;
	last_write_dump = 0.; // время последнего сохранения промежуточных расчетов

	//------------------------------ Инициализация задачи --------------------------	
	Z.IdentifyTask( );
	//Z.PrintTask();

	cout << "Start intialization! Z.Ntr = " << Z.Ntr << endl;
	//------------------------ Инициализация сосудистых сетей ----------------------	
	if ( Z.Ntr > 0 ) {
		TreeLst.resize( Z.Ntr );
		for ( long i = 0; i < Z.Ntr; i++ ) {
			TreeLst [ i ].ID = i;
			TreeInitialization( TreeLst [ i ] , Z );
			TreeInitializationTD( TreeLst [ i ] , Z );
			InitDataTD( TreeLst [ i ] , Z );
		}
		LoadMultiKnots( );
	}
	//Call WriteTreeParams(Z)

	//---------------------- Инициализация бронхиального дерева --------------------	
	if ( Z.useLungs == 1 ) {
		BronchTree.ID = BRONCHIAL;
		TreeInitialization( BronchTree , Z );
		TreeInitializationTD( BronchTree , Z );
		InitDataTD( BronchTree , Z );
	}

	//------------------------- Инициализация модели сердца ------------------------	
	// !!! InitHeart( );
	//--------------------------- Цикл по времени ----------------------------------
	cout  << "Start calculations" << endl;
//#define SOL
//#ifdef SOL
	cout << "T = " << T << " , Time_calc = " << time_calc << " , dt = " << dt << endl;;
	while ( ( T < time_calc ) && ( dt > 0. ) ) {
		Smax = 0.; //Globals::Smax = 0.0;
		T += dt;
		//CalcEdges( Z );
		//CalcVesselVertexes( Z );
		//CalcBronchTree( Z );
		//CalcMatterTransfer( Z );
		//dt = GetDtAboutSMax( Z , Smax ); //Находится рекомендованный правилом Куранта шаг

		//------------------------------ Запись результатов ----------------------------
		steps_skipped = steps_skipped + 1;  // Количество циклов с момента прошлой записи
		if ( TestWriteData( Z , steps_skipped ) ) {
			TDWrite( Z );
			steps_skipped = 0;
		}
		//if ( TestToBreak( Z ) == 0 ) break;
		if ( ( T > last_write_dump + write_dump ) && ( Z.useSaveDump == 1 ) ) {
			for ( long i = 0; i < Z.Ntr; i++ ) {
				SaveDump( TreeLst [ i ] , Z );
			}
			last_write_dump += write_dump;
		}
		cout << "T = " << T << endl;
	} 
//#endif
	//----------converting results for JCNetwork-------------------
#ifdef JCNetwork
	for ( long i = 0; i < Z.Ntr; i++ ) {
		ConvertResults( TreeLst [ i ] , Z );
	}
#endif
	cout << "WORKS!!!" << endl;
	//getchar( );
}

