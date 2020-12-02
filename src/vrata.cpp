#include "vrata.h"

#include <unordered_set>

namespace vp {

bool operator==(const Vrata &v1, const Vrata &v2)
{
    // non-symmetrical compare for more elegant tests:
    // if v1's paran_start/end is nullopt, then v2's paran_start_end can be anything.
    // But if v1's paran_start/end is set, then v2's one must match.
    return v1.type == v2.type && v1.date == v2.date && v1.paran.type == v2.paran.type &&
           v1.masa == v2.masa && v1.paksha == v2.paksha &&
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
        "Kāmadā",             // 01 amānta Chaitra śukla-pakṣa
        "Varūthinī",          // 02 amānta Chaitra kṛṣṇa-pakṣa
        "Mohinī",             // 03 amānta Vaiśākha śukla-pakṣa
        "Aparā",              // 04 amānta Vaiśākha kṛṣṇa-pakṣa
        "Nirjalā",            // 05 amānta Jyeṣṭha śukla-pakṣa
        "Yoginī",             // 06 amānta Jyeṣṭha kṛṣṇa-pakṣa
        "Śayanī",             // 07 amānta Āṣāḍha śukla-pakṣa
        "Kāmikā",             // 08 amānta Āṣāḍha kṛṣṇa-pakṣa
        "Pāvitrā",            // 09 amānta Śṛāvaṇa śukla-pakṣa
        "Ajā",                // 10 amānta Śṛāvaṇa kṛṣṇa-pakṣa
        "Pārśva-parivartinī", // 11 amānta Bhādrapada śukla-pakṣa
        "Indirā",             // 12 amānta Bhādrapada kṛṣṇa-pakṣa
        "Pāśāṅkuśā",          // 13 amānta Aśvin śukla-pakṣa
        "Ramā",               // 14 amānta Aśvin kṛṣṇa-pakṣa
        "Prabodhinī",         // 15 amānta Kārtika śukla-pakṣa
        "Utpattikā",          // 16 amānta Kārtika kṛṣṇa-pakṣa
        "Mokṣadā",            // 17 amānta Mārgaśīrṣa śukla-pakṣa
        "Saphalā",            // 18 amānta Mārgaśīrṣa kṛṣṇa-pakṣa
        "Putradā",            // 19 amānta Pauṣa śukla-pakṣa
        "Ṣaṭ-tilā",           // 20 amānta Pauṣa kṛṣṇa-pakṣa
        "Jayā",               // 21 amānta Māgha śukla-pakṣa
        "Vijayā",             // 22 amānta Māgha kṛṣṇa-pakṣa
        "Āmalakī",            // 23 amānta Phālguna śukla-pakṣa
        "Pāpamocanī",         // 24 amānta Phālguna kṛṣṇa-pakṣa
        "Kamalā",             // 25 adhika-māsa śukla-pakṣa
        "Padmā"               // 26 adhika-māsa kṛṣṇa-pakṣa
    };
    return static_names;
}

const std::vector<std::string> & ekadashi_names_rus()
{
    static const std::vector<std::string> static_names{
        "Кāмадā",             // 01
        "Варӯтӿинӣ",          // 02
        "Мохинӣ",             // 03
        "Апарā",              // 04
        "Нирџалā",            // 05
        "Йогинӣ",             // 06
        "Щайанӣ",             // 07
        "Кāмикā",             // 08
        "Пāвитрā",            // 09
        "Аџā",                // 10
        "Пāрщва-паривартинӣ", // 11
        "Индирā",             // 12
        "Пāщāӈкущā",          // 13
        "Рамā",               // 14
        "Прабодӿинӣ",         // 15
        "Утпаттикā",          // 16
        "Мокшадā",            // 17
        "Сапӿалā",            // 18
        "Путрадā",            // 19
        "Шат̣тилā",            // 20
        "Џайā",               // 21
        "Виџайā",             // 22
        "Āмалакӣ",            // 23
        "Пāпамочанӣ",         // 24
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

Vrata Vrata::SampleVrata()
{
    using namespace date;
    using namespace std::chrono_literals;
    vp::Vrata_Time_Points time_points{
        JulDays_UT{date::sys_days{2000_y/1/1} + 4h + 40min}, // ativrddha_54gh_40vigh; // for ativRddhA
        JulDays_UT{date::sys_days{2000_y/1/1} + 4h + 45min}, // vrddha_55gh;        // vRddhA
        JulDays_UT{date::sys_days{2000_y/1/1} + 4h + 50min}, // samyam_55gh_50vigh; // samyam
        JulDays_UT{date::sys_days{2000_y/1/1} + 4h + 55min}, // hrasva_55gh_55vigh; // hrasva
        JulDays_UT{date::sys_days{2000_y/1/1} + 5h}, // arunodaya;          // 56gh = 4gh before sūryodaya

        JulDays_UT{date::sys_days{1999_y/12/31} + 3h}, // dashami_start;
        JulDays_UT{date::sys_days{2000_y/1/1} + 3h}, // ekadashi_start;
        JulDays_UT{date::sys_days{2000_y/1/2} + 3h}, // dvadashi_start;
        JulDays_UT{date::sys_days{2000_y/1/3} + 3h}, // trayodashi_start;
    };
    Vrata vrata{Vrata_Type::Ekadashi, date::year{2000}/1/1, Chandra_Masa::Margashirsha, Paksha::Krishna, Paran{}, sample_location, time_points};
    vrata.sunset0 = vp::JulDays_UT{date::sys_days{1999_y/12/31} + 21h};
    vrata.sunrise1 = vp::JulDays_UT{date::sys_days{2000_y/1/1} + 6h};
    vrata.sunrise2 = vp::JulDays_UT{date::sys_days{2000_y/1/2} + 6h};
    vrata.sunset2 = vp::JulDays_UT{date::sys_days{2000_y/1/2} + 21h};
    vrata.sunrise3 = vp::JulDays_UT{date::sys_days{2000_y/1/3} + 6h};
    vrata.sunset3 = vp::JulDays_UT{date::sys_days{2000_y/1/3} + 21h};

    return vrata;
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
        // backup: if we don't have any vratas calculated, then decrease starting date by 13 days to ensure we don't skip any vratas in between.
        // Not sure if this code path is realistically reachable, though.
        return current_start_date - date::days{13};
    }
    // General case: step 16 days back from current known vrata.
    // 16 days between two sequential Ekādaśīs is a maximum I've seen
    // anywhere so far (e.g. Udupi 2020-09-27 and 2020-10-13).
    return *l_min_date - date::days{16};
}

} // namespace vp
