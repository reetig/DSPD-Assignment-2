#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define NAME 100
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

// creating the data node for family
family_datanode *create_family_datanode()
{
    family_datanode *new_node = (family_datanode *)malloc(sizeof(family_datanode));
    new_node->data_items = 0;
    new_node->next = NULL;
    new_node->prev = NULL;
    return new_node;
}
// Node creation functions
B_plus_node *create_node(bool is_leaf)
{
    B_plus_node *new_node = (B_plus_node *)malloc(sizeof(B_plus_node));
    new_node->is_leaf = is_leaf;
    new_node->active_keys = 0;
    for (int i = 0; i < ORDER - 1; i++)
    {
        new_node->keys[i] = 0;
    }
    for (int i = 0; i < ORDER; i++)
    {
        new_node->pointers[i] = NULL;
    }
    return new_node;
}

// helper insertion functions
B_plus_node *find_leaf(B_plus_node *root, int family_id)
{
    B_plus_node *current = root;
    while (!current->is_leaf)
    {
        int i = 0;
        // Find the first key > family_id, then go left
        while (i < current->active_keys && family_id >= current->keys[i])
        {
            i++;
        }
        current = (B_plus_node *)current->pointers[i];
    }
    return current;
}

family_datanode *find_data_node(B_plus_node *leaf, int family_id)
{
    int i = 0;
    // Find the last key <= family_id
    while (i < leaf->active_keys && family_id >= leaf->keys[i])
    {
        i++;
    }
    // The correct data node is at pointers[i]
    return (family_datanode *)leaf->pointers[i];
}

void insert_into_data_node(family_datanode *data_node, int family_id, family *new_family)
{
    // 1. Find the position to insert (maintain sorted order by family_id)
    int pos = data_node->data_items;
    // pos should be between 0 to max members -1 i.e 3
    while (pos > 0 && family_id < data_node->family_array[pos - 1].family_id)
    {
        // Shift existing families to make space
        data_node->family_array[pos] = data_node->family_array[pos - 1];
        pos--;
    }

    // 2. Insert the new family
    data_node->family_array[pos] = *new_family;
    data_node->data_items++;

    // Note: The B+ tree leaf key for this data node remains unchanged
    // (still the minimum family_id in the node, which may not be the new family).
}
// case where the B plus tree node / leaf has space for more keys but the data memebers are full so the node needs to be split and redistributed
void handle_data_node_split(B_plus_node *leaf, family_datanode *data_node,
                            int new_family_id, family *new_family)
{
    // 1. Create temp array with all families (existing + new)
    family temp[MAX_DATA_ITEMS + 1];
    memcpy(temp, data_node->family_array, sizeof(family) * MAX_DATA_ITEMS);

    // Insert new family in sorted position
    int insert_pos = 0;
    while (insert_pos < MAX_DATA_ITEMS &&
           new_family_id > temp[insert_pos].family_id)
    {
        insert_pos++;
    }
    memmove(&temp[insert_pos + 1], &temp[insert_pos],
            sizeof(family) * (MAX_DATA_ITEMS - insert_pos));
    temp[insert_pos] = *new_family;

    // 2. Split data node
    int split_pos = (MAX_DATA_ITEMS + 1) / 2; // Right-biased
    family_datanode *new_data = create_family_datanode();

    // Original node keeps first half
    memcpy(data_node->family_array, temp, sizeof(family) * split_pos);
    data_node->data_items = split_pos;

    // New node gets second half
    memcpy(new_data->family_array, &temp[split_pos],
           sizeof(family) * (MAX_DATA_ITEMS + 1 - split_pos));
    new_data->data_items = (MAX_DATA_ITEMS + 1 - split_pos);

    // 3. Update linked list
    new_data->next = data_node->next;
    if (data_node->next)
        data_node->next->prev = new_data;
    data_node->next = new_data;
    new_data->prev = data_node;

    // 4. Update B+ tree leaf (no split needed)
    int new_key = new_data->family_array[0].family_id;

    // Find position in leaf node
    int leaf_pos = 0;
    while (leaf_pos < leaf->active_keys &&
           new_key > leaf->keys[leaf_pos])
    {
        leaf_pos++;
    }

    // Shift to make space
    for (int i = leaf->active_keys; i > leaf_pos; i--)
    {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->pointers[i] = leaf->pointers[i - 1];
    }

    // Insert new key and pointer
    leaf->keys[leaf_pos] = new_key;
    leaf->pointers[leaf_pos] = new_data;
    leaf->active_keys++;
}
B_plus_node *find_parent(B_plus_node *root, B_plus_node *child)
{
    // Case 1: Child is the root (no parent)
    if (root == child || root == NULL)
    {
        return NULL;
    }

    B_plus_node *parent = NULL;
    B_plus_node *current = root;

    // Traverse down the tree
    while (current != NULL && !current->is_leaf)
    {
        // Check all pointers in the current node
        for (int i = 0; i <= current->active_keys; i++)
        {
            if (current->pointers[i] == child)
            {
                return current; // Found parent
            }
            // Recurse into child nodes
            else if (i < current->active_keys &&
                     child->keys[0] < current->keys[i])
            {
                parent = current;
                current = current->pointers[i];
                break;
            }
            // Last pointer case
            else if (i == current->active_keys)
            {
                parent = current;
                current = current->pointers[i];
                break;
            }
        }
    }

    return parent; // Returns NULL if not found (should not happen in valid trees)
}

void insert_into_parent(B_plus_node *old_node, int key, B_plus_node *new_node)
{
    B_plus_node *parent = find_parent(family_tree_pointer, old_node);

    // Case 1: No parent (old_node was root)
    if (!parent)
    {
        create_new_root(old_node, key, new_node);
    }
    else
    {
        int pos = 0;
        while (pos < parent->active_keys && parent->pointers[pos] != old_node)
        {
            pos++;
        }

        // Shift parent keys/pointers
        for (int i = parent->active_keys; i > pos; i--)
        {
            parent->keys[i] = parent->keys[i - 1];
            parent->pointers[i + 1] = parent->pointers[i];
        }

        // Insert new key and pointer
        parent->keys[pos] = key;
        parent->pointers[pos + 1] = new_node;
        parent->active_keys++;

        // Split parent if full
        if (parent->active_keys == ORDER - 1)
        {
            split_internal_node(parent); // Recursively propagate
        }
    }
    // Case 2: Parent has space
}

void split_leaf_node(B_plus_node *old_leaf, B_plus_node **new_leaf, int *promote_key)
{
    *new_leaf = create_node(true); // Create new leaf
    int split_pos = old_leaf->active_keys / 2;

    // Copy second half to new leaf
    for (int i = split_pos; i < old_leaf->active_keys; i++)
    {
        (*new_leaf)->keys[i - split_pos] = old_leaf->keys[i];
        (*new_leaf)->pointers[i - split_pos] = old_leaf->pointers[i];
    }

    (*new_leaf)->active_keys = old_leaf->active_keys - split_pos;
    old_leaf->active_keys = split_pos;

    // Set promote_key = smallest key in new leaf
    *promote_key = (*new_leaf)->keys[0];

    // Link leaves
    (*new_leaf)->pointers[ORDER - 1] = old_leaf->pointers[ORDER - 1];
    old_leaf->pointers[ORDER - 1] = *new_leaf;

    // Propagate to parent
    if (old_leaf == *tree_root)
    {
        create_new_root(old_leaf, *promote_key, *new_leaf);
    }
    else
    {
        insert_into_parent(old_leaf, *promote_key, *new_leaf);
    }
}

void split_internal_node(B_plus_node *old_node, B_plus_node **new_node, int *promote_key)
{
    *new_node = create_node(false); // Internal node
    int split_pos = old_node->active_keys / 2;

    *promote_key = old_node->keys[split_pos]; // Middle key promoted

    // Copy keys/pointers after split_pos
    for (int i = split_pos + 1; i < old_node->active_keys; i++)
    {
        (*new_node)->keys[i - (split_pos + 1)] = old_node->keys[i];
    }
    for (int i = split_pos + 1; i <= old_node->active_keys; i++)
    {
        (*new_node)->pointers[i - (split_pos + 1)] = old_node->pointers[i];
    }

    (*new_node)->active_keys = old_node->active_keys - split_pos - 1;
    old_node->active_keys = split_pos;

    // Propagate to parent
    if (old_node == *tree_root)
    {
        create_new_root(old_node, *promote_key, *new_node);
    }
    else
    {
        insert_into_parent(old_node, *promote_key, *new_node);
    }
}
void insert_into_leaf(B_plus_node *leaf, int new_key, family_datanode *new_data)
{
    int pos = 0;
    // Find position to insert (maintain sorted order)
    while (pos < leaf->active_keys && new_key > leaf->keys[pos])
    {
        pos++;
    }

    // Shift keys/pointers to make space
    for (int i = leaf->active_keys; i > pos; i--)
    {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->pointers[i] = leaf->pointers[i - 1];
    }

    // Insert new key and pointer
    leaf->keys[pos] = new_key;
    leaf->pointers[pos] = new_data;
    leaf->active_keys++;

    // Split leaf if overflow (ORDER is the leaf's max capacity)
    if (leaf->active_keys == ORDER)
    {
        split_leaf_node(leaf); // Propagates changes upward
    }
}
// main insertion function

void insert_into_family_tree(int family_id, family *new_family)
{
    // if the tree is empty
    if (family_tree_pointer == NULL)
    {
        family_tree_pointer = create_node(true);
        family_datanode *new_data = create_family_datanode();
        new_data->family_array[0] = *new_family;
        new_data->data_items = 1;
        family_tree_pointer->pointers[0] = new_data;
        family_tree_pointer->keys[0] = family_id;
        family_tree_pointer->active_keys = 1;
    }
    else
    {
        // case 2:non empty tree - find the target leaf
        B_plus_node *leaf = find_leaf(family_tree_pointer, family_id);
        family_datanode *data_node = find_data_node(leaf, family_id);

        // case 2i data node has empty space so insert at cireect position
        if (data_node->data_items < MAX_DATA_ITEMS)
        {
            insert_into_data_node(data_node, family_id, new_family);
        }

        // case 2ii where the data node is full and needs a split

        else
        { // considering that all the keys are not their in the tree
            // 1. Create temporary array with all families (existing + new)
            family temp[MAX_DATA_ITEMS + 1];
            memcpy(temp, data_node->family_array, sizeof(family) * MAX_DATA_ITEMS);

            // 2. Insert new family in sorted order
            int insert_pos = 0;
            while (insert_pos < MAX_DATA_ITEMS &&
                   family_id > temp[insert_pos].family_id)
            {
                insert_pos++;
            }
            memmove(&temp[insert_pos + 1], &temp[insert_pos],
                    sizeof(family) * (MAX_DATA_ITEMS - insert_pos));
            temp[insert_pos] = *new_family;

            // 3. Split data node
            int split_pos = (MAX_DATA_ITEMS + 1) / 2;
            family_datanode *new_data = create_family_datanode();

            // Original node keeps first half
            memcpy(data_node->family_array, temp, sizeof(family) * split_pos);
            data_node->data_items = split_pos;

            // New node gets second half
            memcpy(new_data->family_array, &temp[split_pos],
                   sizeof(family) * (MAX_DATA_ITEMS + 1 - split_pos));
            new_data->data_items = (MAX_DATA_ITEMS + 1 - split_pos);

            // 4. Update linked list
            new_data->next = data_node->next;
            if (data_node->next)
                data_node->next->prev = new_data;
            data_node->next = new_data;
            new_data->prev = data_node;

            // 5. Update B+ tree leaf
            int new_key = new_data->family_array[0].family_id; // Smallest key in new node
            insert_into_leaf(leaf, new_key, new_data);         // Add to leaf
            /*
            // temproray array with existing and new family

            family temp[MAX_DATA_ITEMS];
            for (int i = 0; i < MAX_DATA_ITEMS; i++)
            {
                // doubt in indexing
                temp[i] = data_node->family_array[i];
            }
            // insert the new family in the sorted position
            int index = 0;
            while (index < MAX_DATA_ITEMS && family_id > temp[index].family_id)
            {
                index++;
            }
            // shifting the elements to make space for new family
            for (int i = MAX_DATA_ITEMS - 1; i > index; i--)
            {
                temp[i] = temp[i - 1];
            }
            // insert the new family
            temp[index] = *new_family;

            // split the data node
            int split_pos = (MAX_DATA_ITEMS + 1) / 2;
            // creating a new data node
            family_datanode *new_data = create_family_datanode();
            // update the old data node
            for (int i = 0; i < split_pos; i++)
            {
                data_node->family_array[i] = temp[i];
            }
            data_node->data_items = split_pos;
            // dount in indexing
            // updating the new data node
            for (int i = split_pos; i < MAX_DATA_ITEMS; i++)
            {
                new_data->family_array[i] = temp[i];
            }
            new_data->data_items = MAX_DATA_ITEMS - split_pos;

            // update the linked list
            new_data->next = data_node->next;
            if (data_node->next)
            {
                data_node->next->prev = new_data;
            }
            data_node->next = new_data;
            new_data->prev = data_node;

            // insert the new key into the B+ tree leaf
            int new_key = new_data->family_array[0].family_id;
            insert_into_leaf(leaf, new_key, new_data);
            */
        }
    }
}

B_plus_node *splitBPlusTreeNode(B_plus_node *root)
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

    else if (root->is_leaf == 1)
    {
        printf("splitting leaf");
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

        B_plus_node *parent = root->parent;
        printf(" %d", parent->active_keys);
        i = parent->active_keys - 1;
        printf("%d", parent->active_keys);

        while (i >= 0 &&)
        {
            parent->keys[i + 1] = parent->keys[i];
            parent->children.tree_node_ptr[i + 2] = parent->children.tree_node_ptr[i + 1];
            i--;
        }
        i++;
        printf("%d", i);
        parent->keys[i] = root->keys[ORDER / 2];
        parent->children.tree_node_ptr[i] = root;
        parent->children.tree_node_ptr[i + 1] = newNode;
        parent->active_keys++;
        printf("\n");
        for (i = 0; i < parent->active_keys; i++)
        {
            printf("%d:%d ", parent->keys[i].Hour, parent->keys[i].Min);
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