#include "vrata.h"

#include <unordered_set>

namespace vp {

bool operator==(const Vrata &v1, const Vrata &v2)
{
    // non-symmetrical compare for more elegant tests:
    // if v1's paran_start/end is nullopt, then v2's paran_start_end can be anything.
    // But if v1's paran_start/end is set, then v2's one must match.
    return v1.type == v2.type && v1.date == v2.date && v1.paran.type == v2.paran.type &&
            (
                (!v1.paran.paran_start.has_value()) ||
                (v1.paran.paran_start == v2.paran.paran_start)
            ) &&
            (
                (!v1.paran.paran_end.has_value()) ||
                (v1.paran.paran_end == v2.paran.paran_end)
            );
}

bool operator!=(const Vrata &v1, const Vrata &v2)
{
    return !(v1 == v2);
}

const std::vector<std::string> & ekadashi_names()
{
    static const std::vector<std::string> static_names{
        "Pāpamocanī",         // 01
        "Kāmadā",             // 02
        "Varūthinī",          // 03
        "Mohinī",             // 04
        "Aparā",              // 05
        "Nirjalā",            // 06
        "Yoginī",             // 07
        "Śayanī",             // 08
        "Kāmikā",             // 09
        "Pāvitrā",            // 10
        "Ajā",                // 11
        "Pārśva-parivartinī", // 12
        "Indirā",             // 13
        "Pāśāṅkuśā",          // 14
        "Ramā",               // 15
        "Prabodhinī",         // 16
        "Utpattikā",          // 17
        "Mokṣadā",            // 18
        "Saphalā",            // 19
        "Putradā",            // 20
        "Ṣaṭ-tilā",           // 21
        "Jayā",               // 22
        "Vijayā",             // 23
        "Āmalakī",            // 24
        "Kamalā",             // 25
        "Padmā"               // 26
    };
    return static_names;
}

const std::vector<std::string> & ekadashi_names_rus()
{
    static const std::vector<std::string> static_names{
        "Пāпамочанӣ",         // 01
        "Кāмадā",             // 02
        "Варӯтӿинӣ",          // 03
        "Мохинӣ",             // 04
        "Апарā",              // 05
        "Нирџалā",            // 06
        "Йогинӣ",             // 07
        "Щайанӣ",             // 08
        "Кāмикā",             // 09
        "Пāвитрā",            // 10
        "Аџā",                // 11
        "Пāрщва-паривартинӣ", // 12
        "Индирā",             // 13
        "Пāщāӈкущā",          // 14
        "Рамā",               // 15
        "Прабодӿинӣ",         // 16
        "Утпаттикā",          // 17
        "Мокшадā",            // 18
        "Сапӿалā",            // 19
        "Путрадā",            // 20
        "Шат̣тилā",            // 21
        "Џайā",               // 22
        "Виџайā",             // 23
        "Āмалакӣ",            // 24
        "Камалā",             // 25
        "Падмā"               // 26
    };
    return static_names;
}

bool ekadashi_name_rus_is_valid(const std::string &name)
{
    static std::unordered_set<std::string> valid_names = []() {
        std::unordered_set<std::string> v;
        for (const auto & n : ekadashi_names_rus()) {
            v.insert(n);
        }
        v.insert("Шат̣-тилā"); // alternative spelling
        v.insert("Амалакӣ");  // alternative spelling
        return v;
    }();
    return valid_names.find(name) != valid_names.end();
}

date::year_month_day Vrata::local_paran_date() const
{
    auto delta = date::days{vp::is_atirikta(type) ? 2 : 1};
    return date::year_month_day{date::sys_days{date} + delta};
}

std::string Vrata::location_name() const
{
    if (location.latitude_adjusted) {
        return fmt::format("{} (adjusted to {} {})", location.name, location.latitude, location.longitude);
    }
    return std::string{location.name};
}

double_ghatikas Vrata_Time_Points::dashami_length() const {
    return ekadashi_start - dashami_start;
}

double_ghatikas Vrata_Time_Points::ekadashi_length() const {
    return dvadashi_start - ekadashi_start;
}

double_ghatikas Vrata_Time_Points::dvadashi_length() const {
    return trayodashi_start - dvadashi_start;
}

JulDays_UT Vrata_Time_Points::ativrddhaditvam_timepoint() const
{
    auto a = ativrddhaadi();
    switch (a) {
    case Vrata_Time_Points::Ativrddhaadi::ativrddha:
        return ativrddha_54gh_40vigh;
    case Vrata_Time_Points::Ativrddhaadi::vrddha:
        return vrddha_55gh;
    case Vrata_Time_Points::Ativrddhaadi::samyam:
        return samyam_55gh_50vigh;
    case Vrata_Time_Points::Ativrddhaadi::hrasva:
        return hrasva_55gh_55vigh;
    }
    throw std::runtime_error("unknown Ativrddhaadi status: " + std::to_string(static_cast<int>(a)));
}

Vrata_Time_Points::Ativrddhaadi Vrata_Time_Points::ativrddhaadi() const
{
    const double_ghatikas l10 = dashami_length();
    const double_ghatikas l11 = ekadashi_length();
    const double_ghatikas l12 = dvadashi_length();
    const double delta1 = (l11 - l10).count();
    const double delta2 = (l12 - l11).count();
    if (delta1 > 0.0 && delta2 > 0.0 && (delta1 >= 4.0 || delta2 >= 4.0)) {
        return Ativrddhaadi::ativrddha;
    }
    if (delta1 > 0.0 && delta2 > 0.0 && (delta1 >= 1.0 || delta2 >= 1.0)) {
        return Ativrddhaadi::vrddha;
    }
    if (delta1 < 0.0 && delta2 < 0.0) return Ativrddhaadi::hrasva;
            return Ativrddhaadi::samyam;
}

MinMaxDate VratasForDate::minmax_date() const
{
    auto [it_min, it_max] = std::minmax_element(
        vector.cbegin(), vector.cend(),
        [](const vp::MaybeVrata & l, const vp::MaybeVrata & r) -> bool {
            if (!l.has_value()) return r.has_value(); // nullopt < date; ! (nullopt < nullopt).
            if (!r.has_value()) return false; // ! (date < nullopt)
            return l->date < r->date;
        });
    std::optional<date::year_month_day> min_date;
    if (it_min != vector.cend()) {
        min_date = it_min->value().date;
    }
    std::optional<date::year_month_day> max_date;
    if (it_max != vector.cend()) {
        max_date = it_max->value().date;
    }
    return {min_date, max_date};
}

std::optional<date::sys_days> VratasForDate::min_date() const
{
    auto [l_min_date, l_max_date] = minmax_date();
    return l_min_date;
}

std::optional<date::sys_days> VratasForDate::max_date() const
{
    auto [l_min_date, l_max_date] = minmax_date();
    return l_max_date;
}

/**
 * True if all vratas in the set are within 2 days from one another.
 * (also true if vrata set is empty).
 * I've never seen in practice more than 1 day difference, but just to be safe I set the limit to 2 days.
 */
bool VratasForDate::all_from_same_ekadashi() const
{
    auto [min_date, max_date] = minmax_date();
    if (!min_date || !max_date) return true;
    date::days length = date::sys_days{*max_date} - date::sys_days{*min_date};
    return length <= date::days{2};
}

date::sys_days VratasForDate::guess_start_date_for_next_ekadashi(date::sys_days current_start_date)
{
    auto l_max_date = max_date();
    if (!l_max_date) {
        return current_start_date + date::days{12};
    }
    return *l_max_date + date::days{1};
}

date::sys_days VratasForDate::guess_start_date_for_prev_ekadashi(date::sys_days current_start_date)
{
    auto l_min_date = min_date();
    if (!l_min_date) {
        return current_start_date - date::days{13};
    }
    return *l_min_date - date::days{15};
}

} // namespace vp
