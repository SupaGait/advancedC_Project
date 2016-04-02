//
// Created by Gerard on 1-4-2016.
//

#include <stdio.h>
#include <string.h>
#include "Map.h"
#include "List.h"


/*************************************************************
 * Function to display the neighbors
 * @param city the City to display
 *************************************************************
 */
static void displayNeighbors(void *neighbor) {
    Neighbour *theNeighbor = (Neighbour*) neighbor;
    printf("\t%s,\tdistance: %d\n",theNeighbor->city, theNeighbor->distance);
}

/*************************************************************
 * Function to display a city of the list
 * @param city the City to display
 *************************************************************
 */
static void displayCity(void *city) {
    City* theCity = (City*)city;
    printf("%s, long:%d, lat:%d\n",theCity->cityName, theCity->longitude, theCity->latitude);
    forEach(theCity->neighbour, displayNeighbors);
}

/*************************************************************
 * Function to compare two elements (Cities) : based on strcmp and cityname
 * @param s1 the first City to compare
 * @param s2 the second City to compare
 * @return <0 if s1 is less than s2
 * @return 0 if s1 equals s2
 * @return >0 otherwise
 *************************************************************/
static int compCities (void * s1, void * s2) {
    City* city1 = (City*)s1;
    City* city2 = (City*)s2;
    return strcmp(city1->cityName, city2->cityName);
}

/*************************************************************
 * Find a specific city
 *  - Returns 0 if not found
  *************************************************************/
City* findCity(char* name, List *pToCityList)
{
    City* city = 0;

    // Dummy city for comparing
    City dummyCity;
    strncpy(dummyCity.cityName, name, MAX_CITYNAME_LENGTH);

    // Find the element containing the city
    Node *pNode = isInList(pToCityList, &dummyCity);
    if(!pNode) {
        return 0;
    }
    else if(pNode == (Node*)1) {
        return (City*)pToCityList->head;  // Element is in head
    }
    else {
        return (City*)pNode->next->val;  // Found the element
    }
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
        *city = (City*)pToCityList->head; // Element is in head
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
        city->neighbour = newList(compCities, displayCity);
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
    *cityList = newList(compCities, displayCity);

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
                    city->g = 10000;
                    city->h = 0;
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
 * Function to clear the g value of a City
 * @param city the City to display
 *************************************************************
 */
static void clearGvalue(void *city) {
    ((City*)city)->g=0;
}
/*************************************************************
 * Function to calculate h of each city
 * @param city the City to display
 *************************************************************
 */
static void calculateHValue(void *city) {
    ((City*)city)->h=0;
}

void findRoute(char *startCityName, char *goalCityName, List *cityList) {

    printf("\nFinding route from: %s to %s", startCityName, goalCityName);

    //TODO: check if cities are in the list
    City *startCity = findCity(startCityName, cityList);

    List openList;
    List closedList;

    //TODO: check all list results.
    // 1 Place n0 in OPEN. compute ˆh(n0) and set ˆg(n0) = 0. All otherˆg = 1
    addList(&openList,startCity);
    startCity->h = 0;
    startCity->g = 0;
    forEach(&openList,clearGvalue);

    // 2 if OPEN is empty, stop (failure)
    if(lengthList(&openList) == 0) {
        printf("Error in route algorithm aStart.");
        exit(-1); //TODO: cleanup memory?
    }

    // 3 remove from OPEN the vertex with minimal ˆf , call it n and add it to CLOSED


    // 4 if n is the goal, stop (success): use pointer chain to retrieve the solution path.
    // 5 For each successor si of n:
        // 5.1 compute ˆg(n) + c(n, si )
        // 5.2 if si is in OPEN or in CLOSED and ˆg(n) + c(n, si ) > ˆg(si ), skip to next successor
        // 5.3 remove si from OPEN and CLOSED if present
        // 5.4 insert si in OPEN and update ˆ g(si ) and backpath pointer
    // 6 go to 2
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