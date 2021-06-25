#include "CatalogManager.h"

#include<algorithm>
#include<fstream>
#include<cstring>
#include<iostream>

CatalogManager::CatalogManager() {
    FILE* tableFile = fopen("tables.inf", "r");      //该文件中存储了所有存在的表
    FILE* indexFile = fopen("indices.inf", "r");      //该文件中存了所有存在的索引
    char str[64], str2[64];

    // 所有表名装载进内存
    if (tableFile) {
        while (fscanf(tableFile, "%s", str) == 1)
            Tables[str] = 1;
        fclose(tableFile);
    }
   
    // 所有索引装载进内存
    if (indexFile) {
        while (fscanf(indexFile, "%s%s", str, str2) == 2)
            Indices[str] = str2;

        fclose(indexFile);
    }
    


};
CatalogManager::~CatalogManager() {
    FILE* tableFile = fopen("tables.inf", "w");
    FILE* indexFile = fopen("indices.inf", "w");

    // 所有表名重新写入文件
    for (auto it = Tables.begin(); it != Tables.end(); it++)
        fprintf(tableFile, "%s\n", it->first.c_str());
    // 所有索引重新写入文件
    for (auto it = Indices.begin(); it != Indices.end(); it++)
        fprintf(indexFile, "%s %s\n", it->first.c_str(), it->second.c_str());

    fclose(tableFile);
    fclose(indexFile);
};

int CatalogManager::CreateNewTable(const Table* t) {
    /*
        tbf文件结构
        table_name                                                                                  64
        table_attriCnt                                                                               1
        table_IndexCnt                                                                               1
        {attri_name,attri_type,attri_length,attri_isUnique,attri_isNull,attri_isPrimary,'\0'}       70
        ...
        {index_name,index_Addr,'\0','\0','\0','\0','\0'}                                            70
        ...
    */
    if (Tables.count(t->name) == 1)
        return 1;           // 已经存在相同的表名了！

    for (int i = 0; i < t->attriCnt; i++)
        for (int j = 0; j < t->attriCnt; j++)
            if (i != j && t->attri[i].name == t->attri[j].name)
                return 2;   // 属性名重复

    // 正常处理
    FILE* file = fopen((t->name + ".tbf").c_str(), "wb");
    char buf[MAX_tbf_SIZE], * p = buf;
    // 填充头部
    memcpy(p, t->name.c_str(), t->name.length());
    memset(p + t->name.length(), '\0', 64 - t->name.length());
    p += 64;
    p[0] = t->attriCnt;
    p[1] = t->indexCnt;
    p += 2;
    // 填充属性
    for (int i = 0; i < t->attriCnt; i++)
    {
        const Attribute& at = t->attri[i];
        memcpy(p, at.name.c_str(), at.name.length());
        memset(p + at.name.length(), '\0', 64 - at.name.length());
        p[64] = at.type;
        p[65] = at.length;
        p[66] = at.isUnique;
        p[67] = at.isNULL;
        p[68] = at.isPrimary;
        p[69] = '\0';               //保留
        p += 70;
    }
    // 填充索引
    for (int i = 0; i < t->indexCnt; i++)
    {
        const Index& id = t->index[i];
        memcpy(p, id.name.c_str(), id.name.length());
        memset(p + id.name.length(), 0x00, 64 - id.name.length());
        p[64] = id.IndexAddr;
        //memcpy(p + 65, '\0', 5);     //保留
        for (int i = 0; i < 5; i++) {//留5个 '\0'
            *(p + 65+i) = '\0'; 
        }
        p += 70;
        this->Indices[id.name] = t->name;
    }
    // 写入到文件，更新Tables;
    fwrite(buf, sizeof(char), p - buf, file);
    this->Tables[t->name] = 1;
    fclose(file);
    return 0;
}

int CatalogManager::DropTable(const std::string t_name) {
    if (remove((t_name + ".tbf").c_str()) < 0)
        return 1; // 删除失败，表不存在

    Tables.erase(t_name);
    for (auto p = Indices.begin(); p != Indices.end();) {
        if (p->second == t_name)
            p = Indices.erase(p);
        else {
            p++;
        }
    }
       
    return 0;
}

int CatalogManager::CreateNewIndex(const Index* i)
{
    if (Tables.count(i->TableName) == 0)
        return 1; // 表不存在
    if (Indices.count(i->name) == 1)
        return 2; // 索引名重复

    FILE* file = fopen((i->TableName + ".tbf").c_str(), "r+");
    char idxCnt, buf[70];
    // 索引数量+1
    //fseek(file, 1, SEEK_SET);
    fseek(file, 65, SEEK_SET);
    fread(&idxCnt, sizeof(char), 1, file);
    idxCnt++;
    fseek(file, 65, SEEK_SET);
    fwrite(&idxCnt, sizeof(char), 1, file);
    // 末尾新建索引
    memset(buf, 0x00, sizeof(buf));
    memcpy(buf, i->name.c_str(), i->name.length());
    buf[64] = i->IndexAddr;
    fseek(file, 0, SEEK_END);
    fwrite(buf, sizeof(char), 70, file);
    // Updata
    Indices[i->name] = i->TableName;
    fclose(file);
    return 0;
}

int CatalogManager::DropIndex(const std::string i_name)
{
    if (Indices.count(i_name) == 0)
        return 1;               //�����ڸ�����
    string name = Indices[i_name];
    FILE* file = fopen((name + ".tbf").c_str(), "r");
    char buf[MAX_tbf_SIZE], * p = buf, * last;

    last = buf + fread(buf, sizeof(char), MAX_tbf_SIZE, file);
    fclose(file);

    // ��λ����λ�ò�ɾ��
    //p = p + 66 + buf[0] * 70;
    p = p + 66 + buf[64] * 70;
    while ((p[63] && memcmp(p, i_name.c_str(), 64) != 0) || strcmp(p, i_name.c_str()) != 0)
        p += 70;
    if (p != last - 70)
        memcpy(p, last - 70, 70); // ������������һ�������������һ������������
    buf[65]--;

    // ����д���ļ�
    file = fopen((Indices[i_name] + ".tbf").c_str(), "w");
    fwrite(buf, sizeof(char), last - buf - 70, file);
    fclose(file);
    Indices.erase(i_name);
    return 0;
}

Table* CatalogManager::SelectTable(const std::string t_name)
{
    if (Tables.count(t_name) == 0)
        return NULL;
        //return nullptr; // 表不存在

    // 正常处理
    FILE* file = fopen((t_name + ".tbf").c_str(), "rb");
    Table* tb = new Table;
    char buf[MAX_tbf_SIZE], * p = buf;

    fread(buf, sizeof(char), MAX_tbf_SIZE, file);
    // 填充头部
    fseek(file, 0, SEEK_SET);
    //fread(&tb->name, sizeof(char), MAX_NAME_SIZE, file);
    tb->name = t_name;
    p += MAX_NAME_SIZE;
    tb->attriCnt = p[0];
    tb->indexCnt = p[1];
    p += 2;
    // 填充属性
    for (int i = 0; i < tb->attriCnt; i++)//
    {
        //Attribute& at = tb->attri[i];
        Attribute at;
        at.name = p;
        switch (p[64])
        {
        case 0:
            at.type = INT;
            break;
        case 1:
            at.type = FLOAT;
            break;
        case 2:
            at.type = STRING;
            break;
        }
        at.length = p[65];
        at.isUnique = p[66];
        at.isNULL = p[67];
        at.isPrimary = p[68];
        p += 70;
        tb->attri.push_back(at);//压给attri
    }
    // 填充索引
    //for (int i = 0; i < buf[1]; i++)
    for(int i = 0; i<tb->indexCnt;i++)
    {
        //Index& id = tb->index[i];//报段错误
        Index id;
        id.name = p;
        id.IndexAddr = p[64];
        tb->index.push_back(id);//修改后的
        p += 70;
    }

    fclose(file);
    return tb;
}

void CatalogManager::showTable(const Table* t)
{
    using std::cout;
    using std::endl;

    cout << "---" << t->name << "---" << endl;

    for (int i = 0; i < t->attriCnt; i++)
    {
        cout << t->attri[i].name << " ";
        if (t->attri[i].type == 0)
            cout << "int ";
        else if (t->attri[i].type == 1)
            cout << "float ";
        else
            cout << "string ";
        if (t->attri[i].isUnique)
            cout << "unique ";
        if (!t->attri[i].isNULL)
            cout << "not NULL ";
        if (t->attri[i].isPrimary)
            cout << "primary key";
        cout << endl;
    }

    for (int i = 0; i < t->indexCnt; i++)
        cout << "index: " << t->index[i].name << "("
        << t->attri[t->index[i].IndexAddr].name << ")" << endl;
    return;
}


vector<string> CatalogManager::FindAllIndex(const Table& t) {//Find all index accordintg to the table
    vector<string> ret;
    if (Tables.count(t.name)) {//the table have index
        std::map<string, string>::iterator iter;
        iter = Indices.begin();
        for (; iter != Indices.end(); iter++) {
            if (iter->second == t.name) {//the table name matches
                ret.push_back(iter->first);
            }
        }
    }
    return ret;
}