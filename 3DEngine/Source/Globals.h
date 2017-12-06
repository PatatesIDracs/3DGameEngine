#ifndef __GLOBALS_H__
#define __GLOBALS_H__


// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#include <windows.h>
#include <stdio.h>


//Define the directory paths of debug and release

//Base directory for debug
#ifdef _DEBUG

#define JOPE_DATA_DIRECTORY "../Data/"

#endif // _DEBUG

//Base directory for release
#ifndef _DEBUG

#define JOPE_DATA_DIRECTORY "./Data/"

#endif // _RELEASE

//Subfolders or files 
#define JOPE_CONFIG_FILENAME "config.json"
#define JOPE_ASSETS_FOLDER "Assets/"
#define JOPE_LIBRARY_FOLDER "Library/"
#define JOPE_ENGINE_FOLDER "Engine/"
#define JOPE_MESHES_FOLDER "Meshes/"
#define JOPE_TEXTURE_FOLDER "Textures/"
#define JOPE_ASSETS_FBX_FOLDER "Fbx/"
#define JOPE_ASSETS_MESH_FOLDER "Meshes/"
#define JOPE_ASSETS_TEXTURE_FOLDER "textures/"

#define MJOPE ".mjope"
#define TEXFORMAT ".tjope"
#define SCENEFORMAT ".jope"
#define METAFORMAT ".meta"

//Identifier for scene serialitzation
#define GAMEOBJECTIDENTIFIER -1
#define COMPONENTIDENTIFIER	 -2
#define ENDFILEIDENTIFIER	 -3


#define LOGC(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI


typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};


// Configuration -----------
//Here in case a config file isn't found
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define VSYNC false	
#define TITLE "JoPe Engine"

#endif // !__GLOBALS_H__