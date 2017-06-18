# Maya Playblast OpenImageIO
Playblast using OpenImageIO (OIIO) to save image files inside Autodesk Maya.

This project is a work in progress, the goals are:
 - Simple plug-in command, relies on python wrapper to do the heavy lifting.
 - Command flags should be logical - we will not maintain compatibility with 'maya.cmds.playblast'.
 - OpenImageIO should be used for file writing and any image manipulation.
 - Maya playblasts images should be able to be written in linear ACES colour-space, using OpenColorIO for implementation of colour conversions.
 - 3D Motion Blur should be possible. This means playblasting more than one image, then averaging the resulting pixels together. This should be done efficiently.

## Features
- Work in progress.
- See goals above.

## Usage
- Compile plugin using CMake.
- Copy library (.so file) into plugin path (MAYA_PLUGIN_PATH)
  - For example, create the following directory and copy the .so file into it: 
    - /home/$USER/maya/2016/plug-ins/
- Open Maya
- Load "playblastOIIO", using MEL or Python.
  - MEL:
    - `loadPlugin "playblastOIIO";`
  - Python:
    - `maya.cmds.loadPlugin("playblastOIIO")`
- Playblast the current Maya scene:
  - Python:
    - `maya.cmds.playblastOIIO(filepath='/path/to/output_filename', startFrame=1, endFrame=24, imageSize=(1920, 1080))`

## Building and Install

### Dependencies

- C++ compiler with support for C++11
- CMake 2.6+
- OpenImageIO 1.2+ (https://sites.google.com/site/openimageio/home)
- Autodesk Maya 2016+ 
  - Older versions could work by have not been tested

### Build

_To be written._

### Install

_To be written._

## Limitations and Known Bugs 

- On Linux build there is a symbol clash between boost inside Maya and OpenImageIO, this causes any parallel computation to crash Maya immediately. Because of this bug the reader has been forced to use a single thread (disable boost) for all image computation inside OpenImageIO.  