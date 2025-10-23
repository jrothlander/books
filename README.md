This is a code repository for samples from a few out-of-print game dev books from 2001-2004 from **The Premier Press - Game Development Series**. I am only providing the sample code and my own updated versions of the code that I have migrated forward from **Microsoft Developer Studio** to **Visual Studio 2022** and have adjusted as necessary to compile and run. I will also include some of the projects I have rewritten in C#. 

If you are trying to track down the source code for any of these books, your may find this repository helpful. I did check with a few authors and editors to make sure this does not violate copyright law, and everyone tells me that it does not. However, if that is proven to be wrong, I will remove the original C++ code and only put up my own rewritten versions in C#. I think if Premier Press still existed today, they would make this code available here on GitHub as well. 

## Currently Available
> Source code is currently avaiable here for the following books:

- _**AI Techniques for Game Programming**_, by Mat Buckland (Premier Press, 2002)
  - ISO image is available from https://archive.org/details/ai-tech  
  - I have included my updated version of the projects that use Visual Studio and C++17.
  - I am building new versions in C# that I will include as I complete them.
    
- _**Real-Time Rendering Tricks and Techniques in DirectX**_, by Kelly Dempski (Premier Press, 2002)

- _**Focus on Curves and Surfaces**_, by Kelly Dempski (Premier Press, 2003) - I am working on getting the C++ and C# version and will upload this soon. 
  
## Changes Made to Migrate the Source
I had to migrate the project forward from **Microsoft Developer Studio** projects (.dsp) and workspaces (.dsw) to **Visual Studio** projects. To do this, I simply added an empty C++ project, added the header (.h) and C++ files (.cpp) to the solution, changed the application type (Project Properties < Linker < System) from **Console** to a **Windows Desktop**, and changed the character-set (Properties < Advanced < Character Set) from **Unicode** to **Multi-byte**. I had to go through the code and make a number of corrections to get the code to run. But nothing too complex. I have documented the changes in README.md files in the source folder and well as in the source code as comments.   

## The Premier Press - Game Development Series

### Why I am using and promoting these old books? For a very good reason!
In my opinion, software development book publishing has negatively changed over the past few decades. Today, what is missing from most modern game dev books is depth, expert details, tricks of the trade, things that come from a depth of knowledge and experiance from being in the trenches for decades. Many modern books on game dev are published by college students, hobbiest, and from authors with less than 2 or 3 years of experiance. The books read more like online blogs from someone trying to learn the technology and document the process. They tend to be riddled with errors and mistakes. But without the depth of experiance and knowledge, the author cannot offer any expert details and insights, which is what I am really look for when purchasing a book. And all of that for a price between $35 to $75.     

A software dev book should be logically organized and step the reader through a process to build up a set of knowledge that in the end, provides the reader with an intended level of understanding. Sections should be designed with intent with emphasis placed on the most important topics, which should be reflected in the amount of coverage. Most modern software development books fail this at every level. As an example, most devote the first couple of chapters, often 50 to 100 pages, to tool installation. I'm tired of purchasing a modern C# or C++ and having to flip through the first 30% of the book to get past how to install the Community Edition of Visual Studio. For a $75 book, I wasted my first $25. Microsoft literally has hundreds of excellent resources online. There is no need to repeat that in every book published on C# and C++. I think Premier Press has done a good job addressing these kinds of concerns and it is one of the reasons I prefer them.  

**The Premier Press Game Developer Series** breaks things down very well and are written by experts with many decades of experiance. I have found them to be excellent resources even if they are a bit dated. The books typically run between $5 to $10 used and are much better organized and written than most modern equivalents, and are very much still relevant for beginner to intermediate level developers. Most of these can be found used on hp.com, triftbooks.com, betterworldbooks.com, or eBay. Some of the content is too old to be useful, typically the first chapter or two. However, most of the content is really good and is timeless, such as game phyics, math(s), most of the C++, etc. I would not purchase these without the CD unless you have found the the CD online, which is hard to do. This is why I have included the code here, for those that have the books but no longer have access to the CDs. 

## Recommend Books
> The ones in blod are the ones I have read and would recommend to others learning game dev. The othes are ones I am currently reading or looking to read.
- _**Data Structures for Game Programmers**_, Ron Penton (Premier Press, 2003)
- _**Isometric Game Programming with DirectX 7.0**_, Ernest Pazera (Premier Press, 2001)
- _**OpenGL Game Programming**_, by Kevin Hawkins and Dave Astle (Premier Press, 2004)
- _**Focus on Curves and Surfaces**_, Kelly Dempski (Premier Press, 2003)
- _**AI Techniques for Game Programming**_, Mat Buckland (Premier Press, 2002)
- _**Real-Time Rendering Tricks and Techniques in DirectX**_, Kelly Dempski (Premier Press, 2002)
- _**Foucus on 3D Models**_, Evan Pipho (2003)
- _Focus on 2D in Direct3D_, Ernest Pazera (2002)
- _Focus on SDL_, by Ernest Pazera (2002)
- _Special Effects Game Programming with DirectX_, McCusky (2001)
- _Game Programming All in One_
- _2D Artwork and 3D Modeling for Game Artist_ (2002)
- _Game Programming Tricks of the Trade_ (2002)
- _Beginning Direct3D Game Programming_, by Wolfgang Engel (2003)
- _Programming Role Playing Games with DirectX 8.0_, 1st ed., by Jim Adams (2002)
- _Advanced Animnation with DirectX_, by Jim Adams (2003)

## Other Books in the Series
> Many of these are books that probably are too old to be useful or I have not reviewed them.
- _Focus on 3D Terrain Programming, Trent Polack (Premier Press, 2003)
- _Focus on MOD Programming in Quote III Arena_, Shawn Holmes (2002)
- _Focus on Photon Mapping_, Marlon John (2003)
- _Game Developer's Market Guide_
- _Java 2 Game Programming_, Thomas Petchel (2001)
- _The Art & Business of Creating Games_
- _MAC Game Programming_ (2002)
- _Palm OS Game Programming_ (2002)
- _MUD Game Programming_, by Ron Penton (2003)  
- _Game Development Business & Legal Guide_, Ashley Salisbury (2003)
- _Visual Basic Game Programming with DirectX_, Jonathan S. Harbour (2002)
- _Game Sripting Mastery_, Alex Varanese (2002)
- _Advanced Animation with DirectX_, Jim Adams, (2003)
- _Multiplayer Game Programming_, Todd Barron (2001)
- _Beginner's Guide to Darkbasic Game Programming_, Jonathan S. Harbour and Joshua Smith (2003)
- _Game Programming for Teens_, Maneesh Sethi (?)
- _Beginning Game Audio Programming_, Mason McCuskey (Premier Press, 2003)
  
## Other Premier Press Game Dev Books (not in this series)
- _**Beginning OpenGL Game Programming**_, by Dave Astle and Kevin Hawkins (Premier Press, 2004)
- _Programming Role Playing Games with DirectX 8.0_, 2nd ed., by Jim Adams (2004) - Note: the first edition (2002) is a part of the Game Dev Series above.
