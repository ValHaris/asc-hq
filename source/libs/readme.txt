Here are some libraries that are used by ASC:
 bzlib  is included for the DOS version of ASC; the linux version requires
        libbz2 to be installed
 jpeg   the independant jpeg library. I had to modify parts of it to be able
        to load images out of ASCs container files. That's why it is included
	here. The Linux port does not use it yet and can't handle JPEG files.
 STL    is not included any more.
 triangul  a polygon triangulation library. Used by DOS and Linux version
 
 
