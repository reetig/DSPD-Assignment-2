#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define NAME 100
#define MAX_MEMBERS 4
typedef enum
{
    rent,
    utilities,
    groceries,
    transportation,
    entertainment
} expense_category;

typedef struct expense_node
{
    int expense_id;
    int user_id;
    expense_category category;
    float expense_amount;
    int date_of_expense;
} expense;

typedef struct individual_node
{
    int user_id;
    char user_name[NAME];
    float user_income;
    LL_expense_node *monthly_indiv_expenses;
    // linked list of 12 expense trees each representing expense of each user in each month

} individual;

typedef struct family_node
{
    int family_id;
    char family_name[NAME];
    user_tree_node *family_members[MAX_MEMBERS]; // pointing towards a usernode
    float total_family_income;                   // calculated as sum of individual incomes
    float monthly_expense;                       // calculated as sum of all expense by family members in a month in all categories

} family;

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
typedef struct AVL_tree_family_node
{
    family family_data_node;
    struct AVL_tree_family_node *right;
    struct AVL_tree_family_node *left;
    int height;
} family_tree_node;
typedef struct AVL_tree_user_node
{
    individual user_data_node;
    struct AVL_tree_user_node *right;
    struct AVL_tree_user_node *left;
    int height;
} user_tree_node;
family_tree_node *family_tree_ptr = NULL; // globalpointer
user_tree_node *user_tree_ptr = NULL;

family_tree_node *create_family_node()
{
    family_tree_node *nptr = (family_tree_node *)malloc(sizeof(family_tree_node));
    nptr->right = NULL;
    nptr->left = NULL;
    nptr->height = 1;
    return nptr;
}

user_tree_node *create_individual_node(int user_id, float income, const char *user_name)
{
    user_tree_node *new_user = (user_tree_node *)malloc(sizeof(user_tree_node));
    if (new_user != NULL)
    {
        new_user->user_data_node.user_id = user_id;
        new_user->user_data_node.user_income = income;
        strcpy(new_user->user_data_node.user_name, user_name);
        new_user->user_data_node.monthly_indiv_expenses = NULL;
        new_user->height = 1;
        new_user->right = NULL;
        new_user->left = NULL;

        // Initialize the linked list of 12 months (all empty trees)
        LL_expense_node *current = NULL;
        for (int i = 0; i < 12; i++)
        {
            LL_expense_node *new_node = (LL_expense_node *)malloc(sizeof(LL_expense_node));
            if (new_node == NULL)
            {
                // Handle memory allocation failure
                while (new_user->user_data_node.monthly_indiv_expenses)
                {
                    printf("memory allocation for expenses failed in heap.hence returning NULL\n");
                    LL_expense_node *temp = new_user->user_data_node.monthly_indiv_expenses;
                    new_user->user_data_node.monthly_indiv_expenses = new_user->user_data_node.monthly_indiv_expenses->next;
                    free(temp);
                }
                free(new_user);
                // return NULL;
            }
            else
            {

                new_node->month = i + 1; // Months 1-12
                new_node->tree_root = NULL;
                new_node->next = NULL;

                if (current == NULL)
                {
                    new_user->user_data_node.monthly_indiv_expenses = new_node;
                    current = new_node;
                }
                else
                {
                    current->next = new_node;
                    current = current->next;
                }
            }
        }

        return new_user;
    }
    else
    {
        printf("failed to created individual node \n");
        return NULL;
    }
}
// helper function in insert family tree
int height_family(family_tree_node *N)
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
family_tree_node *rightRotate(family_tree_node *y)
{
    family_tree_node *x = y->left;
    family_tree_node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = find_max(height(y->left),
                         height(y->right)) +
                1;
    x->height = find_max(height(x->left),
                         height(x->right)) +
                1;

    // Return new root
    return x;
}

// A utility function to left rotate subtree rooted with x
// See the diagram given above.
family_tree_node *leftRotate(family_tree_node *x)
{
    family_tree_node *y = x->right;
    family_tree_node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    //  Update heights
    x->height = find_max(height(x->left),
                         height(x->right)) +
                1;
    y->height = find_max(height(y->left),
                         height(y->right)) +
                1;

    // Return new root
    return y;
}

// Get Balance factor of node N
int getBalance(family_tree_node *N)
{
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

// Recursive function to insert a key in the subtree rooted
// with node and returns the new root of the subtree.
family_tree_node *insert_family_in_tree(family_tree_node *node, int key)
{
    /* 1.  Perform the normal BST insertion */
    if (node == NULL)
        return create_family_node();

    if (key < node->family_data_node.family_id)
        node->left = insert_family_in_tree(node->left, key);
    else if (key > node->family_data_node.family_id)
        node->right = insert_family_in_tree(node->right, key);
    else
    {
        printf("duplicate id found hence family cant be formed.\n");
        return node; // returning the particular node where the id was found
    } // Equal keys are not allowed in BST

    /* 2. Update height of this ancestor node */
    node->height = 1 + find_max(height(node->left),
                                height(node->right));

    /* 3. Get the balance factor of this ancestor
          node to check whether this node became
          unbalanced */
    int balance = getBalance(node);

    // If this node becomes unbalanced, then
    // there are 4 cases

    // Left Left Case
    if (balance > 1 && key < node->left->family_data_node.family_id)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && key > node->right->family_data_node.family_id)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && key > node->left->family_data_node.family_id)
    {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->family_data_node.family_id)
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    /* return the (unchanged) node pointer */
    return node;
}
// helper function for create family

family_tree_node *create_family(int family_id, char *fam_name, float family_income, float monthly_expense, family_tree_node *family_tree_ptr)
{
    family_tree_node *new_family = create_family_node();
    if (new_family != NULL)
    {
        new_family->family_data_node.family_id = family_id;
        strcpy(new_family->family_data_node.family_name, fam_name);
        new_family->family_data_node.total_family_income = family_income;
        new_family->family_data_node.monthly_expense = monthly_expense;

        for (int i = 0; i < MAX_MEMBERS; i++)
        {
            // users idhar add hinge ya nahi dekhna hai
            new_family->family_data_node.family_members[i] = NULL; // if user_ids are in sorted order
        }
        family_tree_node *family_tree_ptr = insert_family_in_tree(family_tree_ptr, family_id);
    }
    else
    {
        printf("heap is full hance family cannot be created hance returning null.\n");
    }
    return new_family;
}
// helper function for add user function
family_tree_node *find_node_in_fam_tree(int family_id)
{
    // Base Cases: root is null or key is
    // present at root
    if (family_tree_ptr == NULL || family_tree_ptr->family_data_node.family_id == family_id)
        return family_tree_ptr;

    // Key is greater than root's key
    if (family_tree_ptr->family_data_node.family_id < family_id)
        return search(family_tree_ptr->right, family_id);

    // Key is smaller than root's key
    return search(family_tree_ptr->left, family_id);
}

// find the user node , finding the user in the user avl tree

user_tree_node *find_user_node(int user_id)
{
    int found = 0;
    // search in avl tree

    // Base Cases: root is null or key is
    // present at root
    if (user_tree_ptr == NULL || user_tree_ptr->user_data_node.user_id == user_id)
        return user_tree_ptr;

    // Key is greater than root's key
    if (user_tree_ptr->user_data_node.user_id < user_id)
        return search(user_tree_ptr->right, user_id);

    // Key is smaller than root's key
    return search(user_tree_ptr->left, user_id);
}
// helper function to insert the user node
int height_user_id(user_tree_node *N)
{
    if (N == NULL)
        return 0;
    return N->height;
}

// A utility function to right rotate subtree rooted with y
// See the diagram given above.
user_tree_node *rightRotate(user_tree_node *y)
{
    user_tree_node *x = y->left;
    user_tree_node *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = find_max(height_user_di(y->left),
                         height(y->right)) +
                1;
    x->height = find_max(height(x->left),
                         height(x->right)) +
                1;

    // Return new root
    return x;
}

// A utility function to left rotate subtree rooted with x
// See the diagram given above.
user_tree_node *leftRotate(user_tree_node *x)
{
    user_tree_node *y = x->right;
    user_tree_node *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    //  Update heights
    x->height = find_max(height(x->left),
                         height(x->right)) +
                1;
    y->height = find_max(height(y->left),
                         height(y->right)) +
                1;

    // Return new root
    return y;
}

// Get Balance factor of node N
int getBalance(user_tree_node *N)
{
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

// Recursive function to insert a key in the subtree rooted
// with node and returns the new root of the subtree.
user_tree_node *insert_user_node(user_tree_node *node, int key)
{
    /* 1.  Perform the normal BST insertion */
    if (node == NULL)
        return create_family_node();

    if (key < node->user_data_node.user_id)
        node->left = insert(node->left, key);
    else if (key > node->user_data_node.user_id)
        node->right = insert(node->right, key);
    else // Equal keys are not allowed in BST
        return node;

    /* 2. Update height of this ancestor node */
    node->height = 1 + find_max(height(node->left),
                                height(node->right));

    /* 3. Get the balance factor of this ancestor
          node to check whether this node became
          unbalanced */
    int balance = getBalance(node);

    // If this node becomes unbalanced, then
    // there are 4 cases

    // Left Left Case
    if (balance > 1 && key < node->left->user_data_node.user_id)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && key > node->right->user_data_node.user_id)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && key > node->left->user_data_node.user_id)
    {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->user_data_node.user_id)
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    /* return the (unchanged) node pointer */
    return node;
}

void addUser(int user_id, char *name, float income, int family_id)
{
    // assuming each user belongs to a family so a if family exists just add user or family doesnt exists make a family and add the user in it

    family_tree_node *family_found = find_node_in_fam_tree(family_id);
    if (family_found == NULL)
    {
        // family doesnt exists hence create a family
        // first we create user and then family
        printf("family doesnt exist and hence you have to enter the family details(family name, family income, family_monthly expense).\n");
        char fam_name[NAME];
        printf("enter family name.\n");
        scanf("%s", fam_name);
        float fam_income;
        printf("enter family income.\n");
        float fam_monthly_exp;
        printf("enter family monthly expense.\n");
        scanf("%f", fam_monthly_exp);
        family_tree_node *new_family = create_family(family_id, fam_name, fam_income, fam_monthly_exp, family_tree_ptr);
        new_family->family_data_node.family_members[0] = create_individual_node(user_id, income, name);
        insert_user_node(new_family->family_data_node.family_members[0], user_id); // insert the user in the tree
        if (new_family->family_data_node.family_members[0] != NULL)
        {
            printf("user successfully added.\n");
        }
        else
        {
            printf("not enough space for user.\n");
        }
    }
    else
    {
        // family exists

        int user_index;
        int found = 0;
        for (int i = 0; i < MAX_MEMBERS && found == 0; i++)
        {
            if (family_found->family_data_node.family_members[i] > user_id)
            {
                user_index = i;
                found = 1;
            }
        }
        if (user_index < MAX_MEMBERS - 1)
        {
            for (int i = MAX_MEMBERS - 1; i > user_index; i--)
            {
                family_found->family_data_node.family_members[i] = family_found->family_data_node.family_members[i - 1];
            }
            family_found->family_data_node.family_members[user_index] = create_individual_node(user_id, income, name);
            printf("user added successfully.\n");
        }
        else
        {
            printf("Family Full No more members can be added. Hence user cant be added as a user must belong to one of the family.\n");
        }
    }
}
// insert expense functions
int height_expense(expense_tree_node *N)
{
    if (N == NULL)
        return 0;
    return N->height;
}

// A utility function to right rotate subtree rooted with y
// See the diagram given above.
expense_tree_node *rightRotate(expense_tree_node *y)
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
expense_tree_node *leftRotate(expense_tree_node *x)
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
int getBalance(expense_tree_node *N)
{
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

// Recursive function to insert a key in the subtree rooted
// with node and returns the new root of the subtree.
expense_tree_node *insert_user_node(expense_tree_node *node, int key)
{
    /* 1.  Perform the normal BST insertion */
    if (node == NULL)
        return create_family_node();

    if (key < node->expense_data_node.expense_id)
        node->left = insert(node->left, key);
    else if (key > node->expense_data_node.expense_id)
        node->right = insert(node->right, key);
    else // Equal keys are not allowed in BST
        return node;

    /* 2. Update height of this ancestor node */
    node->height = 1 + find_max(height(node->left),
                                height(node->right));

    /* 3. Get the balance factor of this ancestor
          node to check whether this node became
          unbalanced */
    int balance = getBalance(node);

    // If this node becomes unbalanced, then
    // there are 4 cases

    // Left Left Case
    if (balance > 1 && key < node->left->expense_data_node.expense_id)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && key > node->right->expense_data_node.expense_id)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && key > node->left->expense_data_node.expense_id)
    {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && key < node->right->expense_data_node.expense_id)
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    /* return the (unchanged) node pointer */
    return node;
}

//  create expense node

void Add_expense(int exp_id, int user_id, expense_category cat, float amount, int date)
{
    user_tree_node *found_user = find_user_node(user_id);
    if (found_user != NULL)
    {
        printf("user exists.\n");
        int find_month = date % 100;

        found_user->user_data_node.monthly_indiv_expenses->month = find_month;
        LL_expense_node *temp = found_user->user_data_node.monthly_indiv_expenses;
        // we found the LL of that particual list in that user list
        while (temp != NULL && temp->month != find_month)
        {
            temp = temp->next;
        }
        // temp is the LL containg the expense month tree
        expense_tree_node *temp_root = temp->tree_root;

        expense_tree_node *new_expense = (expense_tree_node *)malloc(sizeof(expense_tree_node));
        new_expense->expense_data_node.category = cat;
        new_expense->expense_data_node.date_of_expense = date;
        new_expense->expense_data_node.expense_amount = amount;
        new_expense->expense_data_node.expense_id = exp_id;
        new_expense->expense_data_node.user_id = user_id;
        new_expense->height = 1; // CHECK IF THE HAIGHT FACTOR I HAVE WRITTEN CORRECTLY
        // insert the expense node in the correct position

        insert_expense_node(exp_id);
    }
    else
    {
        printf("user doesnt exists hence the expense cant be added.\n");
    }
}
// function for deleting a user
// invoking the function in update delete individual so already checked that the parent ptr is not null
void delete_user_node(user_tree_node **parent_ptr)
{
    user_tree_node *r = *parent_ptr;
    user_tree_node *q;

    if (r == NULL)
    {
        printf("ERROR.\n");
    }
    else if (r->left == NULL && r->right == NULL)
    {
        // leaf case
        parent_ptr = NULL;
    }
    else if (r->left != NULL)
    {
        // means r.right is not null
        *parent_ptr = r->right;
    }
    else if (r->right == NULL)
    {
        *parent_ptr = r->left;
    }
    else
    {
        user_tree_node *p;
        q = r->left;
        p = q = r->left;

        while (q->right != NULL)
        {
            p = q;
            q = q->right;
        }
        p->right = q->left;
        q->left = r->left;
        q->right = r->right;
        *parent_ptr = q;
    }
    user_tree_node *temp = r->user_data_node.monthly_indiv_expenses;

    // freeing all the expense nodes of that particular
    while (temp != NULL)
    {
        free(temp);
    }
    free(r);
}

void delete_family(family_tree_node **parent_ptr)
{
    family_tree_node *r = *parent_ptr;
    family_tree_node *q;

    if (r == NULL)
    {
        printf("ERROR.\n");
    }
    else if (r->left == NULL && r->right == NULL)
    {
        // leaf case
        parent_ptr = NULL;
    }
    else if (r->left != NULL)
    {
        // means r.right is not null
        *parent_ptr = r->right;
    }
    else if (r->right == NULL)
    {
        *parent_ptr = r->left;
    }
    else
    {
        user_tree_node *p;
        q = r->left;
        p = q = r->left;

        while (q->right != NULL)
        {
            p = q;
            q = q->right;
        }
        p->right = q->left;
        q->left = r->left;
        q->right = r->right;
        *parent_ptr = q;
    }

    free(r);
}
void update_or_delete_individual_family_details(int choice)
{
    if (choice == 1)
    {
        printf("enter the user id of the user you want to update.\n");
        int id;
        scanf("%d", &id);

        user_tree_node *found_user = find_user_node(id);
        if (found_user != NULL)
        {
            char name[NAME];
            printf("\nEnter the new name you want to keep: ");
            scanf("%s", name);
            free(found_user->user_data_node.user_name); // Free old memory

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
        printf("update family details.\n");
        int famid;
        printf("\nEnter your family ID to update details: ");
        scanf("%d", &famid);

        family_tree_node *update_family_node = find_node_in_fam_tree(famid);

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
        // delete an individual
        printf("delete an individual.\n");
        int userid;
        printf("Enter the user ID to delete: ");
        scanf("%d", &userid);

        user_tree_node *user_node = find_user_node(userid);

        if (user_node != NULL)
        {
            delete_user_node(user_node);
            printf("user node deleted.\n");

            // if in that family only one member is left delete that family
        }
        else
        {
            printf("User does not exist.\n");
        }
    }
    else if (choice == 4)
    {
        // delete a family

        int famid;
        printf("Enter the family ID to delete: ");
        scanf("%d", &famid);

        family_tree_node *family_found = find_node_in_fam_tree(famid);

        if (family_found != NULL)
        {
            delete_family(family_found);

            printf("\nFamily and members deleted.\n");
        }
        else
        {
            printf("\nFamily does not exist!\n");
        }
    }
}

void delete_expense_node(expense_tree_node **parent_ptr)
{
    expense_tree_node *r = *parent_ptr;
    expense_tree_node *q;

    if (r == NULL)
    {
        printf("ERROR.\n");
    }
    else if (r->left == NULL && r->right == NULL)
    {
        // leaf case
        parent_ptr = NULL;
    }
    else if (r->left != NULL)
    {
        // means r.right is not null
        *parent_ptr = r->right;
    }
    else if (r->right == NULL)
    {
        *parent_ptr = r->left;
    }
    else
    {
        expense_tree_node *p;
        q = r->left;
        p = q = r->left;

        while (q->right != NULL)
        {
            p = q;
            q = q->right;
        }
        p->right = q->left;
        q->left = r->left;
        q->right = r->right;
        *parent_ptr = q;
    }

    free(r);
}

void update_delete_expense(int user_id, int expense_id, int date, int choice)
{

    user_tree_node *user_exists = find_user_node(user_id);
    if (user_exists != NULL)
    {
        LL_expense_node *temp = user_exists->user_data_node.monthly_indiv_expenses;
        int month = date % 100;
        while (temp != NULL && temp->month == month)
        {
            temp = temp->next;
        }

        expense_tree_node *root = temp->tree_root;
        expense_tree_node *expense_found = find_expense_node(root, expense_id);

        if (expense_found != NULL)
        {
            if (choice == 1)
            {
                // update the expense
                printf("Update the expenses.\n");
                printf("Enter new amount: ");

                scanf("%f", expense_found->expense_data_node.expense_amount);
                printf("Enter new expense category: ");

                scanf("%s", expense_found->expense_data_node.category);
                printf("Expense updated successfully!\n");
            }
            else if (choice == 2)
            {
                printf("delete the expenses .\n");

                delete_expense_node(expense_found);
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
        return 0;
    }
    return root->expense_data_node.expense_amount +
           total_family_monthly_expense(root->left) +
           total_family_monthly_expense(root->right);
}

// function to print total family expenses
void get_total_expense(int family_id)
{
    family_tree_node *family_found = find_node_in_fam_tree(family_id);

    if (family_found != NULL)
    {
        int total_yearly_expense = 0;
        for (int i = 0; i < MAX_MEMBERS; i++)
        {
            user_tree_node *temp = family_found->family_data_node.family_members[i]->user_data_node.monthly_indiv_expenses->tree_root; // root of each expense tree and i have to visit each node and caluclate its sum
            total_yearly_expense += total_family_monthly_expense(temp);
        }

        int diff = total_yearly_expense - (family_found->family_data_node.total_family_income);
        if (diff < 0)
        {
            printf("the difference surpasses the total family yearly income.\n");
        }
        else
        {
            printf("the yearly expense is within the total family income.\n");
        }

        printf("the difference is %d : ", abs(diff));
    }
    else
    {
        printf("family doesnt exist.\n");
    }
}

// helper function to calculate sum
int sum_of_nodes(expense_tree_node *node, expense_category cat)
{
    if (node == NULL)
        return 0;

    // Calculate sum for left and right subtrees
    int left_sum = sum_of_nodes(node->left, cat);
    int right_sum = sum_of_nodes(node->right, cat);

    // Only add expense_amount if the condition is met
    if (node->expense_data_node.category == cat)
    {
        return node->expense_data_node.expense_amount + left_sum + right_sum;
    }

    return left_sum + right_sum; // Continue traversal without adding expense_amount
}

void get_categorical_expense(expense_category category, int familyid)
{
    family_tree_node *found_family = find_node_in_fam_tree(familyid);

    if (found_family != NULL)
    {
        // family is present
        int sum = 0;
        for (int i = 0; i < MAX_MEMBERS; i++)
        {
            LL_expense_node *temp = found_family->family_data_node.family_members[i]->user_data_node.monthly_indiv_expenses;

            while (temp != NULL)
            {
                expense_tree_node *root_temp = temp->tree_root;
                if (root_temp->expense_data_node.category == category)
                {
                    sum += root_temp->expense_data_node.expense_amount;
                }
            }
        }
    }
    else
    {
        printf("family doesnt exits so no catgorical expense.\n");
    }
}

// Recursive helper function to search expense tree
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
        float highest_amount = 0.0f;

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
    }
}

void get_individual_expense(int user_id, int month, user_tree_node *user_tree)
{
    user_tree_node *user_exists = find_user_node(user_id);

    if (user_exists != NULL)
    {
        expense_tree_node *tree = user_exists->user_data_node.monthly_indiv_expenses->tree_root;
        float monthly_exp = total_family_monthly_expense(tree);
        printf("The total individual expense for the month %d is : %f ", &user_exists->user_data_node.monthly_indiv_expenses->month, &monthly_exp);

        // print all the expesne in desending order category wise for that month
    }
    else
    {
        printf("the user doesnt exist .\n");
    }
}
// assuming date1 < date 2
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

// Helper function to check and print expenses in a tree that fall within date range
void check_expense_tree(expense_tree_node *node, int date1, int date2)
{
    if (node == NULL)
        return;

    // Check left subtree (lower IDs)
    check_expense_tree(node->left, date1, date2);

    // Check current node
    int expense_date = node->expense_data_node.date_of_expense;
    // Assuming date format is DDMM (you may need to adjust this)
    int day_month = expense_date % 10000; // Get last 4 digits (DDMM)

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

// function 11

void get_expense_in_range(int exp_id1, int exp_id2, int user_id, user_tree_node *user_ptr)
{

    user_tree_node *user_found = find_user_node(user_id);
    if (user_found != NULL)
    {
        LL_expense_node *temp = user_found->user_data_node.monthly_indiv_expenses;
        int done = 0;
        while (temp && done == 0)
        {
            if (temp != NULL && user_found->user_data_node.monthly_indiv_expenses->tree_root->expense_data_node.expense_id < exp_id1)
            {
                temp = temp->next;
            }
            else if (temp->tree_root->expense_data_node.expense_id >= exp_id1 && temp->tree_root->expense_data_node.expense_id >= exp_id2)
            {
                // only print all the values if this particulat tree (skipping the greater values)
                done = 1;
            }
            else if (temp->tree_root->expense_data_node.expense_id >= exp_id1 && temp->tree_root->expense_data_node.expense_id < exp_id2)
            {
                // print this whole tree
                temp = temp->next;
            }
            {
            }
        }
    }
}