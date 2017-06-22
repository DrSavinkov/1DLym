//########################################################################
//	Модуль определения задача-независимых элементов дерева или сети (графа)
// Uzel   - узел (точка ветвления, граничная точка)
// Vetv   - ветвь (ребро)
// Derevo - по сути сеть (граф), дерево - частный случай, содержит списки 
// ветвей, узлов, внещних воздействий и т.п., а также имена файлов с 
// параметрами ветвей и узлов
//########################################################################
#include "TreeDataDepend.h"
#include <vector>
namespace TreeDataIndepend {

	using namespace TreeDataDepend;
	//________________________________________________________________________
	//					Узел
	//	С		- Точка на координатной плоскости
	//	ID	- номер узла
	// Nou	- кол-во выходящих ветвей
	// Nin	- кол-во входящих ветвей
	// IG	- тип узла (ветвление, вход, выход и т.п.)
	// TD	- задача-зависимый набор параметров узла
	// определяет тип вхождения ветви в узел (входящая или исходящая)
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
	//					Ветвь
	//	ID				- номер ветви
	// pts				- количество точек на ветви
	// dx				- шаг на ветви
	// len				- длина ветви
	//	width			- ширина ветви
	//	Kn1, Kn2	- указатели на прилегающие к ребру узлы; Kn1 - начало, Kn2 - конец
	//	VB				- значение вектора переменных на ветви 
	// VBO				- значение вектора переменных на ветви на предыдущем шаге
	// TD				- задача-зависимый набор параметров ветви
	// gravity           - работает ли гравитация (1 или 0)
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
	//					Дерево
	//	ID	- номер дерева
	// Nbr	- количество ветвей
	// Nkn	- количество узлов
	//	B	- список ветвей дерева
	//	K	- список узлов дерева
	// O - список органов
	//	Имена файлов с данными дерева:
	// dirname						- директория размещения файлов (относительно текущей)
	// treefilename			- параметры дерева
	// knotfilename			- список задача-независимых параметров узлов
	// TDknotfilename		- список задача-зависимых параметров узлов 
	// branchfilename		- список задача-независимых параметров ветвей (структура графа)
	// TDbranchfilename	- список задача-зависимых параметров ветвей
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

	// Группа узлов одного дерева для мультиузла
	class MultiKnotTreeGroup {
	public:
		long TrID;
		long sz;
		vector<Uzel> KntLst;
	};

	// Мультиузел  - осуществляет склейку нескольких концевых узлов разных графов (или одного и того же графа) в один
	class MultiKnot {
	public:
		long sz;
		long szin;
		long szou;
		vector<MultiKnotTreeGroup> GrpLst;
	};

	// Список мультиузлов
	class MultiKnotList {
	public:
		long sz;
		vector<MultiKnot> Lst;
		string multiKnotsfilename;
	};
};