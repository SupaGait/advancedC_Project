//
// Created by Gerard on 1-4-2016.
//

#ifndef ADVANCED_C_CLASS_CITY_H
#define ADVANCED_C_CLASS_CITY_H

#include "List.h"

#define DEBUG_INFO              (1)
#define MAX_CITYNAME_LENGTH     (64)
static const int MAX_A_STAR_ITERATIONS = 10000;
/**
 * Forward declaration of structures
 */
typedef struct _City City;
typedef struct _Neighbour Neighbour;

/**
 * City structure containing location for heuristic calculation
 * and the current G and H values during used during A Start algorithm
 * A list of neighbour cities for path finding, and a packpointer to trace back the path
 * */
typedef struct _City{
    char cityName[MAX_CITYNAME_LENGTH];
    int longitude;
    int latitude;
    int g;
    int f;
    List *neighbour;
    City* backpointer;
}City;

/**
 * Neighbour structure pointing to the neighbour city and containing the distance
 */
typedef struct _Neighbour{
    City* city;
    int distance;
}Neighbour;

/**
 * Interpretation of read number of params in a .MAP file
 */
enum MapReadParams{
    CityAndLongitude = 2,
    CityAndLatAndLong = 3
};

status createMap(char *path, List **cityMapList);
status findRoute(char *startCityName, char *goalCityName, List *cityList);
void destroyMap(List *cityList);


#endif //ADVANCED_C_CLASS_CITY_H
