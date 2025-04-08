#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define NAME 100
#define MAX_KEYS 2
#define MAX_DATA_ITEMS 3
#define MIN_DATA_ITEMS 1
#define ORDER 4
#define MAX_MEMBERS 4

typedef enum
{
    rent,
    utilities,
    groceries,
    transportation,
    entertainment
} expense_category;

const char *category_name[] = {
    "rent", "utilities", "groceries", "transportation", "entertainment"};
// data node of individual
typedef struct individual_node
{
    int user_id;
    char username[NAME];
    float income;

} individual;
// data node of family
typedef struct family_node
{
    int family_id;
    char family_name[NAME];
    individual *family_members[MAX_MEMBERS];
    float total_family_income; // calculated as sum of individual incomes
    float monthly_expense;     // calculated as sum of all expense by family members in a moth in all categories

} family;

// data node of expense
typedef struct expense_node
{
    int expense_id;
    int user_id;
    expense_category category; // one og rent,utility,stationary,leisure
    float expense_amount;
    int date_of_expense; // DDMM

} expense;
/*
// internal nodes containing the keys which will be the user id
typedef struct BplustreeNode_indiv
{
    int keys[MAX_KEYS];                                       // used to store sorted data //user id
    struct BplustreeNode_indiv *children[MAX_DATA_ITEMS + 1]; // used for branching non leaf
    struct Bplustreenode_indiv *next;                         // used to link leaf nodes for fast sequential accesss
    int nums_keys;                                            // number of valid keys
    bool isleaf;                                              // to indicate whether its a leaf or not

} Bplusindividual;
// common b plus tree node

typedef struct Bplustreenode_family
{
    int keys[MAX_KEYS]; // family id
    struct Bplustreenode_family *children[MAX_DATA_ITEMS + 1];
    struct Bplustreenode_family *next;
    int num_keys;
    bool isleaf;
} Bplusfamily;

typedef struct Bplustreenode_expense
{
    int keys[MAX_KEYS]; // expense id
    struct Bplustreenode_expense *children[MAX_DATA_ITEMS + 1];
    struct Bplustreenode_expense *next;
    int num_keys;
    bool isleaf;
} Bplusexpense;
*/
// common B plus tree node

typedef struct BPlusTreeNode
{
    int keys[ORDER - 1];   // Keys
    void *pointers[ORDER]; // Child pointers or data
    bool is_leaf;
    int num_keys; // Current keys count
} BPlusTreeNode;

typedef struct BplusTree
{
    BPlusTreeNode *root;
} BplusTree;

struct DataNode *
{
    individual *indidual_data[MAX_DATA_ITEMS];
}
individual *create_indiv_node(int id, char name[], float income)
{
    individual *newnode = (individual *)malloc(sizeof(individual));
    newnode->user_id = id;
    strcpy(newnode->username, name);
    newnode->income = income;
    return newnode;
}
family *create_family_node(int famid, char fam_name[], individual *fam_mem[], float total_income, float month_expense)
{
    family *newnode = (family *)malloc(sizeof(family));
    newnode->family_id = famid;
    strcpy(newnode->family_name, fam_name);
    newnode->monthly_expense = month_expense;
    newnode->total_family_income = total_income;
    for (int i = 0; i < MAX_MEMBERS; i++)
    {
        newnode->family_members[i] = NULL;
    }
    return newnode;
}

expense *create_expense_node(int id, int userid, expense_category category, float amount, int date)
{
    expense *newnode = (expense *)malloc(sizeof(expense));
    newnode->expense_id = id;
    newnode->category = category;
    newnode->expense_amount = amount;
    newnode->date_of_expense = date;
    return newnode;
}

BPlusTreeNode *createTreenode()
{
    BPlusTreeNode *newnode;
    newnode = (BPlusTreeNode *)malloc(sizeof(BPlusTreeNode));
    // newnode->keys=0;
    newnode->num_keys = 0;
    newnode->is_leaf = false;
    for (int i = 0; i < ORDER; i++)
    {
        newnode->pointers[i] = NULL;
    }
    return newnode;
}

/*
Bplusindividual *createindividualtree(bool isleaf)
{
    Bplusindividual *node = (Bplusindividual *)malloc(sizeof(Bplusindividual));
    node->isleaf = isleaf;
    node->nums_keys = 0;
    node->next = NULL;
    for (int i = 0; i <= MAX_DATA_ITEMS; i++)
    {
        node->children[i] = NULL;
    }
    return node;
}

Bplusfamily *createfamily(bool isleaf)
{
    Bplusfamily *node = (Bplusfamily *)malloc(sizeof(Bplusfamily));
    node->isleaf = isleaf;
    node->num_keys = 0;
    node->next = NULL;
    for (int i = 0; i <= MAX_DATA_ITEMS; i++)
    {
        node->children[i] = NULL;
    }
    return node;
}
Bplusexpense *createexpense(bool isleaf)
{
    Bplusexpense *node = (Bplusexpense *)malloc(sizeof(Bplusexpense));
    node->isleaf = isleaf;
    node->num_keys = 0;
    node->next = NULL;
    for (int i = 0; i <= MAX_DATA_ITEMS; i++)
    {
        node->children[i] = NULL;
    }
    return node;
}
*/

// ADD USER
// 1.Create an individual data node and add all the parameters
// 2.
// insert functions
int main()
{
    return 0;
}