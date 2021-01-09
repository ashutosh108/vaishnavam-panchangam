#include "text-interface.h"

#include "calc.h"
#include "vrata_detail_printer.h"

#include <charconv>
#include <cstring>

using namespace vp;

namespace vp::text_ui {

namespace {
constexpr int len(const char * s) { return std::char_traits<char>::length(s); }
static_assert (len("123") == 3);
}

date::year_month_day parse_ymd(const std::string_view s) {
    // YYYY-MM-DD
    // 01234567890
    if (s.length() != len("YYYY-MM-DD")) return {};
    const char * const str = s.data();
    int year{};
    if (auto [p, e] = std::from_chars(str+0, str+len("YYYY"), year); e != std::errc{} || p != str+4)
        return {};
    unsigned int month{};
    if (auto [p, e] = std::from_chars(str+len("YYYY-"), str+len("YYYY-MM"), month); e != std::errc{} || p != str+7)
        return {};
    unsigned int day{};
    if (auto [p, e] = std::from_chars(str+len("YYYY-MM-"), str+len("YYYY-MM-DD"), day); e != std::errc{} || p != str+10)
        return {};
    return date::year{year}/date::month{month}/date::day{day};
}

const std::vector<Location> &LocationDb::locations() {
    static std::vector<Location> locations_ {
        { udupi_coord },
        { gokarna_coord },
        { newdelhi_coord },
        { manali_coord },
        { kalkuta_coord },
        { dushanbe_coord },
        { aktau_coord },
        { aktobe_coord },
        { perm_coord },
        { ufa_coord },
        { ekaterinburg_coord },
        { surgut_coord },
        { chelyabinsk_coord },
        { bishkek_coord },
        { almaata_coord },
        { tekeli_coord },
        { ustkamenogorsk_coord },
        { omsk_coord },
        { novosibirsk_coord },
        { barnaul_coord },
        { tomsk_coord },
        { kophangan_coord },
        { denpasar_coord },
        { mirnyy_coord },
        { habarovsk_coord },
        { vladivostok_coord },
        { petropavlovskkamchatskiy_coord },
        { erevan_coord },
        { tbilisi_coord },
        { samara_coord },
        { volgograd_coord },
        { ulyanovsk_coord },
        { pyatigorsk_coord },
        { stavropol_coord },
        { semikarakorsk_coord },
        { krasnodar_coord },
        { simferopol_coord },
        { donetsk_coord },
        { staryyoskol_coord },
        { voronezh_coord },
        { tambov_coord },
        { kazan_coord },
        { kirov_coord },
        { ryazan_coord },
        { moskva_coord },
        { spb_coord },
        { murmansk_coord },
        { kostomuksha_coord },
        { arhangelsk_coord },
        { smolensk_coord },
        { gomel_coord },
        { minsk_coord },
        { harkov_coord },
        { poltava_coord },
        { kremenchug_coord },
        { krivoyrog_coord },
        { kiev_coord },
        { nikolaev_coord },
        { odessa_coord },
        { kolomyya_coord },
        { kishinev_coord },
        { nicosia_coord },
        { riga_coord },
        { jurmala_coord },
        { tallin_coord },
        { vilnyus_coord },
        { varshava_coord },
        { vena_coord },
        { marsel_coord },
        { barcelona_coord },
        { madrid_coord },
        { london_coord },
        { fredericton_coord },
        { toronto_coord },
        { miami_coord },
        { cancun_coord },
        { meadowlake_coord },
    };
    return locations_;
}

std::optional<Location> LocationDb::find_coord(const char *location_name) {
    auto found = std::find_if(
        std::begin(locations()),
        std::end(locations()),
        [=](auto named_coord){
            return named_coord.name == location_name;
        }
    );
    if (found == std::end(locations())) return std::nullopt;
    return *found;
}

// try decreasing latitude until we get all necessary sunrises/sunsets
tl::expected<vp::Vrata, vp::CalcError> decrease_latitude_and_find_vrata(date::year_month_day base_date, Location location) {
    auto l = location;
    l.latitude_adjusted = true;
    while (1) {
        l.latitude.latitude -= 1.0;
        auto vrata = Calc{l}.find_next_vrata(base_date);
        // Return if have actually found vrata.
        // Also return if we ran down to low enough latitudes so that it doesn't
        // make sense to decrease it further; just report whatever error we got in that case.
        if (vrata || l.latitude.latitude <= 60.0) return vrata;
    }
}

vp::VratasForDate calc_one(date::year_month_day base_date, std::string location_name, CalcFlags flags)
{
    vp::VratasForDate vratas;
    auto location = LocationDb::find_coord(location_name.c_str());
    if (!location) {
        vratas.push_back(tl::make_unexpected(CantFindLocation{std::move(location_name)}));
        return vratas;
    }
    vratas.push_back(calc_one(base_date, *location, flags));
    return vratas;
}

tl::expected<vp::Vrata, vp::CalcError> calc_one(date::year_month_day base_date, Location location, CalcFlags flags) {
    // Use immediately-called lambda to ensure Calc is destroyed before more
    // will be created in decrease_latitude_and_find_vrata()
    auto vrata = [&](){
        return Calc{Swe{location, flags}}.find_next_vrata(base_date);
    }();
    if (vrata) return vrata;

    auto e = vrata.error();
    // if we are in the northern areas and the error is that we can't find sunrise or sunset, then try decreasing latitude until it's OK.
    if ((std::holds_alternative<CantFindSunriseAfter>(e) || std::holds_alternative<CantFindSunsetAfter>(e)) && location.latitude.latitude > 60.0) {
        return decrease_latitude_and_find_vrata(base_date, location);
    }
    // Otherwise return whatever error we've got.
    return vrata;
}


void report_details(const vp::MaybeVrata & vrata, fmt::memory_buffer & buf) {
    if (!vrata.has_value()) {
        fmt::format_to(buf,
                       "# {}*\n"
                       "Can't find next Ekadashi, sorry.\n"
                       "* Error: {}\n",
                       vrata->location_name(),
                       vrata.error());
    } else {
        Vrata_Detail_Printer vd{*vrata};
        fmt::format_to(buf, "{}\n\n", vd);
    }

}

// Find next ekAdashI vrata for the named location, report details to the output buffer.
tl::expected<vp::Vrata, vp::CalcError> calc_and_report_one(date::year_month_day base_date, Location location, fmt::memory_buffer & buf) {
    auto vrata = calc_one(base_date, location);
    report_details(vrata, buf);
    return vrata;
}

tl::expected<vp::Vrata, vp::CalcError> find_calc_and_report_one(date::year_month_day base_date, const char * location_name, fmt::memory_buffer & buf) {
    std::optional<Location> coord = LocationDb::find_coord(location_name);
    if (!coord) {
        fmt::format_to(buf, "Location not found: '{}'\n", location_name);
        return tl::make_unexpected(CantFindLocation{location_name});
    }
    return calc_and_report_one(base_date, *coord, buf);
}

namespace {
void daybyday_print_header(date::year_month_day base_date, const Location & coord, const DayByDayInfo & info, fmt::memory_buffer & buf)
{
    fmt::format_to(buf,
                   "{} {}\n",
                   coord.name, base_date);
    fmt::format_to(buf, FMT_STRING("Saura māsa: {}"), info.saura_masa);
    if (info.saura_masa_until) {
        fmt::format_to(buf, FMT_STRING(" (until {})"), *info.saura_masa_until);
    }
    fmt::format_to(buf, FMT_STRING("\n"));
    fmt::format_to(buf, FMT_STRING("Chāndra māsa: {}"), info.chandra_masa);
    if (info.chandra_masa_until) {
        fmt::format_to(buf, FMT_STRING(" (until {})"), *info.chandra_masa_until);
    }
    fmt::format_to(buf, FMT_STRING("\n"));
}

void daybyday_add_tithi_events(vp::JulDays_UT from, vp::JulDays_UT to, const vp::Calc & calc, DayByDayInfo & info) {
    const auto min_tithi = calc.swe.get_tithi(from).floor();
    const auto max_tithi = calc.swe.get_tithi(to).ceil() + 1.0;
    auto start = from - std::chrono::hours{36};
    // need "!=" to handle cross-amavasya cases correctly, when max_tithi is less than min_tithi
    for (vp::Tithi tithi = min_tithi; tithi != max_tithi; tithi += 1.0) {
        auto tithi_start = calc.find_exact_tithi_start(start, tithi);
        if (tithi_start >= info.sunrise1) {
            if (info.tithi == vp::DiscreteTithi::Unknown()) {
                info.tithi = vp::DiscreteTithi{tithi};
                info.tithi_until = tithi_start;
            } else if (tithi_start < info.sunrise2 && info.tithi2 == vp::DiscreteTithi::Unknown()) {
                info.tithi2 = vp::DiscreteTithi{tithi};
                info.tithi2_until = tithi_start;
            }
        }
        auto description = fmt::format("{:d} starts", tithi);
        if (tithi.is_shukla_pratipat()) {
            description += fmt::format(FMT_STRING(", {} māsa starts"), calc.chandra_masa_amanta(tithi_start + double_days{1.0}));
        }
        info.events.push_back(NamedTimePoint{description, tithi_start});
        if (tithi.is_dvadashi()) {
            const auto dvadashi_end = calc.find_exact_tithi_start(tithi_start, tithi+1.0);
            const auto dvadashi_quarter_end = calc.proportional_time(tithi_start, dvadashi_end, 0.25);
            //            auto dvadashi_quarter_end = calc.find_exact_tithi_start(start, tithi+0.25);
            info.events.push_back(NamedTimePoint{fmt::format("First quarter of {:d} ends", tithi), dvadashi_quarter_end});
        }
    }
}

void daybyday_add_nakshatra_events(vp::JulDays_UT from, vp::JulDays_UT to, const vp::Calc & calc, std::vector<NamedTimePoint> & events) {
    const auto min_nakshatra = calc.swe.get_nakshatra(from).floor();
    const auto max_nakshatra = calc.swe.get_nakshatra(to).ceil() + 1.0;
    auto start = from - std::chrono::hours{36}; // to ensure we get beginning of first nakshatra
    for (vp::Nakshatra n = min_nakshatra; n != max_nakshatra; ++n) {
        auto nakshatra_start = calc.find_nakshatra_start(start, n);
        events.push_back(NamedTimePoint{fmt::format(FMT_STRING("{} starts"), n), nakshatra_start});
    }
    // TODO: implement
}


DayByDayInfo daybyday_events(date::year_month_day base_date, const vp::Calc & calc) {
    DayByDayInfo info;
    const auto local_astronomical_midnight = calc.calc_astronomical_midnight(base_date);
    const auto sunrise = calc.swe.find_sunrise(local_astronomical_midnight);
    if (sunrise) {
        info.sunrise1 = *sunrise;
        info.events.push_back(NamedTimePoint{"sunrise", *sunrise});
        const auto arunodaya = calc.arunodaya_for_sunrise(*sunrise);
        if (arunodaya) {
            info.events.push_back(NamedTimePoint{"arunodaya", *arunodaya});
        }

        const auto sunset = calc.swe.find_sunset(*sunrise);
        if (sunset) {
            info.sunset1 = *sunset;
            info.events.push_back(NamedTimePoint{"sunset", *sunset});
            info.events.push_back(NamedTimePoint{"1/5 of daytime", calc.proportional_time(*sunrise, *sunset, 0.2)});
            info.events.push_back(NamedTimePoint{"middle of the day", calc.proportional_time(*sunrise, *sunset, 0.5)});
            const auto sunrise2 = calc.swe.find_sunrise(*sunset);
            if (sunrise2) {
                info.sunrise2 = *sunrise2;
                const auto middle_of_night = calc.proportional_time(*sunset, *sunrise2, 0.5);
                info.events.push_back(NamedTimePoint{"middle of the night", middle_of_night});
                info.events.push_back(NamedTimePoint{"next sunrise", *sunrise2});
                const auto earliest_timepoint = arunodaya ? * arunodaya : *sunrise;
                const auto latest_timepoint = *sunrise2;
                daybyday_add_tithi_events(earliest_timepoint, latest_timepoint, calc, info);
                daybyday_add_nakshatra_events(earliest_timepoint, latest_timepoint, calc, info.events);
            }
        }
    }
    return info;
}

auto NamedPointComparator = [](const NamedTimePoint & left, const NamedTimePoint & right) {
    return left.time_point < right.time_point;
};
void add_to_sorted(NamedTimePoints & points, NamedTimePoint point) {
    points.insert(
        std::upper_bound(
            points.begin(),
            points.end(),
            point,
            NamedPointComparator),
        point);
}

void daybyday_add_sauramasa_info(DayByDayInfo & info, const vp::Calc & calc) {
    if (info.events.empty()) {
        return;
    }

    if (info.sunrise1) {
        info.saura_masa = calc.saura_masa(*info.sunrise1);
    }

    const auto initial_time = info.events[0].time_point;
    const auto initial_masa = calc.saura_masa(initial_time);
    const auto last_time = info.events.back().time_point;

    const auto next_masa = initial_masa + 1;
    const auto next_masa_start = calc.find_sankranti(initial_time, next_masa);
    info.saura_masa_until = next_masa_start;
    if (next_masa_start <= last_time) {
        auto event_name = fmt::format(FMT_STRING("{} sankranti"), next_masa);
        add_to_sorted(info.events, NamedTimePoint{event_name, next_masa_start});
    }
}

void daybyday_add_chandramasa_info(DayByDayInfo & info, const vp::Calc & calc) {
    if (!info.sunrise1.has_value()) {
        return;
    }
    const auto initial_time = *info.sunrise1;
    info.chandra_masa = calc.chandra_masa_amanta(initial_time, &info.chandra_masa_until);
}
} // anonymous namespace

DayByDayInfo daybyday_calc_one(date::year_month_day base_date, Location coord, CalcFlags flags)
{
    Calc calc{Swe{coord, flags}};
    DayByDayInfo info = daybyday_events(base_date, calc);
    info.location = coord;
    info.date = base_date;
    std::stable_sort(info.events.begin(), info.events.end(), NamedPointComparator);
    daybyday_add_sauramasa_info(info, calc);
    daybyday_add_chandramasa_info(info, calc);
    return info;
}

/* print day-by-day report (-d mode) for a single date and single location */
void daybyday_print_one(date::year_month_day base_date, Location coord, fmt::memory_buffer & buf, vp::CalcFlags flags) {
    auto info = daybyday_calc_one(base_date, coord, flags);

    daybyday_print_header(base_date, coord, info, buf);

    for (const auto & e : info.events) {
        // add separator before sunrises to mark current day better
        if ((info.sunrise1 && e.time_point == *info.sunrise1) || (info.sunrise2 && e.time_point == *info.sunrise2)) {
            fmt::format_to(buf, FMT_STRING("-----\n"));
        }
        fmt::format_to(buf, "{} {}\n", vp::JulDays_Zoned{coord.time_zone(), e.time_point}, e.name);
    }
}

void daybyday_print_one(date::year_month_day base_date, const char * location_name, fmt::memory_buffer & buf, vp::CalcFlags flags) {
    const std::optional<Location> coord = LocationDb::find_coord(location_name);
    if (!coord) {
        fmt::format_to(buf, "Location not found: '{}'\n", location_name);
        return;
    }
    daybyday_print_one(base_date, *coord, buf, flags);
}

void calc_and_report_all(date::year_month_day d) {
    for (auto &l : LocationDb()) {
        fmt::memory_buffer buf;
        calc_and_report_one(d, l, buf);
        fmt::print("{}", std::string_view{buf.data(), buf.size()});
    }
}

namespace detail {
    fs::path determine_exe_dir(const char* argv0) {
        return fs::absolute(fs::path{argv0}).parent_path();
    }

    fs::path determine_working_dir(const char* argv0) {
        auto exe_dir = detail::determine_exe_dir(argv0);

        constexpr int max_steps_up = 2;

        for (int step=0; step < max_steps_up; ++step) {
            // Most common case: "eph" and "tzdata" directories exist in the same directory as .exe
            if (fs::exists(exe_dir / "eph")) {
                return exe_dir;
            }

            // But when running exes from Debug/ or Release/ subdirectories, we need to make one step up.
            // But only make one step up: if the data dir still doesn't exist there either, too bad...
            exe_dir = exe_dir.parent_path();
        }
        // fallback: return whatever we got, even if we couldn't find proper working dir.
        return exe_dir;
    }
}

namespace {
    struct CalcSettings {
        date::year_month_day date;
        vp::CalcFlags flags;
    };

    bool operator==(const CalcSettings & left, const CalcSettings & right);

    struct MyHash {
        std::size_t operator()(const CalcSettings & key) const {
            auto hy = std::hash<int>{}(key.date.year().operator int());
            auto hm = std::hash<unsigned int>{}(key.date.month().operator unsigned int());
            auto hd = std::hash<unsigned int>{}(key.date.day().operator unsigned int());
            using FlagsT = std::underlying_type_t<vp::CalcFlags>;
            auto hash_flags = std::hash<FlagsT>{}(static_cast<FlagsT>(key.flags));
            return hy ^ (hm << 1) ^ (hd << 2) ^ (hash_flags << 3);
        }
    };

    std::unordered_map<CalcSettings, vp::VratasForDate, MyHash> cache;

    bool operator==(const CalcSettings & left, const CalcSettings & right)
    {
        return (left.date == right.date) && (left.flags == right.flags);
    }

} // namespace detail

/* Change dir to the directory with eph and tzdata data files (usually it's .exe dir or the one above) */
void change_to_data_dir(const char* argv0)
{
    auto working_dir = detail::determine_working_dir(argv0);
    fs::current_path(working_dir);
}

std::string version()
{
#ifdef VP_VERSION
    return std::string{VP_VERSION};
#else
    return std::string{"unknown"};
#endif
}

std::string program_name_and_version()
{
    return "Vaiṣṇavaṁ Pañcāṅgam " + version();
}

// Try calculating, return true if resulting date range is small enough (suggesting that it's the same ekAdashI for all locations),
// false otherwise (suggesting that we should repeat calculation with adjusted base_date
bool try_calc_all(date::year_month_day base_date, vp::VratasForDate & vratas, CalcFlags flags) {
    std::transform(
        LocationDb().begin(),
        LocationDb().end(),
        std::back_inserter(vratas),
        [base_date, flags](const vp::Location & location) {
            return calc_one(base_date, location, flags);
        });
    return vratas.all_from_same_ekadashi();
}

vp::VratasForDate calc_all(date::year_month_day base_date, CalcFlags flags)
{
    const auto key = CalcSettings{base_date, flags};
    if (auto found = cache.find(key); found != cache.end()) {
        return found->second;
    }
    vp::VratasForDate vratas;

    if (!try_calc_all(base_date, vratas, flags)) {
        vratas.clear();
        date::year_month_day adjusted_base_date = date::sys_days{base_date} - date::days{1};
        try_calc_all(adjusted_base_date, vratas, flags);
    }
    cache[key] = vratas;
    return vratas;
}

} // namespace vp::text_ui
