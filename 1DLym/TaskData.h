//########################################################################
//				������ ���������� ���������� ������
//########################################################################

//________________________________________________________________________
//________________________________________________________________________
//					������ � �� ����������� ���������
//	Cor	- ���������� ��������� ���������� �� �����
//	ID	- ����� ������
//	Ntr	- ���������� ��������� ��������
//	Kur	- ����� �������
// useDump - ���������� �� ������� ����� ����������
//________________________________________________________________________
#include <iostream>
#include <fstream>
#include "Globals.h"
#include <omp.h>
namespace TaskData {
	using namespace Globals;

	class Zadacha {
	public:
		long Cor , ID , Ntr , useDump , Nmatter , useLungs , useExtImpactToLungs , useOrgans , useFlowAveraging , calcMax , useGravity , useSaveDump , writePmax;
		double TQaver0; // ������ ���������� ��������������� ������������ ������
		double TQaver; // ������ ���������� ������������ ������
		double Kur , P_barorec , h_period_new , T_last_Hbeat , h_period_curr , N_heart_cycles , Pnapol; //� ����� : �������� ��������� , ����� ����������� ��������� ������ , ����� �������� ������� , ���� �������� ���������� �������
		double startEECP , EECPcycle , EECPwork_interval; //����� ������ � , ������������ ����� ( ������ + ����� ) , ������������ ������ ������ �����
		double t_calve , t_thigh , t_abd , t_stop , P_calve , P_thigh , P_abd , corPres; //EECP parameters
		double Tc_st , S_st; //stenosis
		long Nave_cycles , autoreg , N_towrite;
		long* Num_towrite; // ������ ������ ��� ������������ ������ ���� ����������� � ������� �����������

		void IdentifyTask( ) {
			//����������� ���������� ������
			string filename = SharedDirectory + "ini" + slash + "zadacha.ini";
			double tmp[ZadachaPars];
			ReadFile(filename, tmp, ZadachaPars);
			ID = tmp[0];
			Cor = tmp[1];				//���������� ����������
			Ntr = tmp[2];				//���������� ��������
			Kur = tmp[3];
			Nmatter = tmp[4];			//���������� ����������� �������
			useDump = tmp[5];
			useSaveDump = tmp[6];
			useLungs = tmp[7];
			useExtImpactToLungs = tmp[8];
			useOrgans = tmp[9];
			useFlowAveraging = tmp[10];
			TQaver0 = tmp[11];
			TQaver = tmp[12];
			calcMax = tmp[13];
			useGravity = tmp[14];
			autoreg = tmp[15];
			Tc_st = tmp[16];
			
			// ������ ������ ��� ������������ ������ ���� ����������� � ������� �����������
			filename = Globals::SharedDirectory + "ini" + slash + "towrite.ini";
			fstream fin;
			fin.open( filename , ifstream::in );
			fin >> N_towrite; // amount of tracked vessels
			getline(fin , filename);
			if ( N_towrite > 0 ) {
				Num_towrite = new long [ N_towrite ];
				for ( long i = 0; i < N_towrite; i++ ) {
					fin >> Num_towrite [ i ];
				}
			}
			fin.close( );

			this->Nave_cycles = 1;
			this->T_last_Hbeat = 0.0;
			this->h_period_curr = this->Tc_st;
			this->h_period_new = this->Tc_st;
			this->N_heart_cycles = 0;
			this->writePmax = 0;
		}

		void PrintTask() {
			cout << "Task initializing" << endl;
			cout << "ID: BLOOD" << endl;
			cout << "Number of equations: " << this->Cor << endl;
			cout << "Number of networks: " << this->Ntr << endl;
			cout << "Courant number: " << this->Kur << endl;
			cout << "Number of substances: " << this->Nmatter << endl;

			if ( this->useDump == 1 ) {
				cout << "Continued simulations" << endl;
			}
			else {
				cout << "New simulations" << endl;
			}
			if ( this->useLungs == 1 ) {
				cout << "Lungs: Yes" << endl;
			}
			else {
				cout << "Lungs: No" << endl;
			}
			cout << "======================================" << endl;
		}
	};

	//�������� ���������� ����������
	inline void LoadGlobals( ) {
		ifstream fin( "paths.ini" , ifstream::in );
		fin >> Globals::SharedDirectory;
		fin.close( );
		
		string filename = SharedDirectory + "ini" + slash + "globals.ini";
		double tmp[GlobalPars];
		ReadFile(filename, tmp, GlobalPars);

		Globals::time_calc = tmp[0];
		Globals::NKO = tmp[1];
		Globals::Metod = tmp[2];
		Globals::write_time = tmp[3];
		Globals::write_delta = tmp[4];
		Globals::write_dump = tmp[5];
		Globals::use_MATLAB_out = tmp[6];
		Globals::use_GNUPLOT_out = tmp[7];

		cout << "MainDir: " << Globals::SharedDirectory << endl;
		cout << "Total time (time): " << Globals::time_calc << endl;
		cout << "Start results writing (write_time): " << Globals::write_time << endl;
		cout << "Steps to skip while writing (NKO): " << Globals::NKO << endl;
		cout << "Numerical method ID (Metod): " << Globals::Metod << endl;

		if ( Globals::use_MATLAB_out > 0 ) {
			cout << "MATLAB output enabled" << endl;
		}
		else {
			cout << "MATLAB output disabled" << endl;
		}

		if ( Globals::use_GNUPLOT_out > 0 ) {
			cout << "GNUPLOT output enabled" << endl;
		}
		else {
			cout << "GNUPLOT output disabled" << endl;
		}
		
		filename = SharedDirectory + "ini" + slash + "lymphatics.ini";
		double tmp1[LymphPars];
		ReadFile(filename, tmp, LymphPars);
		TDGlobals::teta = tmp[0];
		TDGlobals::Ampl = tmp[1];
		TDGlobals::vel = tmp[2];
		TDGlobals::omega = tmp[3];
		TDGlobals::Pin = tmp[4];
		TDGlobals::Pout = tmp[5];
		
		Globals::start_time = omp_get_wtime( );
		Globals::last_elapsed_time = 0.;
	}
};