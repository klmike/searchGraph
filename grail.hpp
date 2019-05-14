//grail.hpp
#include "strongly.hpp"
#include <time.h>

typedef struct grailNode{
  uint32_t minRank;
  uint32_t rank;
}grailNode;

typedef struct grail{
  grailNode* index;
  int size;
}grail;


grail* createGrail(nodeIndex*,buffer*);
int calculateGrail(nodeIndex*,buffer*,grail*,int,uint32_t*);
bool isReachableGrail(grail*,uint32_t,uint32_t);
void deleteGrail(grail*);
