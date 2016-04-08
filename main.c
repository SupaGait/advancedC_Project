/**
 * @file main.c
 * @author Gerard Klomphaar
 * @date 1-4-2016
 *
 * @brief Application to find the optimal path between locations on a map.
 *
 * Using A* Algorithm to calculate the optimal route
 * The program takes input parameters to specify names of the start and goal city
 */

#include <stdio.h>
#include "Map.h"

/** Path to the Map file */
static char *const DefaultMapFilepath = "./FRANCE.MAP";

/** Program input parameter count */
enum ArgsParamsCount {
    ArgsParamCount_NoInput = 1,
    ArgsParamCount_OnlyStartName = 2,
    ArgsParamCount_StartAndGoalName = 3,
    ArgsParamCount_ALL = 4,
};

/** Possible input parameters given to the program*/
enum ArgsInputParams {
    /*Input_ProgramName = 0,*/
    ArgsInputParam_StartCity = 1,
    ArgsInputParam_GoalCity = 2,
    ArgsInputParam_MapPath = 3
};

/**
 * Compute the optimal point between two cities
 *   Requires input from the user passed when starting
 *      - Start city, if not given will be asked.
 *      - Stop city, if not given will be asked.
 *      - Optional Path to .MAP file (Default="./FRANCE.MAP" )
 *
 * @param argc amount of arguments given by user, should be 2 or 3
 * #param args, 2nd and 3th string should contain start and optional end city Name
 * @return 0 OK
 * #return <0 ERROR CODE
 */
int main(int argc, char** args) {

    char *startCityName = 0;
    char *goalCityName = 0;
    char *mapFilePath = DefaultMapFilepath;

    // Check program input parameters
    switch(argc){
        case ArgsParamCount_NoInput: {
            startCityName = (char*)malloc(MAX_CITYNAME_LENGTH);
            goalCityName = (char*)malloc(MAX_CITYNAME_LENGTH);

            // No parameters given
            printf("What is the starting city?\n");
            scanf("%s", startCityName);
            printf("What is the goal city?\n");
            scanf("%s", goalCityName);
            break;
        }
        case ArgsParamCount_OnlyStartName: {
            // Goal city not specified as input param, ask for it.
            goalCityName = (char*)malloc(MAX_CITYNAME_LENGTH);
            printf("Which city do you want to go?\n");
            scanf("%s", goalCityName);
            startCityName = args[ArgsInputParam_StartCity];
            break;
        }
        case ArgsParamCount_StartAndGoalName: {
            startCityName = args[ArgsInputParam_StartCity];
            goalCityName = args[ArgsInputParam_GoalCity];
            break;
        }
        case ArgsParamCount_ALL: {
            startCityName = args[ArgsInputParam_StartCity];
            goalCityName = args[ArgsInputParam_GoalCity];
            mapFilePath = args[ArgsInputParam_MapPath];
            break;
        }
        default: {
            printf("Incorrect input.\nInput commands: startCityName [goalCityName] [filepathMap, Default=\'./FRANCE.MAP\']\n");
            return 0;
        }
    }

    // Create the list of cities from the .MAP file
    List *pCityList = 0;
    status ret = createMap(mapFilePath, &pCityList);
    if(ret != OK) {
        printf("While populating map from %s\nError: %s\n", mapFilePath, message(ret));
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
    if(argc == ArgsParamCount_NoInput) {
        free(startCityName);
        free(goalCityName);
    }
    else if(argc == ArgsParamCount_OnlyStartName){
        free(goalCityName);
    }

    return 0;
}

// Main page for DoxyGen documentation
/** \mainpage RouteFinding
 *
 * \section Description
 *
 * This project is created for the Advance C class at EPITA
 *
 * \section Building executable
 * Simply use make to build the project
 *   FindRoute application is generated
 *
 * \section Usage
 *    FindRoute without parameters, default FRANCE.MAP will be used as input map.\n
 *    Start city and destination city will be ask\n
 *    \n
 *    FindRoute With parameters; FindRoute startCityName [goalCityName] [filepathMap, Default='./FRANCE.MAP']\n
 *    e.g.:\n
 *        \li FindRoute "Lyon"\n
 *        \li FindRoute "Lyon" "Rennes"\n
 *        \li FindRoute "Lyon" "Rennes" "./FRANCE.MAP"\n
 *
 * \section Code
 *      The code is divided over 4 sources:\n
 *      \li main.c reads the user input and uses \ref Map.h to fill a List containing cities.\n
 *      \li The Map.h createMap() returns a List containing City's and Neighbour's with a distance.\n
 *      \li Possible errors are handled using a generic error value specified in status.h
 *      \li List.h contains a genric List implementaion, used for City's and Neighbour's
 */