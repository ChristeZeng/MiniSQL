#ifndef __H_CATALOGMANAGER_H__
#define __H_CATALOGMANAGER_H__

#include "Defines.h"
#include "BufferManager.h"
#include "Table.h"
#include "Index.h"
#include <map>

class CatalogManager
{
private:
    std::map<std::string, int> Tables;
    std::map<std::string, std::string> Indices;
public:
    CatalogManager();
    ~CatalogManager();
    int CreateNewTable(const Table* t);
    int DropTable(const std::string t_name);
    int CreateNewIndex(const Index* i);
    int DropIndex(const std::string i_name);
    Table* SelectTable(const std::string t_name);
    void showTable(const Table* t);
    vector<string> FindAllIndex(const Table& t);
};
#endif