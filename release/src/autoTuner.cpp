#include "structure.h"
#include <vector>
#include <iostream>
#include <Python.h>
#include "autoTuner.h"
#include <fstream>
#include <cassert>

using namespace std;

bool AutoTuner::callOpenTuner(Task* task, vector<Result*>& results, int rank, string path, string pycode) {
  
  _path = path;
  //if(_tune_type == 1) _path = _path+"/vpr/";
  //if(_tune_type == 2) _path = _path+"/vivado/";

  //if(_tune_type == 1) _path = "/home/xuchang/nas/project/daTuner/experiment/vtr_release/vtr_flow/tasks/openTunerTiming/experiment/";
    //_path = "/proj/xsjhdstaff3/changx/project/parTuner/experiment/vpr/vtr_release/vtr_flow/tasks/openTunerTiming/experiment/";
  //if(_tune_type == 2) _path = "/home/xuchang/nas/project/daTuner/experiment/vivado/";
  //if(_tune_type == 3) _path = "/proj/xsjhdstaff3/changx/project/parTuner/experiment/ise/";

  //if(_exp_type == 1) _path = _path+"daTuner/";
  //if(_exp_type == 2) _path = _path+"whole/";
  //if(_exp_type == 3) _path = _path+"restart/";
  //if(_exp_type == 4) _path = _path+"exp-fix/";
  //if(_exp_type == 5) _path = _path+"exp-fix-MAB/";
  assert(task != NULL);

  param_parse(task,rank);
 
  cout<<"finish parsing"<<endl;  
  if(c2py2(results,rank, pycode) == -1) {
    cout<<"Call OpenTuner failed"<<endl;
    return false;
  }
  cout<<"rank "<<rank<<" results size: "<<results.size()<<endl;
  //save Result
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

void AutoTuner::parse_ISE_result(vector<Result*>& results,int rank) {
   results.resize(0);
  char tmp_buf[50];
  sprintf(tmp_buf,"localresult%d.txt",rank);
  string filename = _path+_design+"/"+string(tmp_buf);
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
    for(int i = 0; i < 13; i++) {
      ftr>>name;
      ftr>>value;
      pair<string,string> tmp = make_pair(name,value);
      result->name2choice.push_back(tmp);
    }
    ftr>>buf;
    result->score = atof(buf.c_str()); //original optuner result is minimization
    results.push_back(result); 
    ftr>>buf;//runtime
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
  assert(ftr.is_open());
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
    results.push_back(result);
  }
  ftr.close();  
}

void AutoTuner::parse_VPR_result(vector<Result*>& results,int rank) {
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
    result->score = -1*atof(buf.c_str());
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
    string name10;
    ftr>>name10;
    string val10;
    ftr>>val10;

    string score;
    ftr>>buf>>score;
    Result* result = new Result();
    result->score = -1*atof(score.c_str()); //maximize WNS
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

    {
      pair<string,string> tmp = make_pair(name10,val10);
      result->name2choice.push_back(tmp);
    }
    results.push_back(result);
   
  }
  ftr.close();
  
}
int AutoTuner::c2py2(vector<Result*>& results,int rank, string pycode) {
  printf("Going to call OpenTuner\n");
  FILE* file;
  string python_name;
  if(_tune_type == 1) python_name = _path+"/tunevpr.py";
  if(_tune_type == 2) python_name = _path+"/tunevivado.py";
  if(_tune_type == 3) python_name = _path+"/tuneProgram.py";
  //"tuneProgram.py";
  
  file=fopen(python_name.c_str(),"r");
  PyRun_SimpleFile(file,python_name.c_str());

  if(PyErr_Occurred()) {
    PyErr_Print();
    abort();
  }

  if(_tune_type == 1) parse_VPR_result(results,rank);
  if(_tune_type == 2) parse_Vivado_result(results,rank);
  if(_tune_type == 3) parse_program_result(results,rank);
  printf("call OpenTuner\n");

  return 0;
}
int AutoTuner::c2py(int argc, char** argv) {
  for(int i = 0; i < argc; i++) {
    printf("%s\n",argv[i]);
  }
  Py_Initialize();
  if(!Py_IsInitialized()) return -1;
  PySys_SetArgv(argc,argv);

  printf("Pass Initialization\n");
  //PyRun_SimpleString("import sys");
  //PyRun_SimpleString("sys.path.append('/home/changx/parTuner/openTuner-test/bgcd/script')");
  //PyRun_SimpleString("print sys.path");

  PyObject* pModule = PyImport_ImportModule("tuneVivado");
  cout<<"pass import module"<<endl;
  if(!pModule) {
    cout<<"Can't import module"<<endl;
    return -1;
  }
  PyObject* pDict = PyModule_GetDict(pModule);
  cout<<"pass find dict"<<endl;
  if(!pDict) {
    cout<<"Can't get dictonary "<<endl;
    return -1;
  }
  PyObject* pClass = PyDict_GetItemString(pDict,"VIVADOFlagsTuner");
  cout<<"pass find class"<<endl;
  if(!pClass) {
    cout<<"Can't find Class"<<endl;
    return -1;
  }
  string str = "fake_main";
  char *name = new char[str.length()+1];
  strcpy(name,str.c_str());
  cout<<name<<endl;
  printf("Start calling OpenTuner\n");
  PyObject_CallMethod(pClass,name,NULL);
  printf("Finish calling OpenTuner\n");  
  if(pModule) Py_DECREF(pModule);
  if(pDict) Py_DECREF(pDict);
  if(pClass) Py_DECREF(pClass);
  Py_Finalize();

  printf("finish calling OPENTUNER\n");
  return 0;
}

