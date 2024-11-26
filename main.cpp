#include <iostream>
#include "databaseXD.cpp"
#include <sstream>

int main(void)
{
    std::string a = "create table name ({autoincrement} qw    : bytes[100] = \"AFFAFADDDDAFD\")\ncreate table bLEp ({autoincrement} qw    : string[100] = \"blyatblyat\")\ninsert (qw =\n\n\n\n\n \"sukasuka\") to bLEp\n create table TAB1 (id : int32 = 31)\n create table ins_test ({autoincrement} id : int32 , str : string[10] , bol : bool = false, {autoincrement, unique, key} word : bytes[15] = AFAFA)\n insert (str = \"check\", bol = true, word = \"0x16\") to ins_test\n insert (,\"check\", true, \"0x16\") to ins_test";
    memdb::Database db;
    std::istringstream s(a);
    db.execute(s);

    return 0;
}
