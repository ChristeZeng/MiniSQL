#ifndef __H_DEFINES_H__
#define __H_DEFINES_H__

#define MAX_BLOCK_SIZE 4096
#define MAX_AttriCnt 32
#define MAX_IndexCnt 32
#define MAX_NAME_SIZE 64
#define MAX_tbf_SIZE 4546
enum SQL_TYPE { no_op, create_table, drop_table, create_index, drop_index, select_record, insert_record, delete_record, quit_system, exec_file };

#endif