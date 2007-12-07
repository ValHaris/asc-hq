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

#include "revel.h"
#include "BaseEncoder.h"

#include "XvidEncoder.h"

#include <cassert>
#include <cstdio>
#include <string>
using std::string;
#include <map>
using std::map;

namespace
{
    typedef map<int,Revel_BaseEncoder*> EncoderMap;
    typedef EncoderMap::iterator EncoderMapItor;
    EncoderMap g_encoders;
    int g_nextHandle = 0;

    bool IsEncoderValid(const int handle)
    {
        EncoderMapItor itor = g_encoders.find(handle);
        if (itor == g_encoders.end())
            return false;
        return true;
    }

    Revel_Error EncoderFactory(const Revel_VideoCodec codec,
        Revel_BaseEncoder **encoderOut)
    {
        Revel_BaseEncoder *enc = NULL;
        
        switch(codec)
        {
        case REVEL_CD_XVID:
            enc = new Revel_XvidEncoder;
            break;
        default:
            return REVEL_ERR_PARAMS;
        }

        if (enc == NULL)
            return REVEL_ERR_MEMORY;
        *encoderOut = enc;
        return REVEL_ERR_NONE;
    }
}

int Revel_GetVersion(void)
{
    return REVEL_VERSION;
}
int Revel_GetApiVersion(void)
{
    return REVEL_API_VERSION;
}

Revel_Error Revel_CreateEncoder(int *encoderHandle)
{
    if (encoderHandle == NULL)
        return REVEL_ERR_PARAMS;

    *encoderHandle = g_nextHandle++;
    g_encoders[*encoderHandle] = NULL;
    
    return REVEL_ERR_NONE;
}


void Revel_InitializeParams(Revel_Params *params)
{
    if (params == NULL)
        return;
    params->width = params->height = 1;
    params->hasAudio = 0;
    params->audioBits = 16;
    params->audioChannels = 2;
    params->audioRate = 44100;
    params->audioSampleFormat = REVEL_ASF_UNKNOWN;
    params->codec = REVEL_CD_XVID;
    params->frameRate = 30;
    params->quality = 1.0f;
}


Revel_Error Revel_EncodeStart(int encoderHandle, const char* filename,
                              Revel_Params *params)
{
    if (filename == NULL || params == NULL)
        return REVEL_ERR_PARAMS;
    if (!IsEncoderValid(encoderHandle))
        return REVEL_ERR_INVALID_HANDLE;

    // Create the codec-specific encoder if it doesn't already exist.
    if (g_encoders[encoderHandle] == NULL)
    {
        Revel_BaseEncoder *newEnc = NULL;
        Revel_Error revError = EncoderFactory(params->codec, &newEnc);
        if (revError != REVEL_ERR_NONE)
            return revError;
        g_encoders[encoderHandle] = newEnc;
    }

    return g_encoders[encoderHandle]->EncodeStart(filename, *params);
}


Revel_Error Revel_EncodeFrame(int encoderHandle,
                              Revel_VideoFrame *frame, int *frameSize)
{
    if (frame == NULL)
        return REVEL_ERR_PARAMS;
    if (!IsEncoderValid(encoderHandle))
        return REVEL_ERR_INVALID_HANDLE;
    if (g_encoders[encoderHandle] == NULL)
        return REVEL_ERR_BUSY;
    return g_encoders[encoderHandle]->EncodeFrame(*frame, frameSize);
}


Revel_Error Revel_EncodeAudio(int encoderHandle, void *sampleBuffer,
                              int numBytes, int *numTotalBytes)
{
    if (!IsEncoderValid(encoderHandle))
        return REVEL_ERR_INVALID_HANDLE;
    if (g_encoders[encoderHandle] == NULL)
        return REVEL_ERR_BUSY;
    return g_encoders[encoderHandle]->EncodeAudio(sampleBuffer, numBytes,
        numTotalBytes);
}


Revel_Error Revel_EncodeEnd(int encoderHandle, int *totalSize)
{
    if (!IsEncoderValid(encoderHandle))
        return REVEL_ERR_INVALID_HANDLE;
    if (g_encoders[encoderHandle] == NULL)
        return REVEL_ERR_BUSY;
    return g_encoders[encoderHandle]->EncodeEnd(totalSize);
}



Revel_Error Revel_DestroyEncoder(int encoderHandle)
{
    if (!IsEncoderValid(encoderHandle))
        return REVEL_ERR_NONE; // okay to delete invalid handles.

    // Delete the encoder, if there is one.
    if (g_encoders[encoderHandle] != NULL)
    {
        delete g_encoders[encoderHandle];
        g_encoders[encoderHandle] = NULL;
    }

    // Finally, remove the map entry.
    g_encoders.erase(encoderHandle);
    return REVEL_ERR_NONE;
}
