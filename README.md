# 3DGameEngine
Simple 3D Engine made for learning purposes

## Instalation
Drag the JoPe engine folder inside the .rar file to somewhere in your computer.
Open the 3DEngine.exe file inside the data folder.

## Usage 
Use left click to move the camera
Use right click to rotate the camera
Drag and drop any .fbx file in the screen to load it, if the .fbx has a texture atached put the texture inside the Game folder.


## Changelog
### v0.1 - Mesh viewer
* Removed glut dependecy
* Render redone (WIP), direct mode rendering is been replaced with indices and vertex arrays, some old primitives are still using direct mode, they can't be used in the release but are on the source code, they will change in future releases.
* Camera controls redone(WIP), right click to rotate the camera and left click to move it.
* The software is now able to render .fbx files, drag and drop them in the screen to visualize it. (Right now only one at a time)
* Added face normals render (only at the sphere), will be added to all figures in the future.
* Profiler updated, now it displays each module separately. It also plots the information in realtime and has a button to stop the recodring.
* Fixed a small bug that made the program unable to read the config.json file.
* Console is working and displays debug messages

### v0.0.2
* Renderer configuration options
* Hardware shows some GPU details
* Simple profiler added (no display functionality at the moment)
* Config shows all configurable modules, most of them are placeholders
* Windows now have a close button
* Simple sphere in the middle to test render configuration

### v0.0.1
* Main menu added
* Close app is now on File->Exit
* Added configuration window
* Added configuration->harware, shows the details of the CPU
* Added window configuration menu to change window properties
* Added application configuration, shows app details
* Added a console placeholder window
* Help menu with info about the software and link to the documentation, bug report and release page
(right now there is no documentation since the software is almost non-existant, documentation will be added when the software gains more functionalities)

### v0.0.test.{build}
Yes the name is weird, we tried to automatize the release and it didn't work
* dear ImGui implemented
* MathGeoLib implemented
* Window to close the app and show the ImGui demo
* Simple window with some MathGeoLib test
