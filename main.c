#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

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

int binaryToDecimal(const char *binary)
{
    int length = strlen(binary);
    int decimal = 0;

    for (int i = length - 1, j = 0; i >= 0; i--, j++)
    {
        if (binary[i] == '1')
        {
            decimal += (int)pow(2, j);
        }
        else if (binary[i] != '0')
        {
            // Handle invalid characters in the binary string
            printf("Invalid character '%c' in binary string.\n", binary[i]);
            return -1; // Error code for invalid input
        }
    }

    return decimal;
}

char *fileTime(char charAT17, char charAT16)
{
    char *bin = (char *)malloc(17);
    if (bin == NULL)
    {
        return NULL;
    }

    int count = 0;
    for (int i = 7; i >= 0; i--)
    {
        bin[count] = ((charAT17 >> i) & 1) ? '1' : '0';
        count++;
    }

    for (int i = 7; i >= 0; i--)
    {
        bin[count] = ((charAT16 >> i) & 1) ? '1' : '0';
        count++;
    }

    bin[count] = '\0';

    char *h = (char *)malloc(6);

    for (int i = 0; i < 5; i++)
    {
        h[i] = bin[i];
    }

    char *m = (char *)malloc(7);

    for (int i = 0; i < 6; i++)
    {
        m[i] = bin[i + 5];
    }

    char *s = (char *)malloc(6);

    for (int i = 0; i < 5; i++)
    {
        s[i] = bin[i + 11];
    }

    char *res = (char *)malloc(10);
    sprintf(res, "%dh%dm%ds", binaryToDecimal(h), binaryToDecimal(m), binaryToDecimal(s));
    return res;
}

char *fileDate(char charAT19, char charAT18)
{
    char *bin = (char *)malloc(17);
    if (bin == NULL)
    {
        return NULL;
    }

    int count = 0;
    for (int i = 7; i >= 0; i--)
    {
        bin[count] = ((charAT19 >> i) & 1) ? '1' : '0';
        count++;
    }

    for (int i = 7; i >= 0; i--)
    {
        bin[count] = ((charAT18 >> i) & 1) ? '1' : '0';
        count++;
    }

    bin[count] = '\0';

    char *y = (char *)malloc(8);

    for (int i = 0; i < 7; i++)
    {
        y[i] = bin[i];
    }

    char *m = (char *)malloc(5);

    for (int i = 0; i < 4; i++)
    {
        m[i] = bin[i + 7];
    }

    char *s = (char *)malloc(6);

    for (int i = 0; i < 5; i++)
    {
        s[i] = bin[i + 11];
    }

    char *res = (char *)malloc(15);
    sprintf(res, "y:%d;m:%d;d:%d", 1980 + binaryToDecimal(y), binaryToDecimal(m), binaryToDecimal(s));
    return res;
}

int indexOfFirstFileAllocTablie(FILE *file)
{
    int numOfFAT;
    int numOfBlockFat;
    char tmp[2];

    fseek(file, 0x10, SEEK_SET);
    fgets(tmp, 2, file);
    numOfFAT = (int)tmp[0];
    fseek(file, 0x16, SEEK_SET);
    fgets(tmp, 2, file);
    numOfBlockFat = (int)tmp[0];
    return (1 + numOfFAT * numOfBlockFat) * 512;
}

void printAll(FILE *file)
{
    int jump = 0;
    int _indexOfFirstFileAllocTablie = indexOfFirstFileAllocTablie(file);
    while (1)
    {
        fseek(file, _indexOfFirstFileAllocTablie + jump, SEEK_SET);
        char tmp[32];
        fgets(tmp, 32, file);
        if (tmp[26] != 0)
        {
            char name[8];
            strncpy(name, tmp, 7);
            printf("Time create:%s | ", fileTime(tmp[23], tmp[22]));
            printf("Date create:%s | ", fileDate(tmp[25], tmp[24]));
            if ((int)tmp[8] == 32)
            {
                printf("%s\n", trimWhitespace(name));
            }
            else
            {
                char *nameExtension = (char *)malloc(4);
                for (int i = 0; i < 3; i++)
                {
                    nameExtension[i] = tmp[8 + i];
                }

                printf("%s.%s\n", trimWhitespace(name), nameExtension);
            }
        }
        if (tmp[0] == 0)
            break;
        else
            jump += 32;
    }
}

int main()
{
    FILE *file = fopen("D://floppy.img", "r");
    // printf("%x", indexOfFirstFileAllocTablie(file));
    printAll(file);
    return 0;
}