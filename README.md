# OSLabs
OSLabs is a repo with my solutions for OS related tasks (labs).
# How to build
## 1) Install dependencies
* Install CMake from here https://cmake.org/download/#latest and add it to PATH environment variable (CMake install wizard will suggest doing this).

<img width="948" height="555" alt="image" src="https://github.com/user-attachments/assets/02b6b855-90c3-46af-836a-ee86b10014bc" />

* Install Ninja build system and add it to PATH. (https://github.com/ninja-build/ninja)
* Also you need a working c++ compiler, prefferably a build environment like ucrt64.

## 2) Build
* Clone the repo, create a folder in the directory with CMakeLists.txt and open a terminal in this folder. Type 'cmake -G "Ninja" ..' and then type 'ninja'. Built executables should be located somewhere in the folder you've created. 

