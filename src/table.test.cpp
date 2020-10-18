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
        void cell(const vp::Table::Cell & cell) override {
            ++cell_count;
            auto expected = fmt::format("cell1,{}", cell_count);
            REQUIRE(cell.text == expected);
        }
        void header_cell(const vp::Table::Cell & /*cell*/) override {
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

TEST_CASE("doesn't merge cells marked as unmergeable") {
    vp::Table table;
    table.add_unmergeable_cell("cellx,1");
    table.add_cell("cell1,2");
    table.start_new_row();
    table.add_cell("cellx,1");
    table.add_cell("cell2,2");

    table.merge_cells_into_rowspans();

    for (auto & cell : table) {
        REQUIRE(cell.rowspan == 1);
    }
}

TEST_CASE("table cell merge keeps rowspans to 12 rows max with even split") {
    vp::Table table;
    for (int i=0; i<13; ++i) {
        table.add_cell("text");
        table.start_new_row();
    }
    table.merge_cells_into_rowspans();

    REQUIRE(table.at(0,0).rowspan == 7);
    REQUIRE(table.at(7,0).rowspan == 6);
    for (auto & cell : table) {
        if (cell.row != 0 && cell.row != 7) {
            REQUIRE(cell.rowspan == 0);
        }
    }
}

TEST_CASE("table cell horizontal merge into colspans works") {
    vp::Table table;
    table.add_cell("cell1,x");
    table.add_cell("cell1,x");
    table.start_new_row();
    table.add_cell("cell2,1");
    table.add_cell("cell2,2");

    table.merge_cells_into_colspans();

    REQUIRE(table.at(0,0).colspan == 2);
    REQUIRE(table.at(0,1).colspan == 0);
}

TEST_CASE("can set column widths") {
    vp::Table table;
    table.set_column_widths({25.0,25.0,25.0,25.0});
}

TEST_CASE("table can set even/odd classes for a column with proper rowspan handling") {
    vp::Table table;
    table.add_cell("row1-2");
    table.start_new_row();
    table.add_cell("row1-2");
    table.start_new_row();
    table.add_cell("row3");
    table.start_new_row();
    table.add_cell("row4");

    table.merge_cells_into_rowspans();
    table.add_even_odd_classes_for_col(0);

    REQUIRE(table.at(0,0).classes == "odd");
    REQUIRE(table.at(2,0).classes == "even");
    REQUIRE(table.at(3,0).classes == "odd");
}

TEST_CASE("table can set even/odd classes for a column with proper rowspan handling while (starting from even)") {
    vp::Table table;
    table.add_cell("row1-2");
    table.start_new_row();
    table.add_cell("row1-2");
    table.start_new_row();
    table.add_cell("row3");
    table.start_new_row();
    table.add_cell("row4");

    table.merge_cells_into_rowspans();
    table.add_even_odd_classes_for_col(0, vp::Table::StartFrom::Even);

    REQUIRE(table.at(0,0).classes == "even");
    REQUIRE(table.at(2,0).classes == "odd");
    REQUIRE(table.at(3,0).classes == "even");
}

TEST_CASE("add_even_odd_classes_for_col works when some row is empty") {
    vp::Table table;
    table.add_cell("row1");
    table.start_new_row();
    table.start_new_row();
    table.add_cell("row3");
    table.start_new_row();
    table.add_cell("row4");

    table.add_even_odd_classes_for_col(0);

    REQUIRE(table.at(0,0).classes == "odd");
    REQUIRE(table.at(2,0).classes == "even");
    REQUIRE(table.at(3,0).classes == "odd");
}

TEST_CASE("merging >12 rows of table cell gives proper classes to help remove borders in CSS") {
    vp::Table table;
    // 27 to ensure we get three runs (9 each in this case)
    for (int i=0; i<27; ++i) {
        table.add_cell("text");
        table.start_new_row();
    }
    table.merge_cells_into_rowspans();

    using Catch::Matchers::Contains;
    REQUIRE_THAT(table.at(0,0).classes, Contains("merge-to-bottom"));
    REQUIRE_THAT(table.at(9,0).classes, Contains("merge-to-top"));
    REQUIRE_THAT(table.at(9,0).classes, Contains("merge-to-bottom"));
    REQUIRE_THAT(table.at(18,0).classes, Contains("merge-to-top"));
}

TEST_CASE("can set 'title' (something like tooltip) for cells") {
    vp::Table table;
    table.add_cell("text").set_title("specific title");

    REQUIRE(table.at(0, 0).title == "specific title");
}
