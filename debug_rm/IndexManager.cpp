#include "IndexManager.h"
#include "Defines.h"
using namespace std;

/*得到当前的键类型大小，方便内存操作*/
template<class T>
IndexManager<T>::IndexManager(string IndexName, string TableName, string AttributeName, int typeLen, BufferManager* bm)
{
    filename = IndexName + "_" + TableName + "_" + AttributeName;   //将要建立的索引文件名
    this->bm = bm;
    if constexpr (is_same<T, string>())
        this->typeLen = typeLen + 1;         //如果是string类型，存在文件中会多一个'\0'，所以要+1以免偏移地址错误
    else
        this->typeLen = typeLen;
}

/*建立索引，本质上就是建立一个文件，文件名以特殊方式命名，方便查找*/
template<class T>
void IndexManager<T>::createIndex()
{
    int KeyLen = typeLen;
    int BlockId = bm->ReadIndexFromFile(filename, 0);               //将索引存入buffer并建立文件
    bm->PinBlock(BlockId);                                          //锁住不让释放

    int order = ((MAX_BLOCK_SIZE - 20) / (KeyLen + 4 + 4 + 4) + 1);
    int NumofBlock = 0;
    int root = -1;
    int NextWriteBlock = -1;
    char* buf = bm->ReadBlockFromFile(BlockId);

    memcpy(buf, &order, 4);
    memcpy(buf + 4, &KeyLen, 4);
    memcpy(buf + 8, &NumofBlock, 4);
    memcpy(buf + 12, &root, 4);
    memcpy(buf + 16, &NextWriteBlock, 4);

    bm->DirtyBlock(BlockId);
    bm->UnPinBlock(BlockId);
}

/*删除索引文件*/
/*需要配合的地方:catalog删除索引记录，API调用*/
template<class T>
bool IndexManager<T>::dropIndex()
{
    string path = filename + ".data";
    cout << path << endl;
    if (remove(path.c_str()) == 0)
        return true;
    else
        return false;
}


template<class T>
int IndexManager<T>::searchviakey(const T& key)
{
    BPlusTree<T> BPT(filename, bm);
    return BPT.FindWithKey(key);
}

template<class T>
vector<int> IndexManager<T>::rangesearch(const T& beg, const T& end)
{
    BPlusTree<T> BPT(filename, bm);
    return BPT.FindRangeWithKey(beg, end);
}

template<class T>
bool IndexManager<T>::insertkey(const T& key, const int addr)
{
    BPlusTree<T> BPT(filename, bm);
    return BPT.Insert(key, addr);
}

template<class T>
bool IndexManager<T>::deletekey(const T& key)
{
    BPlusTree<T> BPT(filename, bm);
    return BPT.FakeDelete(key);
}

template <class T>
void IndexManager<T>::initialize(vector<T> keys, vector<int> record)
{
    BPlusTree<T> BPT(filename, bm);
    int a = BPT.initialize(keys, record);
}

template <class T>
void IndexManager<T>::print()
{
    cout << typeLen << " " << filename << endl;
}

template <class T>
void IndexManager<T>::printBPT()
{
    BPlusTree<T> BPT(filename, bm);
    BPT.printtree(BPT.root, 0);
}