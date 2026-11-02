#include <iostream>
using namespace std;

struct Node {
    long long ID;
    long long priority;
    long long price;
    long long count;
    long long sum;
    long long minID;
    long long maxID;
    Node *left;
    Node *right;
};

void refresh(Node *node){
    if (node == nullptr){
        return;
    }
    node->count = 1;
    node->sum = node->price;
    if (node->left != nullptr){
        node->sum += node->left->sum;
        node->count += node->left->count;
    }
    if(node->right != nullptr){
        node->sum += node->right->sum;
        node->count += node->right->count;
    }
    if (node->left != nullptr){
        node->minID = node->left->minID;
    }
    else {
        node->minID = node->ID;
    }
    if (node->right != nullptr){
        node->maxID = node->right->maxID;
    }
    else {
        node->maxID = node->ID;
    }
}

void split(Node *node, long long ID, Node *&left, Node *&right){
    if (node == nullptr){
        left = right = nullptr;
    }
    else if (node->ID <= ID){
        split(node->right, ID, node->right, right);
        left = node;
        refresh(left);
    }
    else if (node->ID > ID){
        split(node->left, ID, left, node->left);
        right = node;
        refresh(right);
    }
}

void merge(Node *&node, Node *left, Node *right){
    if (left == nullptr || right == nullptr){
        if (left == nullptr && right == nullptr){
            node = nullptr;
        }
        else if (left == nullptr){
            node = right;
        }
        else if (right == nullptr){
            node = left;
        }
    }
    else if (left->priority > right->priority){
        merge(left->right, left->right, right);
        node = left;
        refresh(node);
    }
    else if (left->priority < right->priority){
        merge(right->left, left, right->left);
        node = right;
        refresh(node);
    }
    else {
        merge(left->right, left->right, right);
        node = left;
        refresh(node);
    }
}
Node* find(Node *node, long long ID){
    if (node == nullptr) {
        return nullptr;
    }
    if (node->ID == ID) {
        return node;
    }
    if (ID > node->ID) {
        return find(node->right, ID);
    }
    else{
        return find(node->left, ID);
    }
}
void erase(Node *&node, long long ID){
    if (node == nullptr){
        return;
    }
    else if (node->ID == ID){
        Node *now = node;
        Node *l = node->left;
        Node *r = node->right;
        merge(node, l, r);
        delete now;
    }
    else if (ID > node->ID){
        erase(node->right, ID);
        refresh(node);
    }
    else if (ID < node->ID){
        erase(node->left, ID);
        refresh(node);
    }
}

void add(Node *&node, Node *fresh){
    Node *found = find(node, fresh->ID);
    if (found != nullptr){
        erase(node, fresh->ID);
        add(node, fresh);
        return;
    }
    if (node == nullptr){
        node = fresh;
        return;
    }
    Node *L = nullptr;
    Node *R = nullptr;
    split(node, fresh->ID, L, R);
    merge(L, L, fresh);
    merge(node, L, R);
}

long long sum(Node *node, long long ID_MIN, long long ID_MAX){
    if (ID_MIN > ID_MAX || node == nullptr){
        return 0;
    }
    else if (node->ID < ID_MIN){
        return sum(node->right, ID_MIN, ID_MAX);
    }
    else if(node->ID > ID_MAX){
        return sum(node->left, ID_MIN, ID_MAX);
    }
    long long leftSum = 0;
    long long rightSum = 0;
    if (node->left != nullptr && node->left->minID >= ID_MIN && node->left->maxID <= ID_MAX){
        leftSum = node->left->sum;
    }
    else {
        leftSum = sum(node->left, ID_MIN, ID_MAX);
    }
    if (node->right != nullptr && node->right->minID >= ID_MIN && node->right->maxID <= ID_MAX){
        rightSum = node->right->sum;
    }
    else {
        rightSum = sum(node->right, ID_MIN, ID_MAX);
    }
    return node->price + leftSum + rightSum;
}

long long count(Node *&node){
    if (node == nullptr){
        return 0;
    }
    return node->count;
}
Node* newNode(long long ID, long long price){
    Node *newNode = new Node;
    newNode->ID = ID;
    newNode->priority = rand();
    newNode->price = price;
    newNode->count = 1;
    newNode->sum = price;
    newNode->left = nullptr;
    newNode->right = nullptr;
    newNode->minID = ID;
    newNode->maxID = ID;
    return newNode;
}

int main() {

    long long N;
    cin >> N;

    Node *root = nullptr;

    for (int i = 0; i < N; i++){
        char operation;
        cin >> operation;
        switch(operation){
            case 'A':{
                long long id, price;
                cin >> id >> price;
                Node *fresh = newNode(id, price);
                add(root, fresh);
                break;
            }
            case 'D':{
                long long id;
                cin >> id;
                erase(root, id);
                break;
            }
            case 'S':{
                long long id_min, id_max;
                cin >> id_min >> id_max;
                cout << sum(root, id_min, id_max) << endl;
                break;
            }
            case 'C':{
                cout << count(root) << endl;
                break;
            }
        }
    }

    return 0;
}