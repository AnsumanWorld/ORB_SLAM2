# Set up the environment

This is a manual set up of the environment required to run ORB-SLAM2 related experiments in python. We will provide an automatic configuration reusable across projects ASAP. 

## Install Anaconda

We use Anaconda for Python package management and isolation of environments. You can install Anaconda manually or via Visual Studio installer. 
- [Download Anaconda](https://www.anaconda.com/download/#windows) for your platform manually. 
- [In Visual Studio installer](https://docs.microsoft.com/en-us/visualstudio/python/vs-tutorial-01-00) select "Python Development Workload" and check "Anaconda". 

## Set up the Anaconda environment
 
1. Create an environment: 
```
conda env create -f orb-slam2.yml
```
2. Activate environment (command line):
```
activate orb-slam2
```
