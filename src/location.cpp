#include "location.h"

namespace {
int_fast8_t compare(vp::Latitude l1, vp::Latitude l2) {
    if (l1.latitude < l2.latitude) {
        return -1;
    } else if (l1.latitude > l2.latitude) {
        return +1;
    } else {
        return 0;
    }
}

int_fast8_t compare(vp::Longitude l1, vp::Longitude l2) {
    if (l1.longitude < l2.longitude) {
        return -1;
    } else if (l1.longitude > l2.longitude) {
        return +1;
    } else {
        return 0;
    }
}

int_fast8_t compare(vp::Coord coord1, vp::Coord coord2) {
    if (coord1.latitude < coord2.latitude) {
        return -1;
    } else if (coord1.latitude > coord2.latitude) {
        return +1;
    } else {
        return compare(coord1.longitude, coord2.longitude);
    }
}
}

// Latitude
bool vp::operator ==(vp::Latitude l1, vp::Latitude l2) {
    return compare(l1, l2) == 0;
}

bool vp::operator !=(vp::Latitude l1, vp::Latitude l2) {
    return compare(l1, l2) != 0;
}

bool vp::operator <(vp::Latitude l1, vp::Latitude l2) {
    return compare(l1, l2) < 0;
}

bool vp::operator <=(vp::Latitude l1, vp::Latitude l2) {
    return compare(l1, l2) <= 0;
}

bool vp::operator >(vp::Latitude l1, vp::Latitude l2) {
    return compare(l1, l2) > 0;
}

bool vp::operator >=(vp::Latitude l1, vp::Latitude l2) {
    return compare(l1, l2) >= 0;
}


// Longitude
bool vp::operator ==(vp::Longitude l1, vp::Longitude l2) {
    return compare(l1, l2) == 0;
}

bool vp::operator !=(vp::Longitude l1, vp::Longitude l2) {
    return compare(l1, l2) != 0;
}

bool vp::operator <(vp::Longitude l1, vp::Longitude l2) {
    return compare(l1, l2) < 0;
}

bool vp::operator <=(vp::Longitude l1, vp::Longitude l2) {
    return compare(l1, l2) <= 0;
}

bool vp::operator >(vp::Longitude l1, vp::Longitude l2) {
    return compare(l1, l2) > 0;
}

bool vp::operator >=(vp::Longitude l1, vp::Longitude l2) {
    return compare(l1, l2) >= 0;
}

// Coord
bool vp::operator ==(vp::Coord coord1, vp::Coord coord2) {
    return compare(coord1, coord2) == 0;
}

bool vp::operator !=(vp::Coord coord1, vp::Coord coord2) {
    return compare(coord1, coord2) != 0;
}

bool vp::operator <(vp::Coord coord1, vp::Coord coord2) {
    return compare(coord1, coord2) < 0;
}

bool vp::operator <=(vp::Coord coord1, vp::Coord coord2) {
    return compare(coord1, coord2) <= 0;
}

bool vp::operator >(vp::Coord coord1, vp::Coord coord2) {
    return compare(coord1, coord2) > 0;
}

bool vp::operator >=(vp::Coord coord1, vp::Coord coord2) {
    return compare(coord1, coord2) >= 0;
}
