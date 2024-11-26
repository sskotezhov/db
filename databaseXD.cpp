#include "databaseXD.hpp"

std::string tolow(std::string str)
{
    for (auto &i: str)
    {
        i = tolower(i);
    }
    return str;
}

int find(std::string &str, std::vector<std::string> &dels)
{
    int pos = std::string::npos, add = 0, ind = 0;
    for (int i = 0; i < dels.size(); i++)
    {
        if (pos > str.find(dels[i]))
        {
            pos = str.find(dels[i]);
            add = dels[i].length();
            ind = i;
        }
    }
    std::string tmp = dels[0];
    dels[0] = dels[ind];
    dels[ind] = tmp;
    return pos + add;
}
std::string split(std::string &str, char del)
{
    int pos;
    std::string tmp = "";
    pos = str.find(del);
    tmp = str.substr(0, pos);
    if (pos == std::string::npos) str.clear();
    else str.erase(0, pos+1);
    return tmp;
}
std::string split(std::string &str, std::string del)
{
    int pos = std::string::npos;
    std::string tmp = "";
    std::vector<std::string> dels;
    while ((tmp = split(del, '$')).length() > 0)
    {
        dels.push_back(tmp);
        tmp = "";
    }
    for (auto i: dels)
    {
        if (pos > str.find(i))
            pos = str.find(i);
    }
    tmp = str.substr(0, pos);
    do
    {
        str.erase(0, pos);
    } while((pos = find(str, dels)) == dels[0].length());
    return tmp;
}

std::string pull(std::string str, std::pair<std::string, std::string> del)
{
    str.erase(0, str.find(del.first) + del.first.length());
    if (str.find(del.second) != std::string::npos) str.erase(str.find(del.second));
    return str;
}

memdb::Database::error_interface memdb::Database::execute(std::istream &query)
{
    std::string line, tmp;
    int pos;
    std::string name;
    while(std::getline(query, line))
    {
        if (line[0] == ' ') split(line, " ");
        tmp = tolow(split(line, " "));
        if (tmp == "create")
        {
            if ((tmp = tolow(split(line, " "))) == "table")
            {
                std::string name = split(line, " "), next = "";
                data buffer;
                bd.insert(std::make_pair(name, buffer));
                line = pull(line, {"(",")"});
                next = line;
                while ((line = split(next, ",")).length()>0)
                {
                    if (line[0] == ' ') split(line, " ");
                    if (line[0] == '{')
                    {
                        if (line.find("}") == std::string::npos) 
                        {
                            line += " , " + next;
                            tmp = pull(line, {"{", "}"});
                            split(line, "}");
                            next = line;
                        }
                        else tmp = pull(line, {"{", "}"});
                        std::array<bool, 3> buffer = {false, false, false};
                        bd[name].attrs.push_back(buffer);
                        tmp = tolow(tmp);
                        if (tmp.find("unique") != std::string::npos)
                        {
                            bd[name].attrs.back()[0] = true;
                        }
                        if (tmp.find("autoincrement") != std::string::npos)
                        {
                            bd[name].attrs.back()[1] = true;
                        }
                        if (tmp.find("key") != std::string::npos)
                        {
                            bd[name].attrs.back()[2] = true;
                        }
                        split(line, "}$ ");
                    }
                    bd[name].names.push_back(split(line, " $:"));
                    bd[name].types.push_back(tolow(split(line, " $=")));
                    if (line.length() > 0)
                    {
                        tmp = split(line, " ");
                        if (bd[name].types.back().find("string") != std::string::npos)
                        {
                            int cnt = std::stoi(pull(bd[name].types.back(), {"[","]"}));
                            bd[name].def_val.push_back(new std::string(pull(tmp, {"\"", "\""})));
                            cnt = cnt > (*((std::string*)bd[name].def_val.back())).length() ? (*((std::string*)bd[name].def_val.back())).length() : cnt;
                            (*((std::string*)bd[name].def_val.back())).erase(cnt);
                        }
                        if (bd[name].types.back().find("int32") != std::string::npos)
                        {
                            bd[name].def_val.push_back(new int(std::stoi(tmp)));
                        }
                        if (bd[name].types.back().find("bool") != std::string::npos)
                        {
                            tmp = tolow(tmp);
                            bd[name].def_val.push_back(new bool(tmp.find("true") != std::string::npos));
                        }
                        if (bd[name].types.back().find("bytes") != std::string::npos)
                        {
                            if (tmp[0] == ' ') split(tmp, " ");
                            if (tmp.find("0x") == std::string::npos) tmp = "0x" + tmp;
                            tmp = split(tmp, " ");
                            void *reference = new std::string(tmp);
                            int cnt = std::stoi(pull(bd[name].types.back(), {"[","]"}));
                            cnt = (*(std::string*)reference).length() > cnt ? cnt : (*(std::string*)reference).length();
                            (*(std::string*)(reference)).erase(cnt);
                            bd[name].def_val.push_back(reference);
                        }
                        
                    }

                }
                bd[name].length = bd[name].names.size();
                for (int i = 0; i < bd[name].length; i++)
                {
                    bd[name].cells.push_back(*(new std::vector<void*>));
                }
                _size++;

            }
            else
            {
                return memdb::Database::error_interface(-2);
            }
        }
        else if (tmp == "insert")
        {

            split(line, "($ ");
            tmp = "";
            while (line.find(")") == std::string::npos)
            {
                tmp += line;
                std::getline(query, line);
            }
            tmp += split(line, ")");
            split(line, "to$ ");
            line.erase(std::remove(line.begin(), line.end(), ' '), line.end());
            line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
            std::replace(tmp.begin(), tmp.end(), '\n', ' ');
            std::string name = line;
            data* ref_table = &bd[name];
            for (int i = 0; i < (*ref_table).names.size(); i++)
            {
                void *ref = (*ref_table).def_val[i];
                if ((*ref_table).attrs[i][1]) (*ref_table).cells[i].push_back(new int((*ref_table).cells[i].size()));
                else if ((*ref_table).types[i].find("bytes")  != std::string::npos) (*ref_table).cells[i].push_back(new std::string(*((std::string*)ref)));
                else if ((*ref_table).types[i].find("string") != std::string::npos) (*ref_table).cells[i].push_back(new std::string(*((std::string*)ref)));
                else if ((*ref_table).types[i].find("int32")  != std::string::npos) (*ref_table).cells[i].push_back(new int(*((int*)ref)));
                else if ((*ref_table).types[i].find("bool")   != std::string::npos) (*ref_table).cells[i].push_back(new bool(*((bool*)ref)));
            }
            if (tmp.find("=") != std::string::npos)
            {
                std::string col;
                while ((line = split(tmp, ",")).length() > 0)
                {
                    if (line[0] == ' ') split(line, " ");
                    col = split(line, "=$ ");
                    int index = std::find((*ref_table).names.begin(), (*ref_table).names.end(), col) - (*ref_table).names.begin();
                    void* reference;
                    if ((*ref_table).types[index].find("string") != std::string::npos)
                    {
                        reference = new std::string(pull(line, {"\"", "\""}));
                        int cnt = std::stoi(pull((*ref_table).types[index], {"[","]"}));
                        cnt = (*(std::string*)reference).length() > cnt ? cnt : (*(std::string*)reference).length();
                        (*(std::string*)(reference)).erase(cnt);
                    } 
                    else if ((*ref_table).types[index].find("int32") != std::string::npos) reference = new int(std::stoi(line));  
                    else if ((*ref_table).types[index].find("bool") != std::string::npos) reference = new bool(line.find("true") != std::string::npos);
                    else if ((*ref_table).types[index].find("bytes") != std::string::npos)
                    {
                        if (line[0] == ' ') split(line, " ");
                        if (line.find("0x") == std::string::npos) line = "0x" + line;
                        line = split(line, " ");
                        reference = new std::string(line);
                        int cnt = std::stoi(pull((*ref_table).types[index], {"[","]"}));
                        cnt = (*(std::string*)reference).length() > cnt ? cnt : (*(std::string*)reference).length();
                        (*(std::string*)(reference)).erase(cnt);
                    }
                    (*ref_table).cells[index].back() = reference;
                }
            }
            else
            {
                int index = 0;
                while((line = split(tmp, ",")).length() > 0 || tmp.length() > 0)
                {
                    if (line.length() == 0) {index++;continue;}
                    if (line[0] == ' ') split(line, " ");
                    line = split(line, " ");
                    void* reference;
                    if ((*ref_table).types[index].find("string") != std::string::npos)
                    {
                        reference = new std::string(pull(line, {"\"", "\""}));
                        int cnt = std::stoi(pull((*ref_table).types[index], {"[","]"}));
                        cnt = (*(std::string*)reference).length() > cnt ? cnt : (*(std::string*)reference).length();
                        (*(std::string*)(reference)).erase(cnt);
                    } 
                    else if ((*ref_table).types[index].find("int32") != std::string::npos) reference = new int(std::stoi(line));  
                    else if ((*ref_table).types[index].find("bool") != std::string::npos) reference = new bool(line.find("true") != std::string::npos);
                    else if ((*ref_table).types[index].find("bytes") != std::string::npos)
                    {
                        if (line[0] == ' ') split(line, " ");
                        if (line.find("0x") == std::string::npos) line = "0x" + line;
                        line = split(line, " ");
                        reference = new std::string(line);
                        int cnt = std::stoi(pull((*ref_table).types[index], {"[","]"}));
                        cnt = (*(std::string*)reference).length() > cnt ? cnt : (*(std::string*)reference).length();
                        (*(std::string*)(reference)).erase(cnt);
                    }
                    (*ref_table).cells[index].back() = reference;
                    index++;
                }
            }
            (*ref_table)._size++;


        }
        else return memdb::Database::error_interface(-2);
    }
    return memdb::Database::error_interface(0, &(bd[name]));
}
memdb::Database::error_interface memdb::Database::destroy()
{
    void *ref;
    for (auto obj : bd)
    {
        for (auto col : obj.second.cells)
        {
            while (!col.empty())
            {
                ref = col.back();
                col.pop_back();
                if      (obj.second.types.back().find("bytes")  != std::string::npos) delete (std::string*)       ref;
                else if (obj.second.types.back().find("string") != std::string::npos) delete   (std::string*)ref;
                else if (obj.second.types.back().find("int32")  != std::string::npos) delete   (int*)        ref;
                else if (obj.second.types.back().find("bool")   != std::string::npos) delete   (bool*)       ref;
            }
            ref = obj.second.def_val.back();
            obj.second.def_val.pop_back();
            if      (obj.second.types.back().find("bytes")  != std::string::npos) delete (std::string*)       ref;
            else if (obj.second.types.back().find("string") != std::string::npos) delete   (std::string*)ref;
            else if (obj.second.types.back().find("int32")  != std::string::npos) delete   (int*)        ref;
            else if (obj.second.types.back().find("bool")   != std::string::npos) delete   (bool*)       ref;
            obj.second.types.pop_back();
        }
    }
    return memdb::Database::error_interface(0);
}
std::string memdb::Database::error_interface::get_error()
{
    std::string response = "";
    switch (id)
    {   
        case -3:
            response += "delimiter_error";
            break;
        case -2:
            response += "input_error";
            break;
        case -1:
            response += "type_error";
            break;
        default:
            response += "err_id " + std::to_string(id) + "\n";
            break;
    }
    return response;
}

memdb::Database::data::Iterator::Iterator(struct data* in)
{
    _data = in;
    int index = -1;
    out = "";
    for (int i = 0; i < _data->length; i++)
    {
        if (_data->types[i].find("string") != std::string::npos || _data->types[i].find("byter") != std::string::npos)
        {
            out += *((std::string*)_data->def_val[i]) + ";";
        }
        else if (_data->types[i].find("bool") != std::string::npos)
        {
            if (*((bool*)_data->def_val[i])) out +=  "true;";
            else out +=  "false;";
        }
        else if (_data->types[i].find("int32") != std::string::npos)
        {
            out += std::to_string(*((int*)_data->def_val[i])) + ";";
        }
    }
}
