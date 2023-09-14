#include "map.c"
#include <limits.h>
#include <math.h>

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
    while (c != NULL)
    {
        printRouteCity(c);
        c = c->link;
    }
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

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Error: Needs two cities as args <start> & <end>.\n");
        printf("Usage: %s <start> <end>\n", argv[0]);
        return 1;
    }

    status st;
    List *map;
    open = newList(compCityWithDistance, printRouteCity);
    closed = newList(compCityWithDistance, printRouteCity);

    char *start = argv[1];
    char *end = argv[2];

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

    /** test step 1
        printf("Step 1\n");
        forEach(map, priCity);
        printf("\n");
        printf("Open list\n");
        forEach(open, priCity);
    */

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

    /** test step 2
        printf("Step 2\n");
        printf("Open list\n");
        forEach(open, priCity);
        printf("\n");
        printf("Closed list\n");
        forEach(closed, priCity);
        printf("\n");
    */
}
