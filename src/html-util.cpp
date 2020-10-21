#include "html-util.h"

std::string html::escape_attribute(const std::string &s)
{
    std::string escaped;
    for (auto c : s) {
        switch(c) {
        case '&': escaped += "&amp;"; break;
        case '"': escaped += "&quot;"; break;
        case '\'': escaped += "&#039;"; break;
        case '<': escaped += "&lt;"; break;
        case '>': escaped += "&gt;"; break;
        default: escaped += c; break;
        }
    }
    return escaped;
}
