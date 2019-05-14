//bidbfs.hpp
#include "grail.hpp"

typedef struct tailNode{
  uint32_t id;
  uint32_t depth;
  struct tailNode* next;
}tailNode;

typedef struct tail{
  tailNode* first;
  tailNode* last;
  int  truthCounter;
  int* visited;
  bool empty;
}tail;

void create_tail(tail*,uint32_t);
void deleteTail(tail*);
void tailPush(tail*,uint32_t,uint32_t);
uint32_t tailPull(tail*);
uint32_t bidBfs(nodeIndex*,nodeIndex*,buffer*,buffer*,uint32_t,uint32_t,grail*,cncted*,tail*,tail*,uint32_t);
void addDepth(tail*,nodeIndex*,buffer*,uint32_t,uint32_t);
bool isInTail(tail*,uint32_t);
bool isVisited(tail*,uint32_t);
void printTail(tail*);
