#include "Interpreter.h"
#include<iostream>
#include<algorithm>
#include<ctime>
using namespace std;

Interpreter::Interpreter() { this->sql = ""; };
Interpreter::~Interpreter() {};

//��ʼ����ѯ����ֵ
void Interpreter::get_sql(const std::string& sql) {
    this->sql = sql;
    return;
};

//�����з�ȫ�����ɿո�
void Interpreter::into_one_line() {

    while (1) {
        int pos = 0;
        if ((pos = sql.find("\r")) != string::npos)
            sql.replace(pos, 1, " ");
        else break;
    }
    return;
};

//����make_regular()��������������ǽ����ʡ����š���ֵһ��һ���ĸ���
//������ͷ�ͽ�β�Ķ���ո�
void Interpreter::make_regular() {

    int start_pos = 0;
    int end_pos;
    for (; start_pos < sql.length(); start_pos++)
    {
        if (sql[start_pos] != ' ')
        {
            break;
        }
    }

    sql.erase(0, start_pos);
    for (end_pos = sql.length() - 1; end_pos >= 0; end_pos--)
    {
        if (sql[end_pos] != ' ')
        {
            break;
        }
    }
    sql.erase(end_pos + 1, sql.length() - end_pos - 1);

    //�ڵ��ʺͱ������Լ������֮����Ͽո�
    int pos = 0;
    int len = sql.length();
    //���źͶ��ź͵�����
    while (pos < len)
    {
        int flag = 0;
        if (sql[pos] == '(' || sql[pos] == ')' || sql[pos] == ',' || sql[pos] == '\'')
        {
            if (pos + 1 < len && sql[pos + 1] != ' ')
            {
                sql.insert(pos + 1, " ");
                flag = 1;
            }
            if (pos - 1 >= 0 && sql[pos - 1] != ' ')
            {
                sql.insert(pos, " ");
                flag = 1;
            }
            if (flag) {
                pos = 0;
                len = sql.length();
            }
            else {
                pos++;
            }

        }
        else {
            pos++;
        }
    }

    //�ֺ�
    pos = sql.length() - 1;
    while (pos >= 0)
    {
        if (sql[pos] == ';' && pos - 1 >= 0 && sql[pos - 1] != ' ')
        {
            sql.insert(pos, " ");
            break;
        }
        else {
            pos--;
        }
    }

    //����=,<=,>=,���߼ӿո�
    pos = 0;
    len = sql.length();
    while (pos < sql.length())
    {
        int flag = 0;
        if (sql[pos] == '=')
        {
            if (pos - 1 >= 0 && sql[pos - 1] != '<' && sql[pos - 1] != '>')
            {
                if (pos + 1 < len && sql[pos + 1] != ' ')
                {
                    sql.insert(pos + 1, " ");
                    flag = 1;
                }
                if (pos - 1 >= 0 && sql[pos - 1] != ' ')
                {
                    sql.insert(pos, " ");
                    flag = 1;
                }
            }
            else if (pos - 1 >= 0)
            {
                if (pos + 1 < len && sql[pos + 1] != ' ')
                {
                    sql.insert(pos + 1, " ");
                    flag = 1;
                }
                if (pos - 2 >= 0 && sql[pos - 2] != ' ')
                {
                    sql.insert(pos - 1, " ");
                    flag = 1;
                }
            }
            if (flag) {
                pos = 0;
                len = sql.length();
            }
            else {
                pos++;
            }
        }
        else {
            pos++;
        }
    }
    //����>,<,<>
    pos = 0;
    len = sql.length();
    while (pos < sql.length())
    {
        int flag = 0;
        if (sql[pos] == '<')
        {
            if (pos + 1 < len && sql[pos + 1] == '>')
            {
                if (pos + 2 < len && sql[pos + 2] != ' ')
                {
                    sql.insert(pos + 2, " ");
                    flag = 1;
                }
                if (pos - 1 >= 0 && sql[pos - 1] != ' ')
                {
                    sql.insert(pos, " ");
                    flag = 1;
                }
            }
            else if (pos + 1 < len && sql[pos + 1] != '=' && (sql[pos + 1] != ' ' || pos - 1 >= 0 && sql[pos - 1] != ' '))
            {
                flag = 1;
                sql.insert(pos + 1, " ");
                if (pos - 1 >= 0)
                {
                    sql.insert(pos, " ");
                }
            }
            if (flag) {
                pos = 0;
                len = sql.length();
            }
            else {
                pos++;
            }
        }
        else if (sql[pos] == '>')
        {
            if (sql[pos - 1] != '<' && pos + 1 < len && sql[pos + 1] != '=' && (sql[pos + 1] != ' ' || pos - 1 >= 0 && sql[pos - 1] != ' '))
            {
                flag = 1;
                sql.insert(pos + 1, " ");
                if (pos - 1 >= 0)
                {
                    sql.insert(pos, " ");
                }
            }
            if (flag) {
                pos = 0;
                len = sql.length();
            }
            else {
                pos++;
            }
        }
        else {
            pos++;
        }

    }
    //���������������ո�
    pos = 0;
    len = sql.length();
    while (pos < len)
    {
        if (sql[pos] == ' ' && pos + 1 < len && sql[pos + 1] == ' ')
        {
            sql.erase(pos, 1);
            pos = 0;
            len = sql.length();
        }
        else {
            pos++;
        }
    }
    return;
};


void Interpreter::into_words() {
    //�������õ�sql���ĸ����ʡ����š���ֵ���뵽words��ȥ
    std::string word = "";
    vector <string>::iterator iter = words.begin();
    for (; iter != words.end();)
    {
        iter = words.erase(iter);
    }

    for (int i = 0; i < sql.length(); i++)
    {
        if (sql[i] != ' ')
        {
            word.append(1, sql[i]);
        }
        else {
            words.push_back(word);
            word = "";
        }
    }
    words.push_back(word);
    return;
};


int Interpreter::get_type() {
    //�ж�sql���������ʲô�����飬����sql_type,SQL_TYPE��Defines.h
    //ret=0��ʾ����ʶ��ret=1��ʾ�����ж�������
    int ret = 0;
    if (words.size() == 0)
    {
        ret = -1;
    }
    else {
        if (words[0] == ";") {
            sql_type = no_op;
        }
        else if (words[0] == "create") {
            if (words.size() == 1)
            {
                ret = -1;
            }
            else {
                if (words[1] == "table")
                {
                    sql_type = create_table;
                }
                else if (words[1] == "index") {
                    sql_type = create_index;
                }
                else {
                    ret = -1;
                }
            }
        }
        else if (words[0] == "drop") {
            if (words.size() == 1)
            {
                ret = -1;
            }
            else {
                if (words[1] == "table") {
                    sql_type = drop_table;
                }
                else if (words[1] == "index") {
                    sql_type = drop_index;
                }
                else {
                    ret = -1;
                }
            }
        }
        else if (words[0] == "insert") {
            sql_type = insert_record;
        }
        else if (words[0] == "delete") {

            sql_type = delete_record;
        }
        else if (words[0] == "select") {

            sql_type = select_record;
        }
        else if (words[0] == "quit") {

            sql_type = quit_system;
        }
        else if (words[0] == "execfile") {

            sql_type = exec_file;
        }
        else {

            ret = -1;
        }
    }
    ret += 1;
    //cout << sql_type << endl;
    return ret;
}

int Interpreter::run_sql() {
    into_one_line();
    make_regular();
    into_words();
    int type_ret = get_type();
    clock_t time = clock();
    // ������sql�������ʱ��һ������sql���
    if (type_ret) {
        if (sql_type == no_op) {
            //sql=";" ɶҲ����
        }
        else if (sql_type == create_table) {
            //���е�try-catch ����Ϊ�˲�׽sql��䲻��������ᵼ��words����Խ�磩
            //���е�goto Error_here ����syntax error
            try {
                bool has_primary = false;       //has_primary ��¼��֮ǰ��û�г��ֹ�primary key
                if (words.size() < 3) {
                    throw "Syntax Error";
                }
                std::string t_name = words[2];  //table_name zs
                if (words.size() < 4) {
                    throw "Syntax Error";
                }
                if (words[3] != "(")
                {
                    goto Error_here;
                }
                vector<Attribute> attrs;        //��������
                Attribute ati;                  //��ŵ�ǰ��ȡ������
                if (words.size() < 5) {
                    throw "Syntax Error";
                }
                int pos = 4;                    //���Կ�ʼ���±�
                while (1)
                {
                    if (words.size() < pos + 1) {
                        throw "Syntax Error";
                    }
                    if (words[pos] == ";") {
                        break;
                    }
                    else if (words[pos] == ")")      //����¼�����
                    {
                        if (words.size() < pos + 2) {
                            throw "Syntax Error";
                        }
                        if (words[pos + 1] == ";")
                        {
                            break;              //��������
                        }
                        else {
                            goto Error_here;
                        }

                    }
                    //primary key��������ʽ����
                    else if (words[pos] == "primary") {
                        if (has_primary)
                        {
                            cout << "error: can have only one primary key!" << endl;
                            return 0;
                        }
                        else {
                            has_primary = true;
                        }
                        if (words.size() < pos + 5) {
                            throw "Syntax Error";
                        }
                        if (words[pos + 1] == "key" && words[pos + 2] == "(" && words[pos + 4] == ")")
                        {
                            //attrs[words[pos+3]].isPrimary=1;
                            for (int i = 0; i < words.size(); i++) {
                                if (attrs[i].name == words[pos + 3]) {
                                    attrs[i].isPrimary = 1;
                                    break;
                                }
                                if (i + 1 == words.size()) {
                                    cout << "no such attribute can be set as primary key" << endl;
                                    return 0;
                                }
                            }
                        }
                        else {
                            goto Error_here;
                        }
                        pos += 5;
                        if (words[pos] == "," || words[pos] == ")")
                        {
                            pos++;
                            continue;
                        }
                    }
                    if (words.size() < pos + 1) {
                        throw "Syntax Error";
                    }
                    //��������(�޸�)
                    ati.name = words[pos];
                    if (words.size() < pos + 2) {
                        throw "Syntax Error";
                    }
                    if (words[pos + 1] == "int")
                    {
                        ati.type = INT;
                        pos += 2;
                    }
                    else if (words[pos + 1] == "float") {
                        ati.type = FLOAT;
                        pos += 2;
                    }
                    else if (words[pos + 1] == "char") {
                        ati.type = STRING;
                        if (words.size() < pos + 5) {
                            throw "Syntax Error";
                        }
                        if (words[pos + 2] != "(" || words[pos + 4] != ")")
                        {
                            goto Error_here;
                        }
                        ati.length = atoi(words[pos + 3].c_str()); //ע��
                        if (atoi(words[pos + 3].c_str()) > 64 || atoi(words[pos + 3].c_str()) < 0)
                        {
                            cout << "error:illegal size of char()!" << endl;
                            return 0;
                        }

                        pos += 5;
                    }
                    else {
                        goto Error_here;
                    }
                    if (words.size() < pos + 2) {
                        throw "Syntax Error";
                    }
                    if (words[pos] == ",")
                    {
                        ati.isUnique = 0;
                        ati.isNULL = 1;
                        ati.isPrimary = 0;
                        pos++;
                    }
                    else if (words[pos] == "unique")
                    {
                        ati.isUnique = 1;
                        ati.isNULL = 1;
                        ati.isPrimary = 0;
                        pos++;
                        if (words.size() < pos + 1) {
                            throw "Syntax Error";
                        }
                        if (words[pos] == ",") {
                            pos++;

                        }

                    }
                    else if (words[pos] == "not" && words[pos + 1] == "null")
                    {
                        ati.isUnique = 0;
                        ati.isNULL = 0;
                        ati.isPrimary = 0;
                        pos += 2;
                        if (words[pos] == ",") {
                            pos++;

                        }
                    }
                    else {
                        ati.isUnique = 0;
                        ati.isNULL = 1;
                        ati.isPrimary = 0;
                    }
                    attrs.push_back(ati);
                }

                api.TableCreate(t_name, attrs);  //���ú���
            }
            catch (...)
            {
                goto Error_here;
            }

        }
        else if (sql_type == drop_table) {
            try
            {
                if (words.size() < 3) {
                    throw "Syntax Error";
                }
                std::string t_name = words[2];
                if (words.size() < 4) {
                    throw "Syntax Error";
                }
                if (words[3] != ";")
                {
                    goto Error_here;
                }

                api.TableDrop(t_name);
            }
            catch (...)
            {
                goto Error_here;
            }
        }
        else if (sql_type == create_index) {
            try
            {
                if (words.size() < 9) {
                    throw "Syntax Error";
                }
                if (words[3] != "on" || words[5] != "(" || words[7] != ")" || words[8] != ";")
                {
                    goto Error_here;
                }
                std::string i_name = words[2], t_name = words[4], a_name = words[6];
                vector<Attribute>* attr_temp = new vector<Attribute>;
                api.AttributeGet(t_name, attr_temp);
                int j;
                for (j = 0; j < (*attr_temp).size(); j++) {
                    if ((*attr_temp)[j].name == a_name) {
                        break;
                    }
                }
                if(j < (*attr_temp).size()){
                    api.IndexCreate(i_name, t_name, a_name);
                }
                else {
                    cout << "Create index failed because the attribute doesn't exists on this table !" << endl;
                }

                //cout << i_name << t_name << a_name << endl;
            }
            catch (...)
            {
                goto Error_here;
            }
        }
        else if (sql_type == drop_index) {
            try
            {
                if (words.size() < 4) {
                    throw "Syntax Error";
                }
                if (words[3] != ";")
                {
                    goto Error_here;
                }
                std::string i_name = words[2];
                //cout << i_name << endl;;
                api.IndexDrop(i_name);
            }
            catch (...)
            {
                goto Error_here;
            }
        }
        else if (sql_type == insert_record) {
            try
            {
                if (words.size() < 5) {
                    throw "Syntax Error";
                }
                if (words[1] != "into" || words[3] != "values" || words[4] != "(")
                {
                    goto Error_here;
                }
                std::string t_name = words[2];
                Tuple record;
                int pos = 5;
                //��ʼ¼��ֵ��tuple��
                vector<Attribute>* attrs = new vector<Attribute>;
                if (!api.AttributeGet(t_name, attrs)) {
                    cout << "The table doesn't exists !" << endl;
                    return 0;
                }
                int atnum = (*attrs).size();
                int idx = 0;
                while (1)
                {
                    if (words.size() < pos + 1) {
                        throw "Syntax Error";
                    }
                    if (words[pos] == ",")
                    {
                        pos++;
                        continue;
                    }
                    else if (words[pos] == ")")
                    {
                        if (words.size() < pos + 2) {
                            throw "Syntax Error";
                        }
                        if (words[pos + 1] != ";") {
                            goto Error_here;
                        }
                        break;
                    }
                    else {
                        int temp1;
                        float temp2;
                        idx++;
                        if (idx > atnum)
                        {
                            cout << "the number of values don't match!" << endl;
                            return 0;
                        }
                        if ((*attrs)[idx - 1].type == INT)
                        {
                            temp1 = atoi(words[pos].c_str());
                            record.v.push_back(temp1);
                            pos++;
                        }
                        else if (int((*attrs)[idx - 1].type) == FLOAT)
                        {
                            temp2 = atof(words[pos].c_str());
                            record.v.push_back(temp2);
                            pos++;
                        }
                        else
                        {
                            if (words.size() < pos + 3) {
                                throw "Syntax Error";
                            }
                            record.v.push_back(words[pos + 1]);
                            if (words[pos] != "'" || words[pos + 2] != "'")
                            {
                                goto Error_here;
                            }
                            pos += 3;
                        }

                    }

                }
                if (idx != atnum)
                {
                    cout << "the number of values don't match!" << endl;
                    return 0;
                }
                api.InsertTuple(t_name, record);
            }
            catch (...)
            {
                goto Error_here;
            }
        }
        else if (sql_type == delete_record) {
            try
            {
                if (words.size() < 2) {
                    throw "Syntax Error";
                }
                if (words[1] != "from")
                {
                    goto Error_here;
                }
                else {
                    if (words.size() < 3) {
                        throw "Syntax Error";
                    }
                    std::string t_name = words[2];
                    if (words.size() < 4) {
                        throw "Syntax Error";
                    }
                    if (words.size() == 4 && words[3] == ";")
                    {
                        api.TableDrop(t_name);

                    }
                    else if (words[3] == "where")
                    {
                        int pos = 4;
                        vector<Condition>* cons = new vector<Condition>; //�޸�
                        vector<Attribute>* attrs = new vector<Attribute>;
                        if (!api.AttributeGet(t_name, attrs)) {
                            cout << "The table doesn't exists !" << endl;
                            return 0;
                        }
                        int i = 0;
                        Condition con;
                        while (1)
                        {
                            if (words.size() < pos + 1) {
                                throw "Syntax Error";
                            }
                            if (words[pos] == "and")
                            {
                                pos++;
                                continue;
                            }
                            else if (words[pos] == ";")
                            {
                                break;
                            }


                            con.attri = words[pos];
                            for (i = 0; i < (*attrs).size(); i++)
                            {
                                if ((*attrs)[i].name == words[pos])
                                    break;
                            }
                            if (i == (*attrs).size()) {
                                cout << "The attribute you wanna delete doesn't exists in this table !" << endl;
                                return 0;
                            }

                            if (words.size() < pos + 2) {
                                throw "Syntax Error";
                            }
                            if (words[pos + 1] == "=")
                            {
                                con.type = EQ;
                            }
                            else if (words[pos + 1] == ">=") {
                                con.type = GE;
                            }
                            else if (words[pos + 1] == "<=") {
                                con.type = LE;
                            }
                            else if (words[pos + 1] == ">") {
                                con.type = GT;
                            }
                            else if (words[pos + 1] == "<") {
                                con.type = LT;
                            }
                            else if (words[pos + 1] == "<>") {
                                con.type = NE;
                            }
                            else {
                                goto Error_here;
                            }
                            if (words.size() < pos + 3 || words[pos + 2] == "'" && words.size() < pos + 5) {
                                throw "Syntax Error";
                            }
                            if ((*attrs)[i].type == STRING)
                            {
                                con.val = words[pos + 3];
                                pos += 5;
                            }
                            else if ((*attrs)[i].type == FLOAT)
                            {
                                con.val = float(atof(words[pos + 2].c_str()));
                                pos += 3;
                            }
                            else {
                                con.val = atoi(words[pos + 2].c_str());
                                pos += 3;
                            }
                            (*cons).push_back(con);
                        }
                        //for (int i = 0; i < cons->size(); i++)
                       //     cout << (*cons)[i].attri << endl;
                        api.DeleteTuple(t_name, cons);
                    }
                }
            }
            catch (...)
            {
                goto Error_here;
            }
        }
        else if (sql_type == select_record) {
            try
            {
                if (words.size() < 3) {
                    throw "Syntax Error";
                }
                if (words[1] != "*" && words[2] != "from")
                {
                    goto Error_here;
                }
                if (words.size() < 4) {
                    throw "Syntax Error";
                }
                std::string t_name = words[3];
                vector<Attribute>* attrs = new vector<Attribute>;
                vector<Attribute>* attr = new vector<Attribute>;
                vector<Condition>* cons = new vector<Condition>;
                if (!api.AttributeGet(t_name, attrs)) {
                    cout << "The table doesn't exists !" << endl;
                    return 0;
                }
                int i = 0;
                Condition con;
                if (words.size() < 5) {
                    throw "Syntax Error";
                }
                if (words[4] == ";")
                {
                }
                else if (words[4] == "where")
                {
                    int pos = 5;
                    while (1)
                    {
                        if (words.size() < pos + 1) {
                            throw "Syntax Error";
                        }
                        if (words[pos] == "and")
                        {
                            pos++;
                            continue;
                        }
                        else if (words[pos] == ";")
                        {
                            break;
                        }

                        con.attri = words[pos];

                        for (i = 0; i < (*attrs).size(); i++)
                        {
                            if ((*attrs)[i].name == words[pos])
                                break;
                        }
                        if (i == (*attrs).size()) {
                            cout << "The attribute you wanna select doesn't exists in this table !" << endl;
                            return 0;
                        }

                        if (words.size() < pos + 2) {
                            throw "Syntax Error";
                        }
                        if (words[pos + 1] == "=")
                        {
                            con.type = EQ;
                        }
                        else if (words[pos + 1] == ">=") {
                            con.type = GE;
                        }
                        else if (words[pos + 1] == "<=") {
                            con.type = LE;
                        }
                        else if (words[pos + 1] == ">") {
                            con.type = GT;
                        }
                        else if (words[pos + 1] == "<") {
                            con.type = LT;
                        }
                        else if (words[pos + 1] == "<>") {
                            con.type = NE;
                        }
                        else {
                            goto Error_here;
                        }
                        if (words.size() < pos + 3 || words[pos + 2] == "'" && words.size() < pos + 5) {
                            throw "Syntax Error";
                        }
                        if ((*attrs)[i].type == STRING)
                        {
                            con.val = words[pos + 3];
                            pos += 5;
                        }
                        else if ((*attrs)[i].type == FLOAT)
                        {
                            con.val = float(atof(words[pos + 2].c_str()));
                            pos += 3;
                        }
                        else {
                            con.val = atoi(words[pos + 2].c_str());
                            pos += 3;

                        }
                        (*cons).push_back(con);
                    }
                }
                else {
                    goto Error_here;
                }
                api.SelectData(t_name, attr, cons);
            }
            catch (...)
            {
                goto Error_here;
            }
        }
        else if (sql_type == exec_file) {
            if (words.size() < 3 || words[2] != ";")
            {
                goto Error_here;
            }
            std::string f_name = words[1];
            execfile(f_name);
        }
        else if (sql_type == quit_system) {
            try
            {
                if (words[1] != ";")
                {
                    goto Error_here;
                }
                api.~API();
                exit(0);
            }
            catch (...)
            {
                goto Error_here;
            }
        }
        else {
            goto Error_here;
        }
    }

    else {
    Error_here:
        cout << "error:Sytnax Error!" << endl;
        return 0;
    }
    clock_t time_end = clock();
    cout << "Time consumed : " << time_end - time << "ms" << endl;
    return 1;
}


void Interpreter::execfile(const std::string& f_name) {
    FILE* f = fopen(f_name.c_str(), "r");
    if (f)
    {
        //cout << "Signal" << endl;
        vector<std::string> sqls;
        std::string s = "";
        while (1)
        {
            char str[1024];
            if (fscanf(f, "%s", str) == 1) {
                s = s + string(str) + " ";
                if (s[s.length() - 2] == ';') {
                    sqls.push_back(s);
                    s = "";
                }
            }
            else {
                if (s != "") sqls.push_back(s);     //����в�������sql���
                break;
            }
        }
        //cout << sqls.size() << endl;
        for (int i = 0; i < sqls.size(); i++)
        {
            get_sql(sqls[i]);
            //cout << "before " << sql << endl;
            run_sql();
            //cout << "after " << sql << endl;
        }
    }
    else {
        cout << "file " << f_name << " openning failed!" << endl;
    }
    if (f)
    {
        fclose(f);
    }
    return;
}

void Interpreter::print()
{
    cout << sql << endl;
}
void Interpreter::printword()
{
    for (int i = 0; i < words.size(); i++)
        cout << words[i] << " " << i << " ";
    cout << endl;
}