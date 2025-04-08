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