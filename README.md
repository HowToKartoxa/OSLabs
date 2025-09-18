# OSLabs
OSLabs is a repo with my solutions for OS related tasks (labs).
# How to build
## 1) Install dependencies
* Install CMake from here https://cmake.org/download/#latest and add it to PATH environment variable (CMake install wizard will suggest doing this).

<img width="948" height="555" alt="image" src="https://github.com/user-attachments/assets/02b6b855-90c3-46af-836a-ee86b10014bc" />

* Install a build system (prefferably VS 2022 with c++ development option) such as Ninja or Make.
* With Ninja or Make you would need a sufficient build environment (again, VS is preffered, because it sets up this environment automatically) such as MSYS2 ucrt64.exe or mingw64.exe. Pass path to this environment as a c++ compiler when using cmake. Example - `cmake -G "Ninja" -DCMAKE_CXX_COMPILER=C:\msys64\ucrt64. ..`. 

## 2) Build
* Clone the repo, create a folder in the directory with CMakeLists.txt and open a terminal in this folder. Type `cmake ..` and then (after cmake is done) use build system that you have decided on to build the executables (with VS 2022 the goal would be to open the .sln file and build the solution in Release configuration). Built executables should be located in `bin` subdirectory of the folder you have created previously. To test the progect type `ctest` in the terminal you have opened previously.

