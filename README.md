# Metroid-Dread-Autosplit
- ## **This README is not fully complete yet.**

This is an autosplitter/load remover for Metroid Dread that aims to remove major loading time differences from the run. It does this by analyzing your capture card's live video feed and sending keyboard input when necessary. As a result, this makes it compatible with any speedrun timer as long as it supports global hotkeys (such as LiveSplit).
## How to use
To start the autosplitter in the program, these are the basic steps:
1. Select your capture device when the program asks for it.
2. Ensure your speedrun timer has global hotkeys enabled.
3. Set the hotkeys in the program to match your speedrun timer's hotkeys. 
    - For example:
    
    ![Example](livesplitsettingsexample.png)
    
4. Load a route file 
5. Start the autosplitter

## Route file?
A route file is just a text file with a series of transports that helps the program know when it should pause correctly.

The program comes with routes for you to use, but you can make your own or edit an existing one if you need to.

The easiest way to create or edit a route file is to just use the built-in route editor with the program:
- The route will appear on the right, with the currently selected transport highlighted.
- The buttons on the very left allow you to specify a transport to use. 
- Once you've selected a transport, the middle buttons allow you to insert it.
- "Insert Above" and "Insert Below" will insert in relation to the highlighted transport on the right.
- Once you're done making the route, save it. 
- This route can now be loaded into the main tab of the program.

## Settings
Settings can be saved or loaded like routes. Settings include:
  - Starting the timer automatically
  - Split hotkey
  - Pause hotkey
  - Last route used

In addition to this, your current settings are continuously updated and saved into a separate file. 
This makes it so that when you start the program, it will load these settings automatically for convenience.

## Credits
- Created for the wonderful [Metroid Dread Speedrun Discord](https://discord.gg/BdmYr5TRGT) by OnlySpaghettiCode
- https://github.com/Aoderus for his super useful [capture device listing tool](https://github.com/Aoderus/OpenCV_CameraList_MSMF)
