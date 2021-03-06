﻿# 3DGameEngine
Simple 3D Engine made for learning purposes

## Instalation
Drag the JoPe engine folder inside the .rar file to somewhere in your computer.   
Open the 3DEngine.exe file inside the data folder.

## Usage 

Use left+alt click to rotate the camera    
Use right click to rotate the camera and activate FPS movement      
Drag and drop files to import them, current file soported: .fbx, .obj, .tga, .png. Scenes imported must be loaded from File->Load      
Scene files (.fbx, .obj) will only trye to load textures that are in the same folder as the .fbx            
Use the mouse wheel to zoom in and out, zoom is restricted so you can't go inside the object. If you need a more precise view of some parts we suggest on using the FPS camera       
[F] key will center the camera in the object              
Click the Play button to turn on and off the game mode, click again to stop. When in play mode the Play button will turn green    
Click on a GameObject to select it     
Use the guizmos to modify the selected object transform       
[W] Activate translation guizmo    
[E] Activate rotation guizmo      
[R] Activate scale guizmo       
To save a scene go to File->Save and input the name of the new file, and finally click Save       
To load a scene go to File->Load select a file (the selected file will be displayed below) and click load      
Use the same method to load files from assets window (only meshes(.mjope) and scenes (.jope) can be loaded)        
Meshes will be loaded as a GameObject with a Mesh component (plus a mesh renderer to render it)     
To switch from console or assets window click on the buttons at the bottom of the screen, the selected mode will be highlighted      
Open the tabs in the properties window to open the gameobject components options    
Camera culling is calculated with the MainCamera by default assigned to the camera created in the default scene, to see all objects in the scene select the camera and turn off the main 
camera button, this will deactivate the camera culling for that camera and all objects will be visible       
Click on File->Load_default_scene to clean the scene       

**FPS movement**   
[W] move forwards   
[A] move left   
[S] move backwards   
[D] move right   
[SPACE] shoot balls

**Game mode**
During game mode the camera selected as main camera will be the one used to render the scene, if there is no main camera selected the editor ghost camera will be used.
In game mode objects can't be edited and almost all UI will be hidden.
PhysX simulation will take place during game mode

**Innovation**
Render uses texture order to render
Using PhysX libraries
Scene is serialized in own binary format      
Guizmos added to edit GameObjects      

**Multiple texture**   
Right now the viewer doesn't support multiple texture CHANNELS, this means that you can't load normal or specular maps.   
However if your model is made of diferent meshes and each one has a diferent texture, the textures will be applied to the correct mesh, note that in this case the path to the texture must be defined correctly, if it isn't you won't be able to load textures correctly since the drag & drop PNG functionality doesn't support this method and the same texture will be loaded to all the meshes.   

**Sample Geometry**    
Some sample geometry is provided to test the viewer, below you can find the original sources.     
[Troll](https://www.cgtrader.com/free-3d-models/character/fantasy/earthborn-troll)    
[Megalodon](https://free3d.com/3d-model/megalodon-battlefield-4-67390.html)

## Credits 
This software was made by Pere Rifà and Joan Pareja and is licensed under the MIT license.    
You can check our github page [here](https://github.com/PatatesIDracs/3DGameEngine).

**Libraires**    
This software was developed thanks to the opensource libraries listed below, we do not take credit for any functionality provided by these libraries.     
This software was made using [SDL](https://www.libsdl.org/) libraries.      
We used [OpenGL](https://www.opengl.org/) with [GLEW](http://glew.sourceforge.net/) for our 3D render.        
[MathGeoLib](http://clb.demon.fi/MathGeoLib/nightly/) used for logic calculations         
[Assimp](http://assimp.sourceforge.net/) was used to load .fbx data.      
[Devil](http://openil.sourceforge.net/) was used to read texture files.     
[MMGR](http://www.paulnettle.com/) memory manager was used to monitor memory usage in runtime.    
[ImGui](https://github.com/ocornut/imgui) used for all UI elements       
[ImGuizmo](https://github.com/CedricGuillemet/ImGuizmo) used to create the guizmos    
[PhysX](http://www.nvidia.es/object/nvidia-physx-es.html) used for physics.

  
**Tools**    
[Deleaker](http://www.deleaker.com/) is used to control all memory leaks.


## Changelog
### v0.3 - Assigment 3
* Implemented PhysX
* Added component RigidBody
* Added component Collider
* Added component Distance joint
* Able to shoot balls
* Bug fixing


### v0.2.1 - Assigment 2
* Added simple assets window to Load scenes and meshes        
* Added an octree render option in the render config        
* Added a clean scene button on File Main menu     

### v0.2 - Assigment 2
* Polished fbx importing     
* Added .meta files to resources loaded     
* All resources and components that use them do a correct save and load      
* Load and Save scene UI elements are functional      
* Contenents in assets are imported to library if they weren't yet imported        
* Editing a texture in assets during runtime will change it's resource when the user goes back to the engine, only textures supported do not try with other files      


### v0.1.2 - Assigment 2
* Changed importing methods
* Improved fbx importing
* All usable imported files are now stored as resources in Library (currently only meshes and textures)
* Added GameObject and components system
* Added a hierarchy window for the scene
* Added a properties window for the gameobjects
* Added camera component
* Added frustum culling for cameras	
* Gameobjects can be selected by clicking on them (mouse picking)
* Reference counting for resources added
* Added Octree optimitzation for Static objects
* Added time managment and Game/Editor mode (poor feedback right now)

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
