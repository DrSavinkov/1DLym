#include "TreeInitTD.h"

namespace TreeInit {
	using namespace TreeInitTD;
	
	inline void TreeInitialization( Derevo& Tr , Zadacha& Z ) {
		Uzel *knt1 , *knt2;
		string NID;
		long i , ID , KnID1 , KnID2;

		{
			char wrotmnenogi [ 10 ];
			sprintf( wrotmnenogi , "%ld" , Tr.ID );
			NID = string( wrotmnenogi );
		}
		if ( Tr.ID == BRONCHIAL ) {
			NID = "bronch";
			//InitAlveolar();
		}
		// trim == long to string // 
		Tr.dirname = "tree" + trim( Adjustl( NID ) );
		Tr.treefilename = trim( SharedDirectory ) + "tree" + trim( Adjustl( NID ) ) + slash + "tree.tre";
		Tr.knotfilename = trim( SharedDirectory ) + "tree" + trim( Adjustl( NID ) ) + slash + "knot.tre";
		Tr.branchfilename = trim( SharedDirectory ) + "tree" + trim( Adjustl( NID ) ) + slash + "branch.tre";
		Tr.TDknotExternalFilename = trim( SharedDirectory ) + "tree" + trim( Adjustl( NID ) ) + slash + "tdknotexternal.tre";
		Tr.TDbranchfilename = trim( SharedDirectory ) + "tree" + trim( Adjustl( NID ) ) + slash + "tdbranch.tre";
		Tr.TDImpactfilename = trim( SharedDirectory ) + "tree" + trim( Adjustl( NID ) ) + slash + "tdimpact.tre";

		cout << "Intialization of tree No: " << Tr.ID << endl;
		ifstream fin( Tr.treefilename , ifstream::in );
		fin >> Tr.Nkn;
		fin >> Tr.Nbr;
		fin.close( );
		Tr.B = new  Vetv [ Tr.Nbr ];
		Tr.K = new  Uzel [ Tr.Nkn ];
		//----------------------------------- Reading Knots ----------------------------------
		fin.open( Tr.knotfilename , ifstream::in );
		for ( long i = 0; i < Tr.Nkn; i++ ) {
			fin >> ID;
			Tr.K [ ID ].ID = ID;
			fin >> Tr.K [ ID ].C.X;	
			fin >> Tr.K [ ID ].C.Y;	//  
			fin >> Tr.K [ ID ].C.Z;	//  
			fin >> Tr.K [ ID ].IG; //  
		}
		fin.close( );
		//----------------------------------- Reading Branches  --------------------------------
		fin.open( Tr.branchfilename , ifstream::in );
		for ( long i = 0; i < Tr.Nbr; i++ ) {
			fin >> ID;
			Tr.B [ ID ].ID = ID;
			Tr.B [ ID ].myTreeID = Tr.ID;
			Tr.B [ ID ].InvertPoints = 0;
			fin >> KnID1 >> KnID2;		//     
			fin >> Tr.B [ ID ].len;		// 
			fin >> Tr.B [ ID ].width;	// 
			fin >> Tr.B [ ID ].pts;		//       

			if ( ( Tr.B [ ID ].len > 15.0 ) && ( Tr.B [ ID ].pts < 15 ) ) {
				Tr.B [ ID ].pts = 15;
			}

			Tr.B [ ID ].Kn1 = &Tr.K [ KnID1 ]; 
			Tr.B [ ID ].Kn2 = &Tr.K [ KnID2 ];

			if ( Tr.B [ ID ].pts > 1 ) {
				Tr.B [ ID ].dx = Tr.B [ ID ].len / double( Tr.B [ ID ].pts - 1 ); 
			}
			else {
				cout << "Error! Wrong points number at (tree, branch): " << Tr.ID << Tr.B [ ID ].ID << endl;
				Tr.B [ ID ].dx = Tr.B [ ID ].len;
			}
			{
				Tr.B [ ID ].VB.resize( Z.Cor );
				Tr.B [ ID ].VBO.resize( Z.Cor );
				for ( long i = 0; i < Z.Cor; i++ ) {
					Tr.B [ ID ].VB [ i ].resize( Tr.B [ ID ].pts );
					Tr.B [ ID ].VBO [ i ].resize( Tr.B [ ID ].pts );
				}
			}
		}
		fin.close( );
		
		//----- Making list of branches incoming and outgoing to every knot -----
		for ( long i = 0; i < Tr.Nkn; i++ ) {
			Tr.K [ i ].Nou = 0;
			Tr.K [ i ].Nin = 0;
		}
		long Iin, Iou;
		if ( ( Tr.ID == PULMVEN ) || ( Tr.ID == SYSVEN ) ) {
			//  Venous tree edges direction e1 <-- e2 
			for ( long i = 0; i < Tr.Nbr; i++ ) {
				Tr.B [ i ].InvertPoints = 1;
				Iin = Tr.B [ i ].Kn1 -> ID; 
				Iou = Tr.B [ i ].Kn2 -> ID;
				Tr.K [ Iin ].Nin += 1;
				Tr.K [ Iou ].Nou += 1;
				Tr.K [ Iin ].Bin.push_back( &Tr.B [ i ] );
				Tr.K [ Iou ].Bou.push_back( &Tr.B [ i ] );
			}
		} else {
			//  Arterial and Lymphatic trees edges direction e1 --> e2
			for ( long i = 0; i < Tr.Nbr; i++ ) {
				Tr.B [ i ].InvertPoints = 0;
				Iou = Tr.B [ i ].Kn1 -> ID; 
				Iin = Tr.B [ i ].Kn2 -> ID;
				Tr.K [ Iin ].Nin += 1;
				Tr.K [ Iou ].Nou += 1;
				Tr.K [ Iin ].Bin.push_back( &Tr.B [ i ] );
				Tr.K [ Iou ].Bou.push_back( &Tr.B [ i ] );
			}
		}

		// Marking input and output as BIFURCATION
		for ( long i = 0; i < Tr.Nkn; i++ ) {
			if ( ( Tr.K [ i ].Nin + Tr.K [ i ].Nou ) == 1 ) {
				Tr.K [ i ].IG = FLOW;
			} else {
				Tr.K [ i ].IG = BIFURCATION;
			}
		}
		/*cout << "Total knots loaded: " << Tr.Nkn << endl << "--------------------------------------" << endl;
		for ( long i = 0; i < Tr.Nkn; i++ ) printUzel ( Tr.K[i] );
		cout << "Total branches loaded: " << Tr.Nbr << endl << "--------------------------------------" << endl;
		for ( long i = 0; i < Tr.Nbr; i++ ) printVetv ( Tr.B[i] );*/
	}

	inline void LoadMultiKnots( ) {
		long  KntID , szin , szou , LTrID;
		Derevo LTree;

		Globals::MKnots.multiKnotsfilename = trim( SharedDirectory ) + "multiknots.tre";
		ifstream fin( Globals::MKnots.multiKnotsfilename , ifstream::in );
		fin >> Globals::MKnots.sz;

		if ( Globals::MKnots.sz > 0 ) {
			Globals::MKnots.Lst.resize( Globals::MKnots.sz );
			for ( long i = 0; i < Globals::MKnots.sz; i++ ) {
				fin >> Globals::MKnots.Lst [ i ].sz;
				Globals::MKnots.Lst [ i ].GrpLst.resize( Globals::MKnots.Lst [ i ].sz );
				szin = 0;
				szou = 0;
				for ( long j = 0; j < Globals::MKnots.Lst [ i ].sz; j++ ) {
					fin >> MKnots.Lst [ i ].GrpLst [ j ].TrID;
					fin >> MKnots.Lst [ i ].GrpLst [ j ].sz;
					Globals::MKnots.Lst [ i ].GrpLst [ j ].KntLst.resize( Globals::MKnots.Lst [ i ].GrpLst [ j ].sz );
					for ( long k = 0; k < Globals::MKnots.Lst [ i ].GrpLst [ j ].sz; k++ ) {
						fin >> KntID;
						LTrID = MKnots.Lst [ i ].GrpLst [ j ].TrID;
						LTree = TreeLst [ LTrID ];
						Globals::MKnots.Lst [ i ].GrpLst [ j ].KntLst [ k ] = LTree.K [ KntID ];
						szin = szin + Globals::MKnots.Lst [ i ].GrpLst [ j ].KntLst [ k ].Nin;
						szou = szou + Globals::MKnots.Lst [ i ].GrpLst [ j ].KntLst [ k ].Nou;
					}
				}
				Globals::MKnots.Lst [ i ].szin = szin;
				Globals::MKnots.Lst [ i ].szou = szou;
			}
		}
		fin.close( );

		cout << "Total multiknots loaded: " << MKnots.sz << endl;
	}
};