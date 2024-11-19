This is a code repository for samples from a few out-of-print books from 2001-2003 from **The Premier Press - Game Development Series**. I am not uploading any of the copywrighted materials from the CDs. I am only uploading the sample code and my updated version of the code.

To work with the samples in Visual Studio 2022, I had to migrate the project forward from **Microsoft Developer Studio** projects (.dsp) and workspaces (.dsw) to **Visual Studio** projects. To do this, I simply added an empty C++ project, added the sample header (.h) and C++ files (.cpp) to the solution, changed the application type (Project Properties < Linker < System) from **console** to a **Windows desktop**, and changed the character-set (Properties < Advanced < Character Set) from **unicode** to **multi-byte**. I had to go through the code and make a number of corrections to get the code to run. But nothing too complex. I have documented the changes in README.md files here and in the source.   

I have added a number of README.md files to various folders to explain some of the details I ran into to get the samples to work.

## Currently Available
- _**AI Techniques for Game Programming**_, by Mat Buckland (Premier Press, 2002)
  - ISO image is available from https://archive.org/details/ai-tech  
  - I have included my updated version of the projects that use Visual Studio and C++17.
  - I am building new versions in C# that I will include as I complete them. 
- _**Real-Time Rendering Tricks and Techniques in DirectX**_, by Kelly Dempski (Premier Press, 2002)

## The Premier Press - Game Development Series
> This is a list of the books in this series. The ones in bold I have read through. As I encounter more of the books, I will list them her for anyone else interested in knowing what books are available from this series. 
- _**Data Structures for Game Programmers**_, Ron Penton (Premier Press, 2003)
- _**Isometric Game Programming with DirectX 7.0**_, Ernest Pazera (Premier Press, 2001)
- _**OpenGL Game Programming**_, by Kevin Hawkins (Premier Press, 2001)
- _**Curves and Surfaces**_, by Kelly Dempski (Premier Press, 2003)
- _**AI Techniques for Game Programming**_, by Mat Buckland (Premier Press, 2002)
- _**Real-Time Rendering Tricks and Techniques in DirectX**_, by Kelly Dempski (Premier Press, 2002)
- _2D in Direct3D_
- _Special Effects Game Programming with DirectX_, by McCusky (2001)
- _Game Programming All in One_
- _2D Artwork and 3D Modeling for Game Artist_ (2002)
- _Focus on SDL_ (2002)
- _Game Programming Tricks of the Trade_ (2002)
- _Beginning Direct3D Game Programming_, by Wolfgang Engel (2003)
- _Programming Role Playing Games with DirectX 8.0_, by Adams (2002)

## Other Books in the Series
> These are books that probably are too old to be useful
- _Game Developer's Market Guide_
- _Java 2 Game Programming_, Thomas Petchel (2001)
- _The Art & Business of Creating Games_
- _Focus on MOD Programming in Quote III Arena_ (2002)
- _MAC Game Programming_ (2002)
- _Palm OS Game Programming_ (2002)
- _MUD Game Programming_, by Ron Penton (2003)  
- _Game Development Business & Legal Guide_, Ashley Salisbury (2003)
- _Visual Basic Game Programming with DirectX_ Jonathan S. Harbour (?)
- _Game Sripting Mastery_, Alex Varanese (2002)
- _Advanced Animation with DirectX_, Jim Adams, (2003)
- _Multiplayer Game Programming_, Todd Barron (2001)
- _Beginner's Guide to Darkbasic Game Programming_, Jonathan S. Harbour and Joshua Smith (2003)
- _Game Programming for Tenns, Maneesh Sethi (?)
  
## Other Premier Press Game Dev Books
- _**Beginning OpenGL Game Programming**_, by Dave Astle and Kevin Hawkins (Premier Press, 2004)

