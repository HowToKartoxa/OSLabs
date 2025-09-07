# OSLabs
OSLabs is a repo with my solutions for OS related tasks (labs).
# How to build
## 1) Install dependencies
* To build lab solutions, you need to have Microsoft Visual Studio intalled with Desktop development with C++ item selected + latest version of Windows SDK.

<img width="1220" height="360" alt="image" src="https://github.com/user-attachments/assets/9db59c21-b0e5-41de-b9db-22dc3a31cfc2" />

* Install CMake from here https://cmake.org/download/#latest and add it to PATH environment variable (CMake install wizard will suggest doing this).

<img width="948" height="555" alt="image" src="https://github.com/user-attachments/assets/02b6b855-90c3-46af-836a-ee86b10014bc" />

## 2) Prepare for building
* Now create a new directory in the directory from repository that contains 'CMakeLists.txt' (For example create directory 'build' in directory 'lab1').

<img width="758" height="138" alt="image" src="https://github.com/user-attachments/assets/9ad7e092-ae9d-4a34-952a-a58e2b4c4d3d" />

* Open a terminal (powershell or cmd) in the folder you've created and type 'cmake ..'.

<img width="582" height="42" alt="image" src="https://github.com/user-attachments/assets/72946b1c-4469-4eef-a4e1-2fccd935797d" />

## 3) Build
* If everything went well, you should now have lab1.sln in the directory you've created earlier. Open this file. Now select 'Release' and 'x64' build configuration at the top of the window. Now in the menu at the top of the window select Build > Build Solution.

<img width="838" height="77" alt="image" src="https://github.com/user-attachments/assets/a342d36b-498e-4e66-8693-f7cfadcbcf57" />

## 4) Find executables
* If the build succeeded, the executables should be located at <directory you've created>/Release. Enjoy!


