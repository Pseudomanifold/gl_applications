# Some OpenGL/GLUT applications

I attended a [nice lecture about Computer
Graphics](http://www.iwr.uni-heidelberg.de/groups/ngg/CG2008/lecture.php).
We were given several interesting assignments each week (mostly C/C++
with GLUT). I recently dug out some of my programs and decided to share
them with the world. If I find anything else, I will expand this
section. 

The code below has been polished a bit and is hereby released under a BSD
licence. The licence and copyright do *not* extend to the time routine, which
has been [supplied by my
instructors](http://www.iwr.uni-heidelberg.de/groups/ngg/CG2008/Txt/mytime.cpp).

# gl_maze

`gl_maze` is my lame attempt at creating the next generation of
lame video games. The assignment covered keyboard functions using GLUT, but I
got carried away and added a maze to the program (a maze should be part of all
bigger software packages, in my opinion. That would be a-maze-ing.).  Included
in the download is also a kind of background story about the "game". By the
way: The algorithm used to create the maze is known as ["hunt and kill"](http://www.digitalstratum.com/programming/hkmaze_ti_xb).

# gl_particles

`gl_particles` is a very simple particle system using
OpenGL and GLUT. This program shows a fountain-like thingy that constantly
spawns new particles. Quite soothing, actually, if you look long enough at it.

# gl_photon_map

`gl_photon_map` creates a photon map for a static scene (whose data is
hard-coded). The scene resembles the famous [Cornell
Box](http://www.graphics.cornell.edu/online/box).  If you run the
application, a number of photons will be emitted from the light source. This
photon map is then drawn.
