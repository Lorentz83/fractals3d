Fractals3D
===========

Fractals3D is a c++/qt4 project which generate 3 dimensional fractals.
It applies the normal fractal math but, since it uses quaternions
instead of complex numbers, it has the 3rd dimension to display
(actually it would have a 4th dimension too, but it is discarded).

This project was developed by me, [Lorenzo Bossi](https://github.com/Lorentz83), 
and my friend Stefano Massera as project for the final examination of
the Computer Graphics class during our Master degree in Computer Science.
It was intended to be a sort of "extension" of the [Fractals ;)](https://github.com/Lorentz83/fractals)
project.


Building the project
--------------------

The code requires only QT4 and OpenGL, but it uses some gcc not standard
extensions.

It is tested on linux with QT 4.8.6 and g++ 4.8.2.
It used to work on windows too using an old mingw, to compile it with
vc++ the code needs some minor modification that I'll plan to do in
the next future.

To make the project move in the `src` directory and execute the
following commands: 

```
$ qmake
$ make
$ make install
```

note that the last command does not really install the program
(therefore no root privileges are required) but it just copies in the
`bin` directory the executable and all the files it needs to work. 


Documentation
-------------

The code itself is not really well documented and contains a lot of Italian
comments. I'm sorry for this issue.

The program has 2 different engines to render the fractals (plus one
dummy used only for test):
* Static: which compute the fractal in CPU and renders it as a 3D
  texture;
* Dynamic: which reprograms the video shaders to render better
  fractals;
