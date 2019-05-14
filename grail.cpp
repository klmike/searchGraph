//grail.cpp
#include "grail.hpp"

grail* createGrail(nodeIndex* ind,buffer* bf){
  srand(time(NULL));
  grail* gr;
  gr = (grail*)malloc(sizeof(grail));
  gr->size = ind->size;
  gr->index = (grailNode*)malloc((gr->size)*sizeof(grailNode));
  for(int i =0;i<gr->size;i++){
    gr->index[i].rank = UINT32_MAX;
    gr->index[i].minRank = UINT32_MAX;
  }
  int x = rand() % gr->size;
  while(!ind->nodes[x].used)
    x += 1;
  uint32_t counter = 0;
  for(int i=0;i<gr->size;i++){
    int c = (i+x) % gr->size;
    if(ind->nodes[c].used){
      if(gr->index[c].rank == -1){
          calculateGrail(ind,bf,gr,c,&counter);
      }
    }
  }
  return gr;
}

int calculateGrail(nodeIndex* ind,buffer* bf,grail* gr,int pos,uint32_t* counter){
  if(gr->index[pos].minRank != -1){
    return gr->index[pos].minRank;
  }else{
    uint32_t min = UINT32_MAX;
    for(int j =0;j<H;j++){
      if(ind->nodes[pos].firstList[j] != -1){
        list_node* current = &(bf->lists[ind->nodes[pos].firstList[j]]);
        while(current != NULL){
          for(int i=0;i<N;i++){
            uint32_t temp;
            if(current->neighbour[i] != UINT32_MAX){
              temp = calculateGrail(ind,bf,gr,current->neighbour[i],counter);
              if(temp < min || min == UINT32_MAX)
                min = temp;
            }
          }
          if(current->nextListNode != UINT32_MAX)
            current = &(bf->lists[current->nextListNode]);
          else
            current = NULL;
        }
      }
    }
    if(min != -1){
      (*counter)++;
      gr->index[pos].rank = *counter;
      gr->index[pos].minRank = min;
    }else{
      (*counter)++;
      gr->index[pos].rank = *counter;
      gr->index[pos].minRank = *counter;
    }
  }
  return gr->index[pos].minRank;
}


bool isReachableGrail(grail* gr,uint32_t fr,uint32_t to){
  if(gr->index[fr].minRank == gr->index[to].minRank)
    return true;
  else
    return false;
}

void deleteGrail(grail* gr){
  free(gr->index);
  free(gr);
}
