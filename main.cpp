#include <iostream>
#include "databaseXD.cpp"
#include <sstream>

int main(void)
{
    //std::string a = "create table name ({autoincrement} qw    : bytes[100] = \"AFFAFADDDDAFD\")\ncreate table bLEp ({autoincrement} qw    : string[100] = \"string1\")\ninsert (qw =\n\n\n\n\n \"string\") to bLEp\n create table TAB1 (id : int32 = 31)\n create table ins_test ({autoincrement} id : int32 , str : string[10] , bol : bool = false, {autoincrement, unique, key} word : bytes[15] = AFAFA)\n insert (str = \"check\", bol = true, word = 0x16) to ins_test\n insert (,\"check\", true, 0x16) to ins_test";
    std::string a = "create table name (qw : string[16] = \"blep\", tw : string[16] = \"mlep\", num : int32 = 14, bits: bytes[16] = AB)";
    memdb::Database db;
    std::istringstream s(a);
    auto iter = db.execute(s);
    auto it = iter.begin();
    std::string c = *it;
    std::cout << c << std::endl;
    return 0;
}
