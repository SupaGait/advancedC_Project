//
// Created by Gerard on 1-4-2016.
//
// Application to find the optimal path between locations on a map
// Using A* Algorithm to calculate the optimal route
//
// The program takes input parameters to specify names of the start and goal city
//

#include <stdio.h>
#include "Map.h"

/** Path to the Map file */
static char *const MapFilepath = "./FRANCE.MAP";

/** Program input parameter count */
static const int OnlyStartName_ParamCount = 2;
static const int StartAndGoalName_ParamCount = 3;

/** Possible input parameters given to the program*/
enum InputParams{
    /*Input_ProgramName = 0,*/
    InputParam_StartCity = 1,
    InputParam_GoalCity = 2
};

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

    // Check program input parameters
    if(argc == OnlyStartName_ParamCount) {
        // Goal city not specified as input param, ask for it.
        goalCityName = (char*)malloc(MAX_CITYNAME_LENGTH);
        printf("Which city do you want to go?\n");
        scanf("%s", goalCityName);
        startCityName = args[InputParam_StartCity];
    }
    else if(argc == StartAndGoalName_ParamCount) {
        startCityName = args[InputParam_StartCity];
        goalCityName = args[InputParam_GoalCity];
    }
    else {
        printf("Incorrect input.\nInput commands: startCityName [goalCityName]\n");
        return 0;
    }

    // Create the list of cities from the .MAP file
    List *pCityList = 0;
    status ret = createMap(MapFilepath, &pCityList);
    if(ret != OK) {
        printf("While populating map from %s\nError: %s\n", MapFilepath, message(ret));
        return(0-ret);
    }

    // Start finding Route
    printf("\nFinding shortest route\nFrom:\t%s\nTo:\t%s\n\n", startCityName, goalCityName);
    ret = findRoute(startCityName, goalCityName, pCityList);
    if(ret != OK) {
        printf("Error: %s.\n", message(ret));
        return(0-ret);
    }

    // Clean up
    destroyMap(pCityList);
    if(argc == OnlyStartName_ParamCount){
        free(goalCityName);
    }

    return 0;
}