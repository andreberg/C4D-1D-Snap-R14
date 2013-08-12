Introduction
------------

**AMa_1D_Snap** is a modelling tool plugin that allows you to  
perform a 1-dimensional snap between any points on a model in a   
multitude of ways using an interactive axis gizmo. This makes it  
very easy to snape points to the same plane or to offset them  
by the same distance that other points inherit relative to each  
other.

Installation
------------

Download the repository ZIP file by clicking the **Download ZIP**   
button on this page, and put the extracted folder containing the   
.dylib (OS X), .cdl (Win32) and .cdl64 (Win64) files in CINEMA 4D's  
plugin folder (or the equivalent directory in the user profile folder).

Supported Versions
------------------

The current version, named `AMa_1D_Snap-R14.zip` is for R14,  
but you can also find the old version for R12 through R13 called   
`AMa_1D_Snap-R12-R13.zip` in the `/dist` folder.

Development
-----------

If you want to use the included Xcode 4/VisualStudio 2012 projects  
it is crucial to put the downloaded source folder inside CINEMA 4D's  
plugin directory, under the main installation path (as opposed to   
the plugin folder found in user prefs). This is because all paths  
relevant to the compiler/linker are set up relative to the main  
plugin folder.


Basic Usage
-----------

With any Point object (polygonal or spline) active and in point selection   
mode, go to the *Plugins* menu, and choose *AMa_1D_Snap*.    

This will activate the 1D snap tool which you can use on your current point  
selection. Utilize the axis gizmo to specify which point should define the  
target dimension.

Attribution
-----------

Created by Antosha Marchenko in 2006.  
Updated from the 2006 source code by yours truly.

Copyright
---------

Copyright 2006, Antosha Marchenko  
Copyright 2013, André Berg

License
-------

Since I couldn't find a license notice with the source I downloaded,  
I am putting it under the following license:

Licensed under the Apache License, Version 2.0 (the "License");  
you may not use this file except in compliance with the License.  
You may obtain a copy of the License at

[http://www.apache.org/licenses/LICENSE-2.0](http://www.apache.org/licenses/LICENSE-2.0)

Unless required by applicable law or agreed to in writing, software  
distributed under the License is distributed on an "AS IS" BASIS,  
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  
See the License for the specific language governing permissions and  
limitations under the License.  
