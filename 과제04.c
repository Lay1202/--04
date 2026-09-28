#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char data;
    struct Node* left;
    struct Node* right;
} Node;

static Node* createNode(char data) {
    Node* n = (Node*)malloc(sizeof(Node));
    if (!n) {
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }
    n->data = data;
    n->left = NULL;
    n->right = NULL;
    return n;
}

typedef struct StackNode {
    Node* ptr;
    struct StackNode* next;
} StackNode;

typedef struct {
    StackNode* top;
} Stack;

static void stackInit(Stack* s) { s->top = NULL; }

static int stackIsEmpty(Stack* s) { return s->top == NULL; }

static void stackPush(Stack* s, Node* p) {
    StackNode* sn = (StackNode*)malloc(sizeof(StackNode));
    if (!sn) {
        fprintf(stderr, "메모리 할당 실패\n");
        exit(1);
    }
    sn->ptr = p;
    sn->next = s->top;
    s->top = sn;
}

static Node* stackPop(Stack* s) {
    if (stackIsEmpty(s)) return NULL;
    StackNode* sn = s->top;
    Node* p = sn->ptr;
    s->top = sn->next;
    free(sn);
    return p;
}

static int parseError = 0;

static void skipSpaces(const char** p) {
    while (**p == ' ' || **p == '\t') (*p)++;
}

static Node* parseTree(const char** p) {
    if (parseError) return NULL;

    skipSpaces(p);
    if (**p != '(') { parseError = 1; return NULL; }
    (*p)++;
    skipSpaces(p);

    if (**p == ')') {
        (*p)++;
        return NULL;
    }

    if (**p == '\0' || **p == '(' || **p == ')') {
        parseError = 1;
        return NULL;
    }
    char data = **p;
    (*p)++;

    Node* node = createNode(data);

    skipSpaces(p);
    if (**p == '(') {
        node->left = parseTree(p);
        if (parseError) return NULL;
    }

    skipSpaces(p);
    if (**p == '(') {
        node->right = parseTree(p);
        if (parseError) return NULL;
    }

    skipSpaces(p);
    if (**p != ')') { parseError = 1; return NULL; }
    (*p)++;

    return node;
}

static Node* buildTree(const char* input) {
    parseError = 0;
    const char* p = input;

    skipSpaces(&p);
    if (*p == '\0') { parseError = 1; return NULL; }

    Node* root = parseTree(&p);
    skipSpaces(&p);

    if (parseError || *p != '\0') {
        parseError = 1;
        return NULL;
    }
    return root;
}

void preorder(Node* tree) {
    if (tree == NULL) return;

    Stack s;
    stackInit(&s);
    stackPush(&s, tree);

    while (!stackIsEmpty(&s)) {
        Node* cur = stackPop(&s);
        printf("%c ", cur->data);

        if (cur->right) stackPush(&s, cur->right);
        if (cur->left)  stackPush(&s, cur->left);
    }
}

void inorder(Node* tree) {
    Stack s;
    stackInit(&s);
    Node* cur = tree;

    while (cur != NULL || !stackIsEmpty(&s)) {
        while (cur != NULL) {
            stackPush(&s, cur);
            cur = cur->left;
        }
        cur = stackPop(&s);
        printf("%c ", cur->data);
        cur = cur->right;
    }
}

void postorder(Node* tree) {
    if (tree == NULL) return;

    Stack s1, s2;
    stackInit(&s1);
    stackInit(&s2);

    stackPush(&s1, tree);

    while (!stackIsEmpty(&s1)) {
        Node* cur = stackPop(&s1);
        stackPush(&s2, cur);

        if (cur->left)  stackPush(&s1, cur->left);
        if (cur->right) stackPush(&s1, cur->right);
    }

    while (!stackIsEmpty(&s2)) {
        Node* cur = stackPop(&s2);
        printf("%c ", cur->data);
    }
}

static void printStructure(Node* node, int depth, char branch) {
    if (node == NULL) return;
    for (int i = 0; i < depth; i++) printf("    ");
    if (depth > 0) printf("%c-- ", branch);
    printf("%c\n", node->data);
    printStructure(node->left, depth + 1, 'L');
    printStructure(node->right, depth + 1, 'R');
}

static void printParenForm(Node* node) {
    if (node == NULL) {
        printf("()");
        return;
    }
    printf("(%c", node->data);
    if (node->left || node->right) {
        printParenForm(node->left);
    }
    if (node->right) {
        printParenForm(node->right);
    }
    printf(")");
}

static void freeTree(Node* node) {
    if (node == NULL) return;
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}

int main(void) {
    char input[4096];

    printf("괄호 표기법 예시 : (A(B(D)(E))(C()(F)))\n");
    printf("이진트리를 괄호 표기법으로 입력하세요:\n> ");

    if (!fgets(input, sizeof(input), stdin)) {
        fprintf(stderr, "입력을 읽을 수 없습니다.\n");
        return 1;
    }
    input[strcspn(input, "\n")] = '\0';

    Node* root = buildTree(input);

    if (parseError || (root == NULL && strcmp(input, "()") != 0)) {
        printf("\n[오류] 입력된 괄호 표기법이 올바르지 않습니다.\n");
        printf("       입력 문자열: \"%s\"\n", input);
        return 1;
    }

    if (root == NULL) {
        printf("\n[알림] 빈 트리가 입력되었습니다. 순회할 노드가 없습니다.\n");
        return 0;
    }

    printf("\n1. 입력된 이진트리의 구조\n");
    printf("괄호 표기법 재확인 : ");
    printParenForm(root);
    printf("\n\n트리 구조:\n");
    printStructure(root, 0, ' ');

    printf("\n2. 전위 순회 (Preorder Traversal)\n");
    printf("Preorder  : ");
    preorder(root);
    printf("\n");

    printf("\n3. 중위 순회 (Inorder Traversal)\n");
    printf("Inorder   : ");
    inorder(root);
    printf("\n");

    printf("\n4. 후위 순회 (Postorder Traversal)\n");
    printf("Postorder : ");
    postorder(root);
    printf("\n");

    freeTree(root);
    return 0;
}
