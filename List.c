/**
 * @file List.c
 * @brief Creation of a generic (simply linked) List structure.
 *
 */

#include "List.h"
List *newList(compFun getCompfun, compFun addCompFun, prFun fun1) {
    // Allocate the memory, set the functions for printing and comparing
    List* newList = (List*)malloc(sizeof(List));
    if(newList)
    {
        newList->getComp = getCompfun;
        newList->addComp = addCompFun;
        newList->pr = fun1;
        newList->head = 0;
        newList->nelts = 0;
    }
    return newList;
}

void delList(List *list) {
    // Remove the nodes from the list
    while (list->head) {
        Node* nodeTmp = list->head->next;
        free(list->head);
        list->head = nodeTmp;
    }
    // Cleanup the final list
    free(list);
}

status nthInList(List *list, int i, void **pVoid) {
    Node *node = list->head;
    for (int pos = 0; pos < i && node; ++pos) {
        node = node->next;
    }
    if(node) {
        *pVoid = node->val;
        return OK;
    }
    else {
        return ERRINDEX;
    }
}

status addList(List *list, void *pVoid) {
    // Check for comparable function
    if(!list->addComp) {
        return ERRUNABLE;
    }

    // Create the new node
    Node* newNode = (Node*)malloc( sizeof(Node) );
    if(!newNode) {
        return ERRALLOC;
    }
    newNode->val = pVoid;
    newNode->next = 0;
    ++list->nelts;

    // Add in the list where applicable
    if(!list->head) {
        list->head = newNode;
    }
    else {
        // Compare head
        if (list->addComp(pVoid, list->head->val) <= 0) {
            newNode->next = list->head;
            list->head = newNode;
            return OK;
        }

        // Compare rest of the list
        Node *node = list->head;
        while (node->next) {
            if (list->addComp(pVoid, node->next->val) <= 0) {
                // Add in between
                newNode->next = node->next;
                node->next = newNode;
                return OK;
            }
            node = node->next;
        }
        //Append as last
        node->next = newNode;
    }
    return OK;
}
status addListAt(List *list, int i, void *pVoid) {
    // Create the new node, add value if succeeded.
    Node* newNode = (Node*)malloc(sizeof(Node));
    if(!newNode) {
        return ERRALLOC;
    }
    newNode->val = pVoid;

    // First node?
    if(i==0) {
        newNode->next = list->head;
        list->head = newNode;
    }
    else {
        // Iterate through the list of nodes, up to given element
        Node *node = list->head;
        for (int pos = 0; pos < i-1 && node; ++pos) {
            node = node->next;
        }
        if(node && node->next)
        {
            // Allocate and insert new element, set next node.
            newNode->next = node->next;
            node->next = newNode;
        }
        else {
            // Node not found
            return ERRINDEX;
        }
    }
    ++list->nelts;
    return OK;
}

status remFromListAt(List *list, int i, void **pVoid) {
    // Remove head if index is 0
    if(i==0){
        if(!list->head) {
            return ERRINDEX;
        }
        // Return the element
        *pVoid = list->head->val;
        // Free node, and point to the next node
        Node *tmpNode = list->head->next;
        free(list->head);
        list->head = tmpNode;
    }
    else {
        // Otherwise iterate up to one place before the requested node position
        Node *node = list->head;
        for (int pos = 0; pos < i-1 && node ; ++pos) {
            node = node->next;
        }
        // Remove the node if there is one.
        if(node && node->next) {
            // Return the element
            *pVoid = node->next->val;
            // Free node, and point to the next node
            Node* tmpNode = node->next->next;
            free(node->next);
            node->next = tmpNode;
        }
        else {
            return ERRINDEX;
        }
    }
    --list->nelts;
    return OK;
}

status remFromList(List *list, void *pVoid) {
    // Test if comparable function is available
    if(!list->getComp)
        return ERRUNABLE;

    // Compare with head, free if it is the node.
    if(list->getComp(pVoid, list->head->val) == 0 ) {
        Node* tmpNode = list->head->next;
        free(list->head);
        list->head = tmpNode;
        return OK;
    }
    else {
        // Iterate through the list, and remove element if found using compare
        Node *node = list->head;
        while(node->next)
        {
            if(list->getComp(pVoid, node->next->val) == 0 ) {
                Node* tmpNode = node->next->next;
                free(node->next);
                node->next = tmpNode;
                --list->nelts;
                return OK;
            }
            node = node->next;
        }
    }
    return ERRABSENT;
}

status displayList(List *list) {
    if(!list->pr) {
        return ERRUNABLE;
    }
    // Print all nodes, using the set method
    for (Node *node = list->head; node ; node=node->next) {
        list->pr(node->val);
    }
    return OK;
}

void forEach(List *list, void (*pFunction)(void *)) {
    // Iterate through nodes and call the function
    for (Node* node = list->head; node; node = node->next) {
        pFunction(node->val);
    }
}

int lengthList(List *list) {
    int length = 0;
    // Iterate through the nodes
    for (Node* node = list->head; node; node = node->next) {
        length++;
    }
    return length;
}


Node *isInList(List *list, void *pVoid) {
    Node *node = list->head;
    if(!node) {
        // Not found
        return 0;
    }

    // element is at the head of the list
    if(list->getComp(pVoid, node->val) == 0 ) {
        return (Node*)1;            // Requirement say return 1
    }
    // Iterate through the list and return matching node if found.
    while (node->next){
        if(list->getComp(node->next->val, pVoid) == 0) {
            return node;
        }
        node = node->next;
    }

    // Not found
    return 0;
}
Node *isInListComp(List *list, void *pVoid, compFun compareFun) {
    Node *node = list->head;
    if(!node) {
        // Not found
        return 0;
    }

    // element is at the head of the list
    if(compareFun(pVoid, node->val) == 0 ) {
        return (Node*)1;            // Requirement say return 1
    }
    // Iterate through the list and return matching node if found.
    while (node->next){
        if(compareFun(node->next->val, pVoid) == 0) {
            return node;
        }
        node = node->next;
    }

    // Not found
    return 0;
}
