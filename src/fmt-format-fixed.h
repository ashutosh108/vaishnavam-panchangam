#ifndef FMTFORMATFIXED_H
#define FMTFORMATFIXED_H

#include "disable-warning.h"
DISABLE_WARNING_PUSH
DISABLE_WARNING_SIGN_CONVERSION
#include <fmt/format.h>
DISABLE_WARNING_POP

#include <optional>

template<typename T>
struct fmt::formatter<std::optional<T>> : fmt::formatter<T> {
    template<typename FormatContext>
    auto format(const std::optional<T> & t, FormatContext & ctx) {
        if (!t.has_value()) {
            return fmt::format_to(ctx.out(), "(null)");
        }
        return fmt::format_to(ctx.out(), "{}", *t);
    }
};


#endif // FMTFORMATFIXED_H
