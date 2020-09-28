#include "catch-formatters.h"

#include "table.h"

TEST_CASE("add_cell increases width by 1") {
    vp::Table table;

    CHECK(table.width() == 0);
    table.add_cell("qwe");
    REQUIRE(table.width() == 1);
    table.add_cell("qwe");
    REQUIRE(table.width() == 2);
    table.add_cell("qwe");
    REQUIRE(table.width() == 3);
}

TEST_CASE("add_header_cell increases width by 1") {
    vp::Table table;

    CHECK(table.width() == 0);
    table.add_header_cell("qwe");
    REQUIRE(table.width() == 1);
    table.add_header_cell("qwe");
    REQUIRE(table.width() == 2);
    table.add_header_cell("qwe");
    REQUIRE(table.width() == 3);
}

TEST_CASE("can iterate over cells with iterate()") {
    vp::Table table;
    table.add_cell("cell1,1");
    table.add_cell("cell1,2");
    table.add_cell("cell1,3");

    struct MyIterator : vp::Table::CallBack {
        bool row_began = false;
        bool row_ended = false;
        int cell_count = 0;
        void row_begin(std::string_view /*classes*/) override { row_began = true; }
        void row_end() override { row_ended = true; }
        void cell(std::string_view text, std::string_view /*classes*/, int /*rowspan*/) override {
            ++cell_count;
            auto expected = fmt::format("cell1,{}", cell_count);
            REQUIRE(text == expected);
        }
        void header_cell(std::string_view /*text*/, std::string_view /*classes*/, int /*rowspan*/) override {
            REQUIRE(false);
        }
    };

    MyIterator it;
    table.iterate(it);

    REQUIRE(it.row_began);
    REQUIRE(it.row_ended);
    REQUIRE(it.cell_count == 3);
}

TEST_CASE("can iterate over cells with raw loops and at()") {
    vp::Table table;
    table.add_cell("cell1,1");
    table.add_cell("cell1,2");
    table.add_cell("cell1,3");
    for(std::size_t row=0; row < table.height(); ++row) {
        for (std::size_t col=0; col < table.width(); ++col) {
            auto expected = fmt::format("cell{},{}", row+1, col+1);
            REQUIRE(expected == table.at(row, col).text);
        }
    }
}

TEST_CASE("can start_new_row() for a table") {
    vp::Table table;
    table.add_cell("cell1,1");
    table.add_cell("cell1,2");
    table.start_new_row();
    table.add_cell("cell2,1");
    table.add_cell("cell2,2");

    for(std::size_t row=0; row < table.height(); ++row) {
        for (std::size_t col=0; col < table.width(); ++col) {
            auto expected = fmt::format("cell{},{}", row+1, col+1);
            REQUIRE(expected == table.at(row, col).text);
        }
    }
}

TEST_CASE("can set classes for td") {
    vp::Table table;
    table.add_cell("cell1,1", "class1");
    table.add_cell("cell1,2", "class1");
    table.start_new_row();
    table.add_cell("cell2,1", "class2 class3");
    table.add_cell("cell2,2", "class2 class3");

    REQUIRE(table.at(0, 0).classes == "class1");
    REQUIRE(table.at(0, 1).classes == "class1");
    REQUIRE(table.at(1, 0).classes == "class2 class3");
    REQUIRE(table.at(1, 1).classes == "class2 class3");
}

TEST_CASE("merge_cells_into_rowspans doesn't change table with different cell values") {
    vp::Table table;
    table.add_cell("cell1,1");
    table.add_cell("cell1,2");
    table.start_new_row();
    table.add_cell("cell2,1");
    table.add_cell("cell2,2");

    table.merge_cells_into_rowspans();

    for (auto & cell : table) {
        auto expected = fmt::format("cell{},{}", cell.row+1, cell.col+1);
        REQUIRE(cell.rowspan == 1);
        REQUIRE(cell.text == expected);
    }
}

TEST_CASE("can merge cells into rowspans") {
    vp::Table table;
    table.add_cell("cellx,1");
    table.add_cell("cell1,2");
    table.start_new_row();
    table.add_cell("cellx,1");
    table.add_cell("cell2,2");

    table.merge_cells_into_rowspans();

    REQUIRE(table.at(0, 0).rowspan == 2);
    REQUIRE(table.at(0, 1).rowspan == 1);
    REQUIRE(table.at(1, 0).rowspan == 0); // 0 rowspan means this cell is alreavy covered by another cell's rowspan
}

TEST_CASE("doesn't merge cells with non-existing cells in between") {
    vp::Table table;
    table.add_cell("cellx,1");
    table.add_cell("cell1,2");
    table.start_new_row();
    table.start_new_row();
    table.add_cell("cellx,1");
    table.add_cell("cell2,2");

    table.merge_cells_into_rowspans();

    for (auto & cell : table) {
        REQUIRE(cell.rowspan == 1);
    }
}
