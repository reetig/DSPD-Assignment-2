#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
    void *pointers[ORDER]; // Child pointers or data
    bool is_leaf;
    int active_keys; // count active keys
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
B_plus_node *family_tree_pointer = NULL;




B_plus_node *insertInbPlusTree(B_plus_node *root, int family_id, family family_node)
{
    family_datanode *newNode = (family_datanode *)malloc(sizeof(newNode));
    if (root == NULL)
    {
        root = createTreeNode();
        root->is_leaf = true;
        root->active_keys = 1;
        root->keys[0] = family_id;
        for (int i = 0; i <= ORDER + 1; i++)
        {
            root->pointers[i] = NULL;
        }
        root->pointers[0] = newNode;
        newNode->family_array[0] = family_node;
    }

    else if (root->is_leaf)
    {
        // root is a leaf , so we chech the value in keys of the root and then accordingly go to the leaf data node
        int i = 0;

        // Finding appropriate position
        while (i < root->active_keys && family_id < root->keys[i])
        {
            i++;
        }

        if (root->pointers[i] == NULL)
        {
            root->pointers[i] = newNode;
            root->active_keys++;
            newNode->family_array[0] = family_node;
            newNode->data_items = 1;
            /*check this pointer business
            newNode->next=NULL;
            newNode->prev=
            */
        }

        root->children.dataptr[i]->lptr = insertInPlaneList(root->children.dataptr[i]->lptr, newNode);

        Flight *ptr = root->children.dataptr[i]->lptr;
        DataNode *temp = root->children.dataptr[0];
        root->children.dataptr[i]->size++;

        if (root->children.dataptr[i]->size == order + 1) // DataNode is FULL
        {
            printf("Splitting\n");
            root = splitDataNode(root, i);
            printf("returned");
            // Split Data BPlusTreeNode
        }
        while (temp != NULL)
        {
            ptr = temp->lptr;
            printf("size: %d  \n", temp->size);
            while (ptr != NULL)
            {
                printf("%d:%d ", ptr->departureTime.Hour, ptr->departureTime.Min);
                ptr = ptr->next;
            }
            temp = temp->next;
            printf("\n");
        }
        printf("******************Printing the tree*********************");
        printTree(root);
    }

    else
    {
        int i = 0;
        while (i < root->activeKeys && maxTime(newNode->departureTime, root->key[i]) == (-1))
            i++;

        if (i == root->activeKeys)
            root->children.nodeptr[i] = insertInbPlusTree(root->children.nodeptr[i], newNode);
    }
    return root;
}
// Helper function to create a new node
B_plus_node *create_node(bool is_leaf)
{
    B_plus_node *new_node = (B_plus_node *)malloc(sizeof(B_plus_node));
    if (!new_node)
    {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    new_node->is_leaf = is_leaf;
    new_node->active_keys = 0;
    // new_node->next = NULL;
    memset(new_node->keys, 0, sizeof(new_node->keys));
    memset(new_node->pointers, 0, sizeof(new_node->pointers));
    return new_node;
}

// Function to find position to insert a key in a sorted array
int find_insert_position(int keys[], int num_keys, int key)
{
    int i;
    int index = 0;
    for (i = 0; i < num_keys; i++)
    {
        if (keys[i] > key)
            index = i;
    }
    return index;
}
void insert_into_leaf(B_plus_node *leaf, int key, void *data)
{
    int pos = find_insert_position(leaf->keys, leaf->active_keys, key);

    // Shift keys and pointers to make space
    for (int i = leaf->active_keys; i > pos; i--)
    {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->pointers[i] = leaf->pointers[i - 1];
    }

    // Insert new key and data pointer
    leaf->keys[pos] = key;
    leaf->pointers[pos] = data;
    leaf->active_keys++;
}
// Function to split a leaf node
B_plus_node *split_leaf(B_plus_node *leaf, int *new_key)
{
    int mid = (ORDER - 1) / 2; // Splitting at the middle

    // Create new right leaf
    B_plus_node *new_leaf = create_node(true);

    // Move half of the elements to the new leaf
    for (int i = mid, j = 0; i < ORDER - 1; i++, j++)
    {
        new_leaf->keys[j] = leaf->keys[i];
        new_leaf->pointers[j] = leaf->pointers[i];
    }

    new_leaf->active_keys = (ORDER - 1) - mid;
    leaf->active_keys = mid;

    // Maintain linked list of leaves
    // new_leaf->next = leaf->next;
    // leaf->next = new_leaf;

    *new_key = new_leaf->keys[0]; // First key of new leaf
    return new_leaf;
}

B_plus_node *insert_internal(B_plus_node *node, int key, void *data)
{
    if (node == NULL)
    {
        // Create a new root if the tree is empty
        B_plus_node *root = create_node(true);
        insert_into_leaf(root, key, data);
        //  return root;
    }

    else if (node->is_leaf)
    {
        if (node->active_keys < ORDER - 1)
        {
            insert_into_leaf(node, key, data);
            return NULL;
        }
        else
        {
            int new_key;
            B_plus_node *new_leaf = split_leaf(node, &new_key);
            if (key >= new_key)
            {
                insert_into_leaf(new_leaf, key, data);
            }
            else
            {
                insert_into_leaf(node, key, data);
            }
            return new_leaf;
        }
    }

    // Find the correct child to insert into
    int i;
    for (i = 0; i < node->active_keys; i++)
    {
        if (key < node->keys[i])
            break;
    }
    B_plus_node *split_child = insert_internal(node->pointers[i], key, data);

    if (split_child)
    {
        // Insert new key in the parent
        if (node->active_keys < ORDER - 1)
        {
            int pos = find_insert_position(node->keys, node->active_keys, split_child->keys[0]);
            for (int j = node->active_keys; j > pos; j--)
            {
                node->keys[j] = node->keys[j - 1];
                node->pointers[j + 1] = node->pointers[j];
            }
            node->keys[pos] = split_child->keys[0];
            node->pointers[pos + 1] = split_child;
            node->active_keys++;
            return NULL;
        }
        else
        {
            int new_key;
            B_plus_node *new_internal = split_leaf(node, &new_key);
            return new_internal;
        }
    }
    return NULL;
}
// Function to insert a key into B+ tree
B_plus_node *insert(int key, void *data, B_plus_node *root)
{
    B_plus_node *split_node = insert_internal(root, key, data);
    if (split_node)
    {
        // Create new root
        B_plus_node *new_root = create_node(false);
        new_root->keys[0] = split_node->keys[0];
        new_root->pointers[0] = root;
        new_root->pointers[1] = split_node;
        new_root->active_keys = 1;
        return new_root; // Return new root
    }
    return root; // Return updated root
}

// Function to print B+ Tree (for debugging)
void print_tree(B_plus_node *node)
{
    if (!node)
        return;
    printf("[ ");
    for (int i = 0; i < node->active_keys; i++)
    {
        printf("%d ", node->keys[i]);
    }
    printf("] -> ");
    if (node->is_leaf)
    {
        printf("LEAF\n");
    }
    else
    {
        printf("\n");
        for (int i = 0; i <= node->active_keys; i++)
        {
            print_tree(node->pointers[i]);
        }
    }
}
/*
void create_family(B_plus_node *family_tree_pointer, int family_id)
{
    // created a data node for it
    family *newfamily = (family *)malloc(sizeof(family));

    if (family_tree_pointer == NULL)
    {
        family_tree_pointer = (B_plus_node *)malloc(sizeof(B_plus_node));
        family_tree_pointer->is_leaf = true;
        family_tree_pointer->keys[0] = family_id;
        family_tree_pointer->active_keys = 1;
        family_tree_pointer->pointers[0] = newfamily;

        printf("B plus tree for family created or new root.\n");

        // insert a node with fam id and and two children with one having data nodes
    }
    else
    {
        // insert in an already existing node
        // case 1.insert at leaf
        // case 2. insert into leaf ,split the data node and put the value in the above B tree node
        // case 3.split the data node,borrow from sibling via parent and new root is made
        B_plus_node *cursor = family_tree_pointer;
        B_plus_node *parent = NULL;
        // searching for the possible position for the given key by doing the same procedure we did in search
        while (cursor->is_leaf == false)
        {
            parent = cursor;
            int idx;
            for (int i = 0; i < ORDER; i++)
            {
                if (cursor->keys[i] == family_id)
                {
                    idx = i;
                }
            }
            cursor = cursor->ptr2TreeOrData.ptr2Tree[idx];
        }

        // now cursor is the leaf node in which we'll insert the new key
        if (cursor->keys.size() < maxLeafNodeLimit)
        {

                If current leaf Node is not FULL, find the correct position for the new key and insert!
                */

/*
int i = std::upper_bound(cursor->keys.begin(), cursor->keys.end(), key) - cursor->keys.begin();
cursor->keys.push_back(key);
cursor->ptr2TreeOrData.dataPtr.push_back(filePtr);

if (i != cursor->keys.size() - 1)
{
for (int j = cursor->keys.size() - 1; j > i; j--)
{ // shifting the position for keys and datapointer
cursor->keys[j] = cursor->keys[j - 1];
cursor->ptr2TreeOrData.dataPtr[j] = cursor->ptr2TreeOrData.dataPtr[j - 1];
}

// since earlier step was just to inc. the size of vectors and making space, now we are simplying inserting
cursor->keys[i] = key;
cursor->ptr2TreeOrData.dataPtr[i] = filePtr;
}
cout << "Inserted successfully: " << key << endl;
}
}
*/
void create_family(int family_id, B_plus_node *family_ptr)
{
}
// Main function to test the insertion
int main()
{
    B_plus_node *root = create_node(true); // Initialize root properly

    root = insert(10, NULL, root);
    root = insert(20, NULL, root);
    root = insert(30, NULL, root);
    root = insert(40, NULL, root);
    root = insert(50, NULL, root);
    root = insert(5, NULL, root);
    root = insert(15, NULL, root);

    printf("\nB+ Tree Structure:\n");
    print_tree(root);

    return 0;
}
