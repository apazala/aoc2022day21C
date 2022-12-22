#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

typedef struct TreeOp
{
    int64_t n;
    char op;

    struct TreeOp *parent;
    struct TreeOp *left;
    struct TreeOp *right;
} TreeOp;

TreeOp *newTreeOp()
{
    return (TreeOp *)calloc(1, sizeof(TreeOp)); // assume success!
}

int64_t shout(TreeOp *tp)
{
    switch (tp->op)
    {
    case 's':
        return tp->n;
    case '+':
        return shout(tp->left) + shout(tp->right);
    case '*':
        return shout(tp->left) * shout(tp->right);
    case '-':
        return shout(tp->left) - shout(tp->right);
    case '/':
        return shout(tp->left) / shout(tp->right);
    }
    fprintf(stderr, "Unimlemented operation!\n");
    return 0;
}

#define TRIE_NUMCHILDS 26
typedef struct MonkeyTrie
{
    TreeOp *monkeyOp;
    struct MonkeyTrie **childs;
} MonkeyTrie;

MonkeyTrie *newTrie()
{
    return (MonkeyTrie *)calloc(1, sizeof(MonkeyTrie)); // assume success!
}

TreeOp *getInsertIfAbsent(MonkeyTrie *trie, const char *name)
{
    int ind;

    if (*name == '\0') // no chars remaining
    {
        if (trie->monkeyOp == NULL)
        {
            trie->monkeyOp = newTreeOp();
        }
        return trie->monkeyOp;
    }

    if (trie->childs == NULL)
    {
        trie->childs = (MonkeyTrie **)calloc(TRIE_NUMCHILDS, sizeof(MonkeyTrie *));
    }

    ind = (*name) - 'a';
    if (trie->childs[ind] == NULL)
    {
        trie->childs[ind] = newTrie();
    }

    return getInsertIfAbsent(trie->childs[ind], name + 1);
}

TreeOp *getIfExist(const MonkeyTrie *curr, const char *name)
{
    while (curr && *name)
    {
        curr = (curr->childs ? curr->childs[(*name) - 'a'] : NULL);
        name++;
    }
    return (curr ? curr->monkeyOp : NULL);
}

// This should be done in dynamic memory too!
TreeOp *pathToHuman[128];
int pathCount;

int loadInputFile();
int nextLine(char *dst);

char line[200];

int main()
{
    if (loadInputFile() < 0)
    {
        fprintf(stderr, "Can't open: input.txt\n");
        return 1;
    }

    MonkeyTrie *monkeyDict = newTrie();
    char *token;
    TreeOp *parent, *left, *right;
    while (nextLine(line))
    {
        token = strtok(line, ":");
        parent = getInsertIfAbsent(monkeyDict, token);

        token = strtok(NULL, " ");
        if (*token >= '0' && *token <= '9')
        {
            parent->n = strtol(token, NULL, 10);
            parent->op = 's';
        }
        else
        {
            left = getInsertIfAbsent(monkeyDict, token);

            token = strtok(NULL, " ");
            parent->op = *token;

            token = strtok(NULL, " ");
            right = getInsertIfAbsent(monkeyDict, token);

            left->parent = parent;
            right->parent = parent;
            parent->left = left;
            parent->right = right;
        }
    }

    // part one
    TreeOp *root = getIfExist(monkeyDict, "root");
    printf("part one: %" PRId64 "\n", shout(root));

    // part two
    TreeOp *human = getIfExist(monkeyDict, "humn");
    for (TreeOp *curr = human; curr != root; curr = curr->parent)
        pathToHuman[pathCount++] = curr;

    int64_t target = (pathToHuman[pathCount-1]==root->left? shout(root->right): shout(root->left));
            
    for (int i = pathCount - 2; i >= 0; i--)
    {
        if (pathToHuman[i] == pathToHuman[i + 1]->left)
        {
            int64_t rightValue = shout(pathToHuman[i + 1]->right);
            switch (pathToHuman[i + 1]->op)
            {
            case '+':
                target -= rightValue;
                break;
            case '-':
                target += rightValue;
                break;
            case '*':
                target /= rightValue;
                break;
            case '/':
                target *= rightValue;
                break;
            }
        }
        else
        {
            int64_t leftValue = shout(pathToHuman[i + 1]->left);
            switch (pathToHuman[i + 1]->op)
            {
            case '+':
                target -= leftValue;
                break;
            case '-':
                target = leftValue - target;
                break;
            case '*':
                target /= leftValue;
                break;
            case '/':
                target = leftValue / target;
                break;
            }
        }
    }
    

    printf("part two: %" PRId64 "\n", target);
}

#define BUFSIZE 35000

char fileBuf[BUFSIZE];
char *pbuf = fileBuf;
char *pbufend = fileBuf;

int loadInputFile()
{
    FILE *pf = fopen("input.txt", "r");
    if (!pf)
        return -1;
    pbuf = fileBuf;
    pbufend = fileBuf + fread(fileBuf, 1, BUFSIZE, pf);
    fclose(pf);

    return 0;
}

int nextLine(char *dst)
{
    char c;
    char *orig = dst;

    // read line:
    while (pbuf < pbufend && (c = *(pbuf++)) != '\n')
    {
        *(dst++) = c;
    }
    *dst = '\0';
    // return line length
    return dst - orig;
}