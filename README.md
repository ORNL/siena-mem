Siena: a tool for exploring the design space of heterogeneous memory systems

Siena facilitates quick exploration of diverse memory architectures with flexible configurations of heterogeneous memory systems and various memory access workloads. Siena uses ASPEN modelling language for representing workload memory-access behaviours and couples with popular memory simulators to accurately model new memory technologies.

##Quick Start:
  * 'make all'
  * 'make test'

##Installation:

####1. Dependency:

  1.1 [mandatory]:ASPEN  [dependency:bison,gcc>=5.4,python2.7]

      1.1.1 cd <siena_root>/aspen  
      1.1.2 autoconf -v  
      1.1.3 ./configure --prefix=<siena_root>/aspen  
      1.1.4 'make aspen' #if compiled successfully, libaspen.a should exit in /lib

  1.2 [optional]: DRAMSim2, NVDIMMSim, RAMULATOR  

      1.2.1 to enable RAMULATOR: 'cd ramulator/ && make libramulator.a && cd ..'  
      1.2.2 to enable NVDIMMSim: 'cd NVDIMMSim-2.0.0/src/ && make libnvdsim.a && cd ../..'  
      1.2.3 to enable DRAMSim2:  'cd dramsim2/ && make libdramsim.a && cd ..'  
  
####2. Compile:

  2.1 [default]
 
       type 'make'  

  2.2 [couple with DRAMSim, Ramulator or NVDimmSim] 

      2.2.1 make sure the depended libraries are installed in step 1.5   
      2.2.2 in Makefile, change USE_[DRAMSIM | NVDIMMSIM | RAMULATOR]=yes  
      2.2.3 type 'make' 

####3. clean up:

  3.1 [standard] 

      'make clean'  

  3.2 [full clean] removes installation in other sub-modules like ./aspen ./ramulator...
	  
       'make distclean' 

##Usage: 
	  
       ./siena <Application_Model>.aspen <Machine_Model>.aspen [Mapping_Model].aspen
