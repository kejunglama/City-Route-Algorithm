#ifndef MAP_H
#define MAP_H

typedef struct City
{
    char *name;
    struct List *neighbors;
    int lat;
    int lon;
    int distFromStart;
    int distFromGoal;
    struct City *link;
} City;

typedef struct Neighbor
{
    City *city;
    int distance;
} Neighbor;

/** Function to create a City */
status createCity(char *name, int lat, int lon, City **city);

/** Function to create a Neighbor */
status createNeighbor(List *list, char *neighborName, int distance, Neighbor **n);

/** Function to find a City from name in a List of Cities */
status findCity(List *list, char *name, City **city);

/** Function to display the Map with all Cities and Neighbors */
void displayMap(List *list);

/** Function to generate the Map (City List) from a file */
status generateMap(List **list);

/** Function to free the Map (City List) */
status freeMap(List *list);

#endif /* MAP_H */
