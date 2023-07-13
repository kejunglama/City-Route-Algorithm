#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"
#include "list.c"
#include "./Status/status.h"
#include "./Status/status.c"

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

static int compCity(void *a, void *b)
{
    City *c1 = (City *)a;
    City *c2 = (City *)b;
    return strcmp(c1->name, c2->name);
}

static int compNeighbor(void *a, void *b)
{
    Neighbor *n1 = (Neighbor *)a;
    Neighbor *n2 = (Neighbor *)b;
    return strcmp(n1->city->name, n2->city->name);
}

static void prCity(void *a)
{
    City *c = (City *)a;
    printf("%s (%d, %d)\n", c->name, c->lat, c->lon);
}

static void prNeighbor(void *a)
{
    Neighbor *n = (Neighbor *)a;
    printf("  %s %d\n", n->city->name, n->distance);
}

status createCity(char *name, int lat, int lon, City **city)
{
    City *c = malloc(sizeof(City));
    if (c == NULL)
    {
        free(c);
        return ERRALLOC;
    }

    c->name = malloc(sizeof(char) * (strlen(name) + 1));
    if (c->name == NULL)
    {
        free(c);
        free(c->name);
        return ERRALLOC;
    }

    strcpy(c->name, name);
    c->lat = lat;
    c->lon = lon;
    c->neighbors = newList(NULL, NULL);
    c->distFromStart = 0;
    c->distFromGoal = 0;
    c->link = NULL;

    *city = c;
    return OK;
}

status findCity(List *list, char *name, City **city)
{
    City *c;
    status st;
    if (list == NULL)
        return ERREMPTY;

    st = createCity(name, 0, 0, &c);
    if (st != OK)
    {
        free(c->name);
        free(c);
        return st;
    }

    if (!isInList(list, c))
    {
        free(c->name);
        free(c);
        return ERRABSENT;
    };

    if (isInList(list, c) == (Node *)1) /*  if the city is the head of the list */
    {
        *city = (City *)list->head->val;
        return OK;
    }

    Node *n = isInList(list, c)->next;
    *city = (City *)n->val;
    return OK;
}

status createNeighbor(List *list, char *neighborName, int distance, Neighbor **n)
{
    City *neighborCity;
    status st;

    Neighbor *neighbor = malloc(sizeof(Neighbor));
    if (!neighbor)
    {
        free(neighbor);
        return ERRALLOC;
    }

    st = findCity(list, neighborName, &neighborCity);
    if (st != OK)
    {
        free(neighbor);
        return st;
    }

    neighbor->city = neighborCity;
    neighbor->distance = distance;
    *n = neighbor;
    return OK;
}

void displayMap(List *list)
{
    Node *current = list->head;
    printf("Map:\n");
    while (current)
    {
        City *city = current->val;
        printf("%s (%d, %d)\n", city->name, city->lat, city->lon);
        Node *neighbor = city->neighbors->head;
        while (neighbor)
        {
            Neighbor *n = neighbor->val;
            printf("  %s %d\n", n->city->name, n->distance);
            neighbor = neighbor->next;
        }
        current = current->next;
    }
}

status generateMap(List **list)
{
    status st;
    City *currentCity;
    char line[100];
    char name[50];
    int lat;
    int lon;

    /** Open File  */
    FILE *file = fopen("FRANCE.MAP", "r");
    if (!file)
    {
        fclose(file);
        return ERROPEN;
    }

    /** Initialize  */
    List *map = newList(compCity, prCity);
    if (!map)
    {
        fclose(file);
        return ERRALLOC;
    }

    /** Populate Cities  */
    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "%s %d %d", name, &lat, &lon) == 3)
        {
            st = createCity(name, lat, lon, &currentCity);
            if (st != OK)
            {
                fclose(file);
                return st;
            }
            st = addList(map, currentCity);
        }
    }

    rewind(file);

    /** Populate Neighbors */
    char neighborName[50];
    int distance;

    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "%s %d %d", name, &lat, &lon) == 3)
        {
            st = findCity(map, name, &currentCity);
            if (st != OK)
            {
                fclose(file);
                return st;
            }
            currentCity->neighbors = newList(compNeighbor, prNeighbor);
        }

        if (sscanf(line, "%s %d %d", name, &distance) == 2)
        {
            Neighbor *neighbor;
            st = createNeighbor(map, name, distance, &neighbor);
            if (st != OK)
            {
                fclose(file);
                return st;
            }

            st = addList(currentCity->neighbors, neighbor);
            if (st != OK)
            {
                fclose(file);
                return st;
            }
        }
    }

    *list = map;
    fclose(file);
    return OK;
}

status freeMap(List *list)
{
    if (list == NULL)
        return ERREMPTY;

    Node *current = list->head;
    while (current)
    {
        City *city = current->val;
        free(city->name);
        delList(city->neighbors);
        free(city);
        current = current->next;
    }

    delList(list);
    return OK;
}
