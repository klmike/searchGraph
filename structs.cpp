//structs.cpp
#include "structs.hpp"

void create_index(nodeIndex* ind){
  ind->size = 256;
  ind->nodes = (node*)malloc(256*sizeof(node));
  for(int i =0;i<256;i++)
    ind->nodes[i].used = false;
}

void deleteIndex(nodeIndex* ind){
  free(ind->nodes);
}

void create_buffer(buffer* bf){
  bf->size = 256;
  bf->writeAt = 0;
  bf->lists = (list_node*)malloc(256*sizeof(list_node));
}

void add_node(nodeIndex* nI,uint32_t id){
  if(nI->size <= id){
    while(nI->size <= id){
      node* tmp;
      tmp = (node*)realloc(nI->nodes,sizeof(node)*(2*nI->size));
      if(tmp != NULL){
          nI->nodes = tmp;
          for(int i=nI->size;i<2*nI->size;i++){
              nI->nodes[i].used = false;
          }
          nI->size = 2*nI->size;
      }
    }
  }
  if(!nI->nodes[id].used){
    nI->nodes[id].used = true;
    for(int j=0;j<H;j++){
      nI->nodes[id].firstList[j] = UINT32_MAX;
      nI->nodes[id].lastList[j] = UINT32_MAX;
    }
  }
}

uint32_t return_edge(nodeIndex* nI,uint32_t id,buffer* bf,uint32_t did){
    uint32_t ret = UINT32_MAX;
    if(nI->nodes[id].used == true){
        node* cur = &(nI->nodes[id]);
        if(cur->firstList[did % H] != UINT32_MAX){
            list_node* curList = &(bf->lists[cur->firstList[did % H]]);
            while(curList != NULL){
                for(int i=0;i<curList->nextToUse;i++){
                    if(curList->neighbour[i] == did)
                        ret = did;
                }
                if(curList->nextListNode != UINT32_MAX)
                    curList = &(bf->lists[curList->nextListNode]);
                else
                    curList = NULL;
            }
        }
    }
    return ret;
}

void add_edge(nodeIndex* nI,uint32_t id,buffer* bf,uint32_t did,uint32_t version){
  list_node* temp;
  node* fr = &(nI->nodes[id]);
  if(fr->firstList[did % H] == UINT32_MAX){
    if(bf->writeAt >= bf->size){
        temp = NULL;
        while(temp == NULL){
            temp = (list_node*)realloc(bf->lists,sizeof(list_node)*(2*bf->size));
        }
        bf->lists = temp;
        bf->size = 2*bf->size;
    }
    bf->lists[bf->writeAt].neighbour[0] = did;
    bf->lists[bf->writeAt].edge_property[0] = version;
    bf->lists[bf->writeAt].nextToUse = 1;
    bf->lists[bf->writeAt].nextListNode = UINT32_MAX;
    fr->firstList[did % H] = bf->writeAt;
    fr->lastList[did % H] = bf->writeAt;
    bf->writeAt++;
  }else{
    if(bf->lists[fr->lastList[did % H]].nextToUse != N){
      bf->lists[fr->lastList[did % H]].neighbour[bf->lists[fr->lastList[did % H]].nextToUse] = did;
      bf->lists[fr->lastList[did % H]].edge_property[bf->lists[fr->lastList[did % H]].nextToUse] = version;
      bf->lists[fr->lastList[did % H]].nextToUse++;
    }else{
      if(bf->writeAt >= bf->size){
          temp = NULL;
          while(temp == NULL){
              temp = (list_node*)realloc(bf->lists,sizeof(list_node)*(2*bf->size));
          }
          bf->lists = temp;
          bf->size = 2*bf->size;
      }
      bf->lists[bf->writeAt].neighbour[0] = did;
      bf->lists[bf->writeAt].edge_property[0] = version;
      bf->lists[bf->writeAt].nextToUse = 1;
      bf->lists[bf->writeAt].nextListNode = UINT32_MAX;
      bf->lists[fr->lastList[did % H]].nextListNode = bf->writeAt;
      fr->lastList[did % H] = bf->writeAt;
      bf->writeAt++;
    }
  }
}

void new_list(list_node* l){
  for(int i=0;i<N;i++){
    l->neighbour[i] = UINT32_MAX;
  }
}

void print_buffer(buffer* bf,nodeIndex* ind){
  for(int i=0;i<ind->size;i++){
    if(ind->nodes[i].used){
      cout << i << ": ";
      for(int j=0;j<H;j++){
        int tmp = ind->nodes[i].firstList[j];
        while(tmp != -1){
          for(int z =0;z<N;z++)
            cout << bf->lists[tmp].neighbour[z] << " ";
          tmp = bf->lists[tmp].nextListNode;
        }
        cout << endl;
      }
      cout << endl;
    }
  }
}


// *** connected components ***
void createCC(cncted* compt,nodeIndex* indT,buffer* t,nodeIndex* indF,buffer* f){
  compt->cTable = (uint32_t*)malloc(indT->size*sizeof(uint32_t));
  compt->uSize = indT->size;
  compt->uLast = 0;
  nodeIndex* tIndex = &compt->index;
  buffer* tBf = &compt->bf;
  create_index(tIndex);
  create_buffer(tBf);
  for(int i =0;i<indT->size;i++)
    compt->cTable[i] = UINT32_MAX;
  int counter = 0;
  for(int i =0;i<indT->size;i++){
    if(compt->cTable[i] == UINT32_MAX && indT->nodes[i].used){
      compt->cTable[i] = counter;
      dfsCompt(compt,indT,t,indF,f,i,counter);
      counter++;
    }
  }
}

void dfsCompt(cncted* compt,nodeIndex* indT,buffer* t,nodeIndex* indF,buffer* f,int pos,int counter){
  for(int j =0;j<H;j++){
    if(indT->nodes[pos].firstList[j] != UINT32_MAX){
      list_node* current = &(t->lists[indT->nodes[pos].firstList[j]]);
      while(current != NULL){
        for(int i=0;i<current->nextToUse;i++){
            if(compt->cTable[current->neighbour[i]] == -1){
              compt->cTable[current->neighbour[i]] = counter;
              dfsCompt(compt,indT,t,indF,f,current->neighbour[i],counter);
            }
        }
        if(current->nextListNode != UINT32_MAX)
          current = &(t->lists[current->nextListNode]);
        else
          current = NULL;
      }
    }
  }
  for(int j =0;j<H;j++){
    if(indF->nodes[pos].firstList[j] != UINT32_MAX){
      list_node* current = &(f->lists[indF->nodes[pos].firstList[j]]);
      while(current != NULL){
        for(int i=0;i<current->nextToUse;i++){
            if(compt->cTable[current->neighbour[i]] == UINT32_MAX){
              compt->cTable[current->neighbour[i]] = counter;
              dfsCompt(compt,indT,t,indF,f,current->neighbour[i],counter);
            }
        }
        if(current->nextListNode != UINT32_MAX)
          current = &(f->lists[current->nextListNode]);
        else
          current = NULL;
      }
    }
  }
}

void addNodeCompts(cncted* compt,uint32_t i,uint32_t j){
  nodeIndex* ind = &compt->index;
  buffer* tbf = &compt->bf;
  add_node(ind,i);
  add_node(ind,j);
  add_edge(ind,i,tbf,j,0);
  add_edge(ind,j,tbf,i,0);
  compt->metric = float(compt->uLast)/float(compt->uSize);
  compt->uLast++;
}

int compId(cncted* cmpt,uint32_t id){
  return cmpt->cTable[id];
}

void deleteCC(cncted* compt){
  deleteIndex(&compt->index);
  free(compt->bf.lists);
  free(compt->cTable);
}

void printCC(cncted* cc,nodeIndex* ind){
  for(int i=0;i<ind->size;i++){
    if(ind->nodes[i].used)
      cout << cc->cTable[i] << endl;
  }
}

bool isConected(cncted* compt,uint32_t x,uint32_t y){
  nodeIndex* tIndex = &compt->index;
  buffer* tbf = &compt->bf;
  if(return_edge(tIndex,x,tbf,y) == y)
      return true;
  if(return_edge(tIndex,y,tbf,x) == x)
      return true;
  return false;
}
