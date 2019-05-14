//bidbfs.cpp
#include "bidbfs.hpp"

void create_tail(tail* t,uint32_t mSize){
  t->truthCounter = -1;
  t->empty = true;
  t->first = NULL;
  t->last = NULL;
  t->visited = (int*)malloc(mSize*sizeof(int));
  for(int i=0;i<mSize;i++)
    t->visited[i] = -1;
}

void deleteTail(tail* t){
  free(t->visited);
}

void tailPush(tail* t,uint32_t insert,uint32_t depth){
  if(t->first == NULL){
    t->first = (tailNode*)malloc(sizeof(tailNode));
    t->first->next = NULL;
    t->first->id = insert;
    t->first->depth = depth;
    t->last = t->first;
    t->empty = false;
  }else{
    t->last->next = (tailNode*)malloc(sizeof(tailNode));
    t->last->next->next = NULL;
    t->last->next->id = insert;
    t->last->next->depth = depth;
    t->last = t->last->next;
  }
  t->visited[insert] = t->truthCounter;
}

uint32_t tailPull(tail* t){
  uint32_t ret;
  if(!t->empty){
    tailNode* tmp = t->first;
    ret= tmp->id;
    t->first = t->first->next;
    if(t->first == NULL)
      t->empty = true;
    free(tmp);
  }else{
    ret = UINT32_MAX;
  }
  return ret;
}

bool isVisited(tail* t,uint32_t el){
  return (t->visited[el] == t->truthCounter);
}

uint32_t bidBfs(nodeIndex* to,nodeIndex* fr,buffer* tbf,buffer* fbf,uint32_t start,uint32_t dest,grail* gr,cncted* compt,tail* stTail,tail* deTail,uint32_t version){
  stTail->truthCounter++;
  deTail->truthCounter++;
  bool dynamic = false;
  if(gr == NULL)
      dynamic = true;
  uint32_t ret = UINT32_MAX;
  uint32_t stDepth;
  uint32_t deDepth;
  tailPush(stTail,start,0);
  tailPush(deTail,dest,0);
  bool found = false;
  bool cont = true;
  int stde  = 0;
  while(!found && cont){
    tailNode* tmp1 = stTail->first;
    tailNode* tmp2;
    while(tmp1 != NULL && !found){
      tmp2 = deTail->first;
      while(tmp2 != NULL && !found){
        if(dynamic){
            if((compId(compt,tmp1->id) == compId(compt,tmp2->id)) /*|| isConected(compt,compId(compt,tmp1->id),compId(compt,tmp2->id))*/){
              if(return_edge(to,tmp1->id,tbf,tmp2->id) != UINT32_MAX){
                  ret = tmp1->depth + tmp2->depth +1;
                  found = true;
              }
            }
        }else{
              if(return_edge(to,tmp1->id,tbf,tmp2->id) != UINT32_MAX){
                  ret = tmp1->depth + tmp2->depth +1;
                  found = true;
              }
        }
        tmp2 = tmp2->next;
      }
      tmp1 = tmp1->next;
    }
    if(!found){
      if(stde == 0){
          stDepth = stTail->first->depth;
          addDepth(stTail,to,tbf,tailPull(stTail),stDepth);
          stde = 1;
      }else{
          deDepth = deTail->first->depth;
          addDepth(deTail,fr,fbf,tailPull(deTail),deDepth);
          stde = 0;
      }
    }
    if(stTail->empty || deTail->empty)
        cont = false;
  }
  while(!deTail->empty)
      tailPull(deTail);
  while(!stTail->empty)
      tailPull(stTail);
  return ret;
}

void addDepth(tail* t,nodeIndex* ind,buffer* bf,uint32_t el,uint32_t depth){
  node* n = &(ind->nodes[el]);
  for(int j=0;j<H;j++){
    uint32_t pos = n->firstList[j];
    if(pos != UINT32_MAX){
      list_node* current = &(bf->lists[pos]);
      while(current != NULL){
        for(int i=0;i<current->nextToUse;i++){
          if(!isVisited(t,current->neighbour[i])){
              tailPush(t,current->neighbour[i],depth+1);
          }
        }
        if(current->nextListNode != UINT32_MAX)
          current = &(bf->lists[current->nextListNode]);
        else
          current = NULL;
      }
    }
  }
}

void printTail(tail* t){
  tailNode* tmp;
  tmp = t->first;
  while(tmp != NULL){
    cout << tmp->id << "---";
    tmp = tmp->next;
  }
  cout << endl;
}
