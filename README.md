# A Generic tool for visualization of airflow data

### ENGN2912B Fall18 S01 Scientific Programming in C++
---
##### Data Visualization, Group B

Yabo Yan yabo_yan@brown.edu

Yuqi Chai yuqi_chai@brown.edu

Zhiyu Wang zhiyu_wang@brown.edu

---

##### Date Submitted: December 18 2018
---
### 1. Introduction
#### 1.1 Project Overview
This project aims to visualize the airflow field data generated using different simulation software in different formats. Our project consists of three parts: a data visualization tool, which is the main program of this project, two data preprocessors which can convert the data file to the file format that we are able to use and a data generator used for testing purpose. We use VTK to visualize the dataset, OpenGL to render the result and Qt to design the user interface. In the first release of our visualization tool, we implemented the visualization of several airflow attributes with different visualization techniques. We have two data preprocessors to convert data that were generated from simulation software to the data format which can be directly handled by our program. The test dataset of our program includes a dataset we got from compute Canada[1] and a dataset that we generated from OpenFOAM.

#### 1.2 Goals and Objectives
The goal of this project is to develop a generic tool to visualize airflow field data. To provide the user with a clear view of their data, we need to find out the best way to visualize different kinds of data in an air flow dataset. Therefore, we use cutting plane to visualize pressure data, colored points to visualize Q-criterion and streamlines with colored points to visualize three Cartesian velocity components. We also design our project to be generic, which allows users to use all kinds of simulation software without worrying about first converting their data. Concisely, our goal is to provide the user who would like to look into airflow data with an approach to quickly view their data in a concise and comprehensive way. The detailed objectives can be summed as follows:

1. Use appropriate methods to visualize different kinds of data in an airflow dataset with attributes such as pressure and velocity.

2. Compatible with all the major data formats used by simulation tools such as OpenFOAM.

3. A user interface that allows users to interact with the program, such as selecting which part of data they would like to visualize and adjusting parameters for visualization.

4. Make the visualization tool straightforward to use and as efficient as possible. 

### 2. Software Architectural
#### 2.1 Required External Libraries

The external libraries that we use are VTK-8.1.2 and Qt-5.11.0.

#### 2.2 Functional Block Diagrams

Figure[] : the software architecture of our program

#### 2.3 Data Flow Diagrams

Figure[]: the data flow of our program

#### 2.4 UML Diagrams showing Object Definition/Inheritance

Figure[]: the UML diagram of our program

### 3. Graphical Interface and Input data
#### 3.1 Graphical Interface
We use Qt to develop our graphical interface. A picture of our program is shown as Figure.

The GUI mainly consists of two parts: on the left is a QVTKOpenGLWidget which shows the rendering results using VTK, on the right are radiobuttons, checkboxes, and sliders, which can properly deal with user inputs.
 
The first radiobutton ‘Pressure’ is used to display pressure data on a given x-y surface in the chosen .vtu data file. If the data file contains pressure data, it will show the visualization results as Figure.
 
The second radiobutton ‘Q-criterion’ is used to display q-criterion data in the chosen .vtu data file. If the data file contains Q-criterion data, it will show the visualization results as Figure.
 
The third radiobutton ‘Pressure on Object' can only be selected if the input dataset contains object data, which is stored in vtkpolydata class, and the corresponding pressure data. If the radiobutton ‘Pressure on Object' is selected, the visualization result is shown as Figure. 
 
And it shows the pressure on these objects as described. 

The fourth radiobutton is used to display velocity data if the data files contain a velocity field in the form of a vector field instead of a scalar field. Otherwise, it throws an error. This is shown in Figure.
 
The checkbox named ‘Streamline On’ is only available when the dataset contains streamline data. The visualization result is shown in Figure().
 
On the contrast, the first slider named ‘Streamline (for scalar field only)’ will calculate streamlines onsite but this feature is only available for the dataset that contains scalar velocity field as shown in its name.

The second slider is called ‘Choose surface (for P)’ is used to choose the x-y surface that user would like to visualize. In other words, the slider bar is projected onto the z values of the given dataset. From left to right the bar is corresponding to the z value from small to large.

The menu bar contains two options, File and About. The former provides users the function of choosing other folders for the program’s input, and the latter shows a message about developers.


#### 3.2 Threading and Concurrency

Our goal for this part is to make the rendering module multi-threaded. After a comprehensive investigation, We found that the many functions in VTK are not thread-safe [2], and we did not find the classes in VTK that can help us to accomplish multi-threading in rendering. So we decide to add multi-threading features in the data read-in module. Typically, a complete airflow dataset contains both air data, which is stored in a .vtu file and also object data, which is stored in one or more .vtp files. We then use one thread to read the airflow dataset and another thread to read object file. The multi-threading is implemented in the class constructor (`mainwindow::mainwindow()`) and in the slot function (`mainwindow::open()`).

Figure[]: the thread organization of data read-in part

#### 3.3 Exception Handling

In our program, we use exception handling to determine if the dataset has the data that we need for visualization. We defined an exception structure named no_data. Every time our program wants to read a certain type of data from the dataset, for example, Q-criterion data from the dataset, we use a try catch block to test if this data is available. If there is no corresponding data in the dataset, We throw the exception no_data, which prevent our program from ultimately crashing. In the catch block, We use Qt to pump out a message box to warn the user that the attribute they requested for visualization is not available. Meanwhile, our program will stop and wait for the user to select other visualization options.

---
#### 3.4 Numerical Computation
1. The computation for the absolute value of velocity<br>
From the scalar field dataset, each point’s velocity values are stored in three corresponding data arrays: u, v, and w, which are three Cartesian velocity components <img src="https://latex.codecogs.com/gif.latex?(x,y,z)"/> . As we want to visualize the velocity value as colored points, we need to first compute the absolute velocity value of every point. Thus we use the formula: <p align =center><img src="https://latex.codecogs.com/gif.latex?velocity=\sqrt{u^2+v^2+w^2}" style="border:none;"></p>
After calculating the absolute value of each point, we then visualize the points’ velocity value as colored points.

2. The streamline calculation for scalar field dataset<br>
As described before, the scalar field contains three Cartesian velocity components at each point. The basic idea is to set up a start point and use these velocity components to calculate the absolute value of velocity at that point. We can locate the position of next point with this absolute value of velocity and start to trace how air flows through space. The detailed steps and formulas are described as follows:<br>
Assumed that we have picked a starting point <img src="https://latex.codecogs.com/gif.latex?P_0"/>, whose coordinate is <img src="https://latex.codecogs.com/gif.latex?(x,y,z)" />.<p align = "center"><img src="https://latex.codecogs.com/gif.latex?P_0=(x_0,y_0,z_0)" /></p><br>Then, we get the velocity components of <img src="https://latex.codecogs.com/gif.latex?P_0" /> from the dataset: <img src="https://latex.codecogs.com/gif.latex?u_0,v_0,w_0" />. Here, we set a unit time step <img src="https://latex.codecogs.com/gif.latex?\Delta{t}" />. To get the next point’s position we can use the formula below:<p align = "center"> <img src="https://latex.codecogs.com/gif.latex?P_{next}=(x_0+\Delta{t}*u_0,y_0+\Delta{t}*v_0,z_0+\Delta{t}*w_0)" /> </p ><br>Now we know each point <img src="https://latex.codecogs.com/gif.latex?P_i" />’s coordinate as <img src="https://latex.codecogs.com/gif.latex?(x_i,y_i,z_i)" />, and we can get the next point’s coordinate <img src="https://latex.codecogs.com/gif.latex?P_{i+1}" /> through calculation:<p align = "center"><img src="https://latex.codecogs.com/gif.latex?P_{i+1}=(x_i+\Delta{t}*u_i,y_i+\Delta{t}*v_i,z_i+\Delta{t}*w_i)" /></p ><br>For the reason that there may not exist point data value(the velocity components <img src="https://latex.codecogs.com/gif.latex?u_i" />, <img src="https://latex.codecogs.com/gif.latex?v_i" /> and <img src="https://latex.codecogs.com/gif.latex?w_i" /> ) in the position result we get from our calculation.
We now get the next point through calculation, but there may not necessarily be any points at that exact location. We then implement a built-in VTK function: FindCell to get the cell which the point is in. In the dataset, each cell is of tetrahedron type and has 4 vertices. From the cell we get, we can have the coordinates of the 4 vertices of this cell <img src="https://latex.codecogs.com/gif.latex?(vertex1,vertex2,vertex3,vertex4)" />. Then we can calculate the distances between the 4 vertices and the point pi which are <img src="https://latex.codecogs.com/gif.latex?d1,d2,d3,d4" />, respectively. The distance calculation is carried out using the following formula:<p align = "center">
<img src="https://latex.codecogs.com/gif.latex?d=\sqrt{(x_a-x_b)^2+(y_a-y_b)^2+(z_a-z_b)^2}" /></p >
Then we can calculate the weights <img src="https://latex.codecogs.com/gif.latex?r_1,r_2,r_3,r_4" />:<p align = "center">
<img src="https://latex.codecogs.com/gif.latex?r_k=\frac{1/d_k}{\sum{1/d_k}}(k=1,2,3,4)" /></p >
Since the velocity data of the 4 vertices <img src="https://latex.codecogs.com/gif.latex?(u_{vertex_k},v_{vertex_k},w_{vertex_k})" />is known, we can calculate the velocity data of the point pi. <p align = "center">
<img src="https://latex.codecogs.com/gif.latex?u_i=\sum_{k}u_{vertex_k}*r_k" /></p ><p align = "center">
<img src="https://latex.codecogs.com/gif.latex?v_i=\sum_{k}v_{vertex_k}*r_k" /></p ><p align = "center">
<img src="https://latex.codecogs.com/gif.latex?w_i=\sum_{k}w_{vertex_k}*r_k" /></p >
After we get a series of points positions, we can use another VTK function vtkPolyLine to connect these points as a sequence, generating the streamline. Choosing different starting points can result in different streamlines being generated. 
 

### 4. Testing and Evaluation

#### 4.1 Test Environment

As the VTK library on CCV is missing some critical Qt/VTK interacting libraries that are needed by our program, we then chose OS X as the platform to use. For a new Mac, the first step is to appropriately set up the environment for our program to run. 

The first step is to install Qt. You can download Qt from this website: https://www.qt.io/download. From this website, you will get a disk image (.dmg) file, which contains an online installer.


Open the installer, follow the instructions for each step. If you don’t know what you are supposed to do, simply follow the default selection until the whole process is done. (Note: you may need to register an account for Qt.) In this process, you will be able to choose the version that you would like to install. The version we use is Qt 5.11.0.

Then, you can download VTK from this website: https://www.vtk.org/download/. The version we use is VTK-8.1.2, which is the latest release for now (12/16/2018). In this case, what we have downloaded is the source code of VTK, so we now need to compile the code and install the library to our system. Simply make a directory ‘build’ under the root folder of VTK code, and then: `cd build`. Next step is to use CMake to generate the makefile. We recommend using CMake-GUI for this process (you can download here: https://cmake.org/download/). The picture shows how the GUI looks like:
 
Correctly select the directory for the VTK source code and also the directory for the generated build files. Please make sure ‘VTK_Group_Qt’ and ‘BUILD_SHARED_LIBS’ are checked, then press ‘Configure’. You may have to designate the path for Qt cmake files. Its default path should be ‘/Users/$YOUR_USER_NAME/Qt/5.xx.x/clang_64/lib/cmake/Qt5’. Keep configuring until all the red error messages are properly dealt with, then press ‘Generate’. In this case, the build files should be written to the build folder correctly.

Next step is to run `make` in the build directory. Use `make -jN` to speed up this process by using N threads at the same time. When it’s completed without errors, use `make install` to write the library files into your system so they can be appropriately called by default without having to specify their path. After this, your VTK libraries are correctly installed.

Additionally, we use OpenFOAM for data generation. If you want to use custom datasets to test our program, we recommend you to use OpenFOAM. However, installing OpenFOAM for Mac OS X is not straightforward. You would have to first set up a docker then install OpenFOAM in that docker. The more detailed instructions can be found on this website:  https://openfoam.org/download/dev-macos/. We recommend using OpenFOAM on CCV. You can simply use `module load openfoam/4.1` to load OpenFOAM. Then you will get further instructions for the command line, which is: ` module load mpi/openmpi_2.0.3_intel; source $FOAM_INST_DIR/OpenFOAM-4.1/etc/bashrc`. Simply run these two commands and OpenFOAM is then correctly and completely set up.


#### 4.2 Test steps and results

We use Qt Creator to build our program, and ultimately we get a .app file, which can be directly run by users.

So you can simply double-click on our program. And you’ll get a dialog to choose the folder you would like to open.
 
After choosing the folder, click ‘Open’ or simply double-click on the folder you would like to open. Here is a sample showing the layout of the program:
 
Then you can click on the radiobuttons, checkboxes, and sliders on the right to display the data you expect to visualize.

In the dataset, we focus on several important properties: pressure (P), Q-criterion (Q), velocity and streamlines. Please note that due to the difference between scalar field data and vector field data, some features may not be supported by the dataset you use. You will get error messages when trying to enable unsupported features.

#### 4.3 How to build and use

For data preprocessing, you can use the data conversion tool we provide in the repository. If you would like to use this, create a folder at the root of the repository and then:
```
cd build
cmake ..
make
```
Now you can use our tool to convert legacy VTK data files to XML data files which can be directly read-in by our program. The program requires two input parameters, the first of which is the path of the file to be converted while the second is the path of the file being generated.
```
ConvertFile_vtp.cxx [Origin file path] [Output file path]
```


If you would like to build our main program from source code, You need to use Qt creator to build the code. Please first make sure that you already have Qt and VTK properly installed and configured. Then you can open Qt Creator and create an empty project. Add all the files from the ‘main’ directory of our code repository of this project, then you can simply build and run it from there.

### 5. Conclusions
#### 5.1 Major Accomplishments
The major accomplishments we made are as follows:
Implemented the data read-in module. Enabling the framework to read in different formats of airflow datasets (.vtp, .vtk, .vtu, etc.) generated by simulation tools (e.g. OpenFOAM) in a multi-threaded way.
Implemented the feature of visualizing scalar field data. For example, our program can visualize points in the dataset and map scalar values (e.g. Pressure values, Velocity values, Q-criterion values and so on) to different colors. Users can choose which scalar value to visualize and a color bar will be displayed to show the matching relationship between color scale and data value. 
Implemented the feature of visualizing airflow streamlines for both scalar field dataset and vector field dataset (with different mechanisms). For scalar field data, users can use the slider on the graphical user interface to generate streamlines from different starting points. 
Implemented the feature of selecting cutting planes of the visualized data (for pressure value visualization). Users can use the slider to view different cutting planes. 
Implemented an interactive graphical user interface and build the whole program as an executable application using Qt. The interface includes a menu bar for opening files, the main window for display, and an event-listening part for user interaction (including checkboxes, radiobuttons, and sliders to modify visualization parameters).

#### 5.2 development efforts and outcomes
Learning how to use VTK library. Looking up for vtk methods. Successfully implemented data read-in module, display module, streamline-drawing module and cutting plane module.
Learning how to use Qt library. Successfully implemented the interactive graphical interface. 
Get familiar with OpenFOAM. Successfully using OpenFOAM to simulate airflow data in VTK readable formats

#### 5.3 Compare with our original goals
Previously we wanted to implement a data-preprocessor feature in order to warp raw simulation data into a certain format(e.g. VTK format). However, most simulation tools provide the built-in function of convert the output data into VTK format. So we use this feature of simulation platform instead of implementing a raw data preprocessor. On the contrast, we implemented a preprocessor which can convert data of VTK readable formats (e.g. legacy .vtk formats) into certain VTK XML format (e.g. .vtu) which can be handled by our program. 
Considering the built-in streamline class of VTK cannot work with scalar field data, we implement this feature by calculating absolute value of velocity at each point to locate their next point. After trace the airflow point by point, we use vtkPolyLine method to draw streamline to visualize the airflow. 
Due to the lack of time and being unfamiliar with some vtk methods, we dropped the feature of visualizing stream surfaces. 
For generating airflow dataset, we choose OpenFOAM instead of FEniCS. 

### 6. Future Work
The scalar data (for example, pressure and Q-criterion) is not uniformly distributed from minimum value to maximum value. If we just simply map these values to colors and visualize them, we will not be able to produce a satisfying picture. Besides, the user may be only interested in displaying data of a certain range. So, we need to add a range selection feature, which allows the user to choose how these values are mapped to colors and points in which range they want to hide so that they can get a clearer view.
The unit of the scale value is important. We can add the display of unit while the program is displaying the visualization result.
Integrate the data preprocess program into the main program. In our program, you need to separately call the preprocess program to convert the original data to the data that our program can handle before using the main visualization program. We can add these code to the data_load class so that our program can identify the origin data format and convert it automatically.
We can add more supported file types. Our test dataset comes from OpenFOAM and Compute Canada. There are also many other simulation tools that the user may be using. The data generated by different simulation tools are different. So we can add the support to these simulation tools in the future.
Add more visualization options. There are many ways to visualize a certain type of data, for example, the pressure data. We can also use contour line and cutting plane to visualize in what area the air has the same pressure. 
We can add some data smooth and anti-aliasing features.

### 7. Author Contributions
Yabo Yan:

- Implement the data_load class.
- Map scalar value to different color for visualization
- Implement cutting plane for the visualization of pressure data
- Implement the visualization module for Q-criterion
- Implement reading air dataset and object dataset in a multi-threading way
- Implement exception handling to prevent the program from crushing

Yuqi Chai

- Design the preprocessing tools for data type conversion from legacy .vtk files to XML files
- Design the UI using Qt
- Design the mainwindow class to combine the VTK code into our program as a whole
- Design error-handing methods while opening files
- Use OpenFOAM to generate test datasets

Zhiyu Wang

- Design the streamline drawing algorithm for scalar field dataset
- Implemented the streamline drawing module for scalar field dataset
- Add the visualization module for absolute velocity value
- Implemented the streamline visualization module for vector field data. 
