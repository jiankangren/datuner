#include "autotuner.h"
#include <cassert>
#include <vector>
#include <iostream>
#include <Python.h>
#include <fstream>
#include <signal.h>
#include <pthread.h>
#include "structure.h"

#define DEBUG_MSG
using namespace std;

bool AutoTuner::callOpenTuner(Task* task, vector<Result*>& results, int rank, string path, string pycode) {
  _path = path;
  assert(task != NULL);
  param_parse(task,rank);
  if(c2py(results,rank, pycode) == -1) {
    return false;
  }
#ifdef DEBUG_MSG
  //cout<<"rank "<<rank<<" results size: "<<results.size()<<endl;
#endif
  return true;
}

void AutoTuner::param_parse(Task* task, int rank) {
  _step = task->step;
  Space* space = task->subspace;
  assert(space != NULL);
  
  char tmp_buf[20];
  sprintf(tmp_buf,"space%d.txt",rank);
  string filename = _path+"/"+string(tmp_buf);
  fstream ftr;
  ftr.open(filename.c_str(),fstream::out);

  vector<Param*> params = task->subspace->params;
  for(int i = 0; i < params.size(); i++) {
    Param* param = params[i];
    assert(param != NULL);
    ftr<<param->name<<" "<<param->type<<" "<<param->min<<"  "<<param->max<<" "<<param->options.size();
    for(int j = 0; j < param->options.size(); j++) {
      string opt = param->options[j];
      ftr<<" "<<opt;
    }
    ftr<<endl;
  }
  ftr.close();
}


void AutoTuner::parse_program_result(vector<Result*>& results, int rank) {
  results.resize(0);
  char tmp_buf[50];
  sprintf(tmp_buf,"localresult%d.txt",rank);
  string filename = _path+"/"+string(tmp_buf);
  fstream ftr;
  ftr.open(filename.c_str(),fstream::in);
  if(!ftr.is_open()) {
    printf("Can't measure the searching result... Exit and please check\n");
  }
  assert(ftr.is_open());
  // dump result format: configuration-name : value ; ... ; QoR
  // make sure your result follow this format
  char buf[3000];
  while(!ftr.eof()) {
    ftr.getline(buf,3000);
    if(ftr.eof()) break;
    Result* result = new Result();
    result->id = _task;
    char* pch;
    pch = strtok(buf," ,");
    while(pch != NULL) {
      string name = string(pch);
      pch = strtok(NULL, " ,");
      if(pch) {
        string value = string(pch);
        pair<string,string> tmp = make_pair(name,value);
        result->name2choice.push_back(tmp);
        pch = strtok(NULL, " ,");
      }
      else {//make sure the original tuning objectivie is minimization. Otherwise, multiply -1
        result->score = atof(name.c_str());
      }
    }
    printf("%d->%f\n",result->id,result->score);
    results.push_back(result);
  }
  ftr.close();
}

void AutoTuner::parse_VTR_result(vector<Result*>& results,int rank) {
  results.resize(0);
  char tmp_buf[50];
  sprintf(tmp_buf,"localresult%d.txt",rank);
  string filename = _path+"/"+string(tmp_buf);
  fstream ftr;
  ftr.open(filename.c_str(),fstream::in);
  assert(ftr.is_open());
  string buf;
  while(ftr>>buf) {
    string name = buf;
    string value;
    ftr>>value;
    Result* result = new Result();
    result->id = _task;
    pair<string,string> tmp_pair = make_pair(name,value);
    result->name2choice.push_back(tmp_pair);
    for(int i = 0; i < 22; i++) {
      ftr>>name;
      ftr>>value;
      pair<string,string> tmp = make_pair(name,value);
      result->name2choice.push_back(tmp);
    }
    ftr>>buf;
    result->score = atof(buf.c_str());
    results.push_back(result); 
  }
  ftr.close();
}

void AutoTuner::parse_Vivado_result(vector<Result*>& results,int rank) {
  results.resize(0);
  char tmp_buf[50];
  sprintf(tmp_buf,"localresult%d.txt",rank);
  string filename = _path+"/"+string(tmp_buf);
  fstream ftr;
  ftr.open(filename.c_str(),fstream::in);
  assert(ftr.is_open());
  string buf;
  while(ftr>>buf) {
    string name1;
    ftr>>name1;
    string val1;
    ftr>>buf>>val1;
    string name2;
    ftr>>name2;
    string val2;
    ftr>>buf>>val2;
    ftr>>buf;
    string name3;
    ftr>>name3;
    string val3;
    ftr>>val3;
    string name4;
    ftr>>name4;
    string val4;
    ftr>>val4;
    string name5;
    ftr>>name5;
    string val5;
    ftr>>val5;
    string name6;
    ftr>>name6;
    string val6;
    ftr>>val6;
    string name7;
    ftr>>name7;
    string val7;
    ftr>>val7;
    string name8;
    ftr>>name8;
    string val8;
    ftr>>val8;
    string name9;
    ftr>>name9;
    string val9;
    ftr>>buf>>val9;
    string name10,val10;
    if(_tune_cst) {
      ftr>>name10>>val10;
    }
    string metric;
    ftr>>metric;
    string score;
    ftr>>score;

    Result* result = new Result();
    result->score = atof(score.c_str()); //maximize WNS
    result->id = _task;
    if(name1 == "opt_design") {
      pair<string,string> tmp = make_pair("Optdirective",val1);
      result->name2choice.push_back(tmp);
    }
    if(name2 == "place_design") {
      pair<string,string> tmp = make_pair("Placedirective",val2);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name3,val3);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name4,val4);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name5,val5);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name6,val6);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name7,val7);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name8,val8);
      result->name2choice.push_back(tmp);
    }

    if(name9 == "route_design") {
      pair<string,string> tmp = make_pair("Routedirective",val9);
      result->name2choice.push_back(tmp);
    }
   
    if(_tune_cst) {
      pair<string,string> tmp = make_pair(name10,val10);
      result->name2choice.push_back(tmp);
    }

    results.push_back(result);
   
  }
  ftr.close();
  
}

void AutoTuner::parse_Quartus_result(vector<Result*>& results,int rank) {
  results.resize(0);
  char tmp_buf[50];
  sprintf(tmp_buf,"localresult%d.txt",rank);
  string filename = _path+"/"+string(tmp_buf);
  fstream ftr;
  ftr.open(filename.c_str(),fstream::in);
  assert(ftr.is_open());
  string buf;
  while(ftr>>buf) {
    string name1;
    ftr>>name1;
    string val1;
    ftr>>val1;
    string name2;
    ftr>>name2;
    string val2;
    ftr>>val2;
    string name3;
    ftr>>name3;
    string val3;
    ftr>>val3;
    string name4;
    ftr>>name4;
    string val4;
    ftr>>val4;
    string name5;
    ftr>>name5;
    string val5;
    ftr>>val5;
    string name6;
    ftr>>name6;
    string val6;
    ftr>>val6;
    string name7;
    ftr>>name7;
    string val7;
    ftr>>val7;
    string name8;
    ftr>>name8;
    string val8;
    ftr>>val8;
    string name9;
    ftr>>name9;
    string val9;
    ftr>>val9;
    string name10;
    ftr>>name10;
    string val10;
    ftr>>val10;
 
    string metric;
    ftr>>metric;
    string score;
    ftr>>score;    

    Result* result = new Result();
    result->score = atof(score.c_str()); //maximize WNS
    result->id = _task;
    
    {
      pair<string,string> tmp = make_pair(name1,val1);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name2,val2);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name3,val3);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name4,val4);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name5,val5);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name6,val6);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name7,val7);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name8,val8);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name9,val9);
      result->name2choice.push_back(tmp);
    }
    {
      pair<string,string> tmp = make_pair(name10,val10);
      result->name2choice.push_back(tmp);
    }
        
    results.push_back(result);
  
  }

  ftr.close();

}

void *ProgressIndict(void *t)
{    
   long tid;
   tid = (long)t;
   if (tid == 0) {
     FILE* file = NULL;
     file=fopen("tune_vtr.py","r");
     PyRun_SimpleFile(file,"tune_vtr.py");
   }
   else if (tid == 1) printf("[  INFO] calling VTR");
   else {
     float progress = 0.0;
     while (progress < 0.7) {
       int barWidth = 70;
       std::cout << "[";
       int pos = barWidth * progress;
       for (int i = 0; i < barWidth; ++i) {
         if (i < pos) std::cout << "=";
         else if (i == pos) std::cout << ">";
         else std::cout << " ";
       }  
       std::cout << "] " << int(progress * 100.0) << " %\r";
       std::cout.flush();
       progress += 0.0001;
       if (progress > 1) progress = 0;
     }
     std::cout << std::endl;
   } 
   pthread_exit(NULL); 
}
   
int AutoTuner::c2py(vector<Result*>& results,int rank, string pycode) {
  FILE* file = NULL;
  FILE* sto = fopen("out.txt", "w+");
  FILE* ste = fopen("opentuner_data.txt", "w+");
  PySys_SetObject("stdout", PyFile_FromFile(sto, "out.txt","wb", fclose));
  PySys_SetObject("stderr", PyFile_FromFile(ste, "opentuner_data.txt","wb", fclose));
  if(_tune_type == 1) {
    file=fopen("tune_vtr.py","r");
//    printf("[   INFO]    Calling VTR...\n");
    PyRun_SimpleFile(file,"tune_vtr.py");
  }
  if(_tune_type == 2) {
    file=fopen("tune_vivado.py","r");
//    printf("[    INFO]    Calling Vivado...\n");
    PyRun_SimpleFile(file,"tune_vivado.py");
  }
  if(_tune_type == 3) {
    file=fopen("tune_quartus.py","r");
//    printf("[    INFO]    Calling Quartus...\n");
    PyRun_SimpleFile(file,"tune_quartus.py");
  }
  if(_tune_type == 4) {
    file=fopen("tune_program.py","r");
    PyRun_SimpleFile(file,"tune_program.py");
  }
  if(PyErr_Occurred()) {
    PyErr_Print();
    abort();
  }

  if(_tune_type == 1) parse_VTR_result(results,rank);
  if(_tune_type == 2) parse_Vivado_result(results,rank);
  if(_tune_type == 3) parse_Quartus_result(results,rank);
  if(_tune_type == 4) parse_program_result(results,rank);
  return 0;
}

