# Data Visualization-Team B: A Generic data visualization tool for air flow data

Teammates: Yuqi Chai, Zhiyu Wang, Yabo Yan

# Program Design and overview:

![program architecture graph](https://github.com/ENGN2912B-2018/DV-B/blob/master/Architecture.png)

This is a generic data visualization program to visualize the data associated with flow dynamics. We assume that the dataset can come in different formats, for example .txt and .csv. We need to convert it to standard .vtu and .vtp format. So we need a data_preprocess class to preprocess the data and save it in files. After that, we can read the data into our program. Then our program shall come to the render_data class and render a default scene for display. There are three subclasses in render_data class: stream\line class, stream_surface class and cutting_plane class. Each of them represents one particular visualization case we want to implement. After rendering the data, we will use vtkRenderWindow in our display class to display them on the system. After the users can actually see the result, they can interact with our program with Qt or vtkRenderWindowInteractor. Users can zoom in, zoom out or select other render options (streamline, stream surface or cutting plane). After they click on the window, the program will process the request and render the data with the user-specified option and then display the result, thus forms a loop.
# Build instruction:
Before you start to compile the source code, you need the following dependencies:

VTK 8.1.0

gcc 4.8.3 or newer

QT

compile steps:
1. git clone [repository url]

2. cd DV-B

3. mkdir build

4. cd build

5. cmake ..

6. make


# How to use
The program is still under construction, after we finsih the program. It is expected that you can have a launch file in build directory, which can start a window, where the visualization result is displayed. You can choose how the data is visualized( stream line, stream surface and cutting plane) and which part of the data you want to see. The detailed instruction will come out after we finish the project. It is also expected that you can have a data transform program to transform some kinds of plain data into the data that we can handle. 

# dataset
The test dataset we are using coms from compute Canada(https://www.computecanada.ca/).

Download link: http://bit.ly/2gAEtwM
