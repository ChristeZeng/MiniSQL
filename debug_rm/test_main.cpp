#include "Interpreter.h"
#include<iostream>
#include<string>
#include<vector>

using namespace std;

int main()
{
	cout << "********************Welcome to 'My'SQL********************" << endl;
	
	Interpreter I;
	while (1)
	{
		string sql;
		cout << ">";
		getline(cin, sql);
		while (sql[sql.size() - 1] != ';')
		{
			string tmp;
			getline(cin, tmp);
			sql += " ";
			sql += tmp;
		}
		I.get_sql(sql);
		I.run_sql();
	}
}