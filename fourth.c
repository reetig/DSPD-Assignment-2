#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NAME 100
#define MAX_DATA_ITEMS 5
#define MIN_DATA_ITEMS 2
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

typedef struct individual_node
{
    int user_id;
    char user_name[NAME];
    float user_income;

} individual;
typedef struct expense_node
{
    int expense_id;
    int user_id;
    expense_category category;
    float expense_amount;
    int date_of_expense;
} expense;
typedef struct family_node
{
    int family_id;
    char family_name[NAME];
    individual *family_members[MAX_MEMBERS]; // pointing towards a usernode
    float total_family_income;               // calculated as sum of individual incomes
    float monthly_expense;                   // calculated as sum of all expense by family members in a moth in all categories
    // linked list of Bplus tree expense nodes according to months so 12

} family;

typedef struct B_plus_tree_node
{
    int keys[ORDER - 1];   // Keys
    pointer_type children; // Child pointers or data
    bool is_leaf;
    int active_keys; // count active keys
    struct B_plus_tree_node *parent;
} B_plus_node;

typedef struct Monthly_Expense_Node
{
    int month;                         // 1 to 12 (Jan to Dec)
    B_plus_node *expense_tree;         // Root of the B+ tree for this month
    struct Monthly_Expense_Node *next; // Pointer to the next month’s expense tree
} monthly_expense_node;

// put it into a structure and these will be doubly LL

typedef struct family_datanode
{
    // array of family structure

    family family_array[MAX_DATA_ITEMS];
    int data_items;
    struct family_datanode *next;
    struct family_datanode *prev;

} family_datanode;
typedef union pointer_type_tag
{
    struct B_plus_node *tree_node_ptr[MAX_DATA_ITEMS];
    struct family_datanode *dataptr[MAX_DATA_ITEMS];
} pointer_type;
B_plus_node *family_tree_pointer = NULL;

B_plus_node *splitBPlusTreeNode(B_plus_node *root, int family_id)
{
    printf("split BPlusTreeNode\n");
    int i;
    B_plus_node *newNode = createTreeNode();
    newNode->active_keys = ORDER / 2;

    if (root->parent == NULL)
    {
        printf("splitting root");
        for (i = ORDER / 2 + 1; i <= ORDER; i++)
        {
            newNode->children.dataptr[i - ORDER / 2 - 1] = root->children.dataptr[i];
            newNode->keys[i - ORDER / 2 - 1] = root->keys[i];
            root->children.dataptr[i] = NULL;
        }
        printf("%d", i);
        newNode->is_leaf = 1;
        newNode->children.dataptr[i - ORDER / 2 - 1] = root->children.dataptr[i];

        newNode->active_keys = root->active_keys = ORDER / 2;
        root->children.dataptr[ORDER / 2]->next = newNode->children.dataptr[0];
        newNode->children.dataptr[0]->prev = root->children.dataptr[ORDER / 2];

        B_plus_node *newRoot = createTreeNode();
        newNode->parent = root->parent = newRoot;
        newRoot->keys[0] = root->keys[ORDER / 2];
        // printf ("%d:%d ", newRoot->key[0].Hour, newRoot->key[0].Min);
        newRoot->is_leaf = false;
        newRoot->children.tree_node_ptr[0] = root;
        newRoot->children.tree_node_ptr[1] = newNode;
        newRoot->parent = NULL;
        newRoot->active_keys = 1;
        printTree(newRoot);
    }

    else if (root->is_leaf)
    {
        printf("splitting leaf");
        // copies right half of data of the root into the new node
        for (i = ORDER / 2; i < ORDER; i++)
        {
            newNode->children.dataptr[i - ORDER / 2] = root->children.dataptr[i];
            newNode->keys[i - ORDER / 2 - 1] = root->keys[i];
            root->children.dataptr[i] = NULL;
        }
        printf("%d", i);
        newNode->is_leaf = 1;
        // newNode->children.dataptr[i - ORDER / 2] = root->children.dataptr[i]; // but wont it be out of bound since i=4

        newNode->active_keys = root->active_keys = ORDER / 2;
        root->children.dataptr[ORDER / 2]->next = newNode->children.dataptr[0];
        newNode->children.dataptr[0]->prev = root->children.dataptr[ORDER / 2];

        B_plus_node *parent = root->parent;
        printf(" %d", parent->active_keys);
        i = parent->active_keys - 1;
        printf("%d", parent->active_keys);
        // shift keys in parents
        // finds correct poition to insert new key in parent and shifts the keys
        // parent node keys are not full
        while (i >= 0 && family_id < parent->keys[i])
        {
            parent->keys[i + 1] = parent->keys[i];
            parent->children.tree_node_ptr[i + 2] = parent->children.tree_node_ptr[i + 1]; // doubt since out of bound
            i--;
        }
        i++;
        printf("%d", i);
        parent->keys[i] = root->keys[ORDER / 2];
        parent->children.tree_node_ptr[i] = root;
        parent->children.tree_node_ptr[i + 1] = newNode;
        parent->active_keys++;
        printf("\n");
        // printing the keys
        for (i = 0; i < parent->active_keys; i++)
        {
            printf("%d:%d ", parent->keys[i], parent->keys[i]);
        }
        printf("\n");
        for (i = 0; i <= parent->active_keys; i++)
        {
            if (parent->children.tree_node_ptr[i] != NULL)
            {
                B_plus_node *traversalnode = parent->children.tree_node_ptr[i];
                for (int j = 0; j < traversalnode->active_keys; j++)
                {
                    printf("%d:%d ", traversalnode->keys[j]);
                }
                for (int j = 0; j <= traversalnode->active_keys; j++)
                {
                    printf("Now print respective dataNodes: \n");
                    family_datanode *dataNodeTraversal = traversalnode->children.dataptr[j];
                    if (dataNodeTraversal != NULL)
                    {
                        printf("flighttraversal\n");
                        Flight *flightTraversal = dataNodeTraversal->lptr;
                        while (flightTraversal != NULL)
                        {
                            printf("%d:%d ", flightTraversal->departureTime.Hour, flightTraversal->departureTime.Min);
                            flightTraversal = flightTraversal->next;
                        }
                        printf("\n");
                    }
                }
                printf("\n");
            }
        }
    }
    else
    {
        B_plus_node *newNode = createTreeNode();
        newNode->active_keys = ORDER / 2;
        newNode->is_leaf = 1;
        newNode->parent = root->parent;
        B_plus_node *parent = root->parent;

        for (i = ORDER / 2 + 1; i <= ORDER; i++)
        {
            newNode->children.nodeptr[i - ORDER / 2 - 1] = root->children.nodeptr[i];
            newNode->key[i - ORDER / 2 - 1] = root->key[i];
            root->children.nodeptr[i] = NULL;
        }
        newNode->children.nodeptr[i - ORDER / 2 - 1] = root->children.nodeptr[i];

        while (i >= 0 && maxTime(parent->key[i], root->key[0]) < 0)
        {
            parent->key[i + 1] = parent->key[i];
            parent->children.nodeptr[i + 2] = parent->children.nodeptr[i + 1];
            i--;
        }

        i++;
        printf("%d", i);
        parent->key[i] = root->key[ORDER / 2];
        parent->children.nodeptr[i] = root;
        parent->children.nodeptr[i + 1] = newNode;
    }
    if (root->parent->activeKeys == ORDER + 1)
    {
        printf("here comes the recursion");
        root->parent = splitBPlusTreeNode(root->parent);
    }
    return root->parent;
}