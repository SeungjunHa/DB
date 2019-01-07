#ifndef BTREE_H_
#define BTREE_H_
#include <stdbool.h>

typedef struct BTreeNode {
        int *keys;  // An array of keys
            struct BTreeNode **C; // An array of child pointers
                int n;     // Current number of keys
                    bool leaf; // Is true when node is leaf. Otherwise false
} BTreeNode;


BTreeNode *root; // Pointer to root node
int t;  // B-tree degree

void BTreeInit(int _t); // Initializes tree as empty
BTreeNode* _createNode(bool _leaf);

void traverse(); // A function to traverse all nodes in a subtree rooted with this node
void _traverse(BTreeNode* present);

BTreeNode* search(int k); // function to search a key in this tree
BTreeNode* _search(BTreeNode* present, int k);

// insert part
void insertElement(int k); // The main function that inserts a new key in this B-Tree
void _insert(BTreeNode* present, int k, BTreeNode* parent);
BTreeNode* _splitChild(BTreeNode* present, BTreeNode* parent);
//구현의 편이성을 위하여 기존의 _splitChild는 사용하지 않았습니다.
void simple_insert(BTreeNode *present, int k); 
//제가 정의한 simple_insert함수입니다. 기존의 _insert함수에서 Full인지 아닌지를 체크하고 바로 적을 수 있지만 
//하위 leaf로 넘어갈 경우 그 leaf가 full이면 재귀적으로 split후 다시 insert를 해줘야하는 경우때문에 재귀를 편하게 하기 위하여 새로 구현했습니다.
void simple_split(int index, BTreeNode *present, BTreeNode *parent);
//기존의 _splitChild와 기능적으로는 동일하나 return을 따로 하지 않고 안에서 포인터 배열 연산을 직접 바꿔줘서 void형으로 구현하였습니다.
//또한 Index번호가 필요하여(index는 부모 노드에서 스플릿 하려는 하위 노드가 몇 번째에 있는지, present는 split하려는 노드, parent는 부모 노드입니다.

// print B-tree
int _getLevel(BTreeNode* present);
void _getNumberOfNodes(BTreeNode* present, int* numNodes, int level);
void _mappingNodes(BTreeNode* present, BTreeNode ***nodePtr, int* numNodes, int level);
void printTree();

#endif /* BTREE_H_ */
