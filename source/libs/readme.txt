Here are some libraries that are used by ASC:
 bzlib  is included for the DOS version of ASC; the linux version requires
        libbz2 to be installed
 jpeg   the DOS version requires the Independant Jpeg Library. I had to modify 
        parts of it to be able to load images out of ASCs container files. 
        Since the Linux version does not use it, it is not included here any 
        more. If you want to build the DOS version of ASC, you can get the 
        compiled library (for Watcom C/C++ 11.0) at 
           http://www.asc-hq.org/jpeg_ascdos_bin.zip
        and the source code at
           http://www.asc-hq.org/jpeg_ascdos_src.zip
 STL    is not included any more. For the DOS-Version, get STLPORT from
        www.stlport.org
 triangul  a polygon triangulation library. Used by DOS and Linux version
 
 
