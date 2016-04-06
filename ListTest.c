/*************************************************************
 *
 * $Id: ListTest.c 1972 2014-11-12 10:23:55Z phil $
 * 
 *************************************************************
 */
/**
 * Sample test program for generic Lists : application to strings.
 *
 *************************************************************/

//static char rcsId[] = "@(#) $Id: ListTest.c 1972 2014-11-12 10:23:55Z phil $";

#include <stdio.h>
#include <string.h>
#include "List.h"


/*************************************************************
 * Function to compare two elements (strings) : based on strcmp
 * @param s1 the first string to compare
 * @param s2 the second string to compare
 * @return <0 if s1 is less than s2
 * @return 0 if s1 equals s2
 * @return >0 otherwise
 *************************************************************/
static int compString(void *s1, void *s2) {
    return strcmp((char *) s1, (char *) s2);
}

/*************************************************************
 * Function to display an element of the list
 * @param s the string to display
 *************************************************************
 */
static void prString(void *s) {
    printf("|%s|", (char *) s);
}

/*************************************************************
 * another display function, just to test "forEach"
 * @param s the string to display
 *************************************************************/
static void prString2(void *s) {
    printf("%s\n", (char *) s);
}

/*************************************************************
 * test program: creation of a list of strings then tests
 * for main list functionalities.
 *************************************************************/

int main() {
    int i;
    char *tab[] = {
            "belle marquise",
            "vos beaux yeux",
            "me font",
            "mourir",
            "d'amour"};

    /* list creation */
    List *l = newList(compString,compString, prString);
    if (!l) return 1;

    /* populating list */
    for (i = 0; i < sizeof(tab) / sizeof(char *); i++)
        addList(l, tab[i]);

    /* display list - should be sorted */
    displayList(l);
    putchar('\n');

    /*  test contain predicate */
    if (isInList(l, "mourir"))
        puts("mourir is in list");
    else
        puts("mourir is not in list");
    if (isInList(l, "vivre"))
        puts("vivre is in list");
    else
        puts("vivre is not in list");

    /* test length (beware: sizeof is long int!) */
    printf("length : %d (expected %ld)\n", lengthList(l),
           sizeof(tab) / sizeof(char *));

    /* test forEach */
    puts("forEach:");
    forEach(l, prString2);

    // test nthInList
    char *line = 0;
    nthInList(l,2, (void*)&line);
    if(!line || strcmp(line,tab[2]) != 0) {
        printf("nthInList() does not work correctly, %s != %s\n", line, tab[2] );
    }
    puts("-----------------\n");

    // test inlist with custom compare
    if(isInListComp(l, "me font", compString))
        puts("me font is in list\n");
    else
        puts("me font is not in list\n");
    puts("-----------------\n");

    // test addListAt, add 1nd(head) and 2nd(node).
    char *stringPos0 = "Added at head (pos 0)";
    char *stringPos1 = "Added at 2nd element (pos 1)";
    addListAt(l,0,(void*) stringPos0);
    addListAt(l, 1, (void*) stringPos1);
    displayList(l);
    puts("\n-----------------\n");

    // Test if replacing last element works
    int listSize = lengthList(l);
    printf("List size: %d\n",listSize);
    addListAt(l,listSize-1,(void*)" Added at last pos ");
    displayList(l);
    puts("\n--------------\n");

    char* pEle = 0;
    remFromListAt(l,0, (void**)&pEle);
    remFromListAt(l,listSize-2, (void**)&pEle); // -1 add -1 for remove first element
    displayList(l);
    puts("\n--------------\n");

    // Test if removing works well
    puts("-----Original again:---------\n");
    remFromListAt(l,0,(void**)&pEle);
    displayList(l);
    printf("\nRemoved element: %s should be equal to: %s", pEle, stringPos1);

    // Test remove based on compare
    remFromList(l,"belle marquise");    // Remove head
    remFromList(l,"me font");           // Remove 2nd node

    puts("\n--------------\n");
    displayList(l);

    printf("\nList size: %d\n",lengthList(l));

    /* final cleanup */
    delList(l);

    return 0;
}
/*************************************************************/
