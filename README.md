AStruct - Avidan's C Data Structures
====================================

AStruct is a library of data structures for C. It includes:

* AList
* AVector
* AStack
* AQueue
* AHashtable

Usage
-----

All data structures are used as pointers and are created using `AStruct->ANew()`. They all share the same interface of being operated via function pointers within the data structure which are passed a pointer to the data structure as their first argument. All of the data structures should be destroyed when you're done with them using the `->destroy()` function they provide.

All the values the data structures refer to are generic pointers (`void *`). It's up to the user to provide useful functions to operate on the actual values dereferenced by the pointers.

Build
-----

AStruct can to be built using GCC or MSVC (tested on Linux and Windows). After the build, the library file (.so, .dll) will be in folder `build`.

* __Linux/GCC, Windows/MinGW-MSYS, Windows/Cygwin__

    Use the Makefile. Run the regular

        make
    
    command from the root directory of the project.
    
* __Windows/MSVC__

    Import `AStruct.vcxproj` to Visual Studio and build the project from the Build menu.

Installation
------------

* __Linux__

    Run

        sudo make install

    After `make` to install the shared library and headers to standard system locations.

* __Windows__

    No installation. Just copy the .dll file the and headers to the new project.


Development
-----------

Either of the following tools can be used for development:

* __Make__

    Run

        make debug

    To build a debug build of the library. This will run the required unit tests from `tests` folder as well.

* __Eclipse CDT Project__

    AStruct can be imported to Eclipse CDT. Import the root folder (containing the Eclipse project files `.project` and `.cproject`) as an existing project. The Eclipse CDT project uses the Makefile for builds, and by default uses the Debug configuration.

* __Visual Studio__

    AStruct can be imported to Visual Studio. Open the project file (`AStruct.vcxproj`) to import the project to Visual Studio. Visual Studio builds in Debug mode by default.

Documentation
-------------

Documentation for AStruct is generated using Doxygen (1.8.3 at least). Run `doxygen` from the root folder, which will generate HTML documentation in the folder `doc`. If you're runnning Linux, run

    make doc
    sudo make install-doc

to generate the documentation and install it to the standard location.

Licensing
---------

AStruct is distributed under the BSD-2 License. See [<code>LICENSE.md</code>](LICENSE.md) for details.

Links
-----

* Project home: <https://github.com/Avidanborisov/AStruct>
* Online documentation: <http://Avidanborisov.github.com/AStruct>
