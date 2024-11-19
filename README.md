This is a code repository for samples from a few out-of-print books from 2001-2003 from **The Premier Press - Game Development Series**. I am not uploading any of the copyrighted materials, demos, etc. I am only uploading the sample code.

To work with the samples in Visual Studio 2022, I had to migrate the project forward from **Microsoft Developer Studio** projects (.dsp) and workspaces (.dsw) to **Visual Studio** projects. To do this, I simply added an empty C++ project, added the sample header (.h) and C++ files (.cpp) to the solution, changed the application type (Project Properties < Linker < System) from **console** to a **Windows desktop**, and changed the character-set (Properties < Advanced < Character Set) from **unicode** to **multi-byte**. I had to go through the code and make a number of corrections to get the code to run. But nothing too complex. I have documented the changes in README.md files here and in the source.   

I have added a number of README.md files to various folders to explains some of the details I ran into while working through the samples.

## Currently Available
- _**AI Techniques for Game Programming**_, by Mat Buckland (Premier Press, 2002)
  - ISO image is available from https://archive.org/details/ai-tech  
  - I have included my updated version of the projects that use Visual Studio and C++17.
  - I am building new versions in C# that I will include as I complete them. 
- _**Real-Time Rendering Tricks and Techniques in DirectX**_, by Kelly Dempski (Premier Press, 2002)

## The Premier Press - Game Development Series
- _**Data Structures for Game Programmers**_, Ron Penton (Premier Press, 2003)
- _**Isometric Game Programming with DirectX 7.0**_, Ernest Pazera (Premier Press, 2001)
- _**OpenGL Game Programming**_, by Kevin Hawkins (Premier Press, 2001)
- _**Curves and Surfaces**_, by Kelly Dempski (Premier Press, 2003)
- _**AI Techniques for Game Programming**_, by Mat Buckland (Premier Press, 2002)
- _**Real-Time Rendering Tricks and Techniques in DirectX**_, by Kelly Dempski (Premier Press, 2002)

## Other Premier Press Game Dev Books
- _**Beginning OpenGL Game Programming**_, by Dave Astle and Kevin Hawkins (Premier Press, 2004)

