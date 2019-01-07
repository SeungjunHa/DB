#include <stdio.h>
#include <stdlib.h>
#include  <limits.h>
#include <string.h>
#include "BTREE.h"

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
        //parent->n-1이 자주 사용되기 때문에 position의 의미를 갖는 변수를 선언 해주었습니다.
        int pos = present->n-1;

            //만약 지금 넣으려는 node가 leaf node라면, 단순히 key값의 위치를 찾은 후 insert해줍니다.
            if(present->leaf == true)
                    {
                                //이 과정은 _insert
                                while(pos >= 0 && present->keys[pos] > k)
                                            {
                                                            present->keys[pos+1] = present->keys[pos];
                                                                        pos--;
                                                                                }
                                        //printf("find right? %d\n", pos);
                                        //position을 while을 통해 찾은 후 insert 과정을 해줍니다.
                                        //position이 0보다 큰 조건은 무한 루프 방지 안전성을 위해 추가해줬습니다.
                                        present->keys[pos+1] = k;
                                                present->n = present->n+1;
                                                    }
                else
                        {
                                    //만약 leaf node가 아니라면 일단 position의 위치를 찾아줍니다.
                                    while(pos >= 0 && present->keys[pos] > k)
                                                    pos--;

                                            //만약 들어가야 할 child node가 가득 차 있다면, child node를 split해줍니다.
                                            //그 후 split이 끝난 후 k값을 넣어줄 child node를 찾습니다. 이 때 밑의 split에서 언급한 right child node가 선택됩니다.
                                            //right child node에 k값을 입력해줍니다.
                                            //이 때 right child node는 max degree가 5인 경우 left child와 right child는 2개씩 쪼개져야 하지만 아직 1개만 가진 상태입니다.
                                            //따라서 simple_insert 수행까지 해준다면 site의 수행과정과 동일하게 이루어집니다.
                                            if(present->C[pos+1]->n == t)
                                                        {
                                                                        simple_split(pos+1, present->C[pos+1], present);
                                                                                    if(present->keys[pos+1] < k)
                                                                                                        pos++;
                                                                                            }
                                                    simple_insert(present->C[pos+1],k);
                                                        }
}
void _insert(BTreeNode* present, int k, BTreeNode* parent)
{
        if(present->n == t)
                {
                            //만약 present가 full이라면 새로운 parent를 만들어 줍니다.
                            //새로운 parent는 정말로 처음 만들어졌기 때문에(즉 height가 n인 경우에서 3으로 증가하는 case)
                            //new->C[0]에 present를 연결해줍니다. 그 후 present를 child node라 생각하고, new를 parent node라 생각하고 split을 시행하여줍니다.
                            //그 후 입력 값과 트리의 가장 높은 부분에 존재하는 값과 비교하여 크다면 오른쪽 flow를, 작다면 왼쪽 flow를 타게 해주어 simple_insert를 재귀적으로 시행해줍니다.
                            BTreeNode *new = _createNode(false);
                                    new->C[0] = present;
                                            simple_split(0, present, new);
                                                    int i=0;
                                                            if(new->keys[0] < k)
                                                                            i++;

                                                                    simple_insert(new->C[i], k);
                                                                            //함수에서 parent를 쓰지 않는 것 같지만 insertElement에서 꾸준히 root를 넘겨기 때문에 root값을 새로운 parent인 new로 바꿔주어야 합니다.
                                                                            root = new;

                                                                                    //밑의 주석은 부모 node에 split후 insert가 제대로 되었는지 확인하는 코드입니다.
                                                                                    /*printf("=============================\n");
                                                                                              for(int i=0;i<new->C[0]->n;i++)
                                                                                                          printf("%d ",new->C[0]->keys[i]);
                                                                                                                  printf("\n");
                                                                                                                          printf("%d \n", new->keys[0]);
                                                                                                                                  for(int i=0;i<new->C[1]->n;i++)
                                                                                                                                              printf("%d ",new->C[1]->keys[i]);
                                                                                                                                                      printf("\n");
                                                                                                                                                              printf("=============================\n");
                                                                                                                                                                      */
                                                                                }
            else
                    {
                                //현재 넣으려는 present가 full이지 않은 경우는 
                                //단순히 insert연산만 해주면 되기 때문에 아무 추가 과정을 거칠 필요가 없습니다.
                                simple_insert(present, k);
                                    }
}

void simple_split(int index, BTreeNode *child, BTreeNode* parent)
{
        //site에서 구현된 바로 t가 4인 경우(max degree가 5이며, 한 줄에 최대 4개까지 저장 가능)
        //split 할 때 앞의 2개가 left child node로, 뒤의 2개가 right child node로 변했으며 가운데 값이 parent로 이동하였습니다.
        //따라서 mid는 t/2로 지정하였습니다.
        int mid = t/2;

            //새롭게 right child가 될 node를 설정해주었으며, 그것이 leaf노드인지 아닌지는 기존의 child node의 값을 따라 갔습니다.
            //즉 기존의 child node가 true이면 끝단에서 split하는 경우입니다.
            //또한 새로운 right child node의 값은 mid-1개로 설정해줍니다.
            //t가 4인경우 new_r->n의 값은 2가 아닌 1이 되는데 이 이뉴는 simple_split후 simple_insert에서 재귀적으로 right_child에 값을 넣어주기 때문입니다.
            //따라서 여기서 new_r->n의 값을 mid가 아닌 mid-1로 설정해줘야 segmentation fault가 뜨지 않습니다.
            BTreeNode *new_r = _createNode(child->leaf);
                new_r->n = mid-1;

                    //child의 3,4번째 값을 new_r의 0,1번째에 복사하는 과정입니다.
                    for(int i=0;i<mid;i++){
                                new_r->keys[i] = child->keys[i+mid+1];
                                    }

                        //만약 child->leaf 값이 거짓이라면 즉, leaf node가 아니라면 child가 child pointer node를 갖고 있기 때문에 그 값을 새로운 right child에 복사해줍니다.
                        //만약 mid가 2라면, new_r의 0,1번째에 child의 3,4번째 자식 포인터 노드를 복사해줍니다.
                        if(child->leaf == false)
                                {
                                            for(int i=0;i<mid+1;i++)
                                                            new_r->C[i] = child->C[i+mid+1];
                                                }

                            //child의 절반의 갯수가 새로운 right child인 new_r로 이동을 완료했기 때문에 child->n의 갯수를 mid로 줄여줍니다.
                            child->n = mid;

                                //새로운 new_r이 생겼기 떄문에 기존의 넘겨받은 index의 값 즉, 2번째 값이였다면 3번째까지 기존의 parent의 child node들을 모두 한 칸씩 뒤로 당겨줍니다.
                                //이 과정은 새로운 new_r이 child node로 parent에 들어갈 자리를 마련하는 과정입니다.
                                for(int i=parent->n; i>=index+1;i--)
                                            parent->C[i+1] = parent->C[i];

                                    //위 과정을 완료하면 index로 넘겨받은 값이 2 즉, 2번쨰 child를 split할 때 new_r은 3번쨰 자리에 들어가야 하기 때문에 index+1번째에 넣어줍니다.
                                    parent->C[index+1] = new_r;

                                        //이 과정은 child에서 올라갈 중앙 값의 자리를 마련해주는 과정입니다.
                                        //즉, child에 1,2,3,4가 존재했다면 3이 올라갑니다.
                                        //root node부터 split->insert 과정을 반복해 오기 때문에 부모 node에 빈 자리가 있음은 보장되어 있습니다.
                                        for(int i=parent->n-1; i>=index; i--)
                                                    parent->keys[i+1] = parent->keys[i];

                                            //index로 2번째 값을 수정하엿다면 2번째 자리에 mid값을 넣어주면 됩니다.
                                            parent->keys[index] = child->keys[mid];

                                                //parent의 key값을 1증가시켜줍니다.
                                                parent->n = parent->n+1;
}

BTreeNode * _splitChild(BTreeNode* present, BTreeNode* parent)
{
        /*
                splitChild는 insert연산에서 present가 가득찬 경우 split후 
                        parent에게 left child node와 right child로 나눠줍니다.
                                그 후 parent의 child pointer node를 올바르게 수정해주는 역할입니다.
                                        따라서 딱히 return형을 적지않고, void로 직접 함수내에서 바꿔주었으며
                                                (이미 parent를 인자로 넘겨주었기 때문에 parent의 child pointer node 정보가 해당 함수에 존재함)
                                                        또한 몇 번째 child node를 바꿔주려는지 중복계산을 하지 않기 위하여 index라는
                                                                present node가 parent의 몇 번째 child node인지를 저장하고 있는 변수를 추가해줬습니다.
                                                                        (제 구현상 index 번호를 넘겨주지 않으면 해당 함수 안에서 새롭게 또 구해야한다 생각했습니다.)
                                                                            */
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
