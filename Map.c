//
// Created by Gerard on 1-4-2016.
//

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "Map.h"
#include "List.h"


/*************************************************************
 * Function to display the neighbors
 * @param city the City to display
 *************************************************************
 */
static void displayNeighbors(void *neighbor) {
    Neighbour *theNeighbor = (Neighbour*) neighbor;
    printf(" %s(%d)",theNeighbor->city, theNeighbor->distance);
}

/*************************************************************
 * Function to display a city of the list
 * @param city the City to display
 *************************************************************
 */
static void displayCity(void *city) {
    City* theCity = (City*)city;
    printf("%s, long:%d, lat:%d\t\tneighbors:",theCity->cityName, theCity->longitude, theCity->latitude);
    forEach(theCity->neighbour, displayNeighbors);
    printf("\n");
}

/*************************************************************
 * Function to display the city name and G value
 * @param city the City to display
 *************************************************************
 */
static void displayCityAndGValue(void *city) {
    City* theCity = (City*)city;
    printf("%s: %d\n",theCity->cityName, theCity->g);
}


/*************************************************************
 * Function to compare two elements (Cities) : based on strcmp and cityname
 * @param s1 the first City to compare
 * @param s2 the second City to compare
 * @return <0 if s1 is less than s2
 * @return 0 if s1 equals s2
 * @return >0 otherwise
 *************************************************************/
static int compCitiesBasedOnName(void *s1, void *s2) {
    City* city1 = (City*)s1;
    City* city2 = (City*)s2;
    return strcmp(city1->cityName, city2->cityName);
}

/*************************************************************
 * Function to compare two elements cities based on the F value
 * @param s1 the first City to compare
 * @param s2 the second City to compare
 * @return <0 if s1 is less than s2
 * @return 0 if s1 equals s2
 * @return >0 otherwise
 *************************************************************/
static int compCitiesBasedOnF (void * s1, void * s2) {
    City* city1 = (City*)s1;
    City* city2 = (City*)s2;
    return (city1->f - city2->f);
}
/*************************************************************
 * Function to compare two elements, always returns equal (0)
 * @return 0
 *************************************************************/
static int noCompare (void * s1, void * s2) {
    return 0;
}

/*************************************************************
 * Find a specific city in a list
 *  - Returns 0 if not found
  *************************************************************/
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
/*************************************************************
 * Find a specific city
 *  - Returns 0 if not found
  *************************************************************/
City* findCityByName(char *name, List *pToCityList)
{
    // Dummy city for comparing
    City dummyCity;
    strncpy(dummyCity.cityName, name, MAX_CITYNAME_LENGTH);
    return findCityInList(pToCityList, &dummyCity);
}

/*************************************************************
 * Get a city based on name
 *  - If not available, create and add to list
 *  - Only name will be initialised when newly created
  *************************************************************/
status getCity(char *cityName, List *pToCityList, City **city) {
    status ret = OK;

    // Create a local dummy for searching
    City dummyCity;
    strncpy(dummyCity.cityName, cityName, MAX_CITYNAME_LENGTH);

    // Search the list for the City node
    Node *pNode = isInList(pToCityList, &dummyCity);
    if(!pNode) {
        // Create the new city, and initialize its name
        City *pNewCity = (City*)malloc(sizeof(City));
        if(!pNewCity) {
            return ERRALLOC;
        }
        // Copy the name
        strncpy(pNewCity->cityName, cityName, MAX_CITYNAME_LENGTH);

        // Add to cityList
        ret = addList(pToCityList, pNewCity );
        if(ret != OK) {
            // Unable to add, free it
            free(pNewCity);
            *city = 0;
            return ret;
        }
        *city = pNewCity;
    }
    else if(pNode == (Node*)1) {
        *city = (City*)pToCityList->head->val; // Element is in head
    }
    else {
        *city = (City*)pNode->next->val; // Found the element
    }
    return OK;
}
/*************************************************************
 * Add a neighbor to the city
  *************************************************************/
status addNeighbour(City *city, City *neighbourCity, int distance) {
    // Create neighbor list if empty
    if(!city->neighbour) {
        city->neighbour = newList(compCitiesBasedOnName,compCitiesBasedOnName, displayCity);
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
/*************************************************************
 *  createMap based on input file
  *************************************************************/
void createMap(char *path, List **cityList) {
    FILE *file;
    char cityName[MAX_CITYNAME_LENGTH];
    int mapParam1;
    int mapParam2;

    // Create a new cities list
    *cityList = newList(compCitiesBasedOnName,compCitiesBasedOnF, displayCity);

    // Open the file
    file = fopen(path,"r");
    if(!file) {
        printf("Error while opening: %s", path);
        return;
    }

    // Read complete file and parse
    City *curCity = 0;
    while(!feof(file)) {
        int paramsCount = fscanf(file, "%s\t\t%d\t%d", cityName, &mapParam1, &mapParam2);
        switch (paramsCount){
            case CityAndLatAndLong:
            {
                //printf("%s %d %d\n",cityName, mapParam1, mapParam2);
                // Create the new city, and initialize it.
                City *city = 0;
                status ret = getCity(cityName, *cityList, &city);
                if(ret == OK) {
                    city->latitude = mapParam1;
                    city->longitude = mapParam2;
                    city->g = INT_MAX;
                    city->h = 0;
                    city->f = 0;
                    city->neighbour = 0;
                    city->backpointer = 0;
                    curCity = city;
                }
                else {
                    // Display error message.
                    printf("%s\n",message(ret));
                    curCity = 0;
                }
                break;
            }
            case CityAndLongitude:
            {
                //printf("\t%s %d\n",cityName, mapParam1);
                // Check if the city is already available, if not add.
                City *city = 0;
                status ret = getCity(cityName, *cityList, &city);
                if(ret == OK) {
                    if(curCity) {
                        addNeighbour(curCity, city, mapParam1);
                    }
                    else {
                        printf("Error, no city selected to add neighbour\n");
                    }
                }
                else {
                    // Display error message.
                    printf("%s\n",message(ret));
                    curCity = 0;
                }
                break;
            }
        }
    }
    // Close the file
    fclose(file);
}
/*************************************************************
 * Clean the memory
  *************************************************************/
void destroyMap(List *cityList){
    //TODO: Cleanup
}

/*************************************************************
 * Function to calculate h of each city
 * @param city the City to display
 *************************************************************
 */
int calculateHValue(City *cityFrom, City *cityTo ) {
    int calcH = abs(cityFrom->latitude - cityTo->latitude) + abs(cityFrom->longitude - cityTo->longitude);
    return calcH;
}

void printStatus(List *openList, List *closedList, char* mssg){
#if 0
    printf("-----> %s\nOPEN:\n", mssg);
    displayList(openList);
    printf("CLOSED:\n");
    displayList(closedList);
#endif
}
void printBackPointerRoute(City* endCity) {
    List *pRoute = newList(noCompare, noCompare, 0);
    if(!pRoute) {
        printf("Error creating back-pointer list\n");
        exit(0-ERRALLOC);
    }

    // Iterate back over route, and save in list
    City *city = endCity;
    while(city){
        addList(pRoute, (void*)city);
        city = city->backpointer;
    }

    // Print the list
    printf("Shortest route:\n");
    forEach(pRoute, displayCityAndGValue);

    // Cleanup
    delList(pRoute);
}

void findRoute(char *startCityName, char *goalCityName, List *cityList) {
    status ret;

    printf("Finding shortest route\nFrom: %s\nTo: %s\n\n", startCityName, goalCityName);

    //TODO: check if cities are in the list
    City *startCity = findCityByName(startCityName, cityList);
    City *goalCity = findCityByName(goalCityName, cityList);

    // Create the algorithm helper lists OPEN and CLOSED
    List* openList = newList(compCitiesBasedOnName, compCitiesBasedOnF, displayCity);
    List* closedList = newList(compCitiesBasedOnName, compCitiesBasedOnF, displayCity);

    if(!openList || !closedList) {
        printf("Error allocating memory for OPEN or CLOSE list\n");
        exit(0-ERRALLOC);
    }

    /////////////////////////////////
    // 1 Place n0 in OPEN. compute ˆh(n0) and set ˆg(n0) = 0. All otherˆg = INF
    if((ret = addList(openList, startCity)) != OK) {
        printf("%s\n", message(ret));
    }
    startCity->h = calculateHValue(startCity, goalCity);
    startCity->g = 0;

    /////////////////////////////////
    // 2 if OPEN is empty, stop (failure)
    unsigned int iterationNr = 0;
    while ((lengthList(openList) != 0) && (iterationNr < MAX_ASTART_ITERATIONS)) {

        printStatus(openList, closedList, "After step 2");

        /////////////////////////////////
        // 3 remove from OPEN the vertex with minimal ˆf , call it n and add it to CLOSED
        City *minimalFCity_N;
        remFromListAt(openList, 0, (void**)&minimalFCity_N);
        addList(closedList, minimalFCity_N);

        printStatus(openList, closedList, "After step 3");

        /////////////////////////////////
        // 4 if n is the goal, stop (success): use pointer chain to retrieve the solution path.
        if(compCitiesBasedOnName(minimalFCity_N, goalCity) == 0 ) {
            printBackPointerRoute(minimalFCity_N);
            break;
        }

        /////////////////////////////////
        // 5 For each successor si of n:
        int neighbourCount = lengthList(minimalFCity_N->neighbour);
        for (int neighbourNr = 0; neighbourNr < neighbourCount; neighbourNr++) {

            // Get the neighbor
            Neighbour *neighbour = 0;
            if((ret = nthInList(minimalFCity_N->neighbour, neighbourNr, (void**)&neighbour)) != OK) {
                printf("%s\n", message(ret));
                exit(0-ret);
            }
            City *neighbourCity = neighbour->city;

            /////////////////////////////////
            // 5.1 compute ˆg(n) + c(n, si )
            int gValue = minimalFCity_N->g + neighbour->distance;

            /////////////////////////////////
            // 5.2 if si is in OPEN or in CLOSED and ˆg(n) + c(n, si ) > ˆg(si ), skip to next successor
            City *pCityInOpen = findCityInList(openList, neighbourCity);
            City *pCityInClosed = findCityInList(closedList, neighbourCity);
            if((pCityInOpen && gValue > pCityInOpen->g ) ||
               (pCityInClosed && gValue > pCityInClosed->g)){
                continue;
            }

            /////////////////////////////////
            // 5.3 remove si from OPEN and CLOSED if present
            // Re-Insert puts the city in the correct (new) position.
            if(pCityInOpen) {
                if((ret = remFromList(openList, neighbourCity)) != OK) {
                    printf("%s\n", message(ret));
                    exit(0-ret);
                }
            }
            if(pCityInClosed) {
                if((ret = remFromList(closedList, neighbourCity)) != OK) {
                    printf("%s\n", message(ret));
                    exit(0-ret);
                }
            }

            /////////////////////////////////
            // 5.4 insert si in OPEN and update ˆg(si ) and back-path pointer
            neighbourCity->g = gValue;
            neighbourCity->h = calculateHValue(neighbourCity, goalCity);
            neighbourCity->f = neighbourCity->g + neighbourCity->f;
            neighbourCity->backpointer = minimalFCity_N;

            if((ret = addList(openList, neighbourCity)) != OK) {
                printf("%s\n", message(ret));
                exit(0-ret);
            }
        }

        printStatus(openList, closedList, "After step 5");

        // 6 go to 2
        iterationNr++;
    }
    if(lengthList(openList) == 0) {
        printf("Error in route algorithm, no nodes in OPEN list.\n");
        exit(-100); //TODO: cleanup memory?
    }
    else if(iterationNr >= MAX_ASTART_ITERATIONS) {
        printf("Error in route algorithm, reached max iterations for finding path \n");
        exit(-101);
    }
}

// 1 Place n0 in OPEN. compute ˆh(n0) and set ˆg(n0) = 0. All otherˆg = 1
// 2 if OPEN is empty, stop (failure)
// 3 remove from OPEN the vertex with minimal ˆf , call it n and add it to CLOSED
// 4 if n is the goal, stop (success): use pointer chain to retrieve the solution path.
// 5 For each successor si of n:
// 5.1 compute ˆg(n) + c(n, si )
// 5.2 if si is in OPEN or in CLOSED and ˆg(n) + c(n, si ) > ˆg(si ), skip to next successor
// 5.3 remove si from OPEN and CLOSED if present
// 5.4 insert si in OPEN and update ˆ g(si ) and backpath pointer
// 6 go to 2