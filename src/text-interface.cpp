#include "text-interface.h"

#include "calc.h"
#include "vrata_detail.h"

#include <cstring>
#include <iomanip>
#include <iostream>

using namespace vp;

namespace vp::text_ui {

date::year_month_day parse_ymd(const char *s) {
    std::tm tm{};
    std::istringstream stream{s};
    stream >> std::get_time(&tm, "%Y-%m-%d");
    return date::year_month_day{date::year{tm.tm_year+1900}, date::month{static_cast<unsigned int>(tm.tm_mon+1)}, date::day{static_cast<unsigned int>(tm.tm_mday)}};
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
            return strcmp(named_coord.name, location_name) == 0;
        }
    );
    if (found == std::end(locations())) return std::nullopt;
    return *found;
}

tl::expected<vp::Vrata, vp::CalcError> calc_one(date::year_month_day base_date, Location location, std::ostream &o) {
    auto vrata = Calc{location}.find_next_vrata(base_date);
    if (!vrata.has_value()) {
        o << "# " << location.name << "*\n" <<
            "Can't find next Ekadashi, sorry.\n" <<
            "* Error: " << vrata.error() << "\n";
    } else {
        Vrata_Detail vd{*vrata, location};
        o << vd << "\n\n";
    }
    return vrata;
}

void calc_one(date::year_month_day base_date, const char * location_name, std::ostream &o) {
    std::optional<Location> coord = LocationDb::find_coord(location_name);
    if (!coord) {
        o << "Location not found: '" << location_name << "'\n";
        return;
    }
    calc_one(base_date, *coord, o);
}

void print_detail_one(date::year_month_day base_date, const char *location_name, Location coord, std::ostream &o) {
    o << location_name << ' ' << base_date << '\n';
    o << "<to be implemented>\n";
    Calc calc{coord};
    auto sunrise = calc.swe.find_sunrise(JulDays_UT{base_date});
    if (sunrise) {
        auto arunodaya = calc.arunodaya_for_sunrise(*sunrise);
        if (arunodaya) {
            o << "arunodaya: " << JulDays_Zoned{coord.timezone_name, *arunodaya}
            << ' ' << calc.swe.get_tithi(*arunodaya) << '\n';
        }
        o << "sunrise: " << JulDays_Zoned{coord.timezone_name, *sunrise} << ' ' << calc.swe.get_tithi(*sunrise) << '\n';

        auto sunset = calc.swe.find_sunset(*sunrise);
        if (sunset) {
            if (sunset) {
                auto onefifth = calc.proportional_time(*sunrise, *sunset, 0.2);
                o << "1/5 of daytime: " << JulDays_Zoned{coord.timezone_name, onefifth} << '\n';
            }
            o << "sunset: " << JulDays_Zoned{coord.timezone_name, *sunset} << ' ' << calc.swe.get_tithi(*sunrise) << '\n';
        }
    }
}

void print_detail_one(date::year_month_day base_date, const char * location_name, std::ostream &o) {
    std::optional<Location> coord = LocationDb::find_coord(location_name);
    if (!coord) {
        o << "Location not found: '" << location_name << "'\n";
        return;
    }
    print_detail_one(base_date, location_name, *coord, o);
}

void calc_all(date::year_month_day d, std::ostream &o) {
    for (auto &l : LocationDb()) {
        calc_one(d, l, o);
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

} // namespace vp::text_ui
