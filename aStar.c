#include <limits.h>
#include <math.h>
#include <stdio.h>

#include "./status/status.h"
#include "list.h"
#include "map.h"

City *startCity;
City *currentCity;
City *goalCity;
List *open;
List *closed;

/** Comparision function for City with least Route distance */
static int compCityWithDistance(void *a, void *b)
{
    City *c1 = (City *)a;
    City *c2 = (City *)b;
    return (c1->distFromGoal + c1->distFromStart) - (c2->distFromGoal + c2->distFromStart);
}

/** Function to print City with distance from previous City */
static void printRouteCity(void *a)
{
    City *c = (City *)a;
    printf("%s : (%d km) \n", c->name, c->distFromStart);
}

/** Function to print all Route Cities from Start to Goal */
static void printRoute(List *map)
{
    City *c = goalCity;
    printf("\n");
    while (c != NULL)
    {
        printRouteCity(c);
        c = c->link;
    }
    printf("\n");
}

/** Function to calculate distance from Goal */
static void calcDistanceFromGoal(void *a)
{
    City *c = (City *)a;
    c->distFromGoal = (abs(c->lat - goalCity->lat) + abs(c->lon - goalCity->lon)) / 4;
}

/** Function to set distance from Start to Infinity */
static void setDistFromStartToInfinity(void *a)
{
    City *c = (City *)a;
    c->distFromStart = INT_MAX;
}

/** Function to choose the Route with Less Route Distance */
void findBestRouteNeighbor(void *n)
{
    status st;
    Neighbor *neighbor = (Neighbor *)n;

    int isInOpen = (int)isInList(open, neighbor->city);
    int isInClosed = (int)isInList(closed, neighbor->city);

    int routeDist = currentCity->distFromStart + neighbor->distance;
    int directDist = neighbor->city->distFromStart;

    if (isInOpen || isInClosed && routeDist > directDist)
        return;

    if (isInOpen)
    {
        st = remFromList(open, neighbor->city);
        if (st != OK)
        {
            printf("Error: %s\n", message(st));
            return;
        }
    }
    if (isInClosed)
    {
        st = remFromList(closed, neighbor->city);
        if (st != OK)
        {
            printf("Error: %s\n", message(st));
            return;
        }
    }

    neighbor->city->distFromStart = routeDist;
    neighbor->city->link = currentCity;
    st = addList(open, neighbor->city);
    if (st != OK)
    {
        printf("Error: %s\n", message(st));
        return;
    }
}

/** Function to convert String into Sentence Case*/
void toSentenceCase(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (i == 0)
        {
            if (str[i] >= 'a' && str[i] <= 'z')
                str[i] -= 32;
            i++;
            continue;
        }
        if (str[i] == ' ')
        {
            i++;
            if (str[i] >= 'a' && str[i] <= 'z')
                str[i] -= 32;
            continue;
        }
        if (str[i] >= 'A' && str[i] <= 'Z')
            str[i] += 32;
        i++;
    }
}

int main(int argc, char *argv[])
{
    char *start;
    char *end;

    // if not enough args ask for the start and end city
    if (argc < 3)
    {
        printf("Please enter the start city: ");
        start = malloc(30 * sizeof(char));
        scanf("%s", start);
        printf("Please enter the end city: ");
        end = malloc(30 * sizeof(char));
        scanf("%s", end);

        toSentenceCase(start);
        toSentenceCase(end);
    }
    else
    {
        start = argv[1];
        end = argv[2];
    }

    status st;
    List *map;
    open = newList(compCityWithDistance, printRouteCity);
    closed = newList(compCityWithDistance, printRouteCity);

    /** generate the map from the file */
    st = generateMap(&map);
    if (st == ERROPEN)
    {
        printf("Error: %s\n", message(st));
        return 2;
    }
    if (st != OK)
    {
        printf("Error: %s\n", message(st));
        return 3;
    }

    /** find the city start and end from their names in the Map*/
    st = findCity(map, start, &startCity);
    if (st == ERRABSENT)
    {
        printf("Error: Start City, '%s'. %s\n", start, message(st));
        return 1;
    }
    if (st != OK)
    {
        printf("Error: %s\n", message(st));
        return 3;
    }

    st = findCity(map, end, &goalCity);
    if (st == ERRABSENT)
    {
        printf("Error: Goal City, '%s'. %s\n", end, message(st));
        return 1;
    }
    if (st != OK)
    {
        printf("Error: %s\n", message(st));
        return 3;
    }

    /** ----- STEP 1 -----*/
    currentCity = startCity;
    forEach(map, calcDistanceFromGoal);

    /** add the start city to the open list */
    st = addList(open, currentCity);
    if (st != OK)
    {
        printf("Error: %s\n", message(st));
        return 3;
    }

    /** set the distance from start to infinity for all cities */
    forEach(map, setDistFromStartToInfinity);

    /** set the start city's distance from start to 0 */
    currentCity->distFromStart = 0;

    /** ----- STEP 2 -----*/
    while (lengthList(open) != 0)
    {
        /** ----- STEP 3 -----*/
        /** remove the first city from the open list and add it to the closed list, call it current */
        st = remFromListAt(open, 1, &currentCity);
        if (st != OK)
        {
            printf("Error: %s\n", message(st));
            return 3;
        }

        st = addList(closed, currentCity);
        if (st != OK)
        {
            printf("Error: %s\n", message(st));
            return 3;
        }

        /** ----- STEP 4 -----*/
        /** if current is the end city, return success and use pointer to print the route */
        if (currentCity == goalCity)
        {
            printRoute(map);
            delList(open);
            delList(closed);
            st = freeMap(map);
            if (st != OK)
            {
                printf("Error: %s\n", message(st));
                return 3;
            }
            return 0;
        }

        /** ----- STEP 5 ----- */
        forEach(currentCity->neighbors, findBestRouteNeighbor);
    }
}
