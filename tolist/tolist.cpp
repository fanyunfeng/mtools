#include <stdio.h>

struct Node {
public:
    int value;
    Node* left;
    Node* right;

public:
    Node(int v, Node* l=NULL, Node* r=NULL):
        value(v),left(l),right(r){
    }
};


Node* toList(Node* tree, Node** list, Node* pre){
    if(tree->left){
        Node* ret = toList(tree->left, list, pre);

        list = &ret->right;
        pre = ret;
    }

    *list = tree;
    tree->left = pre;

    Node* right = tree->right;

    if(right){
        return toList(right, &tree->right, tree);
    }

    tree->right = NULL;
    return *list;
}

void printList(Node* list){
    Node* p = list;

    while(p){
        printf("%d ", p->value);
        p = p->right;
    }
}

void printTree(Node* list){
    printf("(");

    if(list != NULL){

        if(list->left){
            printTree(list->left);
        }

        printf(" %d ", list->value);

        if(list->right){
            printTree(list->right);
        }
    }

    printf(")");
}

#define N new Node

int main(){
    Node* tree = N(10, N(8, N(6), N(9)), N(12, N(11), N(19)));

    Node* list = NULL;

    printTree(tree);
    printf("\n");

    toList(tree, &list, NULL);

    printList(list);
    printf("\n");
}
