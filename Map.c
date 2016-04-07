//
// Created by Gerard on 1-4-2016.
//

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "Map.h"


/*************************************************************
 * Function to display the neighbors
 * @param city the City to display
 *************************************************************
 */
static void displayNeighbors(void *neighbor) {
    Neighbour *theNeighbor = (Neighbour*) neighbor;
    printf(" %s(%d)",theNeighbor->city->cityName, theNeighbor->distance);
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
        status ret;
        if((ret = addList(pToCityList, pNewCity )) != OK) {
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
status createMap(char *path, List **cityMapList) {
    FILE *file;
    char cityName[MAX_CITYNAME_LENGTH];
    int mapParam1;
    int mapParam2;

    // Create a new cities list
    *cityMapList = newList(compCitiesBasedOnName,compCitiesBasedOnF, displayCity);

    // Open the file
    file = fopen(path,"r");
    if(!file) {
        printf("Error while opening: %s\n", path);
        return ERREXIST;
    }

    // Read complete file and parse
    City *curCity = 0;
    while(!feof(file)) {
        int paramsCount = fscanf(file, "%s\t\t%d\t%d", cityName, &mapParam1, &mapParam2);
        switch (paramsCount){
            case CityAndLatAndLong:
            {
                //printf("%s %d %d\n",cityName, mapParam1, mapParam2);
                // Create the new city
                City *city = 0;
                status ret;
                if((ret = getCity(cityName, *cityMapList, &city)) != OK) {
                    return ret;
                }
                // Initialise the city
                city->latitude = mapParam1;
                city->longitude = mapParam2;
                city->g = INT_MAX;
                city->f = 0;
                city->neighbour = 0;
                city->backpointer = 0;
                curCity = city;
                break;
            }
            case CityAndLongitude:
            {
                //printf("\t%s %d\n",cityName, mapParam1);
                // Check if the city is already available, if not add.
                City *city = 0;
                status ret;
                if((ret = getCity(cityName, *cityMapList, &city)) != OK) {
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
    return OK;
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
    return (abs(cityFrom->latitude - cityTo->latitude) + abs(cityFrom->longitude - cityTo->longitude))/4;
}
#ifdef DEBUG_INFO
void printStatus(List *openList, List *closedList, char* mssg){
    printf("-----> %s\nOPEN:\n", mssg);
    displayList(openList);
    printf("CLOSED:\n");
    displayList(closedList);
}
#endif
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
    status ret;
    if((ret = addList(openList, startCity)) != OK) {
        return ret;
    }
    startCity->g = 0;

    // --2-- if OPEN is empty, stop (failure)
    unsigned int iterationNr = 0;
    while ((lengthList(openList) != 0) && (iterationNr < MAX_A_STAR_ITERATIONS)) {

        // --3-- remove from OPEN the vertex with minimal ˆf , call it n and add it to CLOSED
        City *minimalFCity_N;
        remFromListAt(openList, 0, (void**)&minimalFCity_N);
        addList(closedList, minimalFCity_N);

        // --4-- if n is the goal, stop (success): use pointer chain to retrieve the solution path.
        if(compCitiesBasedOnName(minimalFCity_N, goalCity) == 0 ) {
            printBackPointerRoute(minimalFCity_N);
            break;
        }

        // --5-- For each successor si of n:
        int neighbourCount = lengthList(minimalFCity_N->neighbour);
        for (int neighbourNr = 0; neighbourNr < neighbourCount; neighbourNr++) {

            // Get the neighbor
            Neighbour *neighbour = 0;
            if((ret = nthInList(minimalFCity_N->neighbour, neighbourNr, (void**)&neighbour)) != OK) {
                return ret;
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
                if((ret = remFromList(openList, neighbourCity)) != OK) {
                    return ret;
                }
            }
            if(pCityInClosed) {
                if((ret = remFromList(closedList, neighbourCity)) != OK) {
                    return ret;
                }
            }

            // --5.4-- insert si in OPEN and update ˆg(si ) and back-path pointer
            neighbourCity->g = gValue;
            neighbourCity->f = neighbourCity->g + calculateHValue(neighbourCity, goalCity);
            neighbourCity->backpointer = minimalFCity_N;

            if((ret = addList(openList, neighbourCity)) != OK) {
                return ret;
            }
        }
        printStatus(openList, closedList, "After iteration.\n");

        // --6-- go to 2
        iterationNr++;
    }
    if(lengthList(openList) == 0) {
        printf("Error in route algorithm, no nodes in OPEN list.\n");
        //TODO: cleanup memory?
        return ERRALGORTIHM;
    }
    else if(iterationNr >= MAX_A_STAR_ITERATIONS) {
        printf("Error in route algorithm, reached max iterations for finding path \n");
        return ERRALGORTIHM;
    }

#ifdef DEBUG_INFO
    printf("\nDEBUG status:\n");
    printf("Iterations: %d\n", iterationNr);
#endif
    return OK;
}