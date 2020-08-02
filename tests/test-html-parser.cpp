#include "catch-formatters.h"

#include "html-parser.h"

TEST_CASE("HtmlTokenStream can stream tokens") {
    html::TokenStream p("<html>");
    auto token = p.next_token();
    REQUIRE(token.has_value());
    REQUIRE("html" == token->tag_name);
}

TEST_CASE("HtmlTokenStream returns empty token at the end") {
    html::TokenStream p("<html>");
    auto token = p.next_token();
    REQUIRE(token.has_value());
    REQUIRE("html" == token->tag_name);

    auto token2 = p.next_token();
    REQUIRE(!token2.has_value());
}

TEST_CASE("HtmlTokenStream can stream three tokens") {
    html::TokenStream p("<tag attr=val>text<tag attr=val>text<tag attr=val>text");

    int count = 0;
    while (auto token = p.next_token()) {
        REQUIRE(token->tag_name == "tag");
        REQUIRE(token->tag_attributes == " attr=val");
        REQUIRE(token->text_after == "text");
        ++count;
    }
    REQUIRE(count == 3);
}

TEST_CASE("HtmlTokenStream can handle closing tags") {
    html::TokenStream p{"<html></html>"};
    int count = 0;
    while (auto token = p.next_token()) {
        if (count == 0)  {
            REQUIRE(token->tag_name == "html");
        } else {
            REQUIRE(token->tag_name == "/html");
        }
        ++count;
    }
}

TEST_CASE("Token get_attr_ul_or_default works") {
    html::TokenStream p{"<tag attr=\"123\" attr2=\"456\" >"};
    auto t = p.next_token();
    REQUIRE(t.has_value());

    REQUIRE(123UL == t->get_attr_ul_or_default("attr", 3UL));
    REQUIRE(456UL == t->get_attr_ul_or_default("attr2", 3UL));
}
