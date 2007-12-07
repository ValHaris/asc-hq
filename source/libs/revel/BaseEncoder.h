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

#ifndef BASEENCODER_H
#define BASEENCODER_H

#include "avilib.h"
#include "revel.h"
#include <string>

class Revel_BaseEncoder
{
public:
    Revel_BaseEncoder(void);
    virtual ~Revel_BaseEncoder(void) =0;

    /**
     * Initialize for a new encoding session.
     * @param filename The output movie will be written to this file.
     * @param params General information about the movie and the encoding
     *               process.
     * @return REVEL_ERR_NONE if no error, an error code otherwise.
     */
    virtual Revel_Error EncodeStart(const std::string& filename,
        const Revel_Params& params);

    /**
     * Encode a new frame and append it to the video stream.
     * @param frame The video frame to encode.
     * @param frameSize If non-NULL, the size of the encoded output frame (in
     *                  bytes) will be stored here.
     * @return REVEL_ERR_NONE if no error, an error code otherwise.
     */
    virtual Revel_Error EncodeFrame(const Revel_VideoFrame& frame,
        int *frameSize = NULL);

    /**
     * Appends a chunk of audio data to the existing audio stream.
     * @param sampleBuffer The array of sample data.
     * @param numBytes The size of sampleBuffer (in bytes).
     * @param numTotalBytes If non-NULL, the total number of bytes of audio
     *                      encoded so far will be stored here.
     * @return REVEL_ERR_NONE if no error, an error code otherwise.
     */
    virtual Revel_Error EncodeAudio(const void *sampleBuffer,
        const int numBytes, int *numTotalBytes = NULL);

    /**
     * Finalize the encoding process.  This function MUST be called after all
     * frames have been written, or else the output movie will be unviewable!
     * @param totalSize If non-NULL, the total size of the output movie (in
     *                  bytes) will be stored here.
     * @return REVEL_ERR_NONE if no error, an error code otherwise.
     */
    virtual Revel_Error EncodeEnd(int *totalSize = NULL);

protected:
    virtual void Reset(void); /**< Resets the encoder to its initial state. */

    char *m_frameBuffer; /**< Buffer containing one frame's worth of encoded data. */

    unsigned int m_totalOutBytes; /**< Running count of how many bytes have been written to m_outFile. */
    Revel_Params m_params; /**< General output movie parameters. */
    std::string m_outFilename; /**< Filename of output movie. */
    avi_t *m_outFile; /**< Output movie object. */
    bool m_isEncoding; /**< True while encoding is in progress (i.e. between calls to EncodeStart() and EncodeEnd() */

private:
    // operator= and copy constructor are disallowed for this class.
    Revel_BaseEncoder& operator=(const Revel_BaseEncoder& rhs) { return *this; }
    Revel_BaseEncoder(const Revel_BaseEncoder& rhs) {}

};


#endif // BASEENCODER_H
