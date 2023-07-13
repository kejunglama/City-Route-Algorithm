/********************************************************************
 *
 * $Id: List.c 2135 2019-09-16 07:22:19Z phil $
 *
 ********************************************************************/
/**
 *
 * Creation of a generic (simply linked) List structure.
 *
 * To create a list, one must provide two functions (one function to
 * compare / order elements, one function to display them). Unlike arrays,
 * indices begins with 1.
 *
 ********************************************************************/

#include <stdio.h>
#include "List.h"

/** stores the list of available Nodes instead of deallocating / reallocating
 * them all the time */
static Node *available = 0;

/* create a new, empty list */
List *newList(compFun comp, prFun pr)
{
  List *l;
  l = (List *)malloc(sizeof(List));
  if (!l)
    return 0;
  l->nelts = 0;
  l->head = 0;
  l->comp = comp;
  l->pr = pr;
  return l;
}

/* destroy the list by deallocating used memory */
void delList(List *l)
{
  Node *tmp = l->head;
  while (tmp)
  {
    l->head = tmp->next;
    tmp->next = available;
    available = tmp;
    tmp = l->head;
  }

  /* tmp == 0 : list is empty */

  l->head = 0;
  l->nelts = 0;
  free(l);
}

/* return  the Nth element of the list if exists */
status nthInList(List *l, int pos, void *res)
{
  Node *tmp = l->head;
  if (pos <= 0 || pos > l->nelts)
    return ERRINDEX;
  while (pos-- > 1)
    tmp = tmp->next;
  *(void **)res = tmp->val;
  return OK;
}

/* Insert element at a given position in the list */
status addListAt(List *l, int pos, void *elt)
{
  if (pos <= 0 || pos > l->nelts + 1)
    return ERRINDEX;

  /* get a new Node and increment length */
  Node *toAdd = available;
  if (!toAdd)
    toAdd = (Node *)malloc(sizeof(Node));
  else
    available = available->next;
  if (!toAdd)
    return ERRALLOC;
  l->nelts++;
  toAdd->val = elt;

  /* if pos is 1, must change list head */
  if (pos == 1)
  {
    toAdd->next = l->head;
    l->head = toAdd;
  }

  /* otherwise we need a temporary pointer */
  else
  {
    Node *tmp = l->head;

    /* tmp points to the predecessor */
    while (pos-- > 2)
      tmp = tmp->next;

    /* actual insertion */
    toAdd->next = tmp->next;
    tmp->next = toAdd;
  }

  return OK;
}

/* remove the element located at a given position in list */
status remFromListAt(List *l, int pos, void *res)
{
  Node *toRem = l->head;

  if (pos <= 0 || pos > l->nelts)
    return ERRINDEX;

  /* particular case: must remove list head */

  if (pos == 1)
    l->head = toRem->next;

  /* otherwise point to predecessor to change links */
  else
  {
    Node *prec = toRem;
    while (pos-- > 2)
      prec = prec->next;
    toRem = prec->next;
    prec->next = toRem->next;
  }

  *(void **)res = toRem->val;
  toRem->next = available;
  available = toRem;
  l->nelts--;
  return OK;
}

/* remove given element from given list */
status remFromList(List *l, void *elt)
{
  Node *prec = l->head, *toRem = 0;

  if (l->comp == 0)
    return ERRUNABLE;
  if (l->nelts == 0)
    return ERRABSENT;
  if (!(l->comp)(elt, prec->val))
    return remFromListAt(l, 1, &(prec->val));

  /* points to the predecessor */

  while (prec->next && (l->comp)(elt, prec->next->val))
  {
    prec = prec->next;
  }

  /* here
   *  - either we point to the last element
   *  - or next one is the one to remove!
   */

  if (!prec->next)
    return ERRABSENT;

  toRem = prec->next;
  prec->next = toRem->next;
  toRem->next = available;
  available = toRem;
  l->nelts--;

  return OK;
}

/* display list elements as "[ e1 ... eN ]" */
status displayList(List *l)
{
  Node *tmp = l->head;
  if (l->pr == 0)
    return ERRUNABLE;
  printf("[ ");
  while (tmp)
  {
    (l->pr)(tmp->val);
    putchar(' ');
    tmp = tmp->next;
  }
  putchar(']');
  return OK;
}

/* sequencially call given function with each element of given list */
void forEach(List *l, void (*f)(void *))
{
  Node *tmp = l->head;
  while (tmp)
  {
    (*f)(tmp->val);
    tmp = tmp->next;
  }
}

/* compute and return the number of elements in given list */
int lengthList(List *l) { return l->nelts; }

/** private function to get the node preceding the one we're looking for
 * @param e the searched element
 * @return 0 if element is not found (typically, list is empty)
 * @return 1 if element is head of list
 * @return a pointer to the predecessor otherwise
 */
static Node *previous(List *l, void *e)
{
  Node *prec = l->head;
  if (l->nelts == 0)
    return 0;
  if (!(l->comp)(e, prec->val))
    return (Node *)1;

  /* prec must address element prior to the one to remove */
  while (prec->next && (l->comp)(e, prec->next->val))
    prec = prec->next;

  /* here,
   *  - either we address nothing (no such element)
   *  - or we address the element prior to the one we're looking for
   */
  return prec;
}

/* add given element to given list according to compFun function */
status addList(List *l, void *e)
{
  Node *prec = l->head, *toAdd;

  if (l->comp == 0)
    return ERRUNABLE;

  /* add to the head if list is empty, if no comparison function is given
   * or if e < head element
   */
  if (l->nelts == 0 || (l->comp)(e, l->head->val) < 0)
    return addListAt(l, 1, e);

  /* otherwise, get predecessor and link new element just after it */
  while (prec && prec->next && (l->comp)(prec->next->val, e) < 0)
    prec = prec->next;

  toAdd = available;
  if (!toAdd)
    toAdd = (Node *)malloc(sizeof(Node));
  else
    available = available->next;
  if (!toAdd)
    return ERRALLOC;
  toAdd->next = prec->next;
  toAdd->val = e;
  prec->next = toAdd;
  l->nelts++;
  return OK;
}

/* test whether the list contains given element */
Node *isInList(List *l, void *e)
{
  Node *prec = previous(l, e);
  if (!prec)
    return 0;
  if (prec == (Node *)1)
    return (Node *)1;
  if (prec->next == 0)
    return 0;
  if (!(l->comp)(e, prec->next->val))
    return prec;
  return 0;
}

/* return the first element in l that matches f, or 0 */
void *firstThat(List *l, int (*f)(void *))
{
  Node *n = l->head;
  while (n)
  {
    if (f(n->val))
      return n->val;
    n = n->next;
  }
  return 0;
}

/* return the list of all elements in l that match f */
List *allThat(List *l, int (*f)(void *))
{
  List *res = newList(l->comp, l->pr);
  Node *n = l->head;
  while (n)
  {
    if (f(n->val))
      addListAt(res, res->nelts + 1, n->val);
    n = n->next;
  }
  return res;
}
