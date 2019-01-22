###################################################
#      Rock, Paper, Scissors CS375 Project 8      #
#      Programmer: Austin Brummett				  #
#	   Instructor: Dr. Tony Richardson			  #
###################################################

Introduction
----------------------------------------------------

This project is composed of three files: referee.cpp, 
player.cpp, gui-player.cpp

A makefile is included along with all files needed for
the gui. To run the gui program, you need to have the gtk+3
libraries on your system.

Running the Program
------------------------------------------------------
1. After making, run the referee program to setup the 
   server for the game. use ./referee
2. Decide if you are running the text or gui-based 
   client.
   a. Text Based Variant:
   	  ./player <ip or hostname> <port number>
   	  a prompt will pop up askking for your choice
   	  If one of the players sends an exit, both players will 
   	  be sent the final score and be disconnected from the server.

   b. Gui Based Variant:
   	  ./gui-player
   	  the gui will pop up and then in the text entry boxes 
   	  that say IP and Port, put the hostname and the port #.
   	  After hitting connect in the textbox you will be told your
   	  player #. 
   	  From here you can then make choices with the radiobuttons
   	  and send them using the Send Choice button. 
   	  If one of the players sends an exit, both players will 
   	  be sent the final score and be disconnected from the server.

