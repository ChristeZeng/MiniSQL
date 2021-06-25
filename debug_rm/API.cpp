#include"API.h"
#include<iomanip>


API::API() {
    BufferManager* b = new BufferManager;
    bm = b;
    RecordManager* r = new RecordManager(bm); 
    rm = r;
    CatalogManager* c = new CatalogManager;
    cm = c;
}

API::~API() {
    rm->~RecordManager();
    cm->~CatalogManager();
}

bool API::ExistTable(string name) {
    bool ret = true;
    if (!cm->SelectTable(name)) { //the table doesn't exists
        ret = false;
    }
    return ret;
}



//drop table according to the name
void API::TableDrop(string TableName) {
    if (ExistTable(TableName)) {
        //需要新函数，根据表名获得这张表中所有建立了index的属性（attribute)对应的index名称(IndexName),如果没有，就返回空的
        //IndexDrop(IndexName);删除index
       
        //delete the table file
        if (rm->DropTable(TableName)) {
            
            cm->DropTable(TableName);
            cout << "Drop table " << TableName << " successfully !" << endl;
        }
        else {
            cout << "Failed to drop table " << TableName << endl;
        }

    }
    else {
        cout << "Failed to drop table " << TableName <<" because the table doesn't exists !" << endl;
    }
}


//create table without primary key
void API::TableCreate(string TableName, vector<Attribute>attribute) {
    //get the primary key and location
    int primary_location = -1;
    string primary = "\0";
    for (int i = 0; i < attribute.size(); i++) {
        if (attribute[i].isPrimary) {//this one is the primary key
            primary_location = i;
            primary = attribute[i].name;
            break;
        }
    }

    if (ExistTable(TableName)) {
        cout << "There already exists one table " << TableName << " !" << endl;
    }
    else {
        rm->CreateTable(TableName);//create rdf_name.data
        Table t;
        t.name = TableName;
        t.attriCnt = attribute.size();
        t.blockNum = 1;
        for (int i = 0; i < attribute.size(); i++) {
            t.attri.push_back(attribute[i]);
        }
        int result = cm->CreateNewTable(&t); //create name.tbf
        if (!result)
            cout << "Create table " << TableName << " successfully!" << endl;
        else
            cout << "Create table " << TableName << " failed because the table already exists !" << endl;

        //create index for primary key
        if (primary_location != -1) {
            string indexName = "PRIMARY_KEY_" ;
            switch (attribute[primary_location].type) {
            case 0: { //int
                //BufferManager* bm_temp = new BufferManager;
                //IndexManager<int> im(indexName, TableName, attribute[primary_location].name, sizeof(int), bm_temp);
                //im.createIndex();
                IndexCreate(indexName +primary , t.name, primary);
            }

                  break;
            case 1: {//float
                //BufferManager* bm_temp = new BufferManager;
                //IndexManager<float> im(indexName, TableName, attribute[primary_location].name, sizeof(float), bm_temp);
                //im.createIndex();
                IndexCreate(indexName + primary, t.name, primary);
            }

                  break;
            case 2: {//string
                //BufferManager* bm_temp = new BufferManager;
                //IndexManager<string> im(indexName, TableName, attribute[primary_location].name, attribute[primary_location].length, bm_temp);
                //im.createIndex();
                IndexCreate(indexName + primary, t.name, primary);
            }
                  break;
            }
    }

    }

}


//create the table with primary key
void API::TableCreate(string TableName, vector<Attribute>attribute, string primary, int primary_location) {
    if (ExistTable(TableName)) {
        cout << "There already exists one table " << TableName << " !" << endl;
    }
    else {
        rm->CreateTable(TableName);//create rdf_name.data
        Table t;
        t.name = TableName;
        t.attriCnt = attribute.size();
        t.blockNum = 1;
        for (int i = 0; i < attribute.size(); i++) {
            t.attri.push_back(attribute[i]);
        }
        cm->CreateNewTable(&t); //create name.tbf
        cout << "Create table " << TableName << " successfully!" << endl;

        //create index for primary key
        /*
        string indexName = "PRIMARY_KEY " + TableName;
        switch (attribute[primary_location].type) {
        case 0: { //int
            IndexManager<int> im(sizeof(int));
            im.createIndex(indexName, TableName, primary);
        }

              break;
        case 1: {//float
            IndexManager<float> im(sizeof(float));
            im.createIndex(indexName, TableName, primary);
        }

              break;
        case 2: {//string
            IndexManager<string> im(sizeof(primary));
            im.createIndex(indexName, TableName, primary);
        }
              break;
        }
        */
    }
}


//drop the index
void API::IndexDrop(string indexName) {
    if (!cm->DropIndex(indexName)) {
        cout << "Delete index " << indexName << " successfully !" << endl;
    }
    else {
        cout << "The index " << indexName << " doesn't exists !" << endl;
    }
}


//create index for sertain table
void API::IndexCreate(string indexName, string TableName, string attributeName) {
    Index temp_index;
    temp_index.AttriName = attributeName;
    temp_index.name = indexName;
    temp_index.TableName = TableName;
    int cnt = 0;
    Table* t = cm->SelectTable(TableName);
    //找到对应属性为第几个属性
    for (int i = 0; i < t->attri.size(); i++) { //IndexAddr 下标从0开始，0表示第一个属性
        if (t->attri[i].name == attributeName) {
            break;
        }
        else {
            cnt++;
        }
    }
    temp_index.IndexAddr = cnt;
    int result = cm->CreateNewIndex(&temp_index);//创建索引
    if (!result) {
        cout << "Create index " << indexName << " successfully !" << endl;
    }
    else if (result == 1) {
        cout << "Create index failed because of the table " << TableName << " doesn't exists !" << endl;
    }
    else if (result == 2) {
        cout << "Create index failed because of the index " << indexName << " already exists !" << endl;
    }

    //为已经存在的数据建立索引
    int len = rm->Getlen(*t);
    t->blockNum = rm->GetBlockNum(*t);
    vector<int>Tuple_offset;

    //BufferManager* bm = new BufferManager;
    //rm->UpdateBufferManager(bm);

    switch (t->attri[cnt].type) {
    case 0: {
        vector<int>record;
        vector<value>temp=rm->FindValue(*t, attributeName);
        for (int i = 0; i < temp.size(); i++) {
            record.push_back(get<int>(temp[i]));
        }
        Tuple_offset = rm->FindValueOffset(*t, attributeName);
        IndexManager<int> im(indexName, TableName, attributeName, 4, bm);
        im.createIndex();
        im.initialize(record, Tuple_offset);//insert tuple to B+ tree
        //im.printBPT();
        break;
    }
    case 1: {
        vector<float>record;
        vector<value>temp = rm->FindValue(*t, attributeName);
        for (int i = 0; i < temp.size(); i++) {
            record.push_back(get<float>(temp[i]));
        }
        Tuple_offset = rm->FindValueOffset(*t, attributeName);
        IndexManager<float> im(indexName, TableName, attributeName, 4, bm);
        im.createIndex();
        im.initialize(record, Tuple_offset);//insert tuple to B+ tree
        break;
    }
    case 2: {
        vector<string>record;
        vector<value>temp = rm->FindValue(*t, attributeName);
        for (int i = 0; i < temp.size(); i++) {
            record.push_back(get<string>(temp[i]));
        }
        Tuple_offset = rm->FindValueOffset(*t, attributeName);
        IndexManager<string> im(indexName, TableName, attributeName, t->attri[cnt].length, bm);
        im.createIndex();
        im.initialize(record, Tuple_offset);//insert tuple to B+ tree
        break;
    }
    }
}


//select data without condition
void API::SelectData(string TableName, vector<Attribute>* attr) {
    if (ExistTable(TableName)) { //the table exists 
        vector<Condition> con;
        SelectData(TableName, attr, &con);
    }
    else {
        cout << "The table " << TableName << " doesn't exists !" << endl;
    }
}


//get the attribute of certain table
bool API::AttributeGet(string TableName, vector<Attribute>* attr) {
    //int index=bm->ReadIndexFromFile(TableName,0);
    if (ExistTable(TableName)){
        Table* t = cm->SelectTable(TableName);
        for (int i = 0; i < t->attri.size(); i++) {
            attr->push_back(t->attri[i]);
        }
        return true;
    }
    else {
        return false;
    }
}


//show the attribute name
void API::AttributeShow(vector<Attribute>attr) {
    for (int i = 0; i < attr.size(); i++) { //左对齐，位宽为10
        cout <<left<<setw(10)<<attr[i].name ;
    }
    cout << endl;
}



//select data with select condition
//attr is the vector of attribute you wanna show
//con is the vector of condition that you wanna the record satisfy
void API::SelectData(string TableName, vector<Attribute>* attr, vector<Condition>* con) {
    if (ExistTable(TableName)) {//the table exists
        Table* t = cm->SelectTable(TableName);
        t->blockNum = rm->GetBlockNum(*t);

        vector<Attribute>attr_temp;
        AttributeGet(TableName, &attr_temp); //get the attribute

        vector<string>AttributeName;
        FindAttributeIndex(t->index,&AttributeName,t);
        vector<Condition>con_set = *con;

        //BufferManager* bm = new BufferManager;
        //rm->UpdateBufferManager(bm);

        if (!attr->size()) {//show all attribute
            AttributeShow(attr_temp);
            if (con->size()) {//with condition bound
                if (con->size() == 1 && (*con)[0].type == EQ) {//Equal check
                    int temp = -1;
                    for (int i = 0; i < t->attri.size(); i++) {
                        if (t->attri[i].name == (*con)[0].attri) {
                            temp = i;
                            break;
                        }
                    }
                    bool flag = false;
                    int j = 0;
                    for (int i = 0; i < t->index.size(); i++) {
                        if (t->index[i].IndexAddr == temp) {
                            flag = true;
                            j = i;
                            break;
                        }
                    }
                    if (flag) {
                        switch (t->attri[temp].type) {
                        case 0: {
                            IndexManager<int>im(t->index[j].name ,t->name,t->attri[temp].name,4,bm);
                            vector<int>result=im.rangesearch(get<int>((*con)[0].val),get<int>( (*con)[0].val));
                            rm->ShowData(result, t);
                            break;
                        }
                        case 1: {
                            IndexManager<float>im(t->index[j].name, t->name, t->attri[temp].name, 4, bm);
                            vector<int>result = im.rangesearch(get<float>((*con)[0].val), get<float>((*con)[0].val));
                            rm->ShowData(result, t);
                            break;
                        }
                        case 2: {
                            IndexManager<string>im(t->index[j].name, t->name, t->attri[temp].name, t->attri[temp].length, bm);
                            vector<int>result = im.rangesearch(get<string>((*con)[0].val), get<string>((*con)[0].val));
                            rm->ShowData(result, t);
                            break;
                        }
                        }
                    }
                    else {
                        vector<Attribute>attr_void;
                        rm->RecordConditionCheck(*t, attr_void, *con);
                    }
                }
                else {
                    vector<Attribute>attr_void;
                    rm->RecordConditionCheck(*t, attr_void, *con);
                }
            }
            else { // no condition bound ,we don't use index to show tuple
                vector<Attribute>attr_void;
                vector<Condition>con_void;
                rm->RecordConditionCheck(*t, attr_void, con_void);
            }
        }
        else { // show certain attribute
            bool flag = CheckAttribute(*attr, attr_temp);
            if (flag) {
                AttributeShow(*attr);
                if (con->size()) {//with condition bound
                    if (AttributeName.size()) {//the index attribute exists
                        vector<int>Tuple_offset;
                        for (int i = 0; i < (*con).size(); i++) {//check each condition
                            bool flag = false;
                            vector<int>temp_Tuple;
                            for (int j = 0; j < AttributeName.size(); j++) {
                                if (con_set[i].attri == AttributeName[j]) {// the attribute have index
                                    temp_Tuple = Index_Condition_Check(con_set[i], *t); //find all tuple offset according to the condition and index
                                    flag = true;
                                    break;
                                }
                                //rm->RecordDeleteCondition(*t, *con);//delete record
                            }
                            if (!flag) {//the attribute doesn't have index on it
                                temp_Tuple = No_Index_Condition_Check(con_set[i], *t); //find tuple
                            }

                            //update Tuple_offset to find the repeated tuples
                            if (!i) {// the first tuple offset vector
                                Tuple_offset = temp_Tuple;
                            }
                            else { // have more than one tuple offset vector
                                Tuple_offset = Combine_two(Tuple_offset, temp_Tuple); //find the repeat offset
                            }
                        }
                        for (int i = 0; i < Tuple_offset.size(); i++) { // delete the tuple
                            int m = Tuple_offset[i] / 4096;//get the block number
                            int offset_now = Tuple_offset[i] % 4096;
                            int index = bm->ReadIndexFromFile(TableName, m);
                            char* p = bm->ReadBlockFromFile(index);
                            Tuple temp_tuple = rm->getTuple(p + offset_now, *t);
                            for (int j = 0; j < (*attr).size(); j++) {//show all data from this tuple
                                //cout << left << setw(10) << temp_tuple.v[j];
                                for (int k = 0; k < t->attri.size(); k++) {
                                    if (t->attri[k].name == (*attr)[j].name) {//show certain attribute
                                        cout << left << setw(10) << temp_tuple.v[i];
                                        break;
                                    }
                                }
                            }
                            cout << endl;
                        }
                    }
                    else { // no index attribute
                        rm->RecordConditionCheck(*t, *attr, *con);
                    }
                }
                else {//no condition bound
                    vector<Condition>con_void;
                    rm->RecordConditionCheck(*t, *attr, con_void);
                }
            }
        }
        /*
        vector<Tuple> tuple;
        tuple = rm->SelectRecord(*t, (*con)[0]);
        for (int i = 0; i < tuple.size(); i++) {
            for (int j = 0; j < tuple[i].v.size(); j++) { //show each tuple
                cout << tuple[i].v[j] << ' ';
            }
            cout << endl; //change the line
        }
        */
    }
    else {
        cout << "The table " << TableName << " doesn't exists !" << endl;
    }
}


vector<int> API::Unique_Tuple(vector<int> few, vector<int>large) {// find all unique tuple offset
    vector<int>result;
    int f_index, l_index;
    f_index = 0;
    l_index = 0;
    while (1) {
        if (few[f_index] == large[l_index]) {
            f_index++;
            l_index++;
        }
        else if (few[f_index] > large[l_index]) {
            result.push_back(large[l_index]);
            l_index++;
        }
        else if (few[f_index] < large[l_index]) {
            f_index++;
        }

        if (f_index == few.size()) {
            break;
        }
    }
    while (l_index < large.size()) {
        result.push_back(large[l_index++]);
    }
    return result;
}


template<typename T>
vector<int> API::RangeSearch(IndexManager<T> im, Condition con, Table t, T value) { // range search using IndexManager
    vector<int>result;
    int len = rm->Getlen(t); //one tuple length
    if (con.type == EQ) {
        result = im.rangesearch(value, value);
    }
    else if (con.type == NE) {
        result = No_Index_Condition_Check(con, t);// doesn't need to use B+ tree
        //result = im.
    }
    else if (con.type == LT) {
        vector<int>temp_result = im.rangesearch(value, value);//find the EQ tuple
        vector<int>temp_result_2; // find the LE tuple
        if constexpr (is_same<T, string>()) {
            string s = "";
            temp_result_2 = im.rangesearch(s, value);
        }
        else if constexpr (is_same<T, float>()) {
            temp_result_2 = im.rangesearch(-100000.00, value);
        }
        else if constexpr (is_same<T, int>()) {
            temp_result_2 = im.rangesearch(-100000, value);
        }
        result = Unique_Tuple(temp_result, temp_result_2);
    }
    else if (con.type == LE) {
        if constexpr (is_same<T, string>()) {
            string s = "";
            result = im.rangesearch(s, value);
        }
        else if constexpr (is_same<T, float>()) {
            result = im.rangesearch(-100000.00, value);
        }
        else if constexpr (is_same<T, int>()) {
            result = im.rangesearch(-100000, value);
        }
    }
    else if (con.type == GT) {
        vector<int>temp_result = im.rangesearch(value, value);//find the EQ tuple
        vector<int>temp_result_2; // find the LE tuple
        if constexpr (is_same<T, string>()) {
            string s = "";
            temp_result_2 = im.rangesearch(value, s);
        }
        else if constexpr (is_same<T, float>()) {
            temp_result_2 = im.rangesearch(value, 100000.00);
        }
        else if constexpr (is_same<T, int>()) {
            temp_result_2 = im.rangesearch(value, 100000);
        }
        result = Unique_Tuple(temp_result, temp_result_2);
    }
    else if (con.type == GE) {
        if constexpr (is_same<T, string>()) {
            string s = "zzzzzzzzzzzz";
            result = im.rangesearch(value, s);
        }
        else if constexpr (is_same<T, float>()) {
            result = im.rangesearch(value, 100000.00);
        }
        else if constexpr (is_same<T, int>()) {
            result = im.rangesearch(value, 100000);
        }
    }
    return result;
}


vector<int> API::Index_Condition_Check(Condition con, Table t) { //return the offset of tuple which satisfy the condition
    vector<int>offset;
    int n = t.index.size();
    string IndexName, AttributeName;

    //BufferManager* bm = new BufferManager;
    //rm->UpdateBufferManager(bm);

    for (int i = 0; i < n; i++) {
        if (t.index[i].AttriName == con.attri) {//find the matching indexName
            IndexName = t.index[i].name;
            AttributeName = t.index[i].AttriName;
        }
    }
    Attribute temp_attr;
    for (int i = 0; i < t.attri.size(); i++) {
        if (t.attri[i].name == AttributeName) {
            temp_attr = t.attri[i];
            break;
        }
    }
    switch (temp_attr.type) {
    case 0: {//int
        IndexManager<int> im(IndexName, t.name, AttributeName, 0, bm);
        offset = RangeSearch<int>(im, con, t, get<int>(con.val));
    }
    case 1: {//float
        IndexManager<float> im(IndexName, t.name, AttributeName, 0, bm);
        offset = RangeSearch<float>(im, con, t, get<float>(con.val));
    }
    case 2: {//string
        IndexManager<string> im(IndexName, t.name, AttributeName, temp_attr.length, bm);
        offset = RangeSearch<string>(im, con, t, get<string>(con.val));
    }
    }

    return offset;
}


int API::min_vector(int a, int b) {
    if (a > b) {
        return 1;
    }
    else {
        return 0;
    }
}

vector<int> API::Combine_two(vector<int> a, vector<int>b) {//combine the tuple offset if both a and b have the offset
    vector<int>result;
    int a_index, b_index;
    a_index = 0;
    b_index = 0;
    int least_vector = min_vector(a.size(), b.size());

    while (1) { //double pointer method
        if (a[a_index] == b[b_index]) {
            a_index++;
            b_index++;
            result.push_back(a[a_index]);
        }
        else if (a[a_index] > b[b_index]) {
            b_index++;
        }
        else if (a[a_index] < b[b_index]) {
            a_index++;
        }

        if (least_vector) {//b has smaller size
            if (b_index == b.size()) {
                break;
            }
        }
        else { // a has smaller size
            if (a_index == a.size()) {
                break;
            }
        }
    }

    return result;
}

vector<int> API::No_Index_Condition_Check(Condition con, Table t) {//condition check without index
    vector<int>result;
    int n = rm->GetBlockNum(t);
    int len = rm->Getlen(t);
    Attribute temp_attr;
    //BufferManager* bm = new BufferManager;
    //rm->UpdateBufferManager(bm);
    for (int i = 0; i < t.attri.size(); i++) {
        if (t.attri[i].name == con.attri) {//find the matching attribute
            temp_attr = t.attri[i];
            break;
        }
    }
    for (int i = 0; i < n; i++) {//check each block
        int index = bm->ReadIndexFromFile(t.name, i);
        char* p = bm->ReadBlockFromFile(index);
        for (int offset = 0; offset + len < 4096; offset += len) {//check each tuple
            if (p[offset]) {//the tuple exists
                switch (temp_attr.type) {
                case 0: {
                    int temp_offset = rm->FindAttributeOffset(t, temp_attr);
                    int num = *(int*)(p + offset + temp_offset);
                    if (rm->ConditionCheck<int>(num, get<int>(con.val), con)) { // this tuple satisfy the condition
                        result.push_back(offset + 4096 * i); //把offset压如堆栈
                    }
                    break;
                }
                case 1: {
                    int temp_offset = rm->FindAttributeOffset(t, temp_attr);
                    float num = *(float*)(p + offset + temp_offset);
                    if (rm->ConditionCheck<float>(num, get<float>(con.val), con)) { // this tuple satisfy the condition
                        result.push_back(offset + 4096 * i); //把offset压如堆栈
                    }
                }
                case 2: {
                    int temp_offset = rm->FindAttributeOffset(t, temp_attr);
                    string num = string((char*)(p + offset + temp_offset));
                    if (rm->ConditionCheck<string>(num, get<string>(con.val), con)) { // this tuple satisfy the condition
                        result.push_back(offset + 4096 * i); //把offset压如堆栈
                    }
                }
                }
            }
        }
    }
    return result;
}

//Delete all tuple
void API::DeleteTuple(string TableName) {
    vector<Condition>con;
    DeleteTuple(TableName, &con);
}



//Delete tuple according to the condition
void API::DeleteTuple(string TableName, vector<Condition>* con) {
    if (ExistTable(TableName)) {
        vector<Attribute> attr;
        AttributeGet(TableName, &attr);
        vector<Condition> con_set = *con;

        Table* t = cm->SelectTable(TableName);
        t->blockNum = rm->GetBlockNum(*t);
        vector<string>indexName = cm->FindAllIndex(*t);

        vector<string>AttributeName;
        FindAttributeIndex(t->index,&AttributeName,t);

        //BufferManager* bm = new BufferManager;
        //rm->UpdateBufferManager(bm);
        /*
        if (AttributeName.size()) {//the index exists
            vector<int>Tuple_offset;
            for (int i = 0; i < (*con).size(); i++) {//check each condition
                bool flag = false;
                vector<int>temp_Tuple;
                for (int j = 0; j < indexName.size(); j++) {
                    if (con_set[i].attri == AttributeName[j]) {// the attribute have index
                        temp_Tuple = Index_Condition_Check(con_set[i], *t); //find all tuple offset according to the condition and index
                        flag = true;
                        break;
                    }
                    //rm->RecordDeleteCondition(*t, *con);//delete record
                }
                if (!flag) {//the attribute doesn't have index on it
                    temp_Tuple = No_Index_Condition_Check(con_set[i], *t); //find tuple
                }

                //update Tuple_offset to find the repeated tuples
                if (!i) {// the first tuple offset vector
                    Tuple_offset = temp_Tuple;
                }
                else { // have more than one tuple offset vector
                    Tuple_offset = Combine_two(Tuple_offset, temp_Tuple); //find the repeat offset
                }
            }
            for (int i = 0; i < Tuple_offset.size(); i++) { // delete the tuple
                int m = Tuple_offset[i] / 4096;//get the block number
                int offset_now = Tuple_offset[i] % 4096;
                int index = bm->ReadIndexFromFile(TableName, m);
                char* p = bm->ReadBlockFromFile(index);
                *(p + offset_now) = 0;//lazy delete
            }

        }*/
        //else {// the index doesn't exists
            rm->RecordDeleteCondition(*t, *con);//delete record
        //}

        cout << "Delete record from table " << TableName << " successfully !" << endl;
    }
    else {
        //cout << "test" << endl;
        cout << "The record from table " << TableName << " doesn't exists !" << endl;
    }
}



//insert one tuple
void API::InsertTuple(string TableName, Tuple record) {
    if (ExistTable(TableName)) {
        Table* t = cm->SelectTable(TableName);
        t->blockNum = rm->GetBlockNum(*t); //update the block number
        

        //cout << "The Block num used is : " << t->blockNum<<"  ";
        int offset = rm->InsertRecord(*t, record);
        if (offset!=-1) {
            for (int i = 0; i < t->index.size(); i++) {
                UpdateIndex(offset, t->index[i], *t , record);
            }
            cout << "Insert record successfully !" << endl;
        }
    }
    else {
        cout << "The table " << TableName << " doesn't exists !" << endl;
    }
}



//check wether the attribute matches the table
bool API::CheckAttribute(vector<Attribute>attr,vector<Attribute>all) {
    bool ret=true;
    for (int i = 0; i < attr.size(); i++) {//the attribute you wanna show
        for(int j=0;j<all.size();j++){
            if (all[j].name == attr[i].name ) {
                if (attr[i].type == all[j].type) {
                    if (attr[i].type == 2 && attr[i].length == all[j].length) { //string type
                        break;
                    }
                    else if (attr[i].type == 1 || attr[i].type == 0) { // int or float type
                        break;
                    }
                    else { //string type but length doesn't matches 
                        ret = false;
                    }
                }
                else {
                    ret = false;
                }
            }
        }
    }
    return ret;
}


// find all attribute name which have bulid index
void API::FindAttributeIndex(vector<Index>temp, vector<string>* a, Table* t) {
    for (int i = 0; i < temp.size(); i++) {
        int j = temp[i].IndexAddr;
        a->push_back(t->attri[j].name);
    }
}


//find all indexName 
void API::FindIndexName(vector<Index>temp,vector<string>*result) {
    for (int i = 0; i < temp.size(); i++) {
        result->push_back(temp[i].name);
    }
    //return result;
}



//update the B+tree
void API::UpdateIndex(int offset, Index id,Table t,Tuple record) {
    int j = id.IndexAddr;
    string attributeName = t.attri[j].name;
    string indexName = id.name;

    switch (t.attri[j].type)
    {
        case 0: {
            IndexManager<int> im(indexName, t.name, attributeName, 4, bm);
            im.insertkey(get<int>(record.v[j]), offset);
            //im.printBPT();
            break;
        }
        case 1:{
            IndexManager<float> im(indexName, t.name, attributeName, 4, bm);
            im.insertkey(get<float>(record.v[j]), offset);
            //im.printBPT();
            break;
        }
        case 2: {
            IndexManager<string> im(indexName, t.name, attributeName, t.attri[j].length, bm);
            im.insertkey(get<string>(record.v[j]), offset);
            //im.printBPT();
            break;
        }
    }
}

