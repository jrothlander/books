# Chapter 1

Chapter 1 steps you through a basic Windows Desktop app written in C++. The four HellowWord samples aew nearly identical code to the default project that Visual Studio 2022 will create for you if you create a new C++ Windows desktop app. If you are using Visual Studio, I recommend creating a new C++ Windows desktop application and reviewing the project created while reading through chapter 1. The 2022 version does add a few more interesting features (menus, about popup message, etc.). The code is essentually the same and chapter 1 does a great job explaining the details of how it works. It does a much better job than most of the modern C++ Windows Desktop app books I have read. 

#### HellowWord1 
A simple example of a WinMain() with a MessageBox.

#### HellowWord2 
A basic window desktop app that demostrates how to setup a windowing app template but that doesn't implement a message pump. So it will simply open and close. The intent is to demostrate everything up to needing the messaging to work.

#### HelloWord3
Extending HelloWord2 by adding a message handler. This version will run and popup a window that you can interact with.  

#### HelloWord4
Similar to HelloWord3 but extends the message handler to support user input. 
