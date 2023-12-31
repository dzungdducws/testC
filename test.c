#include <stdio.h>
#include <stdlib.h>

struct TreeNode
{
    struct TreeNode *pParent;
    struct TreeNode *pChild;
    struct TreeNode *pNext;
    struct TreeNode *pPrev;
    int data; // Dữ liệu của nút
};

// Hàm tạo một nút mới
struct TreeNode *createNode(int data)
{
    struct TreeNode *newNode = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    newNode->pParent = NULL;
    newNode->pChild = NULL;
    newNode->pNext = NULL;
    newNode->pPrev = NULL;
    newNode->data = data;
    return newNode;
}

// Hàm thêm nút con vào một nút cha
void addChild(struct TreeNode *pParent, struct TreeNode *pChild)
{
    if (pParent == NULL || pChild == NULL)
    {
        return;
    }

    pChild->pParent = pParent;
    pChild->pNext = pParent->pChild;
    if (pParent->pChild != NULL)
    {
        pParent->pChild->pPrev = pChild;
    }
    pParent->pChild = pChild;
}

// Hàm thêm nút anh em vào sau một nút
void addSibling(struct TreeNode *pNode, struct TreeNode *pSibling)
{
    if (pNode == NULL || pSibling == NULL)
    {
        return;
    }

    pSibling->pParent = pNode->pParent;
    pSibling->pNext = pNode->pNext;
    pSibling->pPrev = pNode;
    if (pNode->pNext != NULL)
    {
        pNode->pNext->pPrev = pSibling;
    }
    pNode->pNext = pSibling;
}

// Hàm in cây theo chiều xuôi và chiều ngược
void printTree(struct TreeNode *pNode, int depth)
{
    if (pNode == NULL)
    {
        return;
    }

    // In dữ liệu của nút
    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }
    printf("%d\n", pNode->data);

    // Gọi đệ quy cho nút con và nút anh em
    printTree(pNode->pChild, depth + 1);
    printTree(pNode->pNext, depth);
}

int main()
{
    // Tạo các nút
    struct TreeNode *root = createNode(1);
    struct TreeNode *child1 = createNode(2);
    struct TreeNode *child2 = createNode(3);
    struct TreeNode *sibling1 = createNode(4);
    struct TreeNode *sibling2 = createNode(5);
    struct TreeNode *grandchild1 = createNode(6);

    // Thêm nút con và nút anh em
    addChild(child1, grandchild1);
    addSibling(child1, sibling1);
    addSibling(child1, sibling2);
    addChild(root, child1);
    addChild(root, child2);


    // In cây theo chiều xuôi
    printf("Tree (Forward):\n");
    printTree(root, 0);

    free(root);
    free(child1);
    free(child2);
    free(sibling1);
    free(sibling2);

    return 0;
}
