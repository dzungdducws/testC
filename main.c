#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#define entrysize 32

static uint8_t numOfFAT;
static uint8_t numOfBlockFat;
static uint8_t FATtentry;
typedef struct
{
    uint16_t sec : 5;
    uint16_t min : 6;
    uint16_t hour : 5;
} Time_t;

typedef struct
{
    uint16_t day : 5;
    uint16_t month : 4;
    uint16_t year : 7;
} Date_t;

struct TreeNode
{
    uint8_t Name[8];
    uint8_t NameEx[3];
    uint8_t Attributes;
    uint16_t cluster;
    uint16_t CreatedTime;
    uint16_t CreatedDate;
    uint32_t Size;

    struct TreeNode *pParent;
    struct TreeNode *pChild;
    struct TreeNode *pNext;
    struct TreeNode *pPrev;
};

// Hàm tạo một nút mới
struct TreeNode *createNewNode()
{
    struct TreeNode *newNode = (struct TreeNode *)malloc(sizeof(struct TreeNode));

    return newNode;
}

struct TreeNode *createNode(uint8_t *Name, uint8_t *NameEx, uint8_t Attributes, uint16_t cluster, uint16_t CreatedTime, uint16_t CreatedDate, uint32_t Size)
{
    struct TreeNode *newNode = (struct TreeNode *)malloc(sizeof(struct TreeNode));
    newNode->pParent = NULL;
    newNode->pChild = NULL;
    newNode->pNext = NULL;
    newNode->pPrev = NULL;

    memcpy(newNode->Name, Name, sizeof(newNode->Name));
    memcpy(newNode->NameEx, NameEx, sizeof(newNode->NameEx));
    newNode->Attributes = Attributes;
    newNode->cluster = cluster;
    newNode->CreatedTime = CreatedTime;
    newNode->CreatedDate = CreatedDate;
    newNode->Size = Size;

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

char *trimWhitespace(char *str)
{
    int length = strlen(str);

    int startIndex = 0;
    while (isspace(str[startIndex]) && startIndex < length)
    {
        startIndex++;
    }

    int endIndex = length - 1;
    while (isspace(str[endIndex]) && endIndex >= 0)
    {
        endIndex--;
    }

    int newLength = endIndex - startIndex + 1;

    char *result = (char *)malloc((newLength + 1) * sizeof(char));
    strncpy(result, str + startIndex, newLength);
    result[newLength] = '\0';

    return result;
}

int calculateMaxDepth(struct TreeNode *pNode, int depth)
{
    if (pNode == NULL)
    {
        return depth;
    }

    int childDepth = calculateMaxDepth(pNode->pChild, depth + 1);
    int siblingDepth = calculateMaxDepth(pNode->pNext, depth);

    return (childDepth > siblingDepth) ? childDepth : siblingDepth;
}

void printTree(struct TreeNode *pNode, int depth, int treeDepth)
{
    if (pNode == NULL)
    {
        return;
    }

    if (pNode->CreatedTime != NULL)
    {
        int count = 0;

        if (pNode->Size != 0xff00ff)
        {
            for (int i = 0; i < depth - 1; i++)
            {
                printf("->");
            }

            for (int i = 0; i < treeDepth - (depth - 1); i++)
            {
                printf("  ");
            }

            if (isalpha(pNode->NameEx[0]))
                printf("%8s.%3s | ", trimWhitespace(pNode->Name), pNode->NameEx);
            else
                printf("%12s | ", trimWhitespace(pNode->Name));
            printf("%4x | ", pNode->Attributes);
            printf("%4x | ", pNode->cluster);
            Time_t time;
            *(uint16_t *)&time = pNode->CreatedTime;
            printf("Time create:%2dh %2dm %2ds | ", time.hour, time.min, time.sec);
            Date_t date;
            *(uint16_t *)&date = pNode->CreatedDate;
            printf("Date create:%4d-%2d-%2d | ", date.year + 1980, date.month, date.day);
            printf("%x\n", pNode->Size);
        }
    }
    // Gọi đệ quy cho nút con và nút anh em
    printTree(pNode->pChild, depth + 1, treeDepth);
    printTree(pNode->pNext, depth, treeDepth);
}

void solve(FILE *file, int addr, struct TreeNode **node)
{
    int curAddr = addr;
    while (1)
    {
        fseek(file, curAddr, SEEK_SET);
        char tmp[entrysize];
        fread(tmp, sizeof(char), entrysize, file);
        if ((uint8_t)tmp[0x00] == 0x0 || (uint8_t)tmp[0x00] == 0xff)
            break;

        char name[8];
        strncpy(name, tmp, 7);
        char nameExtension[3] = "";
        int j;
        if (tmp[8] != 32)
            for (j = 0; j < 3; j++)
            {
                nameExtension[j] = tmp[8 + j];
            }
        struct TreeNode *newNode = createNode(name, nameExtension, (uint8_t)tmp[0x0b],
                                              (uint16_t)(((unsigned char)tmp[0x1b] << 8) | (unsigned char)tmp[0x1a]),
                                              (uint16_t)(((unsigned char)tmp[0x17] << 8) | (unsigned char)tmp[0x16]),
                                              (uint16_t)(((unsigned char)tmp[0x19] << 8) | (unsigned char)tmp[0x18]),
                                              (uint32_t)(((unsigned char)tmp[0x1f] << 16) | (unsigned char)tmp[0x1e] | (unsigned char)tmp[0x1d] | (unsigned char)tmp[0x1c]));
        if ((uint8_t)tmp[0x0b] == 0x10 && (uint8_t)tmp[0x00] != 0x2E && (uint8_t)tmp[0x0b] != 0x20)
        {
            solve(file, 512 * ((1 + numOfFAT * numOfBlockFat) + FATtentry * 32 / 512 + (uint16_t)(((unsigned char)tmp[0x1b] << 8) | (unsigned char)tmp[0x1a]) - 2), &newNode);
        }
        if ((uint8_t)tmp[0x00] != 0x2E && (uint8_t)tmp[0x0b] != 0x20)
            addChild(*node, newNode);
        curAddr = curAddr + 32;
    }
}

int main()
{
    FILE *file = fopen("./floppy.img", "r");

    char _tmp[2];

    fseek(file, 0x10, SEEK_SET);
    fgets(_tmp, 2, file);
    numOfFAT = (int)_tmp[0];

    fseek(file, 0x16, SEEK_SET);
    fgets(_tmp, 2, file);
    numOfBlockFat = (int)_tmp[0];

    fseek(file, 0x11, SEEK_SET);
    fgets(_tmp, 2, file);
    FATtentry = _tmp[0];
    struct TreeNode *node = createNewNode();
    int addr = (1 + numOfFAT * numOfBlockFat) * 512;

    solve(file, addr, &node);
    int treeDepth = calculateMaxDepth(node, 0);

    printTree(node, 0, treeDepth);
    return 0;
}
