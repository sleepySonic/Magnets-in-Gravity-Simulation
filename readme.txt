# Magnets-in-Gravity-Simulation

Interactive Magnet Box  | By: amimotoAnna
------------------------------------------
A simulation manipulating negative and positive magnetic balls in gravity confined in a box.
Programmed with GLU/GLUT and C++, OpenGL.
------------------------------------------
  NOTE: Blue	    ...	Negative
	      Red	      ...	Positive
        Purple 	  ...	Neutral
The following key bindings perform their respective actions as given.
	arrow keys ---------------- moves the camera angling and viewpoint
  right button -------------- linked to GLUT menu choices:
                                    --- changes number of magnets
		                                --- randomizes stationary magnet locations
				                            --- changes polarities of stationary magnets
				                            --- changes polarities of flying magnets
				                            --- restarts program to default settings
				                            --- exit
	left button --------------- click and hold down to drag magnets
				                            --- CTRL-click to change polarities cyclically
	k ------------------------- increases number of flying magnets to 10 maximum
	j ------------------------- decreases number of flying magnets to 1 minimum
	> ------------------------- increases attraction/ repulsion power between magnets
	< ------------------------- decreases attraction/ repulsion power between magnets
	. ------------------------- increases gravitational acceleration constant
	, ------------------------- decreases gravitational acceleration constant
	m ------------------------- increases the mass of flying magnets
	n ------------------------- decreases the mass of flying magnets
	x, X, y, Y, z, Z ---------- changes light source position
