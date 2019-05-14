//structs.hpp
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <pthread.h>

using namespace std;

#define N 5
#define H 10
#define M 0.4

typedef struct list_node{
  uint32_t neighbour[N];
  uint32_t edge_property[N];
  uint32_t nextListNode;
  int nextToUse;
}list_node;

typedef struct buffer{
  list_node* lists;
  uint32_t writeAt;
  uint32_t size;
}buffer;

typedef struct node{
  bool used;
  uint32_t firstList[H];
  uint32_t lastList[H];
}node;

typedef struct nodeIndex{
  node* nodes;
  uint32_t size;
}nodeIndex;

void  create_index(nodeIndex*);
void  deleteIndex(nodeIndex*);
void  add_node(nodeIndex*,uint32_t);
void  create_buffer(buffer*);
uint32_t   return_edge(nodeIndex*,uint32_t,buffer*,uint32_t);
void  add_edge(nodeIndex*,uint32_t,buffer*,uint32_t,uint32_t);
void  new_list(list_node*);
void  print_buffer(buffer*,nodeIndex*);

// *** connected components ***
typedef struct cncted{
  uint32_t* cTable;
  nodeIndex index;
  buffer bf;
  int uUsed;
  int uLast;
  int uSize;
  float metric;
}cncted;

void createCC(cncted*,nodeIndex*,buffer*,nodeIndex*,buffer*);
void dfsCompt(cncted*,nodeIndex*,buffer*,nodeIndex*,buffer*,int,int);
int  compId(cncted*,uint32_t);
void addNodeCompts(cncted*,uint32_t,uint32_t);
void deleteCC(cncted*);
bool isConected(cncted*,uint32_t,uint32_t);     //crasharei!!!
void printCC(cncted*,nodeIndex*);
