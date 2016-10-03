/************************ Recitation 4 ************************
Recitation 4 by Felix Benavides and Javier Faustino
Purpose:
    This program finds the word frequencies in a text file.
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

typedef struct nodeT{
    char *word;
    int frequency;
    struct nodeT *left;
    struct nodeT *right;
} nodeT;

nodeT *insertNode(nodeT *p, char string[]);
int findHighestFrequency(nodeT *p);
void printFrequency(nodeT *p, int i);
void printTree(nodeT *p);
void freeAll(nodeT *p);

int main(int argc, char *argv[])
{
    FILE *file;
    if(argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(-1);
    }
    file = fopen(argv[1], "r");
    if(file == NULL)
    {
        perror(argv[1]);
        exit(-1);
    }
    char *str;
    char c;
    int i;
    int j;
    int highest;
    nodeT *head = NULL;
    //initial allocation of string
    str = (char *)malloc(sizeof(char));
    //get file input character by character
    while(fscanf(file, "%c", &c) != EOF)
    {
        i = 0;              //get ready for new word
        j = 1;
        //get character by character until space or punctuation
        while(isspace(c) == 0 && ispunct(c) == 0)
        {
            //reallocate string according to characters
            str = (char *)realloc(str, j*sizeof(char));
            //turn uppercase letters in uppercase
            if(isupper(c) != 0)
                c = tolower(c);
            str[i] = c;         //store letter in string
            i++;                
            j++;
            if(fscanf(file, "%c", &c) == EOF)
                break;
        }
        //check if there is characters in string
        if(i > 0)
        {
            //insert null byte at end of string
            str[i] = '\0';
            //insert string into tree
            head = insertNode(head, str);
        }
    }
    //find highest frequency
    highest = findHighestFrequency(head);
    for(i = 1; i <= highest; i++)
    {
        //print frequencies from least to greatest
        printFrequency(head, i);
    }
    //free dynamically allocated string
    free(str);
    //free tree
    freeAll(head);
    fclose(file);           //close file
    return 0;
}

nodeT *insertNode(nodeT *p, char string[])
{
    nodeT *tmp;
    if(p == NULL)
    {
        //string was not found in tree, allocate new node
        tmp = (nodeT *)malloc(sizeof(nodeT));
        if(tmp == NULL)
        {
            printf("Not enough memory\n");
            exit(-1);
        }
        tmp->word = (char *)malloc(strlen(string) + 1);
        strcpy(tmp->word, string);
        tmp->frequency = 1;                 //new word
        tmp->left = NULL;
        tmp->right = NULL;
        return tmp;
    }
    if(strcmp(p->word, string) < 0)
        p->right = insertNode(p->right, string);    //new word is greater
    else if(strcmp(p->word, string) > 0)
        p->left = insertNode(p->left, string);      //new word is less
    else if(strcmp(p->word, string) == 0)
        p->frequency++;     //same word, update frequency at current node
    return p;
}

int findHighestFrequency(nodeT *p)
{
    int left;
    int right;
    if(p == NULL)
        return 0;
    left = findHighestFrequency(p->left);   //find highest in left subtree
    right = findHighestFrequency(p->right); //find highest in right subtree
    if(left > right)
    {
        if(left > p->frequency)
            return left;
        else
            return p->frequency;
    }
    else
    {
        if(right > p->frequency)
            return right;
        else
            return p->frequency;
    }
}

void printTree(nodeT *p)
{
    if(p == NULL)
        return;
    printTree(p->left);
    printf("%d %s\n", p->frequency, p->word);
    printTree(p->right);
}

void printFrequency(nodeT *p, int i)
{
    if(p == NULL)
        return;
    printFrequency(p->left, i);
    if(p->frequency == i)
        printf("%d %s\n", i, p->word);      //word has correct frequency
    printFrequency(p->right, i);
}

void freeAll(nodeT *p)
{
    if(p == NULL)
        return;
    freeAll(p->left);               //free left subtree
    freeAll(p->right);              //free right subtree
    free(p->word);              //free word within node
    free(p);                    //free entire node
}
