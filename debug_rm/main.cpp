#include"RecordManager.h"
#include "Condition.h"
#include "API.h"
#include "Defines.h"
#include "Attribute.h"
#include "Address.h"
#include<iostream>
#include<string>
#include<vector>

using namespace std;

int main() {
	API a;

	Condition con;
	string s;
	vector<Condition>con_temp;
	cin >> s;//table name
	int attriCnt = 5;
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
	Tuple t;
	for (int i = 0; i < attriCnt; i++) {//one tuple
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
	vector<Attribute>attr;

	a.TableCreate(s, attr_vec);//create table
	a.InsertTuple(s, t);//insert one tuple 
	a.SelectData(s, &attr);//no condition select
	a.TableDrop(s);//drop table

	system("pause");
	return 0;

}

//CatalogManager cm;
/*
int attriCnt = 5;
int num;
cin >> attr.name;
cin >> num;
attr.type = (AttributeType)num;
if (attr.type == 2) {
	cin >> attr.length;
}
*/

/*
	for (int i = 0; i < attriCnt; i++) {//get the attribute
		Attribute attr;
		int num;
		cin >> attr.name;
		cin >> num;
		attr.type = (AttributeType)num; //ǿ������ת��
		attr.isUnique = 0;
		attr.isPrimary = 0;
		attr.isNULL = 0;
		if (attr.type == 2) {
			cin >> attr.length;
		}
		//table.attri.push_back(attr);
		attr_vec.push_back(attr);
	}

	Tuple t;
	//����tuple��Ϣ
	for (int i = 0; i < attriCnt; i++) {//����һ������
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

	for (int i = 0; i < 2; i++) { //����������Ϣ
		int num;
		cin >> con.attri;
		cin >> num;
		con.val = num;
		int in;
		cin >> in;
		con.type = (ConditionType)in;
		con_temp.push_back(con);
	}
	//a.TableCreate(s, attr_vec);
	a.InsertTuple(s, t);
	a.InsertTuple(s, t);
	a.DeleteTuple(s, &con_temp);
	//attr_temp.push_back(attr);
	//a.SelectData(s, &attr_temp,&con_temp);//��������ѯ
	//a.TableCreate(s, attr_vec);//������
	//a.DeleteTuple(s,&con_temp);//������ɾ����Ϣ
	*/

	//�����������
	/*
	for (int i = 0; i < attriCnt; i++) {//get the attribute
		Attribute attr;
		int num;
		cin >> attr.name;
		cin >> num;
		attr.type = (AttributeType)num; //ǿ������ת��
		attr.isUnique = 0;
		attr.isPrimary = 0;
		attr.isNULL = 0;
		if (attr.type == 2) {
			cin >> attr.length;
		}
		//table.attri.push_back(attr);
		attr_vec.push_back(attr);
	}
	*/

	//a.TableCreate("test_2", attr_vec);//����һ��������Ϊtest
	/*
	Tuple t;
	//����tuple��Ϣ
	for (int i = 0; i < attriCnt; i++) {//����һ������
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
	*/
	//a.TableCreate("test_f",attr_vec);
	//a.InsertTuple("test_2", t);//��������
	//a.DeleteTuple("test_2");
	//a.TableDrop("test_1");
	//a.IndexCreate("primary_key_2", "test_2", "key");
	//a.IndexDrop("primary_key_2");






//a.~API();
/*

for (int i = 0; i < 2; i++) {//����Tuple����Ϣ
	Tuple t;
	for (int j = 0; j < attriCnt; j++) {
		if (!attr_vec[j].type) {
			int temp;
			cin >> temp;
			t.v.push_back(temp);
		}
		else if (attr_vec[j].type == 1) {
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
	a.InsertTuple("test", t);//����Tuple
}


a.SelectData("test", &attr_vec);//�ӱ�����������
*/


/*
int main() {
	BufferManager bm;
	RecordManager rm(&bm);
	//string name;
	//cin >> name;
	//rm.CreateTable(name); //create table
	//rm.DropTable(name);

	  //insert tuple 

	Table table;
	table.blockNum = 1;
	table.attriCnt = 5;
	string s = "table";
	table.name = s;
	for (int i = 0; i < table.attriCnt; i++) {//get the attribute
		Attribute attr;
		int num;
		cin >> attr.name;
		cin >> num;
		attr.type = (AttributeType)num; //ǿ������ת��
		attr.isUnique = 0;
		attr.isPrimary = 0;
		attr.isNULL = 0;
		if (attr.type == 2) {
			cin >> attr.length;
		}
		table.attri.push_back(attr);
	}

	for (int j = 0; j < 2; j++) {//������������
		Tuple tuple;
		for (int i = 0; i < table.attriCnt; i++) {
			if (table.attri[i].type == 0) {
				int temp;
				cin >> temp;
				tuple.v.push_back(temp);
			}
			else if (table.attri[i].type == 1) {
				float temp;
				cin >> temp;
				tuple.v.push_back(temp);
			}
			else {
				string temp;
				cin >> temp;
				tuple.v.push_back(temp);
			}

		}
		rm.InsertRecord(table, tuple);
		table.tuple.push_back(tuple);

		/*
		for (int k = 0; k < tuple.v.size(); k++) {
			switch (table.attri[k].type) {
			case 0: {
				int temp = get<int>(tuple.v[k]);
				cout << temp << ' ';
				break;
			}
			case 1: {
				float temp = get<float>(tuple.v[k]);
				cout << temp << ' ';
				break;
			}
			case 2: {
				string temp = get<string>(tuple.v[k]);
				cout << temp << ' ';
				break;
			}
			}
		}
		cout << endl;
		*/
	//}

	/*
	
	Condition con;
	con.attri = "key";
	con.type = EQ;
	con.val = 2;
	//vector<Tuple> t;
	//t=rm.SelectRecord(table, con);
	vector<Condition>con_a;
	vector<Attribute>attr_a;
	Attribute temp;
	temp.name = "cno";
	temp.type = INT;
	//con_temp.val = v;
	//con_temp.val;

	attr_a.push_back(temp);
	con_a.push_back(con);

	//cout << "the tuple select from the table according to the " << con.attri << " = 12. " << endl;
	bool result=rm.RecordConditionCheck(table, attr_a, con_a);
	*/

	/*
	Condition con_t;
	vector<Tuple> t = rm.SelectRecord(table, con_t);

	for (int i = 0; i < t.size(); i++) {
		for (int j = 0; j < table.attriCnt; j++) {
			switch (table.attri[j].type) {
			case 0: {
				int temp = get<int>(t[i].v[j]);
				cout << temp << ' ';
				break;
			}
			case 1: {
				float temp = get<float>(t[i].v[j]);
				cout << temp << ' ';
				break;
			}
			case 2: {
				string temp = get<string>(t[i].v[j]);
				cout << temp << ' ';
				break;
			}
			}
		}
		cout << endl;
	}
	*/
	
	/*
	system("pause");
}
*/


/*
	for (int j = 0; j < 2; j++) {//������������
		for (int i = 0; i < table.attriCnt; i++) {
			if(table.attri[i].type==0){
				int temp;
				cin >> temp;
				tuple.v.push_back(temp);
			}
			else if (table.attri[i].type == 1) {
				float temp;
				cin >> temp;
				tuple.v.push_back(temp);
			}
			else {
				string temp;
				cin >> temp;
				tuple.v.push_back(temp);
			}

		}
		rm.InsertRecord(table, tuple);
	}
	*/


//select tuple
/*
vector<Tuple> tuple;
Table table;
table.blockNum = 1;
table.attriCnt = 5;
string s = "table";
table.name = s;
Condition con;
tuple=rm.SelectRecord(table, con); //��������ѯ

for (int i = 0; i < tuple.size(); i++) {
	for (int j = 0; j < table.attriCnt; j++) {
		cout << tuple[i].v[j];
	}
	cout << endl;
}
*/



//drop tuple

/*
Tuple tuple_1;
for (int i = 0; i < table.attriCnt; i++) {
	if (table.attri[i].type == 0) {
		int temp;
		cin >> temp;
		tuple_1.v.push_back(temp);
	}
	else if (table.attri[i].type == 1) {
		float temp;
		cin >> temp;
		tuple_1.v.push_back(temp);
	}
	else {
		string temp;
		cin >> temp;
		tuple_1.v.push_back(temp);
	}
}
rm.DeleteRecord(table, tuple_1);//ɾ��һ������ */