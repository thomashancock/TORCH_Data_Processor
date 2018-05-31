# TORCH Multiboard Data Processor

## Introduction
This program reads raw TORCH data and processes it into a ROOT file.

Test data files are provided to aid in development of the program.

For an overview of the program, see ```link will be inserted```.

The ```Documentation``` directory contains full deoxygen documentation. It can be viewed through ```open Documentation/html/index.html```.

Please report any issues to Thomas Hancock (thomas.hancock@physics.ox.ac.uk).

## Compiling the Processor
The Multiboard Data Processor (MDP) is built using CMake.

A working installation of ROOT is required to build the program. The program has been tested with root 6.08.04, but theoretically any version of ROOT 6 should work (if you have issues related to this, contact Thomas Hancock).

To build the MDP, do:
```
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$(install_path) -DCMAKE_BUILD_TYPE=Release
make
make install
```

The MDP will be built in the ```bin``` directory of the set ```$(install_path)```.

If developing the processor, the program can be build in debug mode by changing the CMake Build Type:
```
-DCMAKE_BUILD_TYPE=Debug
```

## Running the Program
To run the MDP, simply call the program:
```
./$(install_path)/bin/TORCH_Data_Processor
```
followed by the files you wish to process (e.g. ```data/*.txt```)

Data files must contain the string ```Device_$(BoardID)_$(FileNo)_$(Time)``` in their name. This allows the MDP to properly synchronise data across multiple readouts. The string must be enclosed by underscores, but can contain any words on either side.

The program is configured via a xml config file. This file is specified with the ```-con``` command line option. If not set, the program will search for ```Config.xml```. An example configuration file is provided for you to modify (```Example_Config.xml```).

The output file name can be specified ```-out``` command line argument. If not set, the output will be called ```Output.root```.

## Pulling new changes

New versions of the MDP can be acquired via git. When pulling or fetching a new version of the processor, please ensure you do
```
git fetch --tags
```
to make your local tags up to date with the GitLab tags. The version tag is stored in the output root file to ensure replicability of results, and having an incorrect local tag destroys this feature.

In order to properly record the new tag, the MDP must be rebuilt from scratch. Please ensure you do
```
make clean
```
in the build directory before rebuilding the program to ensure the tag is correctly propagated.

## To-do

This section gives a list of changes/features which are yet to be implemented, but are requested. The requester's initials should be put in brackets after the item

* Refactor Edge Sorting to be easily modifiable (THH)
* If block/file is dumped, ensure current WordBundle is cleared (THH)
* Add "No Errors" to ErrorSpy summary if no errors are detected (THH)
* Parallel Mode (THH)
