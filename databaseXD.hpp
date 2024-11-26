#include <iostream>
#include <fstream>
#include <cstring>
#include <cctype>
#include <vector>
#include <unordered_map>
#include <array>
#include <algorithm>

namespace memdb {
class Database
{
    friend class Iterator;
    public:
        struct error_interface
        {
            error_interface(int n): id(n) {};
            error_interface(int n, void* _dt): id(n), _data(_dt){};
            int id;
            bool is_ok() {return (id == 0);}
            std::string get_error();
            void* _data;
        };
        error_interface execute(std::istream &query);
        error_interface load_from_file(std::ifstream in);
        error_interface save_to_file(std::ofstream out);
        error_interface destroy();
    private:
        struct data
        {
            class Iterator: public std::iterator<std::output_iterator_tag, std::string>
            {
                public:
                    Iterator(struct data* in);
                    Iterator& operator++();
                    value_type operator*() {return out;};

                private:
                    int _index;
                    struct data* _data;
                    std::string out;

            };
            std::vector<void*> def_val;
            std::vector<std::string> names;
            std::vector<std::string> types;
            std::vector<std::array<bool, 3>> attrs; //unique[0] autoincrement[1] key[2]
            std::vector<std::vector<void*>> cells;
            Iterator begin() {return Iterator(this);};
            Iterator end();
            int _size = 0, length = 0;
        };
        std::unordered_map<std::string, data> bd;
        int _size;


};
}