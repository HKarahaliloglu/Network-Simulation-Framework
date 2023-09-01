# Network-Simulation-Framework
Performance Simulator Framework for Programmable Networks with C++


This project is about for developing a special purpose simulation framework. The simulation 
framework will be used for simulating high level behavior of programmable networks. The 
programmable network is a new concept that each node of the network topology implements 
some application specific tasks. Intel Tofino technology enables application specific tasks to be 
deployed (offloaded) to the network switches. P4 programming language is used for 
programming such network switches to manipulate network packages. This specific 
programmability infrastructure makes the networks programmable. The simulation framework is 
not going to simulate those switches at the programming level but at the behavioral level where 
the timings of network package handling is considered. At this level of simulation, the timing 
behavior of special network tasks along with the network traffic created will be simulated. This will 
enable user to test the effects of the network specific task before implementing them on the real 
hardware. The C/C++ programming language will be used for implementing the framework.
In addition to the implementation of the framework, a simulation application for 2D/3D 
Mesh/Torus network topologies will also be implemented to test the framework. This group of 
topologies are popular in high performance computing arena. Benchmarking algorithms will also 
be implemented on top of everything to get some preliminary results of the overall performance.