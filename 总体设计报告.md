





<center><h1>MiniSQL 设计总报告</center>
<center> 曾帅，邓承克，朱航</center>























































[TOC]


## MiniSQL总体框架

### MiniSQL实现功能分析

#### 总功能

允许用户通过字符界面输入SQL语句实现表的建立/删除；索引的建立/删除以及表记录的插入/删除/查找；

#### 数据类型

支持三种基本数据类型：INT，CHAR(N)，FLOAT，其中CHAR(N)满足 1 <= N <= 255；

#### 表定义

一个表最多可以定义32个属性，各属性可以指定是否为UNIQUE；支持单属性的主键定义；

#### 索引的建立和删除

对于表的主属性自动建立B+树索引，对于声明为UNIQUE的属性可以通过SQL语句由用户指定建立/删除B+树索引（因此，所有的B+树索引都是单属性单值的）；

#### 查找记录

可以通过指定用AND连接的多个条件进行查询，支持等值查询和区间查询；

#### 插入和删除记录

支持每次一条记录的插入操作；支持每次一条或多条记录的删除操作。

### MiniSQL系统体系结构

<img src="https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210616142635.png" alt="image-20210616142635098" style="zoom:80%;" />

### 设计语言与运行环境

设计语言：C++(含有C++17特性并且加入variant库)

环境：Win10, MinGW64

使用项目管理器：Visual Studio



## MiniSQL各模块实现的功能

### Interpreter实现的功能

Interpreter模块直接与用户交互，主要实现以下功能：

1. 程序流程控制，即“启动并初始化 à ‘接收命令、处理命令、显示命令结果’循环 $\rightarrow$ 退出”流程。

2. 接收并解释用户输入的命令，生成命令的内部数据结构表示，同时检查命令的语法正确性和语义正确性，对正确的命令调用API层提供的函数执行并显示执行结果，对不正确的命令显示错误信息.

### API实现的功能

API模块是整个系统的核心，其主要功能为提供执行SQL语句的接口，供Interpreter层调用。该接口以Interpreter层解释生成的命令内部表示为输入，根据Catalog Manager提供的信息确定执行规则，并调用Record Manager、Index Manager和Catalog Manager提供的相应接口进行执行，最后传递执行结果给Interpreter模块。

### Catalog Manager实现的功能

Catalog Manager负责管理数据库的所有模式信息，包括：

1. 数据库中所有表的定义信息，包括表的名称、表中字段（列）数、主键、定义在该表上的索引。

2. 表中每个字段的定义信息，包括字段类型、是否唯一等。

3. 数据库中所有索引的定义，包括所属表、索引建立在那个字段上等。

Catalog Manager还必需提供访问及操作上述信息的接口，供Interpreter和API模块使用。

### Record Manager实现的功能

Record Manager负责管理记录表中数据的数据文件。主要功能为实现数据文件的创建与删除（由表的定义与删除引起）、记录的插入、删除与查找操作，并对外提供相应的接口。其中记录的查找操作要求能够支持不带条件的查找和带一个条件的查找（包括等值查找、不等值查找和区间查找）。

数据文件由一个或多个数据块组成，块大小应与缓冲区块大小相同。一个块中包含一条至多条记录，为简单起见，只要求支持定长记录的存储，且不要求支持记录的跨块存储。

### Index Manager实现的功能

Index Manager负责B+树索引的实现，实现B+树的创建和删除（由索引的定义与删除引起）、等值查找、插入键值、删除键值等操作，并对外提供相应的接口。

B+树中节点大小应与缓冲区的块大小相同，B+树的叉数由节点大小与索引键大小计算得到。

### Buffer Manager实现的功能

Buffer Manager负责缓冲区的管理，主要功能有：

1. 根据需要，读取指定的数据到系统缓冲区或将缓冲区中的数据写出到文件

2. 实现缓冲区的替换算法，当缓冲区满时选择合适的页进行替换

3. 记录缓冲区中各页的状态，如是否被修改过等

4. 提供缓冲区页的pin功能，及锁定缓冲区的页，不允许替换出去

为提高磁盘I/O操作的效率，缓冲区与文件系统交互的单位是块，块的大小应为文件系统与磁盘交互单位的整数倍，一般可定为4KB或8KB。

### DB FIles

DB Files指构成数据库的所有数据文件，主要由记录数据文件、索引数据文件和Catalog数据文件组成。



## 内部数据形式与各模块接口

### 内部数据形式

#### Tuple类

```c++
typedef boost::variant<int,float,string> value;

class Tuple
{
public:
    vector<value>v;
};
```

#### Attribute类

```c++
enum AttributeType { INT, FLOAT, STRING };

class Attribute
{
public:
    string name;
    AttributeType type;
    int length;         //char的长度
    bool isUnique;
    bool isNULL;
    bool isPrimary;
};
```

#### Table类

```c++
class Table
{
public:
    string name;
    vector<Tuple> tuple;       //存放元组
    vector<Attribute> attri;    //存放属性
    vector<Index> index;       //存放索引
    int attriCnt;            //属性数量
    int tupleCnt;            //元组数量
    int indexCnt;            //索引数量
};
```

#### Index类

```c++
class Index
{
public:
    string name;
    string TableName;
    string AttriName;
    int IndexAddr;
};
```

### 主窗口以及主函数设计

主界面由简单的字符界面以及输入提示组成，具体如图所示

![image-20210623162126060](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623162133.png)

#### Interpreter接口

1. 读取sql语句

   ```c++
   void get_sql(const string& sql);
   ```

2. 运行sql语句

   ```c++
   int run_sql();
   ```

#### Catalog Manager接口

1. 创建表结构文件

   ```c++
   int CreateNewTable(const Table* t);
   ```

2. 删除表结构文件

   ```c++
   int DropTable(const std::string t_name);
   ```

 3. 创建索引

    ```c++
    int CreateNewIndex(const Index* i);
    ```

 4. 删除索引

    ```c++
    int DropIndex(const std::string i_name);
    ```

5. 查询表

   ```c++
   Table* SelectTable(const std::string t_name);
   ```

6. 打印表

   ```c++
   void showTable(const Table* t);
   ```

#### Record Manager接口

1. 创建表

   ```c++
   void CreateTable(string table);
   ```

2. 删除表

   ```c++
   bool DropTable(string table);
   ```

3. 插入数据

   ```c++
   bool InsertRecord(Table& table, Tuple& tuple);  
   ```

4. 删除数据

   删除单个数据

   ```c++
   void DeleteRecord(Table& table, Tuple& tuple);  
   ```

   根据条件删除数据

   ```c++
   bool RecordDeleteCondition(const Table& table, vector<Condition>con);
   ```

5. 查找数据

   ```c++
   vector<Tuple> SelectRecord(const Table& table, const Condition& con);
   ```


#### Index Manager接口

1. 创建索引

   创建索引的参数将由Index Manager的构造函数提供

   ```c++
   void createIndex();
   ```

2. 删除索引

   ```c++
   bool dropIndex();
   ```

3. 等值查找

   等值查找需要提供相应的键值，返回的是键值在record文件中的偏移地址(int类型)

   ```c++
   int searchviakey(const T& key);
   ```

4. 范围查找

   范围查找需要提供范围，返回的是键值的偏移地址的容器

   ```c++
   vector<int> rangesearch(const T& beg, const T& end);
   ```

5. 插入新索引值

   ```c++
   bool insertkey(const T& key, const int addr);      
   ```

6. 删除索引值

   ```c++
   bool deletekey(const T& key);
   ```

7. 初始化索引

   当要对一个属性建立索引时，需要给出此属性的所有键值与相应的偏移地址来初始化B+树

   ```c++
   void initialize(vector<T> keys, vector<int> records);
   ```


#### Buffer Manager接口

1. 将某文件对应的所有块从buffer中清除

   ```c++
   void FreeBlock(const string& filename);
   ```

2. 根据文件名和偏移量获得块号

   ```c++
   int ReadIndexFromFile(const string& filename, int offset);
   ```

3. 根据块号，获得块的内容

   ```c++
   char* ReadBlockFromFile(int BlockId); 
   ```

4. 根据块号，将对应的块写回文件

   ```c++
   bool writetofile(const int BlockId);
   ```

5. 锁定数据块与解锁数据库

   ```c++
   void PinBlock(int BlockId);                                    
   void UnPinBlock(int BlockId);  
   ```

6. 将数据库置为已修改

   ```c++
   void DirtyBlock(int BlockId);
   ```



## MiniSQL系统测试

#### 创建表

##### 成功创建

```mysql
create table test( a int, b float, c char(10), primary key(a) );
```

![image-20210623162439554](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623162439.png)

##### 重复创建

```mysql
create table test( a int, b float, c char(10), primary key(a) );
```

![image-20210623162908077](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623162908.png)



#### 插入数据

##### 成功插入

```mysql
insert into test values(1, 2, '12');
```

![image-20210623162504154](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623162504.png)

##### 主键/唯一约束

```mysql
insert into test values(1, 2, '12');
```

![image-20210623162616863](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623162616.png)

##### 插入的表不存在

```mysql
insert into test2 values(1, 2, '12');
```

![image-20210623162704151](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623162704.png)

##### 插入的元素个数有误

```mysql
insert into test values(1, 2);
```

![image-20210623162801177](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623162801.png)



#### 执行文件

```mysql
execfile test.txt;
```

```mysql
create table test(
    a int,
    b float,
    c char(10),
    primary key(a)
);

insert into test values(1, 2, '12');
insert into test values(2, 3, '13');
insert into test values(3, 4, '15');
insert into test values(2, 5, '13');
select * from test;

select * from test where b<>4;
select * from test where a>=2;
select * from test where a>=2 and a<3;

delete from test where a=3;
insert into test values(3, 8, 'aaaaaa');
insert into test values(2, 3, '13');

create table test(
    a int,
    b int unique,
    c float,
    primary key(a)
);

drop table test;

create table test(
    a int,
    b int unique,
    primary key(a)
);

insert into test values (1,2);
insert into test values (2,3);
insert into test values (3,4);
insert into test values (4,5);

insert into test values (5,2);
insert into test values (5,6);

select * from test;

create index bi on test (b);
select * from test where b<=4;

drop index bi;
```

![image-20210616163841771](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210616163841.png)

![image-20210616163911858](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210616163911.png)



#### 删除记录

##### 成功删除

```mysql
delete from test where a=3;
```

![image-20210623170105028](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623170105.png)

##### 删除的表不存在

```mysql
delete from tests where a=3;
```

![image-20210623170208290](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623170208.png)

##### 删除的属性不存在

```mysql
delete from test where z=3;
```

![image-20210623170259408](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623170259.png)



#### 查询

##### 成功查询（利用batch_test.txt成功插入后）

```mysql
select * from student where sage >= 10000 and sage < 10010;
```

![image-20210623171207359](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623171207.png)

用时0.417s，速度可以接受（含打印时间）

```mysql
select * from student where sage = 10000;
```

![image-20210624142711652](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210624142711.png)

单值查询用时0.384s（含打印时间）

##### 建立索引后查询

```mysql
create index sageIndex on student(sage);
select * from student where sage = 10000;
```

![image-20210624143248999](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210624143249.png)

建立了索引之后查找，仅仅用时**0.006s**，速度提升50倍

##### 表不存在

```mysql
select * from student1 where sage = 10000;
```

![image-20210623171551638](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623171551.png)

##### 属性不存在

```mysql
select * from student where sage1 = 10000;
```

![image-20210623171650878](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623171650.png)



#### 建立索引exe

##### 成功建立

```mysql
create index sageIndex on student(sage);
```

![image-20210624142841333](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210624142841.png)

在一个拥有20000条记录的属性上建立索引需7.208s，并且已经将B+树全部写入文件，性能良好

![image-20210624143034540](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210624143034.png)

##### 重复建立（已有sage_index）

```mysql
create index sage_index on student(sage);
```

![image-20210623171944708](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623171944.png)

##### 属性/表不存在

```mysql
create index sage_index1 on student(sage1);
```

![image-20210623172046791](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623172046.png)



#### 删除索引

##### 成功删除

```mysql
drop index sage_index;
```

![image-20210623172158194](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623172158.png)

##### 删除不存在的索引

```mysql
drop index sage_index;
```

![image-20210623172257729](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210623172257.png)



## 性能评价

#### 内存

本程序不同于直接开一个巨大的数组将所有数据进行Hash, Map的方式，本MiniSQL仅使用100个Block大小的缓存（由所有模块共用），所以能够大大减少内存的使用，当执行20000条插入时，全速插入的过程中内存的消耗仅为7.5M

![image-20210624144054775](https://raw.githubusercontent.com/ChristeZeng/Picture/main/img/20210624144054.png)

#### 执行时间

本MiniSQL由于使用的Block数较小，两万行数据的插入+写入文件+建立Primary Key的过程需要2分钟左右，但在后续建立20000条数据的index(B+树)，并写入文件仅需7s，并且在index的基础上进行查找仅需要6ms，速度相比没有建立index提升巨大。

总的来说，本次MiniSQL性能达到了我们的预期要求。



## 分工说明

Interpreter模块：朱航

API模块：邓承克

Catalog Manager模块：朱航

Record Manager模块：邓承克

Index Manager模块：曾帅

Buffer Manager模块：曾帅

DB Files模块：曾帅，邓承克，朱航

总体设计报告：曾帅，邓承克，朱航

模块汇总：邓承克

联合测试：曾帅、邓承克

