//
// Created by Gerard on 1-4-2016.
//
// Application to find the optimal path between locations on a map
// Using A* Algorithm to calculate the optimal route
//  Heuristic function uses latitude and altitude information
//
// The program takes input parameters to specify locations.
//

#include <stdio.h>
#include "Map.h"

/** Path to the Map file */
static char *const MapFilepath = "./FRANCE.MAP";

/**
 * Compute the optimal point between two cities
 *   Requires input from the user passed when starting
 *      - Start city
 *      - (Optional) stop city, if not given will be asked.
 *
 * @param argc amount of arguments given by user, should be 2 or 3
 * #param args, 2nd and 3th string should contain start and optional end city Name
 * @return 0 OK
 * #return <0 ERROR CODE
 */
int main(int argc, char** args) {

    char *startCityName = 0;
    char *goalCityName = 0;

    // Debug:
    printf("---------------\n");
    for (int i = 0; i < argc; ++i) {
        printf("%d: %s\n",i, args[i]);
    }
    printf("---------------\n");

    // check input parameters, ask for other if necessary
    if(argc == 2) {
        goalCityName = (char*)malloc(MAX_CITYNAME_LENGTH);
        printf("Where do you want to go?\n");
        scanf("%s", goalCityName);
        startCityName = args[Input_StartCity];
    }
    else if( argc == 3 ) {
        startCityName = args[Input_StartCity];
        goalCityName = args[Input_GoalCity];
    }
    else
    {
        printf("Input commands: startCityName [cityName]\n");
        return 0;
    }

    // Create the list of cities from the .MAP file
    List *pCityList = createMap(MapFilepath);

    // print all the cities
    displayList(pCityList);

    // Start finding Route
    printf("Finding shortest route\nFrom: %s\nTo: %s\n\n", startCityName, goalCityName);
    findRoute(startCityName, goalCityName, pCityList);

    // Clean up
    destroyMap(pCityList);
    if(argc == Input_StartCity){
        free(goalCityName);
    }
}