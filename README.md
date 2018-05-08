# TORCH Data Processor

## Introduction
This program reads raw TORCH data and processes it into a ROOT file.

Test data files are provided to aid in development of the program.

The ```Documentation``` directory contains an overview of the program, along with full deoxygen documentation.

Please report any issues to Thomas Hancock (thomas.hancock@physics.ox.ac.uk).

## Compiling the Template
To build the program, simply type:
```
make
```

If developing the processor, the program can be build in debug mode using:
```
make debug
```

Note: A working installation of ROOT is required to build the program

## Running the Program
To run the template, simply call the program:
```
./TORCH_Data_Processor
```
followed by the files you wish to process (e.g. ```data/*.txt```)

The ```-con``` command line option allows a specific config (```.conf```) file to be loaded.

The ```-out``` command line argument specifies the output file name.

## Pulling new changes

When pulling or fetching a new version of the processor, please ensure you do
```
git fetch --tags
```
to make your local tags up to date with the GitLab tags. The version tag is stored in the output root file to ensure replicability of results, and having an incorrect local tag destroys this feature.


## To-do

This section gives a list of changes/features which are yet to be implemented, but are requested. The requester's initials should be put in brackets after the item

* Prevent header/trailer word overwriting in Packet class (THH)
* Add global monitoring for caught issues and inconsistencies (THH)
