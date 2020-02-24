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

std::ostream &operator<<(std::ostream &o, const Vrata &v)
{
    o << v.type << "{" << v.date;
    o << ", " << v.paran;
    return o << "}";
}

std::ostream &operator<<(std::ostream &o, const Vrata_Type &v)
{
    switch (v) {
    case Vrata_Type::Ekadashi:
        o << "Ekadashi"; break;
    case Vrata_Type::Sandigdha_Ekadashi:
        o << "Sandigdha_Ekadashi"; break;
    case Vrata_Type::With_Atirikta_Dvadashi:
        o << "With_Atirikta_Dvadashi"; break;
    case Vrata_Type::Sandigdha_With_Atirikta_Dvadashi:
        o << "Sandigdha_With_Atirikta_Dvadashi"; break;
    case Vrata_Type::Atirikta_Ekadashi:
        o << "Atirikta_Ekadashi"; break;
    case Vrata_Type::Sandigdha_Atirikta_Ekadashi:
        o << "Sandigdha_Atirikta_Dvadashi"; break;
//    default:
//        o << "Vrata#" << static_cast<int>(v);
    }
    return o;
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

bool is_atirikta(Vrata_Type type)
{
    return type == Vrata_Type::With_Atirikta_Dvadashi || type == Vrata_Type::Sandigdha_With_Atirikta_Dvadashi
            ||
            type == Vrata_Type::Atirikta_Ekadashi || type == Vrata_Type::Sandigdha_Atirikta_Ekadashi;
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

} // namespace vp
