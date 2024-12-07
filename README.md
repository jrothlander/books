This is a code repository for samples from a few out-of-print game dev books from 2001-2004 from **The Premier Press - Game Development Series**. I am only providing the sample code and my own updated versions of the code that I have migrated forward from **Microsoft Developer Studio** to **Visual Studio 2022** and have adjusted as necessary to compile and run. I will also include some of the projects I have rewritten in C#. 

If you are trying to track down the source code for any of these books, your may find this repository helpful. 

## Currently Available
> Source code is currently avaiable here for the following books:

- _**AI Techniques for Game Programming**_, by Mat Buckland (Premier Press, 2002)
  - ISO image is available from https://archive.org/details/ai-tech  
  - I have included my updated version of the projects that use Visual Studio and C++17.
  - I am building new versions in C# that I will include as I complete them.
    
- _**Real-Time Rendering Tricks and Techniques in DirectX**_, by Kelly Dempski (Premier Press, 2002)

## Changes Made to Migrate the Source
I had to migrate the project forward from **Microsoft Developer Studio** projects (.dsp) and workspaces (.dsw) to **Visual Studio** projects. To do this, I simply added an empty C++ project, added the header (.h) and C++ files (.cpp) to the solution, changed the application type (Project Properties < Linker < System) from **Console** to a **Windows Desktop**, and changed the character-set (Properties < Advanced < Character Set) from **Unicode** to **Multi-byte**. I had to go through the code and make a number of corrections to get the code to run. But nothing too complex. I have documented the changes in README.md files in the source folder and well as in the source code as comments.   

## The Premier Press - Game Development Series

### Why I am using and promoting these old books? For a very good reason!
Book publishing seems to have changed in the negative over the past few decades. Today, software dev book are not great resources that you save for years and refer back to, but they are more like online intro tutorials that you read through only once. I get that this is not always the case and I often find great newly released dev books. I also understand that this is mostly due to the availability of resources online, so that books serving as references for years to come doesn't really make sense today as it did a few decades ago. It is also due technology changing so fast that it is hard to publish a book that can get to print before the technology has moved on. I think that book publishers have had to change with the technology and this is where we have landed today. However, it doesn't mean I like it. 

When I attack a new technology and/or language, my preference isn't to run through dozens of intro and beginner level online articles and tutorials. I would rather find a well written book that steps you logically through the details and covers all of the major points of what you need to know, then serves a reference for you to return to over time. Modern books just do not offer that. What is missing from modern game dev books is depth, expert details, tricks of the trade, things that come from a depth of knowledge and experiance from being in the trenches for decades. Many modern books on game dev are published by college students, hobbiest, and from authors with less than 2 or 3 years of experiance. The books read more like blogs where someone is trying to learn the technology and has documented the process. But without the depth of experiance and knowledge, they cannot offer any expert details and insights. For books that retail between $35 to $75, I would rather work with poorly written online tutorial.     

That is what **The Premier Press Game Developer Series** does well in my opinion. It breaks things down well and steps you through the process. They are written by experts with many decades of experiance. I have found them to be excellent resources even if they are a bit dated. The books will typically run between $5 to $10 used and are much better organized and written than most modern equivalents, and are very much still relevant for beginner to intermediate level developers. Most of these can be found used on hp.com, triftbooks.com, or eBay. Some of the content is too old to be useful, typically the first chapter or two. However, most of the content is really good and is timeless, such as game phyics, math, most of the C++, etc. I would not purchase these without the CD unless you have found the the CD online, which is hard to do. This is why I have included the code here, for those that have the books but no longer have access to the CDs.

The following lists the books in this series that I am personally interested in and have worked through or that I am working through currently. The ones in bold I own and have worked through and plan to this repository. My struggle is that many of the books are missing the CDs. 

## Recommend Books
> The ones in blod are the ones I have read and would recommend to others learning game dev. The othes are ones I am currently reading or looking to read.
- _**Data Structures for Game Programmers**_, Ron Penton (Premier Press, 2003)
- _**Isometric Game Programming with DirectX 7.0**_, Ernest Pazera (Premier Press, 2001)
- _**OpenGL Game Programming**_, by Kevin Hawkins (Premier Press, 2001)
- _**Curves and Surfaces**_, Kelly Dempski (Premier Press, 2003)
- _**AI Techniques for Game Programming**_, Mat Buckland (Premier Press, 2002)
- _**Real-Time Rendering Tricks and Techniques in DirectX**_, Kelly Dempski (Premier Press, 2002)
- _**Foucus on 3D Models**_, Evan Pipho (2003)
- _Focus on 2D in Direct3D_, Ernest Pazera (2002)
- _Special Effects Game Programming with DirectX_, McCusky (2001)
- _Game Programming All in One_
- _2D Artwork and 3D Modeling for Game Artist_ (2002)
- _Focus on SDL_, by Ernest Pazera (2002)
- _Game Programming Tricks of the Trade_ (2002)
- _Beginning Direct3D Game Programming_, by Wolfgang Engel (2003)
- _Programming Role Playing Games with DirectX 8.0_, 1st ed., by Jim Adams (2002)

## Other Books in the Series
> These are books that probably are too old to be useful and that I have not reviewed.
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
- _Game Programming for Teens_, Maneesh Sethi (?)
- _Beginning Game Audio Programming_, Mason McCuskey (Premier Press, 2003)
  
## Other Premier Press Game Dev Books
- _**Beginning OpenGL Game Programming**_, by Dave Astle and Kevin Hawkins (Premier Press, 2004)
- _Programming Role Playing Games with DirectX 8.0_, 2nd ed., by Jim Adams (2004) - Note: the first edition (2002) is a part of the Game Dev Series above.

