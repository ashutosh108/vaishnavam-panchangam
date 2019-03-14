#ifndef COORD_H
#define COORD_H


struct Coord
{
    double latitude, longitude;
    explicit constexpr Coord(double _latitude, double _longitude)
        : latitude(_latitude), longitude(_longitude) {}
    explicit constexpr Coord(int lat_deg, int lat_min, double lat_sec, int lng_deg, int lng_min, double lng_sec)
        : latitude(lat_deg + lat_min/60.0 + lat_sec/3600.0),
          longitude(lng_deg + lng_min/60.0 + lng_sec/3600.0) {}
};

#endif // COORD_H
