#include"RecordManager.h"
#include<iostream>

int main() {
	RecordManager* rm = new RecordManager;

	string tableName;
	cin >> tableName;//table name
	rm->CreateTable(tableName);//create table
	Table table;
	table.name = tableName;

	int attriCnt;
	cin >> attriCnt;//the attribute number

	vector<Attribute>attr_vec;
	for (int i = 0; i < attriCnt; i++) {//get the attribute
		Attribute attr;
		int num;
		cin >> attr.name;
		cin >> num;
		attr.type = (AttributeType)num;
		attr.isUnique = 0;
		attr.isPrimary = 0;
		attr.isNULL = 0;
		if (attr.type == 2) {
			cin >> attr.length;
		}
		//table.attri.push_back(attr);
		attr_vec.push_back(attr);
	}
	table.attri = attr_vec;
	table.attriCnt = attriCnt;

	for (int j = 0; j < 2; j++) {//insert two tuples
		Tuple t;
		for (int i = 0; i < attriCnt; i++) {
			if (attr_vec[i].type == 0) {
				int temp;
				cin >> temp;
				t.v.push_back(temp);
			}
			else if (attr_vec[i].type == 1) {
				float temp;
				cin >> temp;
				t.v.push_back(temp);
			}
			else {
				string temp;
				cin >> temp;
				t.v.push_back(temp);
			}
		}
		rm->InsertRecord(table, t);
	}
	
}