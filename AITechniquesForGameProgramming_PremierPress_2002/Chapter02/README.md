#### Had to add the following:
- Added **const** to nearly all the main.cpp global variables for g_szApplicationName and g_szWindowClassName
- Added **#include "winnt.h"** to Resources_Cursors.resource.h.
- Had to change DialogProc() to return **INT_PTR** and not a **BOOL**.

#### Resource Files
- The resource fiels did not work for me.
  - It seems to be due to Visual Studio generating them and after 20 years there are too many changes for it to still work.
  - I created new ones using the source .rc file as a blueprint.  
  - I had to set the dialog's style poroperty to "popup" to get them to work correctly.
    - Without setting the style to popup, I was unable to sellect anything on the form and the [OK] button would not fire an event. 
