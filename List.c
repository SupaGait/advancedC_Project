#include "List.h"

List *newList(compFun fun, prFun fun1) {
    // Allocate the memory, set the functions for printing and comparing
    List* newList = (List*)malloc(sizeof(List));
    if(newList)
    {
        newList->comp = fun;
        newList->pr = fun1;
        newList->head = 0;
        newList->nelts = 0;
    }
    return newList;
}

void delList(List *list) {
    // Remove the nodes from the list
    Node *nodeTmp = list->head;
    while (nodeTmp) {
        nodeTmp = nodeTmp->next;
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

status addListAt(List *list, int i, void *pVoid) {
    // Iterate through the list, up to given element
    Node *node = list->head;
    for (int pos = 0; pos < i && node; ++pos) {
        node = node->next;
    }
    if(node)
    {
        // Save next node, allocate and insert new element, set next node.
        Node *tmpNode = node->next;
        node = (Node*)malloc(sizeof(node));
        if(!node) {
            return ERRALLOC;
        }
        node->val = pVoid;
        node->next = tmpNode;
        ++list->nelts;
    }
    else {
        // Node not found
        return ERRINDEX;
    }
    return OK;
}

status remFromListAt(List *list, int i, void **pVoid) {
    Node *node = list->head;
    for (int pos = 0; pos < i && node ; ++pos) {
        node = node->next;
    }
    if(node) {
        Node* tmpNode = node->next;
        free(node);
        node->next = tmpNode;
        --list->nelts;
    }
    else {
        return ERRINDEX;
    }
    return OK;
}

status remFromList(List *list, void *pVoid) {
    // Test if comparable function is available
    if(!list->comp)
        return ERRUNABLE;

    // Iterate through the list, and remove element if found using compare
    Node *node = list->head;
    while(node)
    {
        if( list->comp(pVoid, node->val) == 0 ) {
            Node* tmpNode = node->next;
            free(node);
            node->next = tmpNode;
            --list->nelts;
            return OK;
        }
        node = node->next;
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
    for (Node* node = list->head; node; node = node->next) {
        length++;
    }
    return length;
}

status addList(List *list, void *pVoid) {
    // Check for comparable function
    if(!list->comp) {
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
        Node *node = list->head;
        while (node->next) {
            if (list->comp(pVoid, node->next->val) <= 0) {
                // Add in between
                Node *tmpNode = node->next;
                node->next = newNode;
                newNode->next = tmpNode;
                return OK;
            }
            node = node->next;
        }
        //Append as last
        node->next = newNode;
    }
    return OK;
}

Node *isInList(List *list, void *pVoid) {
    Node *node = list->head;
    if(!node) {
        // Not found
        return 0;
    }

    // element is at the head of the list
    if(list->comp(pVoid, node->val) == 0 ) {
        return (Node*)1;            // Requirement say return 1... but this is not possible with this impl.
    }
    // Iterate through the list and return matching node if found.
    while (node->next){
        if(list->comp(node->next->val, pVoid) == 0) {
            return node;
        }
        node = node->next;
    }

    // Not found
    return 0;
}
