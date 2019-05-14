//strongly.cpp
#include "strongly.hpp"
void createStack(stack* st,uint32_t size){
    st->first = NULL;
    st->top = NULL;
    st->empty = true;
    st->onStack = (bool*)malloc(sizeof(bool)*size);
    for(int j;j<size;j++)
        st->onStack[j] = false;
}

void stackPush(stack* st,stackNode* node){
    if(st->empty){
      st->first = node;
      st->first->next = NULL;
      st->first->prev = NULL;
      st->top = st->first;
      st->empty = false;
    }else{
      st->top->next = node;
      st->top->next->next = NULL;
      st->top->next->prev = st->top;
      st->top = st->top->next;
    }
}
stackNode* stackPop(stack* st){
    stackNode* ret = NULL;
    if(!st->empty){
      ret = st->top;
      st->top = st->top->prev;
      if(st->top == NULL)
          st->empty = true;
    }
    return ret;
}


void newComponent(SCC* scc,uint32_t id){
    scc->comps[id] = (component*)malloc(sizeof(component));
    if(scc->size == scc->compCount){
        component** temp = (component**)realloc(scc->comps,sizeof(component)*2*scc->size);
        if(temp != NULL){
            scc->comps = temp;
            free(temp);
        }
    }
    add_node(scc->pCn,id);
}

void addNodeToComp(SCC* scc,uint32_t cId,uint32_t nId){
    cout << nId<< " added to comp " << cId << endl;
    scc->compPerNode[nId] = cId;
}

void startTarjan(SCC* scc,buffer* bf,nodeIndex* index){
    uint32_t indCounter = 0;
    uint32_t* pC = &indCounter;
    stackNode* nodeArray;
    stack myStack;
    stack* st = &myStack;
    createStack(st,index->size);

    scc->comps = (component**)malloc(sizeof(component*)*256);
    scc->size = 256;
    scc->compCount = 0;

    nodeArray = (stackNode*)malloc(sizeof(stackNode)*index->size);

    scc->compPerNode = (uint32_t*)malloc(sizeof(uint32_t)*index->size);
    scc->pCn = (nodeIndex*)malloc(sizeof(nodeIndex));
    scc->pCb = (buffer*)malloc(sizeof(buffer));
    create_index(scc->pCn);
    create_buffer(scc->pCb);


    nodeArray = (stackNode*)malloc(sizeof(stackNode)*index->size);


    for(uint32_t i=0;i<index->size;i++){
        scc->compPerNode[i] = UINT32_MAX;
        nodeArray[i].id = i;
        nodeArray[i].index = 0;
        nodeArray[i].lowlink = 0;
        nodeArray[i].vindex = 0;
        nodeArray[i].nextToUse = 0;
        nodeArray[i].currentList = UINT32_MAX;
        nodeArray[i].currentHash = 0;
    }

    for(uint32_t i=0;i<index->size;i++){
        if(scc->compPerNode[i] == UINT32_MAX && index->nodes[i].used){
            mainTarjan(i,scc,bf,index,st,pC,nodeArray);
        }
    }
}

void mainTarjan(int pos,SCC* scc,buffer* bf,nodeIndex* index,stack* st,uint32_t* pC,stackNode* nodeArray){
    cout << "___ " << pos << endl;
    stackPush(st,&nodeArray[pos]);
    stackNode  *w, *top, *newLast;
    stackNode* last = st->top;
    st->top->index = *pC;
    st->top->lowlink = *pC;
    st->top->vindex = 0;
    st->top->prev = NULL;
    st->onStack[pos] = true;
    *pC++;

    while(1){
        uint32_t nextChild = UINT32_MAX;
        if(last->currentList != UINT32_MAX && last->nextToUse != bf->lists[last->currentList].nextToUse){
            //cout << last->currentList << "-" << last->currentHash << "-" << last->nextToUse << endl;
            nextChild = bf->lists[last->currentList].neighbour[last->nextToUse];
            last->nextToUse++;
            if(last->nextToUse == bf->lists[last->currentList].nextToUse){
                last->currentList = bf->lists[last->currentList].nextListNode;
                last->nextToUse = 0;
            }
        }else{
          last->currentList = UINT32_MAX;
          while(last->currentList == UINT32_MAX && (last->currentHash < H)){
                cout << last->currentHash << endl;
                last->currentList = index->nodes[last->id].firstList[last->currentHash];
                last->currentHash++;
          }
          if(last->currentList != UINT32_MAX){
              cout << last->currentList << "-" << last->currentHash << "-" << last->nextToUse << endl;
              nextChild = bf->lists[last->currentList].neighbour[last->nextToUse];
              last->nextToUse++;
          }
        }
        //if(nextChild ==0)
        //    nextChild = UINT32_MAX;
        cout << "__" << nextChild << endl;
        if(nextChild != UINT32_MAX){
            w = &nodeArray[nextChild];
            last->vindex++;
            if(!(w->index)){
                stackPush(st,w);
                st->onStack[nextChild] = true;
                w->vindex = 0;
                w->index= *pC;
                w->lowlink = *pC;
                last = w;
                *pC++;
            }else if(st->onStack[w->id]){
                last->lowlink = min(last->lowlink,w->index);
            }
        }else{
            if(last->lowlink == last->index){
                scc->compCount++;
                stackNode* pop = stackPop(st);
                uint32_t currentId = pop->id;
                st->onStack[pop->id] = false;
                newComponent(scc,scc->compCount-1);
                addNodeToComp(scc,scc->compCount-1,pop->id);
                while(currentId != last->id){
                    pop = stackPop(st);
                    currentId = pop->id;
                    st->onStack[pop->id] = false;
                    addNodeToComp(scc,scc->compCount-1,pop->id);
                }
            }

            newLast = last->prev;
            if(newLast != NULL){
                 if(newLast->lowlink > last->lowlink){
                     newLast->lowlink = last->lowlink;
                 }
                 last = newLast;
                 cout << "last:" << last->id << endl;
            }else{ 																				//We've seen all the nodes
                 break;
             }
        }
    }
}
