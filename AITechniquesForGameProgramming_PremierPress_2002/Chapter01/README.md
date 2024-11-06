# Chapter 1
This chapter steps you through four simple window apps that detail how to build a basic desktop application in C++. The four HellowWord samples are nearly identical to the project Visual Studio 2022 creates for you if you create a new C++ Windows desktop app today. This chapter does a really good job explaining the code. In fact, does a much better job than a couple of modern C++ Windows Desktop app books I have read.

Once you work through chapter 1, I recommend creating a new C++ Windows desktop application in Visual Studio and then review the project and compare the code to HellowWorld4. You will notice that they are nearly identical. The 2022 version does add a few more interesting features (menus, about popup message, etc.), which will be covered in chapter 2. 

#### HellowWord1 
A simple example of a WinMain() with a MessageBox.

#### HellowWord2 
A basic window desktop app that demostrates how to setup a windowing app template but that doesn't implement a message pump. So it will simply open and close. The intent is to demostrate everything up to needing the messaging to work.

#### HelloWord3
Extending HelloWord2 by adding a message handler. This version will run and popup a window that you can interact with.  

#### HelloWord4
Similar to HelloWord3 but extends the message handler to support user input. 
