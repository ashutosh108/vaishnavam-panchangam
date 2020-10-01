#include "html-parser.h"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <regex>

inline std::string_view s_view(const std::string::const_iterator & iter, std::string::size_type size) {
    if (size == 0) return {};
    return {&*iter, size};
}

std::optional<html::Token> html::TokenStream::next_token()
{
    enum class State {
        SkipUntilTagStart, SkipUntilTagName, GetTagName, GetAttributes, GetTrail, End
    };
    State state{State::SkipUntilTagStart};
    std::string::const_iterator tag_name_start = iter;
    std::string::size_type tag_name_size{0};
    std::string::const_iterator attributes_start = iter;
    std::string::size_type attributes_size{0};
    std::string::const_iterator trail_start = iter;
    std::string::size_type trail_size{0};

    for (; iter != html.end() && state != State::End; ++iter) {
        auto c = *iter;
        switch (state) {
        case State::SkipUntilTagStart:
            if (c == '<') {
                state = State::SkipUntilTagName;
            }
            break;
        case State::SkipUntilTagName:
            if (std::isalpha(c) || c == '/') {
                tag_name_start = iter;
                state = State::GetTagName;
            }
            break;
        case State::GetTagName:
            if (std::isalpha(c) || c == '/') {
            } else {
                tag_name_size = static_cast<std::string::size_type>(iter - tag_name_start);
                attributes_start = iter;
                trail_start = iter+1;
                state = (c == '>' ? State::GetTrail : State::GetAttributes);
            }
            break;
        case State::GetAttributes:
            if (c != '>') {
            } else {
                attributes_size = static_cast<std::string::size_type>(iter - attributes_start);
                trail_start = iter+1;
                state = State::GetTrail;
            }
            break;
        case State::GetTrail:
            if (c != '<') {
            } else {
                trail_size = static_cast<std::string::size_type>(iter - trail_start);
                --iter; // leave '<' char to be consumed during next call to next_token()
                state = State::End;
            }
            break;
        case State::End:
            assert(false);
        }
    }

    // if we exited the loop because end of string, make sure we set proper length for trail
    if (state == State::GetTrail) {
        trail_size = static_cast<std::string::size_type>(iter - trail_start);
    }

    std::string_view tag_name = s_view(tag_name_start, tag_name_size);
    std::string_view attributes = s_view(attributes_start, attributes_size);
    std::string_view trail = s_view(trail_start, trail_size);

    if (tag_name.empty()) {
        return std::nullopt;
    }

    return Token{tag_name, attributes, trail};
}

std::optional<std::string> html::Token::get_attr(const std::string & attr)
{
    std::string regex_str{std::string{R"~(\b)~"} + attr + R"~(\s*=\s*["']?(\d+)["']?\b)~"};
    std::regex regex{regex_str};
    std::smatch match;
    std::string attr_string{tag_attributes};
    if (std::regex_search(attr_string, match, regex)) {
        if (match.size() >= 2) {
            return match[1].str();
        }
    }
    return std::nullopt;
}

unsigned long html::Token::get_attr_ul_or_default(const std::string & attr, unsigned long default_val)
{
    auto val = get_attr(attr);
    if (val) {
        const char * begin = val->c_str();
        char * str_end{};
        // since from_chars isn't available in gcc 7.x, have to use strtol instead
        long long_val = std::strtol(begin, &str_end, 10);
        if (str_end != begin && long_val >= 0) {
            return static_cast<unsigned long>(long_val);
        }
    }
    return default_val;
}
