#### The Premier Press Game Development Series
# Real-Time Rendering Tricks and Techniques in DirectX (2002)
> written by Kelly Dempski and edited by Andre LaMothe



This is the original source from the CD. I did not alter or modify any of the source. I have not worked through this book or the applications yet. However, based on the other book I have worked through by the same author from the same time period, I suspect to encounter much of the same issues. All of these issues are minor and easy to work through. 

To bring this into Visual Studio, I would recomend creating a new empty C++ project from scratch and copy over the course files into that new project. Then set the code to use unitext and run as a Windows desktop app. I have listed the instructions in the parent folder README for all of these books, and have listed it below again.

For the other book, I had to migrate the project forward from **Microsoft Developer Studio** projects (.dsp) and workspaces (.dsw) to **Visual Studio** projects. To do this, I simply added an empty C++ project, added the header (.h) and C++ files (.cpp) to the solution, changed the application type (Project Properties < Linker < System) from Console to a Windows Desktop, and changed the character-set (Properties < Advanced < Character Set) from Unicode to Multi-byte. I aslo had to go through the code and make a number of minor adjustments to get the application to compile and run. But nothing too complex.

## You may need to:
* Add **const** for some global variables. I had to do this for g_szApplicationName and g_szWindowClassName, which are used in just about every sample project in the other text.
* I had to add **#include "winnt.h"** to **Resources_Cursors.resource.h**.
* I had to change **DialogProc()** to return an **INT_PTR** and not a **BOOL**.

### Resource Files
* The older resource files look to use the same format used by Visual Studio 2022, but they did not work for me for the samples in the other text. 
  * Seems to be due to Visual Studio and some automation that it does for you.
    * If I had more experiance with C++, I expect I could have worked through this.
    * I created new resource files using the original .rc files as a blueprint.
* I had to set the dialog's style properties to "popup" to get them to work correctly.
  * Without setting the style to popup, I was unable to sellect anything on the form and the [OK] button would not fire an event.
