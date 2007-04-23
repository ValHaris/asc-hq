
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef memsizeinterfaceH
 #define memsizeinterfaceH
 
 class MemorySizeQueryInterface {
    public:
       virtual int getMemoryFootprint() const = 0;
       virtual ~MemorySizeQueryInterface(){};
 };
 
 template<typename T>
 int getMemoryFootprint( const T& t ) 
 {
    return t.getMemoryFootprint();
 }; 
 
 template<class T> 
 struct MemorySum : public unary_function<T, void>
 {
    MemorySum() : size(0) {}
    void operator() (const T& x) { size += getMemoryFootprint(x); }
    void operator() (const T* x) { size += getMemoryFootprint(*x); }
    int size;
 };
 
 
#endif
