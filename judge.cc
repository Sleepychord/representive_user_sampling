#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
using namespace std;

set<string> ans;
vector<string> result;

int main(int argc, char **argv) {
	if(argc < 3) return 0;
	string temp;
	ifstream fin1(argv[1]);
	while(getline(fin1, temp) != NULL) ans.insert(temp);
	fin1.close();
	ifstream fin2(argv[2]);
	while(getline(fin2, temp) != NULL) result.push_back(temp);
	fin2.close();
	ofstream *pout = NULL;
	if(argc > 3) pout = new ofstream(argv[3]);
	int m = 0;
	for(vector<string>::size_type i = 0; i < result.size(); ++i) {
		if(ans.find(result[i]) != ans.end()) {
			++m;
			if(pout != NULL) *pout << result[i] << endl;
		}
		double p = double(m) / (i+1);
		double r = double(m) / ans.size();
		if(i+1 == 10 || i+1 == 50 || i+1 == 100 || i + 1== result.size())
		cout << i + 1  << "\t" << p << "\t" << r << "\t" << (p * r * 2) / (p + r) << "\t" << m << endl;
	}
	if(pout != NULL) pout->close();
	return 0;
}
