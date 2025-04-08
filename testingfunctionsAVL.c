#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define NAME 100
#define MAX_MEMBERS 4
typedef enum
{
    rent,
    utilities,
    groceries,
    stationary,
    leisure
} expense_category;

typedef struct expense_node
{
    int expense_id;
    int user_id;
    expense_category category;
    float expense_amount;
    int date_of_expense;
} expense;

typedef struct AVL_tree_expense_node
{

    expense expense_data_node;
    struct AVL_tree_expense_node *right;
    struct AVL_tree_expense_node *left;
    int height;
} expense_tree_node;

typedef struct LL_expense
{
    int month;
    expense_tree_node *tree_root;
    struct LL_expense *next;
} LL_expense_node;
typedef struct individual_node
{
    int user_id;
    char user_name[NAME];
    float user_income;
    LL_expense_node *monthly_indiv_expenses;
    // linked list of 12 expense trees each representing expense of each user in each month

} individual;

typedef struct AVL_tree_user_node
{
    individual user_data_node;
    struct AVL_tree_user_node *right;
    struct AVL_tree_user_node *left;
    int height;
} user_tree_node;
typedef struct family_node
{
    int family_id;
    char family_name[NAME];
    user_tree_node *family_members[MAX_MEMBERS]; // pointing towards a usernode
    float total_family_income;                   // calculated as sum of individual incomes
    float monthly_expense;                       // calculated as sum of all expense by family members in a month in all categories

} family;

typedef struct AVL_tree_family_node
{
    family family_data_node;
    struct AVL_tree_family_node *right;
    struct AVL_tree_family_node *left;
    int height;
} family_tree_node;

// create the family tree node
family_tree_node *create_family_node()
{
    family_tree_node *nptr = (family_tree_node *)malloc(sizeof(family_tree_node));
    nptr->right = NULL;
    nptr->left = NULL;
    nptr->height = 1;
    printf("created a new fam node.\n");
    return nptr;
}
// find user node in tree
user_tree_node *find_user_node(user_tree_node *user_tree_ptr, int user_id)
{
    int found = 0;
    // search in avl tree

    // Base Cases: root is null or key is
    // present at root
    if (user_tree_ptr == NULL || user_tree_ptr->user_data_node.user_id == user_id)
        return user_tree_ptr;

    // Key is greater than root's key
    if (user_tree_ptr->user_data_node.user_id < user_id)
        return find_user_node(user_tree_ptr->right, user_id);

    // Key is smaller than root's key
    return find_user_node(user_tree_ptr->left, user_id);
}
// WORKS
user_tree_node *create_individual_node(int user_id, float income, char *user_name)
{
    printf("i entered the create individual function.\n");
    user_tree_node *new_user = (user_tree_node *)malloc(sizeof(user_tree_node));
    int allocation_failed = 0;
    LL_expense_node *current = NULL;

    if (new_user != NULL)
    {
        new_user->user_data_node.user_id = user_id;
        new_user->user_data_node.user_income = income;
        strncpy(new_user->user_data_node.user_name, user_name, NAME - 1);
        new_user->user_data_node.user_name[NAME - 1] = '\0'; // Ensure null-termination

        new_user->user_data_node.monthly_indiv_expenses = NULL;
        new_user->height = 1;
        new_user->right = NULL;
        new_user->left = NULL;

        for (int i = 0; i < 12 && allocation_failed == 0; i++)
        {
            LL_expense_node *new_node = (LL_expense_node *)malloc(sizeof(LL_expense_node));
            if (new_node == NULL)
            {
                printf("memory allocation for expenses failed in heap. hence returning NULL\n");
                allocation_failed = 1;
            }
            else
            {
                new_node->month = i + 1;
                new_node->tree_root = NULL;
                new_node->next = NULL;

                if (current == NULL)
                {
                    new_user->user_data_node.monthly_indiv_expenses = new_node;
                }
                else
                {
                    current->next = new_node;
                }
                current = new_node;
            }
        }

        if (allocation_failed == 1)
        {
            current = NULL;
            while (new_user->user_data_node.monthly_indiv_expenses)
            {
                LL_expense_node *temp = new_user->user_data_node.monthly_indiv_expenses;
                new_user->user_data_node.monthly_indiv_expenses = temp->next;
                free(temp);
            }
            free(new_user);
            new_user = NULL;
        }
    }
    else
    {
        printf("failed to create individual node \n");
    }
    printf("exiting the function of create indiv node.\n");
    return new_user;
}

// insertion function for the user

int height_user_id(user_tree_node *N)
{
    if (N == NULL)
        return 0;
    return N->height;
}

// A utility function to get maximum of two integers
int find_max(int a, int b)
{
    return (a > b) ? a : b;
}

// A utility function to right rotate subtree rooted with y
// See the diagram given above.
user_tree_node *rightRotate_user(user_tree_node *y)
{
    user_tree_node *x = y->left;
    user_tree_node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = find_max(height_user_id(y->left),
                         height_user_id(y->right)) +
                1;
    x->height = find_max(height_user_id(x->left),
                         height_user_id(x->right)) +
                1;

    // Return new root
    return x;
}

// A utility function to left rotate subtree rooted with x
// See the diagram given above.
user_tree_node *leftRotate_user(user_tree_node *x)
{
    user_tree_node *y = x->right;
    user_tree_node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    //  Update heights
    x->height = find_max(height_user_id(x->left),
                         height_user_id(x->right)) +
                1;
    y->height = find_max(height_user_id(y->left),
                         height_user_id(y->right)) +
                1;

    // Return new root
    return y;
}

// Get Balance factor of node N
int getBalance_user(user_tree_node *N)
{
    if (N == NULL)
        return 0;
    return height_user_id(N->left) - height_user_id(N->right);
}

// Recursive function to insert a key in the subtree rooted
// with node and returns the new root of the subtree.
user_tree_node *insert_user_node(user_tree_node *root, int key, user_tree_node *new_node)
{
    printf("entered the insert_user fun.\n");
    /* 1.  Perform the normal BST insertion */
    if (root == NULL)
        return new_node;

    if (key < root->user_data_node.user_id)
        root->left = insert_user_node(root->left, key, new_node);
    else if (key > root->user_data_node.user_id)
        root->right = insert_user_node(root->right, key, new_node);
    else // Equal keys are not allowed in BST
        return root;

    /* 2. Update height of this ancestor node */
    root->height = 1 + find_max(height_user_id(root->left),
                                height_user_id(root->right));

    /* 3. Get the balance factor of this ancestor
          node to check whether this node became
          unbalanced */
    int balance = getBalance_user(root);

    // If this node becomes unbalanced, then
    // there are 4 cases

    // Left Left Case
    if (balance > 1 && key < root->left->user_data_node.user_id)
        return rightRotate_user(root);

    // Right Right Case
    if (balance < -1 && key > root->right->user_data_node.user_id)
        return leftRotate_user(root);

    // Left Right Case
    if (balance > 1 && key > root->left->user_data_node.user_id)
    {
        root->left = leftRotate_user(root->left);
        return rightRotate_user(root);
    }

    // Right Left Case
    if (balance < -1 && key < root->right->user_data_node.user_id)
    {
        root->right = rightRotate_user(root->right);
        return leftRotate_user(root);
    }

    printf("exiting the funct of insert user funct.\n");
    /* return the (unchanged) node pointer */
    return root;
}

// helper function for add user function
family_tree_node *find_node_in_fam_tree(family_tree_node *family_tree_ptr, int family_id)
{
    // Base Cases: root is null or key is
    // present at root
    if (family_tree_ptr == NULL || family_tree_ptr->family_data_node.family_id == family_id)
        return family_tree_ptr;

    // Key is greater than root's key
    if (family_tree_ptr->family_data_node.family_id < family_id)
        return find_node_in_fam_tree(family_tree_ptr->right, family_id);

    // Key is smaller than root's key
    return find_node_in_fam_tree(family_tree_ptr->left, family_id);
}

// helper function in insert family tree
int height_family(family_tree_node *N)
{
    if (N == NULL)
        return 0;
    return N->height;
}

// A utility function to right rotate subtree rooted with y
// See the diagram given above.
family_tree_node *rightRotate_fam(family_tree_node *y)
{
    family_tree_node *x = y->left;
    family_tree_node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = find_max(height_family(y->left),
                         height_family(y->right)) +
                1;
    x->height = find_max(height_family(x->left),
                         height_family(x->right)) +
                1;

    // Return new root
    return x;
}

// A utility function to left rotate subtree rooted with x
// See the diagram given above.
family_tree_node *leftRotate_fam(family_tree_node *x)
{
    family_tree_node *y = x->right;
    family_tree_node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    //  Update heights
    x->height = find_max(height_family(x->left),
                         height_family(x->right)) +
                1;
    y->height = find_max(height_family(y->left),
                         height_family(y->right)) +
                1;

    // Return new root
    return y;
}

// Get Balance factor of node N
int getBalance_fam(family_tree_node *N)
{
    if (N == NULL)
        return 0;
    return height_family(N->left) - height_family(N->right);
}

// Recursive function to insert a key in the subtree rooted
// with node and returns the new root of the subtree.
family_tree_node *insert_family_in_tree(family_tree_node *node, int key, family_tree_node *new_fam)
{
    printf("enetering the insert family funct\n");
    /* 1.  Perform the normal BST insertion */
    if (node == NULL)
        return new_fam;

    if (key < node->family_data_node.family_id)
        node->left = insert_family_in_tree(node->left, key, new_fam);
    else if (key > node->family_data_node.family_id)
        node->right = insert_family_in_tree(node->right, key, new_fam);
    else
    {
        printf("duplicate id found hence family cant be formed.\n");
        return node; // returning the particular node where the id was found
    } // Equal keys are not allowed in BST

    /* 2. Update height of this ancestor node */
    node->height = 1 + find_max(height_family(node->left),
                                height_family(node->right));

    /* 3. Get the balance factor of this ancestor
          node to check whether this node became
          unbalanced */
    int balance = getBalance_fam(node);

    // If this node becomes unbalanced, then
    // there are 4 cases

    // Left Left Case
    if (balance > 1 && key < node->left->family_data_node.family_id)
        return rightRotate_fam(node);

    // Right Right Case
    if (balance < -1 && key > node->right->family_data_node.family_id)
        return leftRotate_fam(node);

    // Left Right Case
    if (balance > 1 && key > node->left->family_data_node.family_id)
    {
        node->left = leftRotate_fam(node->left);
        return rightRotate_fam(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->family_data_node.family_id)
    {
        node->right = rightRotate_fam(node->right);
        return leftRotate_fam(node);
    }

    /* return the (unchanged) node pointer */
    return node;
}

// create a family and insert it in the family avl tree
family_tree_node *create_family(int family_id, char *fam_name, float family_income, float monthly_exp, family_tree_node **family_tree)
{
    printf("i entered create_family funct.\n");

    family_tree_node *new_family = create_family_node();
    if (new_family != NULL)
    {
        printf("new family is creating.\n");
        new_family->family_data_node.family_id = family_id;
        strcpy(new_family->family_data_node.family_name, fam_name);
        new_family->family_data_node.total_family_income = family_income;
        float monthly_expense = 0;

        new_family->family_data_node.monthly_expense = monthly_exp;

        for (int i = 0; i < MAX_MEMBERS; i++)
        {
            new_family->family_data_node.family_members[i] = NULL;
        }

        // Update the root pointer
        *family_tree = insert_family_in_tree(*family_tree, family_id, new_family);
    }
    else
    {
        printf("Heap is full, hence family cannot be created. Returning NULL.\n");
        return NULL; // Explicitly return NULL if allocation fails
    }

    printf("Exiting the create_family funct.\n");
    return new_family; // Return the newly created family node
}

void addUser(int user_id, char *name, float income, int family_id, user_tree_node **user_ptr_root, family_tree_node **family_ptr)
{
    // 1.check if user id exists or not
    // 2. if not create a user tree node and put data in it (call a function preferably)
    // 3.check if that family exists , if not then create a family and add that user in it
    // 3. otherwise just simply point the family in that node

    user_tree_node *user_found = find_user_node(*user_ptr_root, user_id); // WORKS
    if (user_found != NULL)
    {
        printf("User already exists.\n");
    }
    else
    {
        // WORKS
        user_tree_node *new_user = create_individual_node(user_id, income, name); // WORKS

        if (new_user != NULL)
        {
            // insert the user in the avl tree
            *user_ptr_root = insert_user_node(*user_ptr_root, user_id, new_user); // the root of the avl user tree //WORKS

            // user needs to be added in the correct family

            family_tree_node *family_found = find_node_in_fam_tree(*family_ptr, family_id);

            if (family_found != NULL)
            {

                int inserted = 0;
                printf("family exists , hence will add directly to the family.\n");
                for (int i = 0; i < MAX_MEMBERS && inserted == 0; i++)
                {
                    if (family_found->family_data_node.family_members[i] == NULL)
                    {
                        family_found->family_data_node.family_members[i] = new_user;
                        inserted = 1;
                    }
                }
                if (inserted == 0)
                {
                    printf("Family Full No more members can be added. Hence user cant be added as a user must belong to one of the family.\n");
                }
                else
                {
                    printf("user added successfully.\n");
                }
            }
            else
            {
                // WORKS

                // family doesnt exists hence create a family
                // first we create user and then family
                printf("family doesnt exist and hence you have to enter the family details(family name, family income, family_monthly expense).\n");
                char fam_name[NAME];
                printf("enter family name.\n");
                fgets(fam_name, sizeof(fam_name), stdin);
                fam_name[strcspn(fam_name, "\n")] = '\0'; // Remove trailing newline
                float fam_income;
                printf("enter family income.\n");
                scanf("%f", &fam_income);
                float fam_monthly_exp;
                printf("enter family monthly expense.\n");
                scanf("%f", &fam_monthly_exp);
                getchar(); // Clear input buffer

                family_tree_node *new_family = create_family(family_id, fam_name, fam_income, fam_monthly_exp, family_ptr); // ISSUE
                new_family->family_data_node.family_members[0] = new_user;
                *family_ptr = insert_family_in_tree(*family_ptr, family_id, new_family);
                // insert_user_node(user_ptr_root, user_id, new_family->family_data_node.family_members[0]); // insert the user in the tree //already added user in the tree

                if (new_family->family_data_node.family_members[0] != NULL)
                {
                    // family_tree_ptr = insert_family_in_tree(family_ptr, family_id, new_family);
                    printf("user successfully added.\n");
                }
                else
                {
                    printf("not enough space for user.\n");
                }
            }
        }
        else
        {
            printf("user cant be inserted because of less space.\n");
        }
    }
    printf("exit add user function.\n");
}

expense_tree_node *create_expense_node()
{
    expense_tree_node *nptr = (expense_tree_node *)malloc(sizeof(expense_tree_node));
    nptr->right = NULL;
    nptr->left = NULL;
    nptr->height = 1;
    printf("created a new fam node.\n");
    return nptr;
}
// insert the expense node

// insert expense functions
int height_expense(expense_tree_node *N)
{
    if (N == NULL)
        return 0;
    return N->height;
}

// A utility function to right rotate subtree rooted with y
// See the diagram given above.
expense_tree_node *rightRotate_exp(expense_tree_node *y)
{
    expense_tree_node *x = y->left;
    expense_tree_node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = find_max(height_expense(y->left),
                         height_expense(y->right)) +
                1;
    x->height = find_max(height_expense(x->left),
                         height_expense(x->right)) +
                1;

    // Return new root
    return x;
}

// A utility function to left rotate subtree rooted with x
// See the diagram given above.
expense_tree_node *leftRotate_exp(expense_tree_node *x)
{
    expense_tree_node *y = x->right;
    expense_tree_node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    //  Update heights
    x->height = find_max(height_expense(x->left),
                         height_expense(x->right)) +
                1;
    y->height = find_max(height_expense(y->left),
                         height_expense(y->right)) +
                1;

    // Return new root
    return y;
}

// Get Balance factor of node N
int getBalance_exp(expense_tree_node *N)
{
    if (N == NULL)
        return 0;
    return height_expense(N->left) - height_expense(N->right);
}

// Recursive function to insert a key in the subtree rooted
// with node and returns the new root of the subtree.
expense_tree_node *insert_exp_node(expense_tree_node *node, int key, expense_tree_node *new_exp)
{
    printf("entered insert expense node funct.\n");
    /* 1.  Perform the normal BST insertion */
    if (node == NULL)
        return new_exp;

    if (key < node->expense_data_node.expense_id)
        node->left = insert_exp_node(node->left, key, new_exp);
    else if (key > node->expense_data_node.expense_id)
        node->right = insert_exp_node(node->right, key, new_exp);
    else // Equal keys are not allowed in BST
        return node;

    /* 2. Update height of this ancestor node */
    node->height = 1 + find_max(height_expense(node->left),
                                height_expense(node->right));

    /* 3. Get the balance factor of this ancestor
          node to check whether this node became
          unbalanced */
    int balance = getBalance_exp(node);

    // If this node becomes unbalanced, then
    // there are 4 cases

    // Left Left Case
    if (balance > 1 && key < node->left->expense_data_node.expense_id)
        return rightRotate_exp(node);

    // Right Right Case
    if (balance < -1 && key > node->right->expense_data_node.expense_id)
        return leftRotate_exp(node);

    // Left Right Case
    if (balance > 1 && key > node->left->expense_data_node.expense_id)
    {
        node->left = leftRotate_exp(node->left);
        return rightRotate_exp(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->expense_data_node.expense_id)
    {
        node->right = rightRotate_exp(node->right);
        return leftRotate_exp(node);
    }

    /* return the (unchanged) node pointer */
    return node;
}

// Helper function to print the expense tree
void print_expense_tree(expense_tree_node *node, int level)
{
    if (node == NULL)
        return;

    // Print right subtree first (for better visualization)
    print_expense_tree(node->right, level + 1);

    // Indent based on level
    for (int i = 0; i < level; i++)
    {
        printf("    ");
    }

    // Print current node

    const char *categories[] = {"Rent", "Utilities", "Groceries",
                                "stationary", "leisure"};
    printf("[ID:%d] %s: $%.2f (Date: %d)\n",
           node->expense_data_node.expense_id,
           categories[node->expense_data_node.category],
           node->expense_data_node.expense_amount,
           node->expense_data_node.date_of_expense);

    // Print left subtree
    print_expense_tree(node->left, level + 1);
}

// search for expense node in a tree
expense_tree_node *find_exp_node(expense_tree_node *exp_tree_ptr, int exp_id)
{
    int found = 0;
    // search in avl tree

    // Base Cases: root is null or key is
    // present at root
    if (exp_tree_ptr == NULL || exp_tree_ptr->expense_data_node.expense_id == exp_id)
        return exp_tree_ptr;

    // Key is greater than root's key
    if (exp_tree_ptr->expense_data_node.expense_id < exp_id)
        return find_exp_node(exp_tree_ptr->right, exp_id);

    // Key is smaller than root's key
    return find_exp_node(exp_tree_ptr->left, exp_id);
}
// ADD EXPENSE

void Add_expense(int exp_id, int user_id, expense_category cat, float amount, int date, user_tree_node **user_ptr)
{
    user_tree_node *found_user = find_user_node(*user_ptr, user_id);
    if (found_user != NULL)
    {
        printf("user exists.\n");
        // check if the expense id exists in the user or not
        int i = 11;
        int found = 0;
        LL_expense_node *node = found_user->user_data_node.monthly_indiv_expenses;
        while (node != NULL && i >= 0 && found == 0)
        {
            expense_tree_node *exp_exists = find_exp_node(node->tree_root, exp_id);
            if (exp_exists != NULL)
            {
                printf("the exp exists in the user list hence wont be added.\n");
                found = 1;
            }
            node = node->next;
            i--;
        }
        if (found == 0)
        {
            int find_month = (date % 100); // Extract last 2 digits for month in DDMM format

            // found_user->user_data_node.monthly_indiv_expenses->month = find_month;
            LL_expense_node *temp = found_user->user_data_node.monthly_indiv_expenses;
            // we found the LL of that particuar list in that user list
            while (temp != NULL && temp->month != find_month)
            {
                temp = temp->next;
            }
            // temp is the LL containg the expense month tree
            if (temp != NULL)
            {

                expense_tree_node *new_expense = (expense_tree_node *)malloc(sizeof(expense_tree_node));

                if (new_expense != NULL)
                {
                    new_expense->right = NULL;
                    new_expense->left = NULL;
                    new_expense->expense_data_node.category = cat;
                    new_expense->expense_data_node.date_of_expense = date;
                    new_expense->expense_data_node.expense_amount = amount;
                    new_expense->expense_data_node.expense_id = exp_id;
                    new_expense->expense_data_node.user_id = user_id;
                    new_expense->height = 1; // CHECK IF THE HEIGHT FACTOR I HAVE WRITTEN CORRECTLY
                                             // insert the expense node in the correct position
                    temp->tree_root = insert_exp_node(temp->tree_root, exp_id, new_expense);
                    print_expense_tree(temp->tree_root, 0);
                    printf("expense added successfully.\n");
                }
                else
                {
                    printf("memeory allocation failed.\n");
                }
            }
            else
            {
                // this condition shouldnt arise beacuse in user node only we make space foot expenese LL nodes too
                printf("that month expense tree doesnt exist.\n");
            }
        }
    }
    else
    {
        printf("user doesnt exists hence the expense cant be added.\n");
    }
}

// Helper function for user tree printing
void print_user_node(user_tree_node *node, int depth)
{
    if (node == NULL)
        return;

    // Print right subtree first (for better visualization)
    print_user_node(node->right, depth + 1);

    // Indent based on depth
    for (int i = 0; i < depth; i++)
        printf("    ");

    // Print user info
    printf("User ID: %-5d | Name: %-20s | Income: %8.2f\n",
           node->user_data_node.user_id,
           node->user_data_node.user_name,
           node->user_data_node.user_income);

    // Print left subtree
    print_user_node(node->left, depth + 1);
}

// Main user tree print function
void print_user_tree(user_tree_node *root)
{
    if (root == NULL)
    {
        printf("\n=== USER TREE ===\n(empty)\n===============\n");
        return;
    }

    printf("\n=== USER TREE ===\n");
    printf("ID    | Name                 | Income\n");
    printf("------+----------------------+-----------\n");
    print_user_node(root, 0);
    printf("===============\n");
}

// Helper function for family tree printing
void print_family_node(family_tree_node *node, int depth)
{
    if (node == NULL)
        return;

    // Print right subtree first
    print_family_node(node->right, depth + 1);

    // Indent based on depth
    for (int i = 0; i < depth; i++)
        printf("    ");

    // Print family info
    printf("Family ID: %-5d | Name: %-20s\n",
           node->family_data_node.family_id,
           node->family_data_node.family_name);
    printf("Total Income: %8.2f | Monthly Expense: %8.2f\n",
           node->family_data_node.total_family_income,
           node->family_data_node.monthly_expense);

    // Print members
    printf("Members: ");
    bool has_members = false;
    for (int i = 0; i < MAX_MEMBERS; i++)
    {
        if (node->family_data_node.family_members[i] != NULL)
        {
            printf("%d(%s) ",
                   node->family_data_node.family_members[i]->user_data_node.user_id,
                   node->family_data_node.family_members[i]->user_data_node.user_name);
            has_members = true;
        }
    }
    printf("%s\n\n", has_members ? "" : "(none)");

    // Print left subtree
    print_family_node(node->left, depth + 1);
}

// Main family tree print function
void print_family_tree(family_tree_node *root)
{
    if (root == NULL)
    {
        printf("\n=== FAMILY TREE ===\n(empty)\n=================\n");
        return;
    }

    printf("\n=== FAMILY TREE ===\n");
    print_family_node(root, 0);
    printf("=================\n");
}

// Helper function for recursive printing with indentation
void PrintExpenseTreeHelper(expense_tree_node *root, int depth)
{
    if (root == NULL)
        return;

    // First recur on left subtree
    PrintExpenseTreeHelper(root->left, depth + 1);

    // Print current node with indentation
    for (int i = 0; i < depth; i++)
    {
        printf("    "); // 4 spaces per depth level
    }

    // Get category name
    const char *category_names[] = {"Rent", "Utilities", "Groceries",
                                    "stationary", "leisure"};
    const char *cat_name = category_names[root->expense_data_node.category];

    // Print expense details
    printf("[ID:%d] %s: $%.2f (Date: %d)\n",
           root->expense_data_node.expense_id,
           cat_name,
           root->expense_data_node.expense_amount,
           root->expense_data_node.date_of_expense);

    // Recur on right subtree
    PrintExpenseTreeHelper(root->right, depth + 1);
}
void PrintExpenseTreeForMonth(user_tree_node *user_ptr, int user_id, int month)
{
    if (user_ptr == NULL)
    {
        printf("User tree is empty.\n");
        return;
    }

    // Find the user
    user_tree_node *found_user = find_user_node(user_ptr, user_id);
    if (found_user == NULL)
    {
        printf("User with ID %d not found.\n", user_id);
        return;
    }

    // Find the monthly expense list for the specified month
    LL_expense_node *month_expenses = found_user->user_data_node.monthly_indiv_expenses;
    while (month_expenses != NULL && month_expenses->month != month)
    {
        month_expenses = month_expenses->next;
    }

    if (month_expenses == NULL || month_expenses->tree_root == NULL)
    {
        printf("No expenses found for month %d.\n", month);
        return;
    }

    printf("\nExpense Tree for User %d, Month %d:\n", user_id, month);
    printf("================================\n");

    // Print the tree in-order with indentation
    PrintExpenseTreeHelper(month_expenses->tree_root, 0);
}

// delete functions for user and family

// delete functions for user and family
/* Given a non-empty binary search tree, return the
   node with minimum key value found in that tree.
   Note that the entire tree does not need to be
   searched. */
user_tree_node *minValueNode(user_tree_node *node)
{
    user_tree_node *current = node;

    /* loop down to find the leftmost leaf */
    while (current->left != NULL)
        current = current->left;

    return current;
}

// Recursive function to delete a node with given key
// from subtree with given root. It returns root of
// the modified subtree.
user_tree_node *delete_user_node(user_tree_node *root, int key)
{
    // STEP 1: PERFORM STANDARD BST DELETE

    if (root == NULL)
        return root;

    // If the key to be deleted is smaller than the
    // root's key, then it lies in left subtree
    if (key < root->user_data_node.user_id)
        root->left = delete_user_node(root->left, key);

    // If the key to be deleted is greater than the
    // root's key, then it lies in right subtree
    else if (key > root->user_data_node.user_id)
        root->right = delete_user_node(root->right, key);

    // if key is same as root's key, then This is
    // the node to be deleted
    else
    {
        // node with only one child or no child
        if ((root->left == NULL) || (root->right == NULL))
        {
            user_tree_node *temp = root->left ? root->left : root->right;

            // No child case
            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else               // One child case
                *root = *temp; // Copy the contents of
                               // the non-empty child
            free(temp);
        }
        else
        {
            // node with two children: Get the inorder
            // successor (smallest in the right subtree)
            user_tree_node *temp = minValueNode(root->right);

            // Copy the inorder successor's data to this node
            root->user_data_node.user_id = temp->user_data_node.user_id;

            // Delete the inorder successor
            root->right = delete_user_node(root->right, temp->user_data_node.user_id);
        }
    }

    // If the tree had only one node then return
    if (root == NULL)
        return root;

    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
    root->height = 1 + find_max(height_user_id(root->left),
                                height_user_id(root->right));

    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to
    // check whether this node became unbalanced)
    int balance = getBalance_user(root);

    // If this node becomes unbalanced, then there are 4 cases

    // Left Left Case
    if (balance > 1 && getBalance_user(root->left) >= 0)
        return rightRotate_user(root);

    // Left Right Case
    if (balance > 1 && getBalance_user(root->left) < 0)
    {
        root->left = leftRotate_user(root->left);
        return rightRotate_user(root);
    }

    // Right Right Case
    if (balance < -1 && getBalance_user(root->right) <= 0)
        return leftRotate_user(root);

    // Right Left Case
    if (balance < -1 && getBalance_user(root->right) > 0)
    {
        root->right = rightRotate_user(root->right);
        return leftRotate_user(root);
    }

    return root;
}

/* Given a non-empty binary search tree, return the
   node with minimum key value found in that tree.
   Note that the entire tree does not need to be
   searched. */
family_tree_node *minValueNode_fam(family_tree_node *node)
{
    family_tree_node *current = node;

    /* loop down to find the leftmost leaf */
    while (current->left != NULL)
        current = current->left;

    return current;
}

// Recursive function to delete a node with given key
// from subtree with given root. It returns root of
// the modified subtree.
family_tree_node *delete_fam_node(family_tree_node *root, int key)
{
    // STEP 1: PERFORM STANDARD BST DELETE

    if (root == NULL)
        return root;

    // If the key to be deleted is smaller than the
    // root's key, then it lies in left subtree
    if (key < root->family_data_node.family_id)
        root->left = delete_fam_node(root->left, key);

    // If the key to be deleted is greater than the
    // root's key, then it lies in right subtree
    else if (key > root->family_data_node.family_id)
        root->right = delete_fam_node(root->right, key);

    // if key is same as root's key, then This is
    // the node to be deleted
    else
    {
        // node with only one child or no child
        if ((root->left == NULL) || (root->right == NULL))
        {
            family_tree_node *temp = root->left ? root->left : root->right;

            // No child case
            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else               // One child case
                *root = *temp; // Copy the contents of
                               // the non-empty child
            free(temp);
        }
        else
        {
            // node with two children: Get the inorder
            // successor (smallest in the right subtree)
            family_tree_node *temp = minValueNode_fam(root->right);

            // Copy the inorder successor's data to this node
            root->family_data_node.family_id = temp->family_data_node.family_id;

            // Delete the inorder successor
            root->right = delete_fam_node(root->right, temp->family_data_node.family_id);
        }
    }

    // If the tree had only one node then return
    if (root == NULL)
        return root;

    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
    root->height = 1 + find_max(height_family(root->left),
                                height_family(root->right));

    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to
    // check whether this node became unbalanced)
    int balance = getBalance_fam(root);

    // If this node becomes unbalanced, then there are 4 cases

    // Left Left Case
    if (balance > 1 && getBalance_fam(root->left) >= 0)
        return rightRotate_fam(root);

    // Left Right Case
    if (balance > 1 && getBalance_fam(root->left) < 0)
    {
        root->left = leftRotate_fam(root->left);
        return rightRotate_fam(root);
    }

    // Right Right Case
    if (balance < -1 && getBalance_fam(root->right) <= 0)
        return leftRotate_fam(root);

    // Right Left Case
    if (balance < -1 && getBalance_fam(root->right) > 0)
    {
        root->right = rightRotate_fam(root->right);
        return leftRotate_fam(root);
    }

    return root;
}

// Helper function to find the family containing the user
family_tree_node *find_users_family(family_tree_node *root, int user_id)
{
    if (root == NULL)
        return NULL;

    // Check left subtree first
    family_tree_node *left_result = find_users_family(root->left, user_id);
    if (left_result != NULL)
        return left_result;

    // Check current family
    for (int i = 0; i < MAX_MEMBERS; i++)
    {
        if (root->family_data_node.family_members[i] != NULL &&
            root->family_data_node.family_members[i]->user_data_node.user_id == user_id)
        {
            return root;
        }
    }

    // Check right subtree if not found yet
    return find_users_family(root->right, user_id);
}
// update or delete individual or family
void update_or_delete_individual_family_details(int choice, user_tree_node *user_ptr, family_tree_node *fam_ptr)
{
    if (choice == 1)
    {
        // WORKS
        printf("enter the user id of the user you want to update.\n");
        int id;
        scanf("%d", &id);

        user_tree_node *found_user = find_user_node(user_ptr, id);
        if (found_user != NULL)
        {
            char name[NAME];
            printf("\nEnter the new name you want to keep: ");
            scanf("%s", name);
            // free(found_user->user_data_node.user_name); // Free old memory

            strcpy(found_user->user_data_node.user_name, name);

            printf("\nEnter the updated income: ");
            float updated_income;
            scanf("%f", &updated_income);
            found_user->user_data_node.user_income = updated_income;
            printf("updated user.\n");
            // not considering expenses over here
        }
        else
        {
            printf("user doent exist.\n");
        }
    }
    else if (choice == 2)
    {
        // WORKS
        printf("update family details.\n");
        int famid;
        printf("\nEnter your family ID to update details: ");
        scanf("%d", &famid);

        family_tree_node *update_family_node = find_node_in_fam_tree(fam_ptr, famid);

        if (update_family_node != NULL)
        {
            char familyname[NAME];
            printf("Enter updated family name: ");
            scanf("%s", familyname);
            strcpy(update_family_node->family_data_node.family_name, familyname);

            printf("Enter updated total family income: ");
            float newincome;
            scanf("%f", &newincome);
            update_family_node->family_data_node.total_family_income = newincome;

            printf("Enter updated family monthly expense: ");
            float expense;
            scanf("%f", &expense);
            update_family_node->family_data_node.monthly_expense = expense;
        }
        else
        {
            printf("Error: Family not found.\n");
        }
    }
    else if (choice == 3)
    {
        // GIVING GARBAGE VALUES WHEN DELETING A MIDDLE NODE IN USER (IN MEMBER LIST OF FAM ) ALSO WHEN (WE DELETE THE NODE OF USER / FAM )
        printf("Delete an individual.\n");
        int userid;
        printf("Enter the user ID to delete: ");
        scanf("%d", &userid);

        user_tree_node *user_node = find_user_node(user_ptr, userid);

        if (user_node != NULL)
        {
            // Find the family containing this user
            family_tree_node *users_family = find_users_family(fam_ptr, userid);

            if (users_family != NULL)
            {
                // Remove user from family and compact the array
                int found = 0;
                int i = 0;

                // Find and remove the user from family
                while (i < MAX_MEMBERS && !found)
                {
                    if (users_family->family_data_node.family_members[i] != NULL &&
                        users_family->family_data_node.family_members[i]->user_data_node.user_id == userid)
                    {

                        // Compact the array
                        int j = i;
                        while (j < MAX_MEMBERS - 1)
                        {
                            users_family->family_data_node.family_members[j] =
                                users_family->family_data_node.family_members[j + 1];
                            j++;
                        }
                        users_family->family_data_node.family_members[MAX_MEMBERS - 1] = NULL;
                        found = 1;

                        // Check if family is now empty
                        int member_count = 0;
                        int k = 0;
                        while (k < MAX_MEMBERS)
                        {
                            if (users_family->family_data_node.family_members[k] != NULL)
                            {
                                member_count++;
                            }
                            k++;
                        }

                        if (member_count == 0)
                        {
                            printf("Deleting family %s (ID: %d) as it has no members left.\n",
                                   users_family->family_data_node.family_name,
                                   users_family->family_data_node.family_id);
                            delete_fam_node(fam_ptr, users_family->family_data_node.family_id);
                        }
                    }
                    i++;
                }
            }

            // Delete user from user tree
            delete_user_node(user_ptr, userid);
            printf("User node deleted successfully.\n");
        }
        else
        {
            printf("User does not exist.\n");
        }
    }
    else if (choice == 4)
    {
        // delete a family
        // WORKS
        int famid;
        printf("Enter the family ID to delete: ");
        scanf("%d", &famid);

        family_tree_node *family_found = find_node_in_fam_tree(fam_ptr, famid);

        if (family_found != NULL)
        {
            delete_fam_node(family_found, family_found->family_data_node.family_id);

            printf("\nFamily and members deleted.\n");
        }
        else
        {
            printf("\nFamily does not exist!\n");
        }
    }
}
/* Given a non-empty binary search tree, return the
   node with minimum key value found in that tree.
   Note that the entire tree does not need to be
   searched. */
expense_tree_node *minValueNode_expense(expense_tree_node *node)
{
    expense_tree_node *current = node;

    /* loop down to find the leftmost leaf */
    while (current->left != NULL)
        current = current->left;

    return current;
}

// Recursive function to delete a node with given key
// from subtree with given root. It returns root of
// the modified subtree.
expense_tree_node *delete_expense(expense_tree_node *root, int key)
{
    // STEP 1: PERFORM STANDARD BST DELETE

    if (root == NULL)
        return root;

    // If the key to be deleted is smaller than the
    // root's key, then it lies in left subtree
    if (key < root->expense_data_node.expense_id)
        root->left = delete_expense(root->left, key);

    // If the key to be deleted is greater than the
    // root's key, then it lies in right subtree
    else if (key > root->expense_data_node.expense_id)
        root->right = delete_expense(root->right, key);

    // if key is same as root's key, then This is
    // the node to be deleted
    else
    {
        // node with only one child or no child
        if ((root->left == NULL) || (root->right == NULL))
        {
            expense_tree_node *temp = root->left ? root->left : root->right;

            // No child case
            if (temp == NULL)
            {
                temp = root;
                root = NULL;
            }
            else               // One child case
                *root = *temp; // Copy the contents of
                               // the non-empty child
            free(temp);
        }
        else
        {
            // node with two children: Get the inorder
            // successor (smallest in the right subtree)
            expense_tree_node *temp = minValueNode_expense(root->right);

            // Copy the inorder successor's data to this node
            root->expense_data_node.expense_id = temp->expense_data_node.expense_id;

            // Delete the inorder successor
            root->right = delete_expense(root->right, temp->expense_data_node.expense_id);
        }
    }

    // If the tree had only one node then return
    if (root == NULL)
        return root;

    // STEP 2: UPDATE HEIGHT OF THE CURRENT NODE
    root->height = 1 + find_max(height_expense(root->left),
                                height_expense(root->right));

    // STEP 3: GET THE BALANCE FACTOR OF THIS NODE (to
    // check whether this node became unbalanced)
    int balance = getBalance_exp(root);

    // If this node becomes unbalanced, then there are 4 cases

    // Left Left Case
    if (balance > 1 && getBalance_exp(root->left) >= 0)
        return rightRotate_exp(root);

    // Left Right Case
    if (balance > 1 && getBalance_exp(root->left) < 0)
    {
        root->left = leftRotate_exp(root->left);
        return rightRotate_exp(root);
    }

    // Right Right Case
    if (balance < -1 && getBalance_exp(root->right) <= 0)
        return leftRotate_exp(root);

    // Right Left Case
    if (balance < -1 && getBalance_exp(root->right) > 0)
    {
        root->right = rightRotate_exp(root->right);
        return leftRotate_exp(root);
    }

    return root;
}

void update_delete_expense(int user_id, int expense_id, int choice, user_tree_node *user_tree)
{

    user_tree_node *user_exists = find_user_node(user_tree, user_id);
    if (user_exists != NULL)
    {
        LL_expense_node *temp = user_exists->user_data_node.monthly_indiv_expenses;
        expense_tree_node *root = temp->tree_root;
        expense_tree_node *expense_found;

        while (temp != NULL && expense_found == NULL)
        {
            expense_found = find_exp_node(root, expense_id);
            if (expense_found == NULL)
            {
                temp = temp->next;
            }
            root = temp->tree_root;
        }

        if (expense_found != NULL)
        {
            if (choice == 1)
            {
                // update the expense
                printf("Update the expenses.\n");
                printf("Enter new amount: ");

                scanf("%f", &expense_found->expense_data_node.expense_amount);
                printf("Enter new expense category: ");

                scanf("%d", &expense_found->expense_data_node.category);
                printf("Expense updated successfully!\n");
                print_expense_tree(root, 0);
            }
            else if (choice == 2)
            {
                printf("delete the expenses .\n");

                delete_expense(root, expense_id);
                print_expense_tree(root, 0);
                printf("That expense is deleted .\n");
            }
        }
        else
        {
            printf("No changes made.Since expense hasnt been found .\n");
        }
    }
    else
    {
        printf("User doesnt exists hence couldnt update the expense.\n");
    }
}
// e=helper function to visit each nc=ode and calculate the total yearly family income

float total_family_monthly_expense(expense_tree_node *root)
{
    if (root == NULL)
    {
        return 0.0;
    }
    return root->expense_data_node.expense_amount +
           total_family_monthly_expense(root->left) +
           total_family_monthly_expense(root->right);
}
// function to print total family expenses
void get_total_expense(family_tree_node *fam_ptr, int family_id)
{
    family_tree_node *family_found = find_node_in_fam_tree(fam_ptr, family_id);

    if (family_found != NULL)
    {
        float total_yearly_expense = 0;
        for (int i = 0; i < MAX_MEMBERS; i++)
        {
            if (family_found->family_data_node.family_members[i] != NULL)
            {
                // Sum expenses across all months
                LL_expense_node *month_exp = family_found->family_data_node.family_members[i]->user_data_node.monthly_indiv_expenses;
                while (month_exp != NULL)
                {
                    total_yearly_expense += total_family_monthly_expense(month_exp->tree_root);
                    month_exp = month_exp->next;
                }
            }
        }
        printf("The income of the family is %f\n", family_found->family_data_node.total_family_income);
        printf("The total yearly expense for the family id : %d is $%f\n", family_id, total_yearly_expense);

        float diff = total_yearly_expense - (family_found->family_data_node.total_family_income);
        if (diff < 0)
        {
            printf("the difference surpasses the total family yearly income.\n");
        }
        else
        {
            printf("the yearly expense is within the total family income.\n");
        }

        printf("the difference is : %f ", diff);
    }
    else
    {
        printf("family doesnt exist.\n");
    }
}
// get categorical month for a user for one single month
float get_user_categorical_month(expense_tree_node *expense_ptr, expense_category category)
{
    if (expense_ptr == NULL)
    {
        return 0.0;
    }

    float sum = 0.0;

    // Check current node
    if (expense_ptr->expense_data_node.category == category)
    {
        sum += expense_ptr->expense_data_node.expense_amount;
    }

    // Recursively check left and right subtrees
    sum += get_user_categorical_month(expense_ptr->left, category);
    sum += get_user_categorical_month(expense_ptr->right, category);

    return sum;
}
// function 7
// print the total family expense in a particular category
// print individual contributions(sum) and print in descending order(most contro first)
// Recursive helper function to search expense tree

void get_categorical_expense(expense_category category, int familyid, family_tree_node *family_ptr)
{
    float contribution[MAX_MEMBERS] = {0};
    family_tree_node *found_family = find_node_in_fam_tree(family_ptr, familyid);

    if (found_family != NULL)
    {
        // family is present
        int sum = 0;
        for (int i = 0; i < MAX_MEMBERS; i++)
        {
            if (found_family->family_data_node.family_members[i] != NULL)
            {
                LL_expense_node *temp = found_family->family_data_node.family_members[i]->user_data_node.monthly_indiv_expenses;
                float user_cat_mnth = 0;
                while (temp != NULL)
                {
                    expense_tree_node *root_temp = temp->tree_root;
                    user_cat_mnth += get_user_categorical_month(root_temp, category);
                    temp = temp->next;
                }
                contribution[i] = user_cat_mnth;
            }
        }

        // sort function
        int sorted = 0;
        for (int i = 0; i < MAX_MEMBERS - 1 && sorted == 0; i++)
        {
            sorted = 1;
            for (int j = 0; j < MAX_MEMBERS - i - 1; j++)
            {
                if (contribution[j] > contribution[j + 1])
                {
                    float temp = contribution[j];
                    contribution[j] = contribution[j + 1];
                    contribution[j + 1] = temp;
                    sorted = 0;
                }
            }
        }
        float fam_contr = 0.0;
        for (int i = MAX_MEMBERS - 1; i >= 0; i--)
        {
            printf("the contribution is : %f \n", contribution[i]);
            fam_contr += contribution[i];
        }
        printf("The contribution for the whole family is %f ", fam_contr);
    }
    else
    {
        printf("family doesnt exits so no catgorical expense.\n");
    }
}
// function 8
// track all expenses in a family and find teh hifghest expense day
void search_expense_tree(expense_tree_node *node, int *highest_date, float *highest_amount)
{
    if (node == NULL)
    {
        return;
    }

    // Check left subtree
    search_expense_tree(node->left, highest_date, highest_amount);

    // Check current node
    if (node->expense_data_node.expense_amount > *highest_amount)
    {
        *highest_amount = node->expense_data_node.expense_amount;
        *highest_date = node->expense_data_node.date_of_expense;
    }

    // Check right subtree
    search_expense_tree(node->right, highest_date, highest_amount);
}
void get_highest_expense_day(int fam_id, family_tree_node *family_ptr)
{
    // 1. Check if family exists
    family_tree_node *found_family = find_node_in_fam_tree(family_ptr, fam_id);
    if (found_family == NULL)
    {
        printf("Family doesn't exist and hence can't track the highest expense day.\n");
    }
    else
    {
        // Variables to track highest expense
        int highest_date = 0;
        float highest_amount = 0.0;

        // 2. Check all family members
        for (int i = 0; i < MAX_MEMBERS; i++)
        {
            if (found_family->family_data_node.family_members[i] != NULL)
            {
                user_tree_node *current_user = found_family->family_data_node.family_members[i];

                // Traverse through all months (12 months)
                LL_expense_node *current_month = current_user->user_data_node.monthly_indiv_expenses;
                while (current_month != NULL)
                {
                    // Recursively search the expense tree for this month
                    search_expense_tree(current_month->tree_root, &highest_date, &highest_amount);
                    current_month = current_month->next;
                }
            }
        }

        // Print the result
        if (highest_amount > 0)
        {
            // Extract date components (assuming DDMM format)

            int month = (highest_date % 100);
            int day = highest_date / 100;

            printf("\nHighest Expense Day for Family %d:\n", fam_id);
            printf("Date: %02d/%02d\n", day, month);
            printf("Amount Spent: $%.2f\n", highest_amount);
        }
        else
        {
            printf("No expenses found for family %d.\n", fam_id);
        }
    }
}
// function 9
// total individual expense for a month
// print individual expense for each category in descending order
void get_individual_expense(int user_id, int month, user_tree_node *user_tree)
{
    user_tree_node *user_exists = find_user_node(user_tree, user_id);

    if (user_exists != NULL)
    {
        LL_expense_node *month_node = user_exists->user_data_node.monthly_indiv_expenses;
        if (month_node != NULL)
        {
            while (month_node != NULL && month_node->month != month)
            {
                month_node = month_node->next;
            }
        }
        expense_tree_node *tree = month_node->tree_root;
        float monthly_exp = total_family_monthly_expense(tree);
        float category_totals[5] = {0};
        const char *category_names[] = {
            "Rent", "Utilities", "Groceries", "Transportation", "Entertainment"};
        int sorted = 0;
        for (int i = 0; i < 5; i++)
        {
            category_totals[i] = get_user_categorical_month(tree, i);
        }
        for (int i = 0; i < 5 && sorted == 0; i++)
        {
            sorted = 1;
            for (int j = 0; j < 5 - i - 1; j++)
            {
                if (category_totals[j + 1] > category_totals[j])
                {
                    // Swap amounts
                    float temp = category_totals[j];
                    category_totals[j] = category_totals[j + 1];
                    category_totals[j + 1] = temp;

                    // Swap names to keep them aligned
                    const char *temp_name = category_names[j];
                    category_names[j] = category_names[j + 1];
                    category_names[j + 1] = temp_name;
                    sorted = 0;
                }
            }
        }

        printf("\nCategory-wise Expenses (Descending Order):\n");
        for (int i = 0; i < 5; i++)
        {
            printf("%d. %-15s: %.2f\n", i + 1, category_names[i], category_totals[i]);
        }
        /*
          printf("The total individual expense for the month %d is : %f \n", month, monthly_exp);
        printf("The expense for Rent is : %f\n", get_user_categorical_month(tree, 0));
        printf("The expense for Utilities is : %f\n", get_user_categorical_month(tree, 1));
        printf("The expense for Groceries is : %f\n", get_user_categorical_month(tree, 2));
        printf("The expense for stationaries is : %f\n", get_user_categorical_month(tree, 3));
        printf("The expense for Leisures is : %f\n", get_user_categorical_month(tree, 4));
        */

        // print all the expesne in desending order category wise for that month
    }
    else
    {
        printf("the user doesnt exist .\n");
    }
}

// function 10
// Helper function to check and print expenses in a tree that fall within date range
void check_expense_tree(expense_tree_node *node, int date1, int date2)
{
    if (node == NULL)
        return;

    // Check left subtree (lower IDs)
    check_expense_tree(node->left, date1, date2);

    // Check current node
    int expense_date = node->expense_data_node.date_of_expense;
    // Assuming date format is DDMM
    int day_month = expense_date;

    if (day_month >= date1 && day_month <= date2)
    {
        const char *categories[] = {"Rent", "Utilities", "Groceries",
                                    "Transportation", "Entertainment"};
        printf("  [%02d/%02d] %-15s: $%.2f (Expense ID: %d)\n",
               day_month / 100, day_month % 100,
               categories[node->expense_data_node.category],
               node->expense_data_node.expense_amount,
               node->expense_data_node.expense_id);
    }

    // Check right subtree (higher IDs)
    check_expense_tree(node->right, date1, date2);
}

// Helper function to traverse user tree and print expenses in range
void print_expenses_in_range(user_tree_node *user, int date1, int date2, int month1, int month2)
{
    if (user == NULL)
        return;

    // Process left subtree
    print_expenses_in_range(user->left, date1, date2, month1, month2);

    // Process current user
    printf("\nUser: %s (ID: %d)\n", user->user_data_node.user_name, user->user_data_node.user_id);

    // Check only relevant months (optimization)
    LL_expense_node *month_exp = user->user_data_node.monthly_indiv_expenses;
    while (month_exp != NULL)
    {
        int current_month = month_exp->month;

        // Only check months that could contain dates in our range
        if (current_month >= month1 && current_month <= month2)
        {
            check_expense_tree(month_exp->tree_root, date1, date2);
        }
        month_exp = month_exp->next;
    }

    // Process right subtree
    print_expenses_in_range(user->right, date1, date2, month1, month2);
}
// function 11
// Helper: Get minimum node in a tree
expense_tree_node *get_min_node(expense_tree_node *node)
{
    while (node && node->left)
    {
        node = node->left;
    }

    return node;
}

// Helper: Get maximum node in a tree
expense_tree_node *get_max_node(expense_tree_node *node)
{
    while (node && node->right)
    {
        node = node->right;
    }

    return node;
}
bool print_in_range(expense_tree_node *exp_ptr, int expid1, int expid2)
{
    bool ret_val = false;
    if (exp_ptr == NULL)
    {
        ret_val = false;
    }
    else
    {
        if (exp_ptr->expense_data_node.expense_id > expid1)
        {
            ret_val = print_in_range(exp_ptr->left, expid1, expid2);
        }

        if (exp_ptr->expense_data_node.expense_id >= expid1 &&
            exp_ptr->expense_data_node.expense_id <= expid2)
        {

            const char *categories[] = {"Rent", "Utilities", "Groceries",
                                        "Transportation", "Entertainment"};

            printf("ID: %-5d | Month: %-2d | %-15s | $%-8.2f | Date: %d\n",
                   exp_ptr->expense_data_node.expense_id,
                   exp_ptr->expense_data_node.date_of_expense % 100, // Extract month
                   categories[exp_ptr->expense_data_node.category],
                   exp_ptr->expense_data_node.expense_amount,
                   exp_ptr->expense_data_node.date_of_expense);
            ret_val = true;
        }

        if (exp_ptr->expense_data_node.expense_id < expid2)
        {
            ret_val = print_in_range(exp_ptr->right, expid1, expid2);
        }
    }
    return ret_val;
}
// print the expenses between the given period
// have to go in each and expenses of each user
void Get_expense_in_period(int date1, int date2, user_tree_node *user_tree)
{

    // Extract months from dates
    int month1 = date1 % 100;
    int month2 = date2 % 100;

    printf("\nExpenses between %02d/%02d and %02d/%02d:\n",
           date1 / 100, date1 % 100, date2 / 100, date2 % 100);
    printf("========================================\n");

    // Traverse all users (in-order traversal of user tree)
    print_expenses_in_range(user_tree, date1, date2, month1, month2);
}
// function 11
// print all the expenses within the given expense id range for a particular user
void get_expense_in_range(int exp_id1, int exp_id2, int user_id, user_tree_node *user_ptr)
{

    user_tree_node *user_found = find_user_node(user_ptr, user_id);
    if (user_found != NULL)
    {
        LL_expense_node *temp = user_found->user_data_node.monthly_indiv_expenses;
        int done = 0;
        int found_any = 0;
        while (temp && done == 0)
        {
            expense_tree_node *min_node = get_min_node(temp->tree_root);
            expense_tree_node *max_node = get_max_node(temp->tree_root);
            if (max_node != NULL && max_node->expense_data_node.expense_id < exp_id1)
            {
                temp = temp->next;
            }
            else if (min_node && min_node->expense_data_node.expense_id > exp_id2)
            {

                done = 1;
            }
            else if (print_in_range(temp->tree_root, exp_id1, exp_id2))
            {
                found_any = 1;
            }
            temp = temp->next;
        }
        if (found_any == 0)
        {
            printf("No expenses found in range %d to %d\n", exp_id1, exp_id2);
        }
    }
    else
    {
        printf("User Not Found!\n");
    }
}
int main()
{
    user_tree_node *user_tree_ptr = NULL;
    family_tree_node *family_tree_ptr = NULL;
    addUser(3, "reeti", 89.00, 2, &user_tree_ptr, &family_tree_ptr);
    // addUser(3, "reeti", 89.00, 2, &user_tree_ptr, &family_tree_ptr);
    //  addUser(4, "sara", 90.00, 2, &user_tree_ptr, &family_tree_ptr);

    // addUser(5, "teri", 70.90, 2, &user_tree_ptr, &family_tree_ptr);
    // addUser(6, "teri", 70.90, 2, &user_tree_ptr, &family_tree_ptr);
    // addUser(7, "teri", 70.90, 23, &user_tree_ptr, &family_tree_ptr);
    //  addUser(8, "teri", 70.90, 22, &user_tree_ptr, &family_tree_ptr);
    //    addUser(6, "Shriyans", 123.00, 7, &user_tree_ptr, &family_tree_ptr);

    //  Add_expense(5, 5, 2, 789.00, 1111, &user_tree_ptr);
    // Add_expense(2, 6, 2, 890.00, 1209, &user_tree_ptr);
    // Add_expense(2, 5, 0, 890.00, 1209, &user_tree_ptr);
    Add_expense(8, 3, 3, 1.00, 911, &user_tree_ptr);
    Add_expense(9, 3, 3, 2.00, 1201, &user_tree_ptr);
    Add_expense(11, 3, 2, 3.00, 912, &user_tree_ptr);
    Add_expense(12, 3, 3, 4.00, 1211, &user_tree_ptr);
    Add_expense(13, 3, 4, 5.00, 909, &user_tree_ptr);
    Add_expense(14, 3, 4, 6.00, 1212, &user_tree_ptr);
    // PrintExpenseTreeForMonth(user_tree_ptr, 5, 9);

    // update_or_delete_individual_family_details(3, user_tree_ptr, family_tree_ptr);
    // update_delete_expense(5, 5, 1, user_tree_ptr);
    // get_total_expense(family_tree_ptr, 2);
    // get_categorical_expense(2, 2, family_tree_ptr);
    // get_highest_expense_day(2, family_tree_ptr);
    // get_individual_expense(3, 11, user_tree_ptr);
    // Get_expense_in_period(111, 912, user_tree_ptr);
    printf("get expense in range function coming up.");
    get_expense_in_range(9, 13, 3, user_tree_ptr);
    //  WORKING
    // print_family_tree(family_tree_ptr);
    // print_user_tree(user_tree_ptr);

    return 0;
}