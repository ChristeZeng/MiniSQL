#include "RecordManager.h"
#include<iostream>
#include<cstring>
#include<iomanip>
using namespace std;


RecordManager::RecordManager(BufferManager * bm_temp) {
    bm = bm_temp;
    //BufferManager* b = new BufferManager;
    //bm = b;
}


RecordManager::~RecordManager() {
    bm->~BufferManager();
}
int RecordManager::Getlen(const Table table) {//get the length of tuple
    int ret = 0;
    int n = table.attri.size();//number of attribute
    for (int i = 0; i < n; i++) {
        switch (table.attri[i].type)
        {
        case 0: //int type
            ret += 4;
            break;

        case 1: //float type
            ret += 4;
            break;

        case 2: //string type
            ret += table.attri[i].length+1; //char length
            break;
        }
    }
    return ret + 1; //one extra '1' is for label the tuple exists or not
}


int RecordManager::Caculate_Offset(const Table table, int n) {//caculate the offset starts from n th attribute
    int ret = 0;
    for (int i = 0; i < n; i++) {
        switch (table.attri[i].type)
        {
        case 0: //int type
            ret += 4;
            break;

        case 1: //float type
            ret += 4;
            break;

        case 2: //string type
            ret += table.attri[i].length+1; //char length
            break;
        }
    }
    return ret + 1;//the '1'  means the first char reprsenting the existence
}


bool RecordManager::InsertTuple(Table& table, Tuple& tuple, char* p) {//insert one tuple to table

    int i;
    bool ret = false;

    *p = 1; //this line is no longer empty
    p++;  //start from next address
    int offset = 0;
    for (i = 0; i < table.attri.size(); i++) {//insert information to block
        switch (table.attri[i].type)
        {
        case 0:  //int type
        {
            int integer = get<int>(tuple.v[i]);
            memcpy(p + offset, &integer, sizeof(int));
            offset += 4;
            break;
        }
        case 1: //float type
        {
            float f = get<float>(tuple.v[i]);
            memcpy(p + offset, &f, sizeof(float));
            offset += 4;
            break;
        }
        case 2: //string type
        {
            string s = get<string>(tuple.v[i]);
            
            //cout << "the string get from InsertTuple: " << s << endl;
            //cout << "the length of Tuple :" << s.size() << endl;
            int size = strlen(s.c_str())+1; //char * len(string)

            //memcpy(p + offset, &str, size*sizeof(char));
            strcpy(p + offset, s.c_str());
            string temp = string(reinterpret_cast<char*>(p + offset));
            //cout << "the string insert to buf : " << temp << endl;

            offset += table.attri[i].length+1;
            break;
        }
        }

    }

    if (i == table.attri.size()) {
        ret = true;
    }

    return ret;
}


void RecordManager::CreateTable(string table) //create table
{
    string name = "rdf_" + table;
    int BlockId = bm->ReadIndexFromFile(name, 0);
    if (BlockId != -1) {
        //cout << "Create table " << table << " successfully !" << endl;
    }
    else {
        cout << "Failed to create table " << table << " !" << endl;
    }
}


bool RecordManager::DropTable(string table) { //delete table
    //string name=table.name+".data";
    //string name = "rdf_" + table + ".data";
    string name = "rdf_" + table;
    bm->FreeBlock(name);
    name = name + ".data";
    if (!remove(name.c_str())) {//successful delete       the address need absolute address!!!
        //cout << "Drop table " << table << " successfully !" << endl;
        return true;
    }
    else {//failed to delete
        //cout << "Failed to delete table" << table << endl;  //可以改为throw一个异常
        return false;
    }
}



bool RecordManager::checkUnique(Table& table, int i, Tuple& tuple) { //check whether the data satisfy the requirement of uniqueness
    if (!table.attri[i].isUnique && !table.attri[i].isPrimary) { //this attribute does not need to be unique
        return true;
    }
    else {
        int len = Getlen(table);
        int j = 0;
        for (int j = 0; j < table.blockNum; j++) {//for each block, check the unique requirement
            int index = bm->ReadIndexFromFile("rdf_"+table.name, j);
            char* p = bm->ReadBlockFromFile(index);  //get the pointer of the buf
            int pre_offset = Caculate_Offset(table, i);

            for (int start = 0; start + pre_offset < 4096; start += len) {
                if (p[start]) {//this tuple exists
                    switch (table.attri[i].type)
                    {
                    case 0: //int type
                    {
                        int temp = *(int*)(start + pre_offset + p);
                        if (temp == get<int>(tuple.v[i])) {
                            return false;
                        }
                        break;
                    }
                        
                    case 1: //float type
                    {
                        float temp = *(float*)(start + pre_offset + p);
                        if (temp == get<float>(tuple.v[i])) {
                            return false;
                        }
                        break;
                    }
                        
                    case 2: //string type
                    {
                        string temp = string((char*)(start + pre_offset + p));
                        if (temp == get<string>(tuple.v[i])) {
                            return false;
                        }
                        break;
                    }
                    }
                }
            }
        }
    }
    return true;
}


int RecordManager::InsertRecord(Table& table, Tuple& tuple) { //insert tuple
    int ret = -1;
    int n = table.attri.size();//find the number of attribute
    for (int i = 0; i < n; i++) {
        if (!checkUnique(table, i, tuple)) {// the attribute should be unique but there already exists one in the table
            cout << "The attribute " << table.attri[i].name << " is not unique if you insert this tuple!" << endl; 
            return ret;
        }
    }

    int block_num = table.blockNum;
    int index = bm->ReadIndexFromFile("rdf_"+table.name, block_num - 1);//get the block that can insert data
    char* p = bm->ReadBlockFromFile(index);  //get the buf address
    int len = Getlen(table);
    bool flag = false;

    for (int offset = 0; offset + len < 4096; offset += len) {
        if (!p[offset]) {//this line is empty
            flag = InsertTuple(table, tuple, p+offset); //insert successfully
            if (flag) {
                bm->DirtyBlock(index);
                ret = offset+(block_num-1)*4096;
                break;
            }
        }
    }

    if (!flag) { //this block is full
        table.blockNum++;//the block num increase one
        index = bm->ReadIndexFromFile("rdf_"+table.name, block_num);//new block
        p = bm->ReadBlockFromFile(index);
        for (int offset = 0; offset + len < 4096; offset += len) {
            if (p[offset]) {
                flag = InsertTuple(table, tuple, p);//insert tuple in the new block
                if (flag) {
                    bm->DirtyBlock(index);
                    ret = offset + block_num * 4096;
                    break;
                }
            }
        }
        Block temp = bm->FindBlock(index);
        bm->writetofile(index);
        //bm->writetofile(&temp); //write the revised block to file
    }
    else {
        //bm->DirtyBlock(index);
        Block temp = bm->FindBlock(index);
        //bm->writetofile(&temp); //write the revised block to file
        bm->writetofile(index);
    }

    //return true;
    return ret;
}


void RecordManager::DeleteRecord(Table& table, Tuple& tuple) { //delete tuple
    int n = table.blockNum;
    for (int i = 0; i < n; i++) {//for each block
        int index = bm->ReadIndexFromFile("rdf_"+table.name, i);
        char* p = bm->ReadBlockFromFile(index);
        int len = Getlen(table);
        for (int offset = 0; offset + len < 4096; offset += len) {//check each tuple
            bool flag = true; //this tuple match or not
            if (p[offset]) {
                vector<Attribute> attr = table.attri;
                int offset_attr = offset +1;
                //if (!offset) {
                  //  offset_attr++;
                //}
                for (int j = 0; j < attr.size() && flag; j++) {//check each attribute match or not
                    switch (attr[j].type)
                    {
                    case 0: //int
                    {
                        int temp = *(int*)(offset_attr + p);
                        if (temp != get<int>(tuple.v[j])) {
                            flag = false;
                        }
                        offset_attr += sizeof(int);
                        break;
                    }

                    case 1: //float
                    {
                        float temp = *(float*)(offset_attr + p);
                        if (temp != get<float>(tuple.v[j])) {
                            flag = false;
                        }
                        offset_attr += sizeof(float);
                        break;
                    }

                    case 2:
                    {
                        string s = string((char*)(offset_attr + p));
                        if (strcmp(s.c_str(), get<string>(tuple.v[j]).c_str())) {//not equal
                            flag = false;
                        }
                        offset_attr += attr[j].length+1;
                        break;
                    }
                    }
                }
                if (flag) {//already find the matching tuple
                    p[offset] = 0;//lazy deletion
                    bm->DirtyBlock(index);
                    Block b = bm->FindBlock(index);
                    //bm->writetofile(&b);//write back to file
                    bm->writetofile(index);
                }
            }
        }
    }
}

template<typename T>
bool ConditionCheck(const T& a, const T& b, const Condition& con) { //check a b with the condition
    bool result=true;

    if (con.type == EQ) {
        result = (a == b);
    }
    else if (con.type == NE) {
        result = (a != b);
    }
    else if (con.type == LT) {
        result = (a < b);
    }
    else if (con.type == LE) {
        result = (a <= b);
    }
    else if (con.type == GT) {
        result = (a > b);
    }
    else if (con.type == GE) {
        result = (a >= b);
    }

    return result;
}

Tuple RecordManager::getTuple(char* p, const Table& table) {//get tuple according to the pointer p and table
    Tuple t;
    vector<value> ret;
    int offset = 0;
    p++;
    for (int i = 0; i < table.attri.size(); i++) {
        switch (table.attri[i].type)
        {
        case 0://int
        {
            int a = *(int*)(p + offset);
            ret.push_back(a);
            offset += 4;
            break;
        }
        case 1://float
        {
            float a = *(float*)(p + offset);
            ret.push_back(a);
            offset += 4;
            break;
        }

        case 2://string
        {
            string a = string((char*)(p + offset));
            //string a = string(reinterpret_cast<char*>(p + offset));
            //cout << "the string get from getTuple: " << a << endl;
            //cout << "the length of string: " << a.size() << endl;
            //cout<<"the address of p :"<<p<<endl;
            ret.push_back(a);
            offset += table.attri[i].length+1;
            break;
        }
        }
    }
    t.v = ret;
    return t;
}

void RecordManager::check_and_insert(char* p, int offset, int pre_offset, int type_attr, const Table& table, const Condition& con, vector<Tuple>& ret) {
    bool result = true;

    switch (type_attr)
    {
    case 0://int
    {
        int a = *(int*)(p + offset + pre_offset);
        int b = get<int>(con.val);
        result = ConditionCheck(a, b, con);
        break;
    }

    case 1://float
    {
        float c = *(float*)(p + offset + pre_offset);
        float b = get<float>(con.val);
        result = ConditionCheck(c, b, con);
        break;
    }

    case 2://string
    {
        string d = string((char*)(p + offset + pre_offset));
        string b = get<string>(con.val);
        result = ConditionCheck(d, b, con);
        break;
    }
    }

    if (result) {//satisfy the condition,insert to ret
        Tuple t = getTuple(p, table);
        ret.push_back(t);
    }
}


vector<Tuple> RecordManager::SelectRecord(const Table& table, const Condition& con) {
    vector<Tuple> ret;
    int n = table.blockNum;
    for (int i = 0; i < n; i++) {
        int index = bm->ReadIndexFromFile("rdf_"+table.name, i);//get the index of ith block of table
        char* p = bm->ReadBlockFromFile(index);//get the pointer of buf
        int len = Getlen(table);//length of tuple
        int pre_offset = 0;
        int type_attr = -1;
        if (con.attri.size()) { //the condition exists
            for (int j = 0; j < table.attri.size(); j++) {
                if (!strcmp(con.attri.c_str(), table.attri[j].name.c_str())) {//find the attibute
                    type_attr = table.attri[j].type;//record the type of this attribute
                    break;
                }
                else {
                    switch (table.attri[j].type)
                    {
                    case 0: //int
                        pre_offset += 4;
                        break;
                    case 1: //float
                        pre_offset += 4;
                        break;
                    case 2: //string
                        pre_offset += table.attri[j].length+1;
                    }
                }
            }
        }
        
        pre_offset++;//add the first byte
        for (int offset = 0; offset + pre_offset < 4096; offset += len) {
            if (p[offset]) {// the tuple exists
                if (con.attri.size()) {
                    check_and_insert(p, offset, pre_offset, type_attr, table, con, ret);
                }
                else {
                    Tuple t = getTuple(p+offset, table);
                    ret.push_back(t);
                }
                
            }
        }
    }

    return ret;
}


vector<value> RecordManager::FindValue(const Table& table, string IndexName) {//return the column with attribute name IndexName
    vector<value> ret;
    for (int i = 0; i < table.blockNum; i++) { //for each block
        int index = bm->ReadIndexFromFile("rdf_"+table.name, i);
        char* p = bm->ReadBlockFromFile(index);
        int pre_offset = 0;
        int t;
        for (int j = 0; j < table.attriCnt; j++) { //caculate the pre_offset
            if (IndexName == table.attri[j].name) {
                t = table.attri[j].type;
                break;
            }
            switch (table.attri[j].type) {
            case 0://int
                pre_offset += 4;
                break;
            case 1://float
                pre_offset += 4;
                break;
            case 2://string
                pre_offset += table.attri[j].length+1;
                break;
            }
        }
        pre_offset++;// caculate the first byte taken to represent the existence
        int len = Getlen(table);
        for (int offset = 0; offset + pre_offset < 4096; offset += len) {
            if (p[offset]) {
                switch (t) {
                case 0://int
                {
                    int a = *(int*)(p + offset + pre_offset);
                    ret.push_back(a);
                    break;//float
                }

                case 1:
                {
                    float a = *(float*)(p + offset + pre_offset);
                    ret.push_back(a);
                    break;
                }

                case 2:
                {
                    string a = string((char*)(p + offset + pre_offset));
                    ret.push_back(a);
                    break;
                }

                }
            }
        }
    }
    return ret;
}

vector<int> RecordManager::FindValueOffset(const Table& table, string IndexName) { //return the offset of the index attribute
    int n = table.blockNum;
    int len = Getlen(table);
    int final_offset = 0;
    vector<int>ret;
    for (int i = 0; i < n; i++) {
        int index = bm->ReadIndexFromFile("rdf_"+table.name, i);
        char* p = bm->ReadBlockFromFile(index);
        for (int offset = 0; offset < 4096; offset += len) {

            final_offset = offset + i * 4096; //if there are one more block ,the offset is added by i*4095

            if (p[offset]) {//this tuple exists
                ret.push_back(final_offset);
            }
        }
    }
    return ret;
}



int RecordManager::GetAttributeSize(Attribute attr) {//get the size of attribute
    int ret = 0;
    switch (attr.type)
    {
    case 0:
        ret = sizeof(int);//int type
        break;

    case 1:
        ret = sizeof(float);//float type
        break;

    case 2:
        ret = attr.length+1;//string type
        break;
    }
    return ret;
}



int RecordManager::FindAttributeOffset(const Table& table, Attribute attr) {//find the attribute offset in block
    int ret = -1;
    bool first = true;
    for (int i = 0; i < table.attri.size(); i++) {
        if (first) {//the first time
            ret += 1;
            first = false;
        }
        else {
            ret += GetAttributeSize(table.attri[i - 1]);
        }
        if (table.attri[i].name == attr.name) {
            break;
        }
    }
    ret++;//the first byte is the "exist or not" sign
    return ret;
}


//condition check for each tuple
bool RecordManager::TupleConditionCheck(const Table& table, char* p, int offset,  vector<Condition>con) { //check signle tuple with the certain condition
    bool ret = true;
   // for (int i = 0; i < attr.size(); i++) {//check each attribute you wanna show
    //    int temp_offset = FindAttributeOffset(table, attr[i]);
   //     if (!temp_offset) {// the attribute is empty
  //          continue;
  //      }
        //string AttrName = attr[i].name;
        for (int j = 0; j < con.size(); j++) {
            Tuple t = getTuple(p + offset, table);
            for (int m = 0; m < table.attri.size(); m++) {
                if (table.attri[m].name == con[j].attri) {//the attribute matches
                    switch (table.attri[m].type) {
                        case 0: {
                            int temp =get<int>(t.v[m]);
                            int val = get<int>(con[j].val);
                            bool result = ConditionCheck(temp, val, con[j]);
                            if (!result) {
                                ret = false; //the attribute doesn't matches the condition bound
                            }
                            //condition check
                            break;
                        }
                        case 1: {
                            float temp = get<float>(t.v[m]);
                            float val = get<float>(con[j].val);
                            bool result = ConditionCheck(temp, val, con[j]);
                            if (!result) {
                                ret = false;
                            }
                            break;
                        }
                        case 2: {
                            string temp = get<string>(t.v[m]);
                            string val = get<string>(con[j].val);
                            bool result = ConditionCheck(temp, val, con[j]);
                            if (!result) {
                                ret = false;
                            }
                            break;
                        }
                    }
                    break;
                }
            }
        }
    return ret;
}



bool RecordManager::RecordConditionCheck(const Table& table, vector<Attribute>attr, vector<Condition>con) {//check all tuple with certain condition
    bool ret = false;
    if (con.size()) {//there are condition that need to fit
        int i;
        int n = table.blockNum;
        int len = Getlen(table);//one tuple length
        for (i = 0; i < n; i++) { //check each block
            int index = bm->ReadIndexFromFile("rdf_"+table.name, i);
            char* p = bm->ReadBlockFromFile(index);
            for (int offset = 0; offset + len < 4096; offset += len) {//check each tuple
                if (p[offset]) {//the tuple exists
                    if (TupleConditionCheck(table, p, offset, con)) {//if at least one tuple is found, the return value should be true
                        ret = true;
                        if (attr.size()) {
                            for (int j = 0; j < attr.size();) { //display the tuple
                                Tuple t = getTuple(p + offset, table);
                                for (int k = 0; k < table.attri.size(); k++) {//find the matching attribute
                                    if (table.attri[k].name == attr[j].name) {
                                        cout <<left<<setw(10) <<t.v[k] ;
                                        j++;
                                        break;
                                    }
                                }
                                cout << endl;
                            }
                        }
                        else {
                            Tuple t = getTuple(p + offset, table);
                            for (int j = 0; j < table.attri.size(); j++) {
                                cout <<left<<setw(10) <<t.v[j] ;
                            }
                            cout << endl;
                        }
                        
                    }
                }
            }
        }
    }
    else {// no condition bound
        int i;
        int n = table.blockNum;
        int len = Getlen(table);//one tuple length
        for (i = 0; i < n; i++) { //check each block
            int index = bm->ReadIndexFromFile("rdf_"+table.name, i);
            char* p = bm->ReadBlockFromFile(index);
            for (int offset = 0; offset + len < 4096; offset += len) {//check each tuple
                if (p[offset]) {//the tuple exists
                    if (attr.size()) {
                        for (int j = 0; j < attr.size();) { //display the tuple
                            Tuple t = getTuple(p + offset, table);
                            for (int k = 0; k < table.attri.size(); k++) {//find the matching attribute   
                                if (table.attri[k].name == attr[j].name) {
                                    cout <<left<<setw(10) <<t.v[k] ;
                                    j++;
                                    break;
                                }
                            }
                            cout << endl;
                        }
                    }
                    else {
                        //int index_t = i * (4096 / len) + offset / len;
                        Tuple t = getTuple(p + offset, table);
                        for (int j = 0; j < table.attri.size(); j++) {
                            cout <<left<<setw(10)<< t.v[j] ;
                        }
                        cout << endl;
                    }
                }
            }
        }
        ret = true;
    }
    return ret;
}


int RecordManager::GetBlockNum(const Table& table) {//caculate the number of block used by this table
    string name = "rdf_"+table.name;
    int index = bm->ReadIndexFromFile("rdf_"+table.name, 0);//update one block to store the information of table
    int sum = 0;
    for (int i = Buffer_MAX-1; i >=0; i--) {//check each block
        Block b = bm->FindBlock(i);
        if (b.file == name) {//this block belong to our table
            sum++;
        }
    }
    return sum;
}

bool RecordManager::RecordDeleteCondition(const Table& table,  vector<Condition>con) {//delete all tuple satisfying the condition
    int n = table.blockNum;
    int len = Getlen(table);
    bool ret = false;
    for (int i = 0; i < n; i++) {//check each block
        int index = bm->ReadIndexFromFile("rdf_" + table.name, i);
        char* p = bm->ReadBlockFromFile(index);
        for (int offset = 0; offset < 4096; offset += len) {//check each tuple
            if (p[offset]) {//the tuple exists 
                bool result = true;
                //condition check
                result = TupleConditionCheck(table, p, offset,con);
                if (result) {
                    *(p+offset) = 0;//lazy delete
                    ret = true;
                }
            }
        }
        bm->DirtyBlock(index);
        Block b = bm->FindBlock(index);
        //bm->writetofile(&b);//写回给文件
        bm->writetofile(index);
    }

    return ret;
}

template<typename T>
bool RecordManager::ConditionCheck(const T& a, const T& b, const Condition& con) { //check a b with the condition
    bool result = true;

    if (con.type == EQ) {
        result = (a == b);
    }
    else if (con.type == NE) {
        result = (a != b);
    }
    else if (con.type == LT) {
        result = (a < b);
    }
    else if (con.type == LE) {
        result = (a <= b);
    }
    else if (con.type == GT) {
        result = (a > b);
    }
    else if (con.type == GE) {
        result = (a >= b);
    }

    return result;
}


void RecordManager::UpdateBufferManager(BufferManager* bm_temp) {
    Block buffer_temp = bm_temp->FindBlock(0);//第一块
    bm->UpdateBuffer_pool(&buffer_temp);
}


//show all tuples
void RecordManager::ShowData(vector<int>offset, Table* t) {
    for (int i = 0; i < offset.size(); i++) {
        int blockNumOffset = offset[i]/4096;
        int temp_offset = offset[i]%4096;
        int index = bm->ReadIndexFromFile("rdf_"+t->name, blockNumOffset);
        char* p = bm->ReadBlockFromFile(index);
        Tuple temp = getTuple(p+temp_offset, *t);
        for (int j = 0; j < temp.v.size(); j++) {
            cout << left << setw(10) << temp.v[j];
        }
        cout << endl;
    }
}