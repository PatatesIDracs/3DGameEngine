# 3DGameEngine
Simple 3D Engine made for learning purposes

## Instalation
Drag the JoPe engine folder inside the .rar file to somewhere in your computer.   
Open the 3DEngine.exe file inside the data folder.

## Usage 
Use left click to move the camera    
Use right click to rotate the camera    
Drag and drop any .fbx or .obj file in the screen to load it, if the .fbx has a texture atached put the texture inside the Game folder.  
[F] key will center the camera in the object    
[C] key will center the camare in the origin    
In some cases the object might be too big for the viewer to render, the viewer will try to rescale the object so it's visible, this process will be written in the console, we recommend checking both console and scale in properties to chack that the file has not been resized. This process doesn't always work, if you still can see the object try prressing [C] to go back to the origin and you may find it.   


**Multiple texture**   
Right now the viewer doesn't support multiple texture CHANNELS, this means that you can't load normal or specular maps.   
However if your model is made of diferent meshes and each one has a diferent texture, the textures will be applied to the correct mesh, note that in this case the path to the texture must be defined correctly, if it isn't you won't be able to load textures correctly since the drag & drop PNG functionality doesn't support this method and the same texture will be loaded to all the meshes.   

**Sample Geometry**    
Some sample geometry is provided to test the viewer, below you can find the original sources.    
[Sci-fi Spacechip](https://www.cgtrader.com/free-3d-models/space/spaceship/sf-imperial-corvette-f3)     
[Troll](https://www.cgtrader.com/free-3d-models/character/fantasy/earthborn-troll)   
[Megalodon](https://free3d.com/3d-model/megalodon-battlefield-4-67390.html)

## Credits 
This software was made by Pere Rifà and Joan Pareja and is licensed under the MIT license.    
You can check our github page [here](https://github.com/PatatesIDracs/3DGameEngine).

**Libraires**    
This software was developed thanks to the opensource libraries listed below, we do not take credit for any functionality provided by these libraries.   
This software was made using [SDL](https://www.libsdl.org/) libraries.  
We used [OpenGL](https://www.opengl.org/) with [GLEW](http://glew.sourceforge.net/) for our 3D render.
[Assimp](http://assimp.sourceforge.net/) was used to load .fbx data.   
[Devil](http://openil.sourceforge.net/) was used to read texture files.
[MMGR](http://www.paulnettle.com/) memory manager was used to monitor memory usage in runtime.

  
**Tools**    
[Deleaker](http://www.deleaker.com/) is used to control all memory leaks.


## Changelog
### v0.1.1 - Mesh viewer
* Removed non used primitives for release
* Camera controls imporved
* Loaded .fbx will search for a .png texture in the .fbx path
* UI is able to receive input
* "F" key added to center the camera onto the fbx
* Properties window added, showing info about the loaded geometry and texture (if the fbx has more than one mesh the information show is the combination of all the meshes)
* UI windows blocked and rearanged
* Application now displays memory usage
* Software versions are now displayed


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
