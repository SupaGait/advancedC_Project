/**
 * @file Map.c
 * @author Gerard Klomphaar
 * @date 1-4-2016
 *
 * @brief Functions for finding the optimal route between two cities using the A* algorithm.
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "Map.h"

/**
 * Function to display the neighbours name and distance
 * @param neighbour The neighbour to display
 */
static void displayNeighbours(void *neighbour) {
    Neighbour *theNeighbour = (Neighbour*)neighbour;
    printf(" %s(%d)", theNeighbour->city->cityName, theNeighbour->distance);
}

/**
 * Function to display a city
 * @param city The city to display
 */
static void displayCity(void *city) {
    City* theCity = (City*)city;
    printf("%s, long:%d, lat:%d\t\tneighbours:", theCity->cityName, theCity->longitude, theCity->latitude);
    forEach(theCity->neighbour, displayNeighbours);
    printf("\n");
}

/**
 * Function to print a city with distance(G) value (distance from start city)
 * @param city The city to display
 */
static void displayCityAndDistance(void *city) {
    City* theCity = (City*)city;
    printf("%s (%d)\n", theCity->cityName, theCity->g);
}

/**
 * Function to compare two Cities on Name: based on strcmp
 * @param s1 the first City to compare
 * @param s2 the second City to compare
 * @return <0 if s1 is less than s2
 * @return 0 if s1 equals s2
 * @return >0 otherwise
 */
static int compCitiesBasedOnName(void *s1, void *s2) {
    City* city1 = (City*)s1;
    City* city2 = (City*)s2;
    return strcmp(city1->cityName, city2->cityName);
}

/**
 * Function to compare two Cities based on the F value
 * @param s1 the first City to compare
 * @param s2 the second City to compare
 * @return <0 if s1 is less than s2
 * @return 0 if s1 equals s2
 * @return >0 otherwise
 */
static int compCitiesBasedOnF(void *s1, void *s2) {
    City* city1 = (City*)s1;
    City* city2 = (City*)s2;
    return (city1->f - city2->f);
}
/**
 * Function to compare two elements, always returns equal (0)
 * @param s1 Not used
 * @param s1 Not used
 * @return 0
 */
static int noCompare (void *s1, void *s2) {
    return 0;
}

/**
 * Find a specific city in a list
 * @param list The list to search in
 * @param city The city to search for, using the list compare function
 * @return 0 if city was not found
 * @return The city when the city is in the list
  */
City* findCityInList(List *list, City *city){
    Node *pNode = isInList(list, city);
    if(!pNode) {
        return 0;
    }
    else if(pNode == (Node*)1) {
        return (City*)list->head->val;  // City is in head node value
    }
    else {
        return (City*)pNode->next->val;  // City is in next node value
    }
}
/**
 * Find a city by name
 * @param name The name of the city to search for
 * @param list The list to search in
 * @return 0 if city was not found
 * @return The city when the city is in the list
  */
City* findCityByName(char *name, List *pToCityList)
{
    // Dummy city for comparing
    City dummyCity;
    dummyCity.cityName = name;
    return findCityInList(pToCityList, &dummyCity);
}

 /**
 * Get a city based on name, if it does not exist, it creates the city.
 * @param cityName The name of the city to search for
 * @param pToCityList The list to search in
 * @param city Pointer to city pointer which will be assigned to allocated city.
 * @return error code if unable to get or create city
 * @return OK if city was found or created and set in city
 */
status getOrCreateCity(char *cityName, List *pToCityList, City **city) {
    // Search the list for the City node
    City *foundCity = findCityByName(cityName, pToCityList);
    if(!foundCity) {
        // Create the new city
        City *pNewCity = (City*)malloc(sizeof(City));
        if(!pNewCity) {
            return ERRALLOC;
        }
        // Create memory for name
        pNewCity->cityName = (char*)malloc(MAX_CITYNAME_LENGTH);
        if(!pNewCity->cityName) {
            free(pNewCity);
            return ERRALLOC;
        }

        // Copy the name
        strncpy(pNewCity->cityName, cityName, MAX_CITYNAME_LENGTH);

        // Add to cityList
        status ret;
        if((ret = addList(pToCityList, pNewCity )) != OK) {
            // Unable to add, free it or it will be lost.
            free(pNewCity);
            *city = 0;
            return ret;
        }
        *city = pNewCity;
    }
    else {
        *city = foundCity;
    }
    return OK;
}
/**
 * Add a neighbor to the given city
 * @param city The city to add the neighbour to
 * @param neighbourCity The city to add as a neighbour to city
 * @param distance The distance to the given neighbour
 * @return error code if unable to add neighbour
 * @return OK if neighbour was correctly added
  */
status addNeighbour(City *city, City *neighbourCity, int distance) {
    // Create neighbor list if empty
    if(!city->neighbour) {
        city->neighbour = newList(compCitiesBasedOnName, compCitiesBasedOnName, displayCity);
        if(!city->neighbour) {
            return ERRALLOC;
        }
    }
    // Add the neighbor
    Neighbour* newNeighbor = (Neighbour*)malloc(sizeof(Neighbour));
    if(!newNeighbor) {
        return ERRALLOC;
    }
    // Set the neighbour values
    newNeighbor->city = neighbourCity;
    newNeighbor->distance = distance;
    status ret = addList(city->neighbour, newNeighbor);
    if(ret != OK) {
        free(newNeighbor);
    }
    return ret;
}
status createMap(char *path, List **cityMapList) {
    FILE *file;
    char cityName[MAX_CITYNAME_LENGTH];
    int mapParam1;
    int mapParam2;

    // Create a new cities list
    *cityMapList = newList(compCitiesBasedOnName, compCitiesBasedOnF, displayCity);

    // Open the file
    file = fopen(path,"r");
    if(!file) {
        printf("Error while opening: %s\n", path);
        return ERROPEN;
    }

    // Read complete file and parse
    City *curCity = 0;
    while(!feof(file)) {
        int paramsCount = fscanf(file, "%s\t\t%d\t%d", cityName, &mapParam1, &mapParam2);
        switch (paramsCount){
            case CityAndLatAndLong:
            {
#ifdef ENABLE_DEBUG_INFO
                printf("%s %d %d\n",cityName, mapParam1, mapParam2);
#endif
                // Create the new city
                City *city = 0;
                status ret;
                if((ret = getOrCreateCity(cityName, *cityMapList, &city)) != OK) {
                    return ret;
                }
                // Initialise the city
                city->latitude = mapParam1;
                city->longitude = mapParam2;
                city->g = INT_MAX;
                city->f = 0;
                city->neighbour = 0;
                city->backPointer = 0;
                curCity = city;
                break;
            }
            case CityAndLongitude:
            {
#ifdef ENABLE_DEBUG_INFO
                printf("\t%s %d\n",cityName, mapParam1);
#endif
                // Check if the city is already available, if not add.
                City *city = 0;
                status ret;
                if((ret = getOrCreateCity(cityName, *cityMapList, &city)) != OK) {
                    return ret;
                }
                if((ret = addNeighbour(curCity, city, mapParam1)) != OK) {
                    return ret;
                }
                break;
            }
        }
    }
    // Close the file
    fclose(file);

#ifdef ENABLE_DEBUG_INFO
    printf("Found cities: %d\n", lengthList(*cityMapList));
    displayList(*cityMapList);
#endif
    return OK;
}
void destroyMap(List *cityList){
    if(!cityList){
        return;
    }

    // Free all allocated cities
    while(cityList->head) {
        Node *pNodeTmp = cityList->head->next;
        free( ((City*)cityList->head->val)->cityName);     // Free allocated memory for the name
        free(cityList->head->val);                          // Free the City
        cityList->head = pNodeTmp;
    }

    // Free the list
    delList(cityList);
}

/**
 * Function to calculate h of each city
 * @param city the City to display
 *************************************************************
 */
int calculateHValue(City *cityFrom, City *cityTo ) {
    return (abs(cityFrom->latitude - cityTo->latitude) + abs(cityFrom->longitude - cityTo->longitude))/4;
}
#ifdef ENABLE_DEBUG_INFO
void printStatus(List *openList, List *closedList, char* mssg){
    printf("\n---> %s <---\nOPEN:\n", mssg);
    displayList(openList);
    printf("CLOSED:\n");
    displayList(closedList);
}
#endif

/**
 * Print the route from origin city to the given goal city based on back-pointer
 * @param city The goal city
 * @return error code if unable to print route
 * @return OK if route printed successfully
  */
status printBackPointerRoute(City* endCity) {
    List *pRoute = newList(noCompare, noCompare, 0);
    if(!pRoute) {
        printf("Error creating back-pointer list\n");
        return(ERRALLOC);
    }

    // Iterate back over route, and save in list
    City *city = endCity;
    status ret;
    while(city){
        if((ret = addList(pRoute, (void*)city)) != OK) {
            delList(pRoute);
            return ret;
        }
        city = city->backPointer;
    }

    // Print the list
    printf("Shortest route:\n");
    forEach(pRoute, displayCityAndDistance);

    // Cleanup
    delList(pRoute);
    return OK;
}

status findRoute(char *startCityName, char *goalCityName, List *cityList) {
    // Validate a valid city map
    if(!cityList) {
        printf("The given city map is incorrect.\n");
        return ERREMPTY;
    }
    // Validate that the given names are cities in the given city map file
    City *startCity = findCityByName(startCityName, cityList);
    City *goalCity = findCityByName(goalCityName, cityList);
    if(!startCity) {
        printf("The given start city: %s does not exist on the map.\n",startCityName);
        return ERRABSENT;
    }
    if(!goalCity) {
        printf("The given goal city: %s does not exist on the map.\n",goalCityName);
        return ERRABSENT;
    }

    // Create the algorithm lists OPEN and CLOSED
    List* openList = newList(compCitiesBasedOnName, compCitiesBasedOnF, displayCity);
    List* closedList = newList(compCitiesBasedOnName, compCitiesBasedOnF, displayCity);
    if(!openList || !closedList) {
        printf("Error allocating memory for OPEN or CLOSE list\n");
        return(ERRALLOC);
    }

    /////////////////////////////////
    // 1 Place n0 in OPEN. compute ˆh(n0) and set ˆg(n0) = 0. All otherˆg = INF
    status retStatus;
    if((retStatus = addList(openList, startCity)) != OK) {
        return retStatus;
    }
    startCity->g = 0;

    unsigned int iterationNr = 0;
    while (iterationNr < MAX_A_STAR_ITERATIONS) {
        // --2-- if OPEN is empty, stop (failure)
        if(lengthList(openList) == 0) {
            printf("Error in route algorithm, no nodes in OPEN list.\n");
            retStatus = ERRALGORTIHM;
            break;
        }

        // --3-- remove from OPEN the vertex with minimal ˆf , call it n and add it to CLOSED
        City *minimalFCity_N;
        remFromListAt(openList, 0, (void**)&minimalFCity_N);
        if((retStatus = addList(closedList, minimalFCity_N)) != OK) {
            break; // Return error after cleanup
        }

        // --4-- if n is the goal, stop (success): use pointer chain to retrieve the solution path.
        if(compCitiesBasedOnName(minimalFCity_N, goalCity) == 0 ) {
            printBackPointerRoute(minimalFCity_N);
            break; // Success
        }

        // --5-- For each successor si of n:
        int neighbourCount = lengthList(minimalFCity_N->neighbour);
        for (int neighbourNr = 0; neighbourNr < neighbourCount; neighbourNr++) {

            // Get the neighbor
            Neighbour *neighbour = 0;
            if((retStatus = nthInList(minimalFCity_N->neighbour, neighbourNr, (void**)&neighbour)) != OK) {
                break; // Return error after cleanup
            }
            City *neighbourCity = neighbour->city;

            // --5.1-- compute ˆg(n) + c(n, si )
            int gValue = minimalFCity_N->g + neighbour->distance;

            // --5.2-- if si is in OPEN or in CLOSED and ˆg(n) + c(n, si ) > ˆg(si ), skip to next successor
            City *pCityInOpen = findCityInList(openList, neighbourCity);
            City *pCityInClosed = findCityInList(closedList, neighbourCity);
            if((pCityInOpen && gValue > pCityInOpen->g ) ||
               (pCityInClosed && gValue > pCityInClosed->g)){
                continue;
            }

            // --5.3-- remove si from OPEN and CLOSED if present
            // Re-Insert puts the city in the correct (new) position.
            if(pCityInOpen) {
                if((retStatus = remFromList(openList, neighbourCity)) != OK) {
                    break; // Return error after cleanup
                }
            }
            if(pCityInClosed) {
                if((retStatus = remFromList(closedList, neighbourCity)) != OK) {
                    break; // Return error after cleanup
                }
            }

            // --5.4-- insert si in OPEN and update ˆg(si ) and back-path pointer
            neighbourCity->g = gValue;
            neighbourCity->f = neighbourCity->g + calculateHValue(neighbourCity, goalCity);
            neighbourCity->backPointer = minimalFCity_N;

            if((retStatus = addList(openList, neighbourCity)) != OK) {
                break; // Return error after cleanup
            }
        }
#ifdef ENABLE_DEBUG_INFO
        printStatus(openList, closedList, "After iteration");
#endif
        // --6-- go to 2
        iterationNr++;
    }

#ifdef ENABLE_DEBUG_INFO
    printf("\nDEBUG status:\n");
    printf("A* iterations: %d\n", iterationNr);
#endif

    // Cleanup the used lists
    delList(openList);
    delList(closedList);

    // Return the correct status
    if(retStatus != OK){
        return retStatus;
    }
    else if(iterationNr >= MAX_A_STAR_ITERATIONS) {
        printf("Error in route algorithm, reached max iterations for finding path \n");
        return ERRALGORTIHM;
    }
    return OK;
}