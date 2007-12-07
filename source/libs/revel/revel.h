/** \mainpage Revel (The Really Easy Video Encoding Library
 * The Revel library provides the shortest path between your application and
 * high-quality video output.
 *
 * Using Revel generally involves the following sequence of function calls:
 * - Ensure binary compatibility by testing REVEL_API_VERSION against
 *   Revel_GetApiVersion().
 * - Create an encoder with Revel_CreateEncoder().
 * - Declare a Revel_Params object. initialize it to safe defaults with
 *   Revel_InitializeParams(), and then fill in the fields to fit your
 *   application's needs. Pass the object to Revel_EncodeStart() to begin
 *   the encoding process.
 * - Use Revel_EncodeFrame() to pass in individual video frames in
 *   the order you'd like them to appear.  Optionally, you can
 *   use Revel_EncodeAudio() to provide an audio track for your movie.
 * - Stop the encoding process with Revel_EncodeEnd().  This finalizes
 *   the output movie and makes it viewable.  Don't skip this step!
 * - Destory the encode object with Revel_DestoryEncoder().
 */

/** \example reveltest.cpp
 * This file contains a complete demonstration of Revel in action.
 * It generates a short movie of an animating checkerboard
 * pattern, complete with audio.
 */

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

#ifndef REVEL_H
#define REVEL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Version information. */
#define REVEL_VERSION 0x010100 /**< Updated for every release. */
#define REVEL_API_VERSION 2 /**< Updated only when the public API changes. */

/**
 * Retrieves the run-time value of REVEL_VERSION.
 * Use this function for run-time binary compatibility tests.
 * @return The run-time value of REVEL_VERSION.
 */
int Revel_GetVersion(void);

/**
 * Use this function to check for run-time binary compatibility.
 * Specifically, if REVEL_API_VERSION != Revel_GetApiVersion(),
 * then your library and header files are out of date, and
 * Revel's behavior will be undefined!
 * @return The run-time value of REVEL_API_VERSION.
 */
int Revel_GetApiVersion(void);

/**
 * List of legal pixel formats for input video frames.
 */
typedef enum
{
    REVEL_PF_BGRA = 0, /**< 32-bit BGRA */
    REVEL_PF_ABGR,     /**< 32-bit ABGR */
    REVEL_PF_RGBA,     /**< 32-bit RGBA */
    REVEL_PF_ARGB,     /**< 32-bit ARGB */
    REVEL_PF_BGR,      /**< 24-bit BGR */
    REVEL_PF_RGB555,   /**< 16-bit RGB 5-5-5 packed */
    REVEL_PF_RGB565,   /**< 16-bit RGB 5-6-5 packed */

    REVEL_PF_COUNT     /**< Number of pixel formats (this is NOT a legal pixel format!) */
} Revel_PixelFormat;


/**
 * List of supported video codecs.
 */
typedef enum
{
    REVEL_CD_XVID = 0, /**< XviD (http://www.xvid.org) */

    REVEL_CD_COUNT     /**< Number of video codecs (this is NOT a legal codec!) */
} Revel_VideoCodec;

/**
 * Partial list of supported audio sample formats.
 * This enum only contains the most common sample formats.
 * In actuality, any of the Microsoft WAVE_FORMAT_XXX values is 
 * legal.  Please refer to the Windows mmreg.h file (available
 * on the web at:
 * http://graphics.cs.uni-sb.de/NMM/dist-0.6.0/Docs/Doxygen/html/mmreg_8h.html)
 * for a full list of supported sample formats and their associated
 * numerical values.
 */
typedef enum 
{
    REVEL_ASF_UNKNOWN = 0x0000, /**< Unknown format / no audio present. */
    REVEL_ASF_PCM     = 0x0001, /**< PCM format (the most common choice). */
    REVEL_ASF_ADPCM   = 0x0002, /**< ADPCM format. */
    REVEL_ASF_ALAW    = 0x0006, /**< A-law format. */
    REVEL_ASF_MULAW   = 0x0007, /**< mu-law format. */
} Revel_AudioSampleFormat;


/**
 * List of error codes.
 */
typedef enum 
{
    REVEL_ERR_NONE = 0,         /**< No error (the operation completed successfully). */
    REVEL_ERR_UNKNOWN,          /**< An unknown/unclassified error. */
    REVEL_ERR_INVALID_HANDLE,   /**< An invalid handle was passed to the function. */
    REVEL_ERR_PARAMS,           /**< Invalid parameters passed to the function. */
    REVEL_ERR_FILE,             /**< A file I/O error. */
    REVEL_ERR_MEMORY,           /**< A memory-related error. */
    REVEL_ERR_BUSY,             /**< Revel is busy with another operation. */

    REVEL_ERR_COUNT             /**< Number of error types (this is nOT a legal error code!) */
} Revel_Error;

/**
 * General info about the video to encode.
 */
typedef struct
{
    unsigned int width, height; /**< width/height in pixels */
    float frameRate; /**< frames per second. */
    float quality; /**< ranges 0 to 1 */
    Revel_VideoCodec codec; /**< This codec will be used to compress the video frames. */
    
    /**
     * If 1, the output movie will include an audio stream.
     * Any other value means the movie will be silent.
     */
    int hasAudio;

    /* The following fields are ignored unless hasAudio is 1 */
    int audioRate; /**< audio sample rate, e.g. 22050, 44100 */
    int audioBits; /**< bits per audio sample, e.g. 8, 16 */
    int audioChannels; /**< Number of channels in the audio stream (1=mono, 2=stereo) */
    int audioSampleFormat; /**< Format of the audio sample data. */
} Revel_Params;


/**
 * Represents a frame of video.  Use this to pass raw images
 * into Revel to be encoded.
 */
typedef struct
{
    unsigned int width, height; /**< width/height in pixels. */
    unsigned char bytesPerPixel; /**< color depth of pixels, in bytes. */
    Revel_PixelFormat pixelFormat; /**< color channel ordering for these pixels. */
    void *pixels; /**< Pointer to pixel data. */
} Revel_VideoFrame;


/**
 * Create an encoder for your movie.  Call me first!
 * @param encoderHandle A handle to the new encoder will be stored here.
 * @return REVEL_ERR_NONE if success, an error code if not.
 */
Revel_Error Revel_CreateEncoder(int *encoderHandle);


/**
 * Initialize a Revel_Params structure to safe default values.
 *
 * It is very important that you call this function on your
 * Revel_Params structure before filling it out yourself!  Otherwise,
 * if you upgrade to a new version of Revel in which new fields
 * were added to Revel_Param, they would be left uninitialized in
 * your application, and Terrible Things could happen.
 *
 * You should not rely on any of the specific default values used in this
 * function -- while they are guarenteed to be safe, they are NOT guarenteed
 * to be correct for your application.
 *
 * @param params This structure will be filled with safe default values.
 */
void Revel_InitializeParams(Revel_Params *params);

/**
 * Start encoding a new movie.
 *
 * @param encoderHandle Must be a valid encoder handle.
 * @param filename The output movie will be written to this file.
 * @param params Fill this structure with your movie settings before calling.
 * @return REVEL_ERR_NONE if success, an error code if not.
 */
Revel_Error Revel_EncodeStart(int encoderHandle, const char* filename,
                              Revel_Params *params);


/**
 * Encode a single frame and write it to the video stream.
 *
 * @param encoderHandle must be a valid encoder handle.
 * @param frame The video frame to encode.  It will not be written to at all,
 *              so it is unnecessary to make a copy of your framebuffer first.
 * @param frameSize If non-NULL, the size of the encoded frame (in bytes) will
 *                  be stored here.
 * @return REVEL_ERR_NONE if success, an error code if not.
 */
Revel_Error Revel_EncodeFrame(int encoderHandle,
                              Revel_VideoFrame *frame, int *frameSize = 0);


/**
 * Encode a chunk of audio data to the movie.
 *
 * This function will have no effect if the hasAudio field of Revel_Params
 * passed to Revel_EncodeStart() was 0.
 * Each new audio chunk will be appended to the end of the existing audio
 * data.  See the Revel FAQ (http://www.dangerware.org/cgi-bin/revelfaq.py)
 * for important ramifications of this behavior!
 *
 * @param encoderHandle must be a valid encoder handle.
 * @param sampleBuffer The array of audio samples.  The sample data must
 *                     be of the same format specified in the Revel_Params
 *                     structure passed to Revel_EncodeStart().
 * @param numBytes The size of the sampleBuffer array, in bytes.
 * @param numTotalBytes If non-NULL, the total number of bytes of audio
 *                      encoded so far will be stored here.
 * @return REVEL_ERR_NONE if success, an error code if not.
 */
Revel_Error Revel_EncodeAudio(int encoderHandle, void *sampleBuffer,
                              int numBytes, int *numTotalBytes = 0);


/**
 * Finalize the new movie.
 *
 * This function MUST be called when you're finished encoding, or the output
 * movie will be corrupted and unviewable!
 *
 * @param encoderHandle must be a valid encoder handle.
 * @param totalSize If non-NULL, the total size of the output movie (in bytes)
 *                  will be stored here.
 * @return REVEL_ERR_NONE if success, an error code if not.
 */
Revel_Error Revel_EncodeEnd(int encoderHandle, int *totalSize = 0);


/**
 * Destroys an encoder through its handle, and frees the memory associated
 * with it.
 * 
 * @param encoderHandle A handle to the encoder to destroy.
 * @return REVEL_ERR_NONE if success, an error code if not.
 */
Revel_Error Revel_DestroyEncoder(int encoderHandle);

#ifdef __cplusplus
}
#endif
#endif
