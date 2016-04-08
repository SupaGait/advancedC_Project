/**
 * @file Map.h
 * @author Gerard Klomphaar
 * @date 1-4-2016
 *
 * @brief Finding the optimal route between two cities using the A* algorithm.
 * Using an input file to create a City map with location and route details.
 * A* algorithm uses an heuristic function based on latitude and altitude position of cities
 *
 * Program created for Advanced C Course at EPITA
 */
#ifndef ADVANCED_C_CLASS_CITY_H
#define ADVANCED_C_CLASS_CITY_H

#include "List.h"

//#define ENABLE_DEBUG_INFO
#define MAX_CITYNAME_LENGTH     (64)
#define MAX_A_STAR_ITERATIONS   (10000)

/**
 * City structure containing location for heuristic calculation
 * and the current G and H values during used during A Start algorithm
 * A list of neighbour cities for path finding, and a packpointer to trace back the path
 * */
typedef struct City {
    char* cityName;
    int longitude;
    int latitude;
    int g;
    int f;
    List *neighbour;
    struct City *backPointer;
}City;

/**
 * Neighbour structure pointing to the neighbour city and containing the distance
 */
typedef struct Neighbour {
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

/**
 * Populate a List with Cities with their position and neighbours.
 * Input for the list is an file which contains all the information
 *
 * @param path Location of the input file
 * @param cityMapList Pointer to list pointer which will be assigned to populated list
 * @return OK if no error
 * @return Error code when there was an error
 */
status createMap(char *path, List **cityMapList);

/**
 * Find a Route between given cities based on the given city map
 * The algorithm uses an A* implementation to calculate the best route
 * An heuristic function based on latituda and altitude is used.
 * Max iterations of A* algorithm can be set with: MAX_A_STAR_ITERATIONS
 *
 * @param startCityName Name of the city to start from.
 * @param goalCityName Name of the city which is the goal.
 * @param startCityName Map containing all cities and necessary location information.
 * @return OK if no error
 * @return Error code when there was an error
 */
status findRoute(char *startCityName, char *goalCityName, List *cityList);

/**
 * Clean up of the created Map containing the City list
 * !! Should always be called to prevent memory leaks !!
 *   Even when there was an error during creating of the Map, which could be partially filled.
 *
 * @param cityList Name of the city to start from.
 */
void destroyMap(List *cityList);


#endif //ADVANCED_C_CLASS_CITY_H
