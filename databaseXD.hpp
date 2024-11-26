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
        struct data
        {
            std::vector<void*> def_val;
            std::vector<std::string> names;
            std::vector<std::string> types;
            std::vector<std::array<bool, 3>> attrs; //unique[0] autoincrement[1] key[2]
            std::vector<std::vector<void*>> cells;
            int _size = 0, length = 0;
        };
        class error_interface
        {
            public:
                error_interface(int n): id(n) {};
                error_interface(int n, data* _dt): id(n), _data(_dt){};
                bool is_ok() {return (id == 0);}
                class Iterator: public std::iterator<std::input_iterator_tag, std::string>
                {
                    public: 
                        Iterator(data* in);
                        Iterator& operator++();
                        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.index != b.index; };
                        reference operator*(){return out;}
                    private:
                        int index = -1;
                        data* _data;
                        std::string out;
                };
                Iterator begin()
                {
                    return Iterator(_data);
                }
                std::string get_error();
            private:
                int id;
                data* _data;
        };
        error_interface execute(std::istream &query);
        error_interface load_from_file(std::ifstream in);
        error_interface save_to_file(std::ofstream out);
        error_interface destroy();
    private:
        std::unordered_map<std::string, data> bd;
        int _size;


};
}