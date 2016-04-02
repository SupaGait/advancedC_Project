//
// Created by Gerard on 1-4-2016.
//

#include <stdio.h>
#include "Map.h"

/**
 * Compute the optimal point between two cities
 * Different input possibilities:
 *  -
 *  -
 *
 */
int main(int argc, char** args) {

    // Create a new cities list
    List *pCityList = 0;
    char startCity[] = "Rennes";    // Test TODO: Remove
    char goalCity[] = "Lyon";       // Test TODO: Remove

    // Debug:
    printf("---------------\n");
    for (int i = 0; i < argc; ++i) {
        printf("%d: %s\n",i, args[i]);
    }
    printf("---------------\n");

    // check input parameters, ask for other if necessary
#if 0
    if(argc == InputOnlyStart) {
        // Our sort
        printf("Where do you want to go?\n");
    }
    else if(argc == InputStartAndGoal) {
        printf("Currently hardcoded\n"); //TODO: implement
    }
    else
    {
        printf("Input commands: start [goal]");
        return 0;
    }
#endif

    // Hardcode for now.
    createMap("C:\\Users\\Gerard\\Google Drive\\Epita_\\Courses\\Advanced C\\Project\\FRANCE.MAP", &pCityList);

    // print all the cities
    displayList(pCityList);

    findRoute(startCity, goalCity, pCityList);

    // Clean up
    destroyMap(pCityList);
}