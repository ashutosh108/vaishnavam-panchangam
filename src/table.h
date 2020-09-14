#ifndef VP_TABLE_H
#define VP_TABLE_H

#include <string>
#include <vector>

namespace vp {

class Table
{
private:
    struct Cell {
        std::string text;
        bool is_header;
        std::string classes;
        Cell(std::string _text, bool _is_header=false, std::string _classes="") : text(_text), is_header(_is_header), classes(_classes){}
    };
    struct Row {
        std::vector<Cell> data;
        std::string classes;
        Row(std::string classes_=""):classes(classes_){}
    };

    std::vector<Row> rows;

public:
    Table();
    int width() const;
    int height() const;
    void add_cell(std::string text, std::string classes="");
    void add_cell(std::string_view text, std::string classes="");
    void add_cell(const char * const text, std::string classes="");
    void add_header_cell(std::string text, std::string classes="");
    Cell & at(int row, int col);
    const Cell & at(int row, int col) const;
    void start_new_row(std::string classes="");

    struct CallBack {
        virtual void row_begin(std::string_view classes="") = 0;
        virtual void row_end() = 0;
        virtual void cell(std::string_view text, std::string_view classes="") = 0;
        virtual void header_cell(std::string_view text, std::string_view classes="") = 0;
    };

    void iterate(CallBack & it) const;
};

} // namespace vp

#endif // VP_TABLE_H
