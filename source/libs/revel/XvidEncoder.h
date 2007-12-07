/*
Copyright (C) (2004) (Cort Stratton) <cort at cortstratton dot org>

This program is free software; you can redistribute it and/or 
modify it under the terms of the GNU General Public License 
as published by the Free Software Foundation; either 
version 2 of the License, or (at your option) any later 
version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
 

#ifndef XVIDENCODER_H
#define XVIDENCODER_H

#include "BaseEncoder.h"

#include <cstdio>
#include <string>

class Revel_XvidEncoder : public Revel_BaseEncoder
{
public:
    Revel_XvidEncoder();
    ~Revel_XvidEncoder();

    // Inherited from Revel_BaseEncoder
    virtual Revel_Error EncodeStart(const std::string& filename,
        const Revel_Params& params);
    virtual Revel_Error EncodeFrame(const Revel_VideoFrame& frame,
        int *frameSize = NULL);
    virtual Revel_Error EncodeEnd(int *totalSize = NULL);


protected:
    // Inherited from Revel_BaseEncoder
    virtual void Reset(void);

private:
    // operator= and copy constructor are disallowed for this class.
    Revel_XvidEncoder& operator=(const Revel_XvidEncoder& rhs) { return *this; }
    Revel_XvidEncoder(const Revel_XvidEncoder& rhs) {}

    int CreateXvidEncoder(void **encoderOut);

    // Handle to the XviD encoder.
    void *m_xvidEncoderHandle;
};

#endif // XVIDENCODER_H
