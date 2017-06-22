//########################################################################
//	������ ����������� ������-����������� ��������� ������ ��� ���� (�����)
// Uzel   - ���� (����� ���������, ��������� �����)
// Vetv   - ����� (�����)
// Derevo - �� ���� ���� (����), ������ - ������� ������, �������� ������ 
// ������, �����, ������� ����������� � �.�., � ����� ����� ������ � 
// ����������� ������ � �����
//########################################################################
#include "TreeDataDepend.h"
#include <vector>
namespace TreeDataIndepend {

	using namespace TreeDataDepend;
	//________________________________________________________________________
	//					����
	//	�		- ����� �� ������������ ���������
	//	ID	- ����� ����
	// Nou	- ���-�� ��������� ������
	// Nin	- ���-�� �������� ������
	// IG	- ��� ���� (���������, ����, ����� � �.�.)
	// TD	- ������-��������� ����� ���������� ����
	// ���������� ��� ��������� ����� � ���� (�������� ��� ���������)
	//________________________________________________________________________
	class Vetv;
	class Uzel {
	public:
		Dot C;
		long ID , Nou , Nin , IG;
		vector<Vetv*> Bou;
		vector<Vetv*> Bin;
		bool LYMPHNODE;
		TDUzelParameter TD;

	};

	//________________________________________________________________________
	//					�����
	//	ID				- ����� �����
	// pts				- ���������� ����� �� �����
	// dx				- ��� �� �����
	// len				- ����� �����
	//	width			- ������ �����
	//	Kn1, Kn2	- ��������� �� ����������� � ����� ����; Kn1 - ������, Kn2 - �����
	//	VB				- �������� ������� ���������� �� ����� 
	// VBO				- �������� ������� ���������� �� ����� �� ���������� ����
	// TD				- ������-��������� ����� ���������� �����
	// gravity           - �������� �� ���������� (1 ��� 0)
	//________________________________________________________________________
	class Vetv {
	public:
		long ID , pts , myTreeID , InvertPoints;
		Uzel *Kn1 , *Kn2;
		vector< vector<double> > VB , VBO;
		double len , width , dx;
		TDVetvParameter TD;
	};
	
	void printVetv ( Vetv& B ) {
		cout << "ID = " << B.ID << " pts = " << B.pts << " myTreeID = " << B.myTreeID << endl;
		cout << "len = " << B.len << " width = " << B.width << " dx = " << B.dx << endl;
		cout << "Kn1 = " << B.Kn1->ID << " Kn2 = " << B.Kn2->ID << endl;
		cout << "--------------------------------------" << endl;
	}
	void printUzel ( Uzel& K ) {
			cout << "ID = " << K.ID << " IG = " << K.IG << " LYMPHNODE = " << K.LYMPHNODE << endl;
			cout << "Nin = " << K.Nin << " Nou = " << K.Nou << endl;
			cout << "Bin : ";
			for (int i = 0; i < K.Bin.size(); ++i) cout << K.Bin[i]->ID << " ";
			cout << endl <<  "Bou : ";
			for (int i = 0; i < K.Bou.size(); ++i) cout << K.Bou[i]->ID << " ";
			cout << endl << "--------------------------------------" << endl;
		}

	//________________________________________________________________________
	//					������
	//	ID	- ����� ������
	// Nbr	- ���������� ������
	// Nkn	- ���������� �����
	//	B	- ������ ������ ������
	//	K	- ������ ����� ������
	// O - ������ �������
	//	����� ������ � ������� ������:
	// dirname						- ���������� ���������� ������ (������������ �������)
	// treefilename			- ��������� ������
	// knotfilename			- ������ ������-����������� ���������� �����
	// TDknotfilename		- ������ ������-��������� ���������� ����� 
	// branchfilename		- ������ ������-����������� ���������� ������ (��������� �����)
	// TDbranchfilename	- ������ ������-��������� ���������� ������
	//________________________________________________________________________
	class Derevo {
	public:
		long ID , Nbr , Nkn , Nimp , Norg;
		string treefilename , dirname;
		string knotfilename , TDknotfilename , TDknotExternalFilename;
		string branchfilename , TDbranchfilename;
		string TDImpactfilename;
		Vetv *B;
		Uzel *K;
		vector<TDExternalImpact> I;
	};

	// ������ ����� ������ ������ ��� ����������
	class MultiKnotTreeGroup {
	public:
		long TrID;
		long sz;
		vector<Uzel> KntLst;
	};

	// ����������  - ������������ ������� ���������� �������� ����� ������ ������ (��� ������ � ���� �� �����) � ����
	class MultiKnot {
	public:
		long sz;
		long szin;
		long szou;
		vector<MultiKnotTreeGroup> GrpLst;
	};

	// ������ �����������
	class MultiKnotList {
	public:
		long sz;
		vector<MultiKnot> Lst;
		string multiKnotsfilename;
	};
};