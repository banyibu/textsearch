#include "textsch.h"
int main()
{
	ifstream  f;
	f.open("test.txt");
	if (!f) {
		cout << "文件打开失败" << endl;
	}
	TextQuery t1(f);
	Query q = Query("Solidity")&(~Query("an"));
	QueryResult r = q.eval(t1);
	//string a;
	//cin >> a;
	//queryresult r = t1.query(a);
	print(cout, r);
	system("pause");
	return 0;
}