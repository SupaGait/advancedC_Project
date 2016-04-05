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

status addListAt(List *list, int i, void *pVoid) {
    // Create the new node, add value if succeeded.
    Node* newNode = (Node*)malloc(sizeof(Node));
    if(!newNode) {
        return ERRALLOC;
    }
    newNode->val = pVoid;

    // First node?
    if(i==0) {
        Node *tmpNode = list->head;
        list->head = newNode;
        newNode->next = tmpNode;
    }
    else {
        // Iterate through the list of nodes, up to given element
        Node *node = list->head;
        for (int pos = 0; pos < i-1 && node; ++pos) {
            node = node->next;
        }
        if(node && node->next)
        {
            // Save next node, allocate and insert new element, set next node.
            Node *tmpNode = node->next;
            node->next = newNode;
            newNode->next = tmpNode;
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
        // Compare head
        if (list->comp(pVoid, list->head->val) <= 0) {
            Node *tmpNode = list->head;
            list->head = newNode;
            newNode->next = tmpNode;
            return OK;
        }

        // Compare rest of the list
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
        return (Node*)1;            // Requirement say return 1
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
