#include "map.c"
#include <limits.h>
#include <math.h>

City *startCity;
City *currentCity;
City *goalCity;
List *open;
List *closed;

// create compare function for city with least distance
static int compCityWithDistance(void *a, void *b)
{
    City *c1 = (City *)a;
    City *c2 = (City *)b;
    return (c1->distFromGoal + c1->distFromStart) - (c2->distFromGoal + c2->distFromStart);
}

// create function to calculate distance from end using void* as args
static void calcDistanceFromGoal(void *a)
{
    City *c = (City *)a;
    // goal city is the end city
    // c->distFromGoal = sqrt(pow(c->lat - goalCity->lat, 2) + pow(c->lon - goalCity->lon, 2));
    // absolute value of the difference between the latitudes and longitudes
    c->distFromGoal = (abs(c->lat - goalCity->lat) + abs(c->lon - goalCity->lon)) / 4;
}

static void priCity(void *a)
{
    City *c = (City *)a;
    // print city name and coordinates and distance from end and start
    printf("%s (%d, %d) %d %d\n", c->name, c->lat, c->lon, c->distFromStart, c->distFromGoal);
}

// create a function to print the city name, link to the previous city
static void printCityName(void *a)
{
    City *c = (City *)a;
    printf("%s <- ", c->name);
    if (c->link == NULL)
        printf("NULL\n");
    else
        printf("%s\n", c->link->name);
}

// create a function to print the city name, distance from start
static void printCityNameDist(void *a)
{
    City *c = (City *)a;
    printf("%s : (%d km) \n", c->name, c->distFromStart);
}

// create function taking List * map as arg 
// that adds currentCity to goalCity and in between using the city->link  as string
// starting from currentCity
static void printRoute(List *map)
{
    City *c = goalCity;
    while (c != NULL)
    {
        printCityNameDist(c);
        c = c->link;
    }
}

// create function to set distance from start to infinity
static void setDistFromStartToInfinity(void *a)
{
    City *c = (City *)a;
    c->distFromStart = INT_MAX;
}

// create function to check if city is in open list or closed list and distance > 100 skip else remove from open list and add to closed list
void checkNeighbor(void *n)
{
    Neighbor *neighbor = (Neighbor *)n;
    if (isInList(open, neighbor->city) || isInList(closed, neighbor->city) && currentCity->distFromStart + neighbor->distance > neighbor->city->distFromStart)
        return;
    remFromList(open, neighbor->city);
    remFromList(closed, neighbor->city);
    neighbor->city->distFromStart = currentCity->distFromStart + neighbor->distance;
    neighbor->city->link = currentCity;
    addList(open, neighbor->city);
}

// create main function with args
int main(int argc, char *argv[])
{
    status st;
    List *map;
    open = newList(compCityWithDistance, priCity);
    closed = newList(compCityWithDistance, prCity);

    // first arg is a city start and second arg is a city end
    char *start = "Rennes";
    char *end = "Lyon";

    st = generateMap(&map);
    if (st != OK)
    {
        printf("Error: %s\n", message(st));
        return 1;
    }

    // find the city start and city end
    st = findCity(map, start, &startCity);
    if (st != OK)
    {
        printf("Error: %s\n", message(st));
        return 1;
    }

    st = findCity(map, end, &goalCity);
    if (st != OK)
    {
        printf("Error: %s\n", message(st));
        return 1;
    }

    // ----- STEP 1 -----
    currentCity = startCity;
    // calculate the distance from the end city of each city in the map
    forEach(map, calcDistanceFromGoal);

    // add the start city to the open list
    st = addList(open, currentCity);
    if (st != OK)
    {
        printf("Error: %s\n", message(st));
        return 1;
    }

    // set distance from start of all other cities to infinity
    forEach(map, setDistFromStartToInfinity);

    // set the start city's distance from start to 0
    currentCity->distFromStart = 0;

    // test step 1
    // printf("Step 1\n");
    // forEach(map, priCity);
    // printf("\n");
    // printf("Open list\n");
    // forEach(open, priCity);

    // ----- STEP 2 -----
    while (lengthList(open) != 0)
    {
        // ----- STEP 3 -----
        // remove the first city from the open list and add it to the closed list, call it current
        st = remFromListAt(open, 1, &currentCity);
        if (st != OK)
        {
            printf("Error: %s\n", message(st));
            return 1;
        }

        st = addList(closed, currentCity);
        if (st != OK)
        {
            printf("Error: %s\n", message(st));
            return 1;
        }

        // ----- STEP 4 -----
        // if current is the end city, return success and use pointer to print the route
        if (currentCity == goalCity)
        {
            printf("Route found\n");
            // TODO: Print the route stored as link
            // display the map
            // forEach(map, printCityName);
            printRoute(map);
            return 0;
        }

        // ----- STEP 5 -----
        // print each neighbor of current
        printf("Neighbors: ");
        forEach(currentCity->neighbors, checkNeighbor);
    }

    // test step 2
    // printf("Step 2\n");
    // printf("Open list\n");
    // forEach(open, priCity);
    // printf("\n");
    // printf("Closed list\n");
    // forEach(closed, priCity);
    // printf("\n");
}
