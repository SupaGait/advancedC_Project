//
// Created by Gerard on 1-4-2016.
//

#ifndef ADVANCED_C_CLASS_CITY_H
#define ADVANCED_C_CLASS_CITY_H

#include "List.h"

#define MAX_CITYNAME_LENGTH 64

typedef struct _City City;
typedef struct _Neighbour Neighbour;

typedef struct _City{
    char cityName[MAX_CITYNAME_LENGTH];
    int longitude;
    int latitude;
    int g;
    int h;
    int f;
    List *neighbour;
    City* backpointer;
}City;

typedef struct _Neighbour{
    City* city;
    int distance;
}Neighbour;

enum MapParams{
    CityAndLongitude = 2,
    CityAndLatAndLong = 3
};

enum InputParams{
    InputOnlyStart= 2,
    InputStartAndGoal = 3
};

void createMap(char *path, List **cityList);
void destroyMap(List *cityList);
void findRoute(char *startCityName, char *goalCityName, List *cityList);

#endif //ADVANCED_C_CLASS_CITY_H
