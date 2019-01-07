#include <stdio.h>
#include <stdlib.h>
#include  <limits.h>
#include <string.h>
#include "btree.h"

void BTreeInit(int _t)
{
    root = NULL;  t = _t - 1;
}

void traverse()
{
    if (root != NULL) _traverse(root);
}


BTreeNode* search(int k)
{
    return (root == NULL) ? NULL : _search(root, k);
}


BTreeNode* _createNode(bool _leaf)
{
    BTreeNode* newNode = (BTreeNode*)malloc(sizeof(BTreeNode));
    int i;

    // Copy the given minimum degree and leaf property
    newNode->leaf = _leaf;
    // Allocate memory for maximum number of possible keys
    // and child pointers
    newNode->keys = (int*)malloc(sizeof(int) * (t+1));
    newNode->C = (BTreeNode**)malloc(sizeof(BTreeNode*)*(t+2));

    // Initialize child
    for (i = 0; i < t + 2; i++)
        newNode->C[i] = NULL;

    // Initialize the number of keys as 0
    newNode->n = 0;
    return newNode;
}


void _traverse(BTreeNode* present)
{
    // There are n keys and n+1 children, travers through n keys and first n children
    int i;
    for (i = 0; i < present->n; i++)
    {
        // If this is not leaf, then before printing key[i],
        // traverse the subtree rooted with child C[i].
        if (present->leaf == false)
            _traverse(present->C[i]);

        printf(" ");
        printf("%d", present->keys[i]);
    }

    // Print the subtree rooted with last child
    if (present->leaf == false)
        _traverse(present->C[i]);
}


BTreeNode* _search(BTreeNode* present, int k)
{
    // Find the first key greater than or equal to k
    int i = 0;
    while (i < present->n && k > present->keys[i])
        i++;

    // If the found key is equal to k, return this node
    if (i < present->n && present->keys[i] == k)
        return present;

    // If key is not found here and this is a leaf node
    if (present->leaf == true)
        return NULL;

    // Go to the appropriate child
    return _search(present->C[i], k);
}


void insertElement(int k)
{ 
    // Find key in this tree, and If there is a key, it prints error message.
    if (search(k) != NULL)
    {
        printf("The tree already has %d \n", k);
        return;
    }

    // If tree is empty
    if (root == NULL)
    {
        //printf("First Insertion %d\n", k);
        // Allocate memory for root
        root = _createNode(true);
        root->keys[0] = k;  // Insert key
        root->n = 1;  // Update number of keys in root
    }
    else // If tree is not empty
        _insert(root, k, NULL);
}

void simple_insert(BTreeNode *present, int k)
{
    int pos = present->n-1;
    
    if(present->leaf == true) {
        while(pos >= 0 && present->keys[pos] > k){
            present->keys[pos+1] = present->keys[pos];
            pos--;
        }
        
        present->keys[pos+1] = k;
        present->n+=1;
    } else {
        while(pos >= 0 && present->keys[pos--] > k){
            pos--;
        }

        //int real_pos = pos+1;
        if(present->C[pos+1]->n == t) {
            simple_split(pos+1, present->C[pos+1], present);
            if(present->keys[pos+1] < k)
                pos++;
        }
        
        simple_insert(present->C[pos+1],k);
    }
}
void _insert(BTreeNode* present, int k, BTreeNode* parent)
{
    if(present->n == t) {
        BTreeNode *new = _createNode(false);
        new->C[0] = present;
        simple_split(0, present, new);
        int pos=0;
        if(new->keys[0] < k)
            pos++;
       
        simple_insert(new->C[pos], k);
        root = new;
    }
    else
        simple_insert(present, k);
}
void simple_split(int index, BTreeNode *child, BTreeNode* parent)
{
    int mid = t/2;
    int copy_pos = mid+1;
    BTreeNode *new_r = _createNode(child->leaf);
    new_r->n = mid-1;
    
    for(int i=0;i<mid;i++)
        new_r->keys[i] = child->keys[i+copy_pos];
    
    if(child->leaf == false) 
        for(int i=0;i<copy_pos;i++)
            new_r->C[i] = child->C[i+copy_pos];
    
    child->n = mid;
    
    int compare = index+1;
    for(int i=parent->n; i>=compare;i--)
        parent->C[i+1] = parent->C[i];

    parent->C[compare] = new_r;

    for(int i=parent->n-1; i>=index; i--)
        parent->keys[i+1] = parent->keys[i];

    parent->keys[index] = child->keys[mid];

    parent->n+=1;
}

BTreeNode * _splitChild(BTreeNode* present, BTreeNode* parent)
{
}


// This code related to print the B-tree
int _getLevel(BTreeNode* present)
{
    int i;
    int maxLevel = 0;
    int temp;
    if(present == NULL) return maxLevel;
    if(present->leaf == true)
        return maxLevel+1;

    for (i = 0; i < present->n+1; i++)
    {
        temp = _getLevel(present->C[i]);

        if (temp > maxLevel)
            maxLevel = temp;
    }

    return maxLevel + 1;
}

// This code related to print the B-tree
void _getNumberOfNodes(BTreeNode* present, int* numNodes, int level)
{
    int i;
    if (present == NULL) return;

    if (present->leaf == false)
    {
        for (i = 0; i < present->n+1; i++)
            _getNumberOfNodes(present->C[i], numNodes, level + 1);
    }
    numNodes[level] += 1;
}

// This code related to print the B-tree
void _mappingNodes(BTreeNode* present, BTreeNode ***nodePtr, int* numNodes, int level)
{
    int i;
    if (present == NULL) return;

    if (present->leaf == false)
    {
        for (i = 0; i < present->n+1; i++)
            _mappingNodes(present->C[i], nodePtr, numNodes, level + 1);
    }

    nodePtr[level][numNodes[level]] = present;
    numNodes[level] += 1;
}

// This code related to print the B-tree
void printTree()
{
    int level;
    int *numNodes;
    int i,j,k;

    level = _getLevel(root);
    numNodes = (int *)malloc(sizeof(int) * (level));
    memset(numNodes, 0, level * sizeof(int));

    _getNumberOfNodes(root, numNodes, 0);

    BTreeNode ***nodePtr;
    nodePtr = (BTreeNode***)malloc(sizeof(BTreeNode**) * level);
    for (i = 0; i<level; i++) {
        nodePtr[i] = (BTreeNode**)malloc(sizeof(BTreeNode*) * numNodes[i]);
    }

    memset(numNodes, 0, level * sizeof(int));
    _mappingNodes(root, nodePtr, numNodes, 0);

    for (i = 0; i < level; i++) {   
        for (j = 0; j < numNodes[i]; j++) {
            printf("[");

            for (k = 0; k < nodePtr[i][j]->n; k++)
                printf("%d ", nodePtr[i][j]->keys[k]);

            printf("] ");
        }
        printf("\n");
    }

    for (i = 0; i<level; i++) {
        free(nodePtr[i]);
    }
    free(nodePtr);
}
