//maint.cpp
#include "thread.hpp"

int main(int argc, char* argv[]){
    int currentVersion = 0;
    int lastAct = 0;
    int threadsN = 3;

    //arxikopoihsh database;
    clock_t t1 = clock();
    nodeIndex tindex;
    nodeIndex findex;
    nodeIndex* PTindex;
    nodeIndex* PFindex;
    PTindex = &tindex;
    PFindex = &findex;
    create_index(PTindex);
    create_index(PFindex);
    buffer tbf;
    buffer fbf;
    buffer* pTbf;
    buffer* pFbf;
    pTbf = &tbf;
    pFbf = &fbf;
    create_buffer(pTbf);
    create_buffer(pFbf);

    database db;
    db.toIndex = PTindex;
    db.fromIndex = PFindex;
    db.toBuffer = pTbf;
    db.fromBuffer = pFbf;
    db.gr = NULL;


    jobScheduler* tP;
    tP = createPool(threadsN);

    uint32_t x,y;
    string line;
    ifstream stFile(argv[1]);
    while(getline(stFile,line)){
        if(line != "S"){
            istringstream iss(line);
            iss >> x >> y;
            add_node(PTindex,x);
            add_node(PTindex,y);
            add_node(PFindex,x);
            add_node(PFindex,y);
            add_edge(PTindex,x,pTbf,y,0);
            add_edge(PFindex,y,pFbf,x,0);
        }else{
            break;
        }
    }
    stFile.close();

    char z;
    ifstream comFile(argv[2]);


    tail** stTail;
    tail** deTail;
    stTail = (tail**)malloc(sizeof(tail*)*threadsN);
    deTail = (tail**)malloc(sizeof(tail*)*threadsN);
    for(int j=0;j<threadsN;j++){
        stTail[j] = (tail*)malloc(sizeof(tail));
        deTail[j] = (tail*)malloc(sizeof(tail));
        create_tail(stTail[j],PTindex->size);
        create_tail(deTail[j],PTindex->size);
    }
    db.toTail = stTail;
    db.fromTail = deTail;
    grail* gr = NULL;
    cncted cIndex;
    cncted* pcIndex = &cIndex;
    createCC(pcIndex,PTindex,pTbf,PFindex,pFbf);
    db.cnt = pcIndex;

    task currentTask;
    task* pTask = &currentTask;
    pTask->arg.mutex = &tP->lock;
    pTask->arg.db = &db;
    pTask->function = &(jobSeperator);
    pTask->arg.id = 0;
    bool stat = false;
    while(getline(comFile,line)){
      if(line == "STATIC"){
          getline(comFile,line);
          //create grail
      }
      istringstream iss(line);
      iss >> z;
      if(z == 'Q'){
          lastAct = 1;
          iss >> x >> y;
          pTask->arg.node1 = x;
          pTask->arg.node2 = y;
          pTask->arg.version = currentVersion;
          jobSchedulerAdd(tP,pTask->function,&pTask->arg);
          pTask->arg.id++;
      }else if(z == 'A'){
        if(lastAct == 1){
            currentVersion++;
            lastAct = 0;
        }
        iss >> x >> y;
        add_node(PTindex,x);
        add_node(PTindex,y);
        add_node(PFindex,x);
        add_node(PFindex,y);
        if(return_edge(PTindex,x,pTbf,y) == UINT32_MAX){
            add_edge(PTindex,x,pTbf,y,currentVersion);
            add_edge(PFindex,y,pFbf,x,currentVersion);
        }
        if(compId(pcIndex,x) != compId(pcIndex,y))
            addNodeCompts(pcIndex,compId(pcIndex,x),compId(pcIndex,y));
        if(pcIndex->metric > M){
            deleteCC(pcIndex);
            createCC(pcIndex,PTindex,pTbf,PFindex,pFbf);
        }
      }else{
          pthread_mutex_lock(&tP->lock);
          db.results = (uint32_t*)malloc(sizeof(uint32_t)*pTask->arg.id);
          pthread_cond_broadcast(&(tP->notify));
          pthread_mutex_unlock(&tP->lock);

          pthread_mutex_lock(&tP->lockM);
          while(tP->running != 0 || tP->count !=0){
              pthread_cond_wait(&tP->notifyM,&tP->lockM);
          }
          pthread_mutex_unlock(&tP->lockM);

          for(int i=0;i<pTask->arg.id;i++){
              if(db.results[i] == UINT32_MAX)
                  cout << -1 << endl;
              else
                  cout << db.results[i] << endl;
          }
          pthread_mutex_lock(&tP->lock);
          free(db.results);
          pTask->arg.id = 0;
          pthread_mutex_unlock(&tP->lock);
      }
    }
    t1 = clock() - t1;
    float t2 = ((float)t1)/CLOCKS_PER_SEC;
    cout << t2 << endl;
    deleteIndex(PTindex);
    deleteIndex(PFindex);
    for(int j=0;j<threadsN;j++){
        //deleteTail(stTail[j]);
        //deleteTail(deTail[j]);
        free(stTail[j]);
        free(deTail[j]);
    }
    free(stTail);
    free(deTail);
    deleteCC(pcIndex);
    free(pTbf->lists);
    free(pFbf->lists);
    jobSchedulerDestroy(tP);

    return 0;
}
