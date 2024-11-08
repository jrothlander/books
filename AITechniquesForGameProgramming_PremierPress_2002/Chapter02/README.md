Had to add the following:
- const to nearly all the main.cpp global variables for g_szApplicationName and g_szWindowClassName
- #include "winnt.h" to Resources_Cursors.resource.h.

- I could not get the Resource files to work by simply loading the version here. I had to recreate them to get them to work. I am not sure why that is. I suspect it has to do with Visual Studio autogenerating them. 
- Had to change DialogProc() to return INT_PTR, but still cannot get Resources_Dialog_Box to fire off the dialog button press event. 

