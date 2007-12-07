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

#include "BaseEncoder.h"

#include <string>
using std::string;

Revel_BaseEncoder::Revel_BaseEncoder(void)
{
    m_frameBuffer = NULL;
    m_outFile = NULL;

    Reset();
}

Revel_BaseEncoder::~Revel_BaseEncoder(void)
{
    if (m_isEncoding)
        EncodeEnd();
    Reset();
}


void Revel_BaseEncoder::Reset(void)
{
    if (m_frameBuffer != NULL)
    {
        delete [] m_frameBuffer;
        m_frameBuffer = NULL;
    }
    if (m_outFile != NULL)
    {
        AVI_close(m_outFile);
        m_outFile = NULL;
    }
    m_outFilename = "";

    memset(&m_params, 0, sizeof(Revel_Params));

    m_totalOutBytes = 0;
    m_isEncoding = false;
}

Revel_Error Revel_BaseEncoder::EncodeStart(const string& filename,
                                           const Revel_Params& params)
{
    if (m_isEncoding)
        return REVEL_ERR_BUSY;

    // check args
    if (params.width == 0 || params.height == 0)
    {
        Reset();
        return REVEL_ERR_PARAMS;
    }
    m_params = params;

    // Create output file
    m_outFile = AVI_open_output_file(const_cast<char*>(filename.c_str()));
    if (m_outFile == NULL)
    {
        Reset();
        return REVEL_ERR_FILE;
    }
    AVI_set_video(m_outFile, params.width, params.height, params.frameRate, "xvid");
    if (params.hasAudio == 1)
    {
        // Sanity-check the audio settings first, since there's a good chance
        // the caller will leave all these fields uninitialized when updating
        // from Revel 1.0.x.
        if (params.audioBits < 0 || params.audioBits > 256 ||
            params.audioChannels < 0 || params.audioChannels > 256 ||
            params.audioRate < 0)
        {
            Reset();
            return REVEL_ERR_PARAMS;
        }
        AVI_set_audio(m_outFile, params.audioChannels, params.audioRate,
            params.audioBits, params.audioSampleFormat);
    }
    else
    {
        AVI_set_audio(m_outFile, 0, 44100, 16, WAVE_FORMAT_UNKNOWN);
    }

    // Allocate frame buffer
    m_frameBuffer = new char[params.width*params.height*4];
    if (m_frameBuffer == NULL)
    {
        Reset();
        return REVEL_ERR_MEMORY;
    }

    m_outFilename = filename;
    m_totalOutBytes = 0;
    m_isEncoding = true;

    return REVEL_ERR_NONE;
}


Revel_Error Revel_BaseEncoder::EncodeFrame(const Revel_VideoFrame& frame,
                                           int *frameSize)
{
    // Make sure it's safe to be encoding.
    if (!m_isEncoding)
        return REVEL_ERR_BUSY;

    // Check args
    if (frame.width != m_params.width ||
        frame.height != m_params.height ||
        (frame.pixelFormat < 0 || frame.pixelFormat >= REVEL_PF_COUNT))
    {
        return REVEL_ERR_PARAMS;
    }

    return REVEL_ERR_NONE;
}


Revel_Error Revel_BaseEncoder::EncodeAudio(const void *sampleBuffer,
        const int numBytes, int *numTotalBytes)
{
    // Make sure it's safe to be encoding.
    if (!m_isEncoding)
        return REVEL_ERR_BUSY;
    
    if (m_params.hasAudio != 1)
        return REVEL_ERR_NONE;

    // Check args
    if (numBytes == 0)
        return REVEL_ERR_NONE;
    if (sampleBuffer == NULL)
        return REVEL_ERR_PARAMS;
    
    int err = AVI_write_audio(m_outFile, (char*)sampleBuffer, numBytes);
    if (err != 0)
        return REVEL_ERR_UNKNOWN;

    m_totalOutBytes += numBytes;
    if (numTotalBytes != NULL)
        *numTotalBytes = AVI_audio_bytes(m_outFile);

    return REVEL_ERR_NONE;
}


Revel_Error Revel_BaseEncoder::EncodeEnd(int *totalSize)
{
    if (!m_isEncoding)
        return REVEL_ERR_BUSY;
    int aviErr = 0;
    if (m_outFile != NULL)
    {
        aviErr = AVI_close(m_outFile);
        m_outFile = NULL;
    }

    if (totalSize != NULL)
        *totalSize = m_totalOutBytes;
    Reset();
 
    if (aviErr != 0)
        return REVEL_ERR_FILE;
    return REVEL_ERR_NONE;
}
