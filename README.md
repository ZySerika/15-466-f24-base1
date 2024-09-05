# PacCars

Author: Dante Liang

Design: A no-ghost pacman game where pellets randomly change color from green to yellow and then to red. The player should avoid pellets turning red and pick them up when they are green or yellow. 

Screen Shot:

![Screen Shot](dist2/car.png)

How Your Asset Pipeline Works:
Assets include sprites and levels.
Sprite is drawn using GIMP and inputed as .png files. 
Levels are 28x28 txt files that one can edit easily.
Processing code includes Two c++ scripts that processes one of sprite files and level files. The scripts parse txt and png information and stores them into POD chunks, so runtime code reads it on Load. 

(TODO: make sure the source files you drew are included. You can [link](your/file.png) to them to be a bit fancier.)

How To Play:

The game currently has loading failures. Use arrow keys to control the paccar and pick up green/yellow pellets. Avoid red ones. 

This game was built with [NEST](NEST.md).

