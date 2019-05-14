//strongly.hpp
#include "structs.hpp"

typedef struct component{
    uint32_t  compId;
    uint32_t  nodeCount;
}component;

typedef struct SCC{
    component** comps;
    uint32_t    compCount;
    uint32_t    size;
    uint32_t*   compPerNode;
    nodeIndex*  pCn;
    buffer*     pCb;
}SCC;

typedef struct compCursor{
    uint32_t cursor;
}compCursor;

typedef struct stackNode{
    uint32_t id;
    int nextToUse;
    uint32_t currentList;
    int currentHash;
    uint32_t index;
    uint32_t lowlink;
    uint32_t vindex;
    struct stackNode* next;
    struct stackNode* prev;
}stackNode;

typedef struct stack{
    stackNode* first;
    stackNode* top;
    bool       empty;
    bool*      onStack;
}stack;

    //stack functions
void    createStack(stack*,uint32_t);
void    stackPush(stack*,stackNode*);
stackNode* stackPop(stack*);
void    deleteStack(stack*);
bool    isEmpty(stack*);
void    startTarjan(SCC*,buffer*,nodeIndex*);

void    newComponent(SCC*,uint32_t);
void    addNodeToComp(SCC*,uint32_t,uint32_t);

void startTarjan(SCC*,buffer*,nodeIndex*);
void mainTarjan(int,SCC*,buffer*,nodeIndex*,stack*,uint32_t*,stackNode*);
