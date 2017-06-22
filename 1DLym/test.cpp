#include "CalcUtils.h"
#include <iostream>
using namespace std;
using namespace CalcUtils;

int main (void) {
	int m, n, k;
	cin >> m >> n;
	vector<vector<double> > A;
	A.resize(m);

	for (int i = 0; i < m; ++i) {
		A[i].resize(n);
		for (int j = 0; j < n; ++j) {
			cin >> A[i][j];
		}
	}
	/*
	for (int i = 0; i < n; ++i) {
		B[i].resize(k);
		for (int j = 0; j < k; ++j) {
			cin >> B[i][j];
		}
	}*/
	
	Matrix MA(A);
	Matrix MC;
	MC = MA.InvMatrix();
	MC = MA * MC;
	
	for (int i = 0; i < m; ++i) {
		for (int j = 0; j < n; ++j) {
			cout << MC(i,j) << " ";
		}
		cout << endl;
	}

	return 0;
}