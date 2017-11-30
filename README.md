# NOTE: need update

---------------------

[![Build Status](https://travis-ci.com/cornell-zhang/datuner.svg?token=KrcYXzoSR4bbR5JiDUuz&branch=master)](https://travis-ci.com/cornell-zhang/datuner)

# DATuner User Guide

### Publication

If you use this code in your research, please cite our FPGA'17 paper:

      @article{xu-datuner-fpga2017,
        title   = "{A Parallel Bandit-Based Approach for Autotuning FPGA Compilation}",
        author  = {Chang Xu and Gai Liu and Ritchie Zhao and Stephen Yang and Guojie Luo and Zhiru Zhang}
        journal = {Int'l Symp. on Field-Programmable Gate Arrays (FPGA)},
        month   = {Feb},
        year    = {2017},
      } 

### Supported platfroms and EDA tools

DATuner has been tested on CentOS and Ubuntu.
Supported EDA tools: VTR, Vivado, and Quartus. Make sure the tool is on your PATH.

### Compilation
(tested on zhang-05)

To build DATuner (including python, mpich and opentuner):

      % cd $HOME
      % git clone git@github.com:cornell-zhang/datuner.git
      % cd datuner
      % make -j8
            
Build time: ~18 minitues on zhang-05.
    
### Environment setup

      % cd $HOME/datuner
      % source setup.sh
       
### Enable password-less SSH between machines

 * Specify the list of machines in `$HOME/releases/Linux_x86_64/scripts/my_hosts`. The default is using zhang-05 to zhang-08.
 * Make sure you can ssh without password from the host to each other machine. *You actually need to ssh to each of the machines at least once, because the first time you ssh into a new machine, a confirmation to the command line prompt is needed. Without this, mpich will actually error out.*
  
One way to enable password-less SSH:
            
      % ssh to each machine
      % cd $HOME/releases/Linux_x86_64/scripts
      % ssh-keygen -t rsa
      % ./envset.sh

###  Tuning with DATuner

  * Run DATuner
  
    DATuner is an autotuner to tune EDA tools' parameters. We currently support VTR, Vivado, and Quartus. If you want to tune other EDA tools, please include the required documents. Please refer to the application part.
    
    To run DATuner, first copy the relevant Python template (i.e. [flow].py) from $HOME/datuner/releases/Linux_x86_64/scripts/tests to the directory where DATuner will be called. This Python template includes information such as the design path, the top module (for Vivado and Quartus), and the parameters (EDA tool options) to be tuned, and can be modified according to the user preferences. Options (i.e. flags) can be removed/commented out from the script, but should not be renamed. Then, DATuner can be run by the following command:
  
          % datuner.py -f [vtr|vivado|quartus|custom] -b [budget] -t [timeout] -p [parallelization_factor]

    Here, f stands for flow, b for maximum number of executions, t for stop time, and p for the process number. Timeout should be a string with the following format: <#>d:<#>h:<#>m:<#>s. The ordering of the time units can be changed, and any time unit can be omitted if it has zero value. Example usage: 

          % datuner.py -f quartus -b 20 -t 1d:1h:20m:10s -p 2 

    If the arguments b, t, or p are omitted from the command, their default values will be assumed (i.e. 1 for b, 0.0d:0.0h:0.0m:7200.0s for t, and 1 for p).

  * Get results
  
      1) Under $workspace/$tool/$design, we dump database and log file.
      
         database: $workspace/$tool/$design/results/result.db 
      
         There is one table named "result" in the database. The table contains the following columns: id, parameters, and QoR. id is the index of the configuration, which is unique and works as primary key. parameters are tool options, which are strings. QoR is the metric users want to tune, which is of float type.
      
         The schema of table:
      
         |column_1|column_2|column_3|
         |----|----|----|
         |id|integer|primary key|
         |parameter_1|text|not null|
         |parameter_2|text|not null|
         |...|...|...|
         |parameter_n|text|not null|
         |QoR|float|not null|
      
         log file: tune.log. 
      
         We calculate the best found configuration and QoR in each iteration.
      
      2) Visualization
  
         Under $HOME/releases/Linux_x86_64/scripts/visualization folder contains python script to plot the trace of DATuner tuning. We support plot multiple designs in one graph for comparison. Please define the series of designs you want to plot in the design list file. e.g, vivado_design.txt
     
               % cd $HOME/releases/Linux_x86_64/scripts/visualization
               % ./plot_design_performance.py <workspace> <design_list> <proc_num> <search_num> 
               % <workspace>: where to find the tunning results; <design_list>: text file; <proc_num>: the number of machines used; <search_num>: the number of iterations to plot.
            
            

###  Applications

   * Tune VTR

     We chose to use "k6_frac_N10_mem32K_40nm.xml" VTR FPGA architecture by default. If users want to use another architecture, tune_vtr.py (under scripts/eda_flows/vtr) can be modified accordingly.

     To tune VTR, please make sure that "TOOL_PATH" points to vtr_flow_holder in vtr.py.
   
   * Tune Vivado

     Inside the design folder, please provide both Verilog file (.v) and the timing constraint (.xdc).

   * Tune Quartus

     Inside the design folder, please provide both Verilog file (.v) and the timing constraint (.sdc).

   * Tune other programs
     
     Please modify your python code according to the tuneProgramTemplate.py under user_programe_example folder. make sure your python code import the programWrapper.py, and custom.py under you current directory includes the path of space definition textfile.


### License
DATuner is offered under the terms of the Open Source Initiative BSD 3-Clause License. More information about this license can be found here:
 - http://choosealicense.com/licenses/bsd-3-clause
 - http://opensource.org/licenses/BSD-3-Clause
