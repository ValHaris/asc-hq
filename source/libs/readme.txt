Here are some libraries that are used by ASC:
 bzlib  is used for the DOS and the Linux version. Although most Linux systems
        have this library preinstalled, there are two different and incompatible
	versions around ( 0.9 and 1.0 ). 
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
 
 
