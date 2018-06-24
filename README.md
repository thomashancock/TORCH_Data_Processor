# TORCH Multiboard Data Processor

## Quick Reference
1. [Introduction](#Introduction)
2. [Requirements](#Requirements)
3. [Compiling the Processor](#Compiling)
4. [Running the Processor](#Running)
    1. [Running a quick test](#RunQuickTest)
    2. [Running over long data runs](#RunLongRuns)
5. [Pulling New Changes](#Pulling)
6. [Macros](#Macros)
7. [To-do](#ToDo)
8. [Known Bugs](#KnownBugs)

## Introduction <a name="Introduction"></a>
This program reads raw TORCH data and processes it into a ROOT file.

For an overview, see [slides from the Testbeam Meeting on 1st June 18](https://indico.cern.ch/event/731827/contributions/3026751/attachments/1660291/2659581/Multiboard_Data_Processor.pdf).

The ```Documentation``` directory contains full deoxygen documentation. It can be viewed through ```open Documentation/html/index.html```.

Please report any issues to Thomas Hancock (thomas.hancock@physics.ox.ac.uk).

## Requirements <a name="Requirements"></a>
The Multiboard Data Processor (MDP) requires:

* A working installation of CMake
* A C++14 compliant compiler (e.g. gcc 4.9 or greater)
* A working installation of ROOT 6

Note: The program has been tested with root 6.08.04, but theoretically any version of ROOT 6 should work (if you have issues related to this, contact Thomas Hancock).

## Compiling the Processor <a name="Compiling"></a>
The Multiboard Data Processor (MDP) is built using CMake.

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

## Running the Processor <a name="Running"></a>
To run the MDP, simply call the program:
```
./$(install_path)/bin/TORCH_Data_Processor
```
followed by the files you wish to process (e.g. ```data/*.txt```)

Data files must contain the string ```Device_$(BoardID)_$(FileNo)_$(Time)``` in their name. This allows the MDP to properly synchronise data across multiple readouts. The string must be enclosed by underscores, but can contain any words on either side.

The program is configured via a xml config file. This file is specified with the ```-con``` command line option. If not set, the program will search for ```Config.xml```. An example configuration file is provided for you to modify (```Example_Config.xml```).

The output file name can be specified ```-out``` command line argument. If not set, the output will be called ```Output.root```.

### Running a quick test <a name="RunQuickTest"></a>
A script is provided which runs a quick test of the MDP in ```Serial``` mode.

To run the test, do:
```
. run_test.sh
```

This runs the MDP on the data contained in ```./data/Test_Data``` using the config specified by ```Example_Config.xml```.

The test data is deliberately not perfect. The resulting output should show several "Dumping events due to buffer bloat" warnings, and the Errors Summary should contain a large number of errors (of types "Bad Packet Dumped", "Dumped incomplete packet", "Word found out of sequence").

### Running over long data runs <a name="RunLongRuns"></a>
Attempting to run over large numbers of files (more than ~1000) may result in the following error:
```
-bash: ./Install/bin/DataProcessor: Argument list too long
```

In this case, an alternative method for passing data files to the MDP is provided. In the ```scripts``` directory is a python script which, when passed one or more directories, will produce a newline separated list of files. E.g.
```
python scripts/MakeFileList.py data/Test_Data/TimeRef_01_Device_?
```

The output is stored in ```filelist.lst```:
```
data/Test_Data/TimeRef_01_Device_0/TimeRef_01_Device_0_000000_20180523110722.txt
data/Test_Data/TimeRef_01_Device_0/TimeRef_01_Device_0_000001_20180523110738.txt
data/Test_Data/TimeRef_01_Device_0/TimeRef_01_Device_0_000002_20180523110753.txt
data/Test_Data/TimeRef_01_Device_0/TimeRef_01_Device_0_000003_20180523110809.txt
data/Test_Data/TimeRef_01_Device_0/TimeRef_01_Device_0_000004_20180523110823.txt
...
```

File lists can be passed to the MDU using the ```-list``` option. E.g.
```
./Install/bin/DataProcessor -con Config.xml -list filelist.lst
```

The MDU will run over all the files given in ```filelist.lst``` as if they were passed on the command line.

## Pulling New Changes  <a name="Pulling"></a>

New versions of the MDP can be acquired via git. When pulling or fetching a new version of the processor, please ensure you do
```
git fetch --tags
```
to make your local tags up to date with the GitLab tags. The version tag is stored in the output root file to ensure replicability of results, and having an incorrect local tag destroys this feature.

In order to properly record the new tag, the MDP must be rebuilt from scratch. Please ensure you do
```
make clean
```
in the build directory, then follow the steps in the "Compiling the Processor" section from the call to ```cmake``` onwards.

## Macros <a name="Macros"></a>
ROOT macros for quickly processing the MDP output can be found in the ```macros``` directory. Each take the file to run over as input, and can be run on the output of both ```LowLevel``` and ```Serial``` mode.

E.g.
```
root 'macros/MakeChannelHisto.cxx("Output.root")' -b -q
```

Currently two macros are provided:

* **MakeChannelHisto**

Makes a histogram of the hits on each channel, separation out time reference channels according to the ```Std8x64Mapping``` Channel Mapping.

The output will be called ```ChannelIDHistogram.pdf```.

An additional argument can be provided to set the y axis scale to be logarithmic. In order to do this, add a 1 after the input file. E.g.
```
root 'macros/MakeChannelHisto.cxx("Output.root",1)' -b -q
```

* **MakeHitmap**

Makes a hitmap of detected hits, assuming the  reference channels are laid out according to the ```Std8x64Mapping``` Channel Mapping.

The output will be called ```Hitmap.pdf```.

Like ```MakeChannelHisto```, additional optional arguments can be passed. The first extra argument will set the z axis to a logarithmic scale if set to ```1```, and the second will exclude the time reference channels. E.g.
```
root 'macros/MakeHitmap.cxx("Output.root",1,1)' -b -q
```
will create a hitmap with a logarithmic z axis and no time references included.

To manually specify the output file name, pass the desired name as the second argument (i.e. after the input file name). The remaining optional arguments follow as previosuly detailed. E.g.
```
root 'macros/MakeHitmap.cxx("Output.root","CustomOutputName.pdf",1,1)' -b -q
```

## To-do <a name="ToDo"></a>

This section gives a list of changes/features which are yet to be implemented, but are requested. The requester's initials should be put in brackets after the item.

* Add possibility of logging errors in ErrorSpy which don't required a Board ID and/or TDC ID (THH)
* Refactor Edge Sorting to be easily modifiable (THH)
* Add "No Errors" to ErrorSpy summary if no errors are detected (THH)
* Parallel Mode (THH)

## Known Bugs <a name="KnownBugs"></a>

This section gives a list of known bugs which require attention. The reporter's initials should be put in brackets after the item.

* If block/file is dumped, current WordBundle should be cleared (THH)
* "Invalid Datatype" Error has TDC ID = 100 for all occurrences (THH)
