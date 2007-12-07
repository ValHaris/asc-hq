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

#include "XvidEncoder.h"

#include <xvid.h>

#include <cassert>
#include <cstring>
#include <string>
using std::string;

namespace
{
    const int MAX_QUALITY = 6;
    const int motion_presets[MAX_QUALITY+1] =
    {
	    // quality 0
	    0,

	    // quality 1
	    XVID_ME_ADVANCEDDIAMOND16,

	    // quality 2
	    XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16,

	    // quality 3
	    XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 |
	    XVID_ME_ADVANCEDDIAMOND8 | XVID_ME_HALFPELREFINE8,

	    // quality 4
	    XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 |
	    XVID_ME_ADVANCEDDIAMOND8 | XVID_ME_HALFPELREFINE8 |
	    XVID_ME_CHROMA_PVOP | XVID_ME_CHROMA_BVOP,

	    // quality 5
	    XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 |
	    XVID_ME_ADVANCEDDIAMOND8 | XVID_ME_HALFPELREFINE8 |
	    XVID_ME_CHROMA_PVOP | XVID_ME_CHROMA_BVOP,

	    // quality 6
	    XVID_ME_ADVANCEDDIAMOND16 | XVID_ME_HALFPELREFINE16 | XVID_ME_EXTSEARCH16 |
	    XVID_ME_ADVANCEDDIAMOND8 | XVID_ME_HALFPELREFINE8 | XVID_ME_EXTSEARCH8 |
	    XVID_ME_CHROMA_PVOP | XVID_ME_CHROMA_BVOP,
    };

    const int vop_presets[MAX_QUALITY+1] =
    {
	    // quality 0
	    0,

	    // quality 1
	    0,

	    // quality 2
	    XVID_VOP_HALFPEL,

	    // quality 3
	    XVID_VOP_HALFPEL | XVID_VOP_INTER4V,

	    // quality 4
	    XVID_VOP_HALFPEL | XVID_VOP_INTER4V,

	    // quality 5
	    XVID_VOP_HALFPEL | XVID_VOP_INTER4V |
	    XVID_VOP_TRELLISQUANT,

	    // quality 6
	    XVID_VOP_HALFPEL | XVID_VOP_INTER4V |
	    XVID_VOP_TRELLISQUANT | XVID_VOP_HQACPRED,
    };

    int GetXvidQuality(const float q)
    {
        if (q < 0.0f)
            return 0;
        if (q > 1.0f)
            return MAX_QUALITY;
        return static_cast<int>(q*MAX_QUALITY);
    }

    int GetXvidPixelFormat(const Revel_PixelFormat pf)
    {
        switch(pf)
        {
        case REVEL_PF_RGBA:
            return XVID_CSP_RGBA;
        case REVEL_PF_BGRA:
            return XVID_CSP_BGRA;
        case REVEL_PF_ABGR:
            return XVID_CSP_ABGR;
        case REVEL_PF_ARGB:
            return XVID_CSP_ARGB;
        case REVEL_PF_BGR:
            return XVID_CSP_BGR;
        case REVEL_PF_RGB555:
            return XVID_CSP_RGB555;
        case REVEL_PF_RGB565:
            return XVID_CSP_RGB565;

        default:
            return XVID_CSP_NULL;
        }
    }
}

Revel_XvidEncoder::Revel_XvidEncoder(void)
{
    // Must initialize pointer members before calling Reset!
    m_xvidEncoderHandle = NULL;
    Reset();
}


Revel_XvidEncoder::~Revel_XvidEncoder(void)
{
    
}


void Revel_XvidEncoder::Reset(void)
{
    // Must call base Reset() first!
    Revel_BaseEncoder::Reset();

    if (m_xvidEncoderHandle != NULL)
    {
	    int xerr = xvid_encore(m_xvidEncoderHandle, XVID_ENC_DESTROY, NULL, NULL);
        m_xvidEncoderHandle = NULL;
    }
}


int Revel_XvidEncoder::CreateXvidEncoder(void **encoderOut)
{
    // Initialize XviD core
    xvid_gbl_init_t xvid_gbl_init;
    memset(&xvid_gbl_init, 0, sizeof(xvid_gbl_init));
	xvid_gbl_init.version = XVID_VERSION;
    xvid_gbl_init.debug = 0;
    xvid_gbl_init.cpu_flags = 0;
	xvid_global(NULL, XVID_GBL_INIT, &xvid_gbl_init, NULL);
    
    // Initialize XviD encoder
    xvid_enc_create_t xvid_enc_create;
	memset(&xvid_enc_create, 0, sizeof(xvid_enc_create));
	xvid_enc_create.version = XVID_VERSION;
	xvid_enc_create.width = m_params.width;
	xvid_enc_create.height = m_params.height;
	xvid_enc_create.profile = XVID_PROFILE_AS_L4;

	// init plugins
    xvid_enc_zone_t zones[64];
    const int NUM_ZONES = 0;
    xvid_enc_create.zones = zones;
    xvid_enc_create.num_zones = NUM_ZONES;
	xvid_enc_plugin_t plugins[7];
	xvid_enc_create.plugins = plugins;
	xvid_enc_create.num_plugins = 0;
    
    xvid_enc_create.num_threads = 0;

    xvid_enc_create.fincr = 1;
	xvid_enc_create.fbase = 24;
    xvid_enc_create.max_key_interval = xvid_enc_create.fbase * 10;

    xvid_enc_create.max_bframes = 0;
	xvid_enc_create.bquant_ratio = 150;
	xvid_enc_create.bquant_offset = 100;

    xvid_enc_create.frame_drop_ratio = 0;
    xvid_enc_create.global = 0;

    int xerr = xvid_encore(NULL, XVID_ENC_CREATE, &xvid_enc_create, NULL);

	// Retrieve the encoder instance from the structure
	*encoderOut = xvid_enc_create.handle;

	return xerr;
}



Revel_Error Revel_XvidEncoder::EncodeStart(const string& filename,
                                      const Revel_Params& params)
{
    Revel_Error revError = Revel_BaseEncoder::EncodeStart(filename, params);
    if (revError != REVEL_ERR_NONE)
        return revError;
        
    // Create XviD encoder
    int error = CreateXvidEncoder(&m_xvidEncoderHandle);
    if (error != 0 || m_xvidEncoderHandle == NULL)
    {
        Reset();
        return REVEL_ERR_UNKNOWN;
    }

    return REVEL_ERR_NONE;
}


Revel_Error Revel_XvidEncoder::EncodeFrame(const Revel_VideoFrame& frame,
                                      int *frameSize)
{
    Revel_Error revError = Revel_BaseEncoder::EncodeFrame(frame, frameSize);
    if (revError != REVEL_ERR_NONE)
        return revError;

	// Version for the frame and the stats
	xvid_enc_frame_t xvid_enc_frame;
	memset(&xvid_enc_frame, 0, sizeof(xvid_enc_frame));
	xvid_enc_frame.version = XVID_VERSION;

	xvid_enc_stats_t xvid_enc_stats;
	memset(&xvid_enc_stats, 0, sizeof(xvid_enc_stats));
	xvid_enc_stats.version = XVID_VERSION;

	// Bind output buffer
	xvid_enc_frame.bitstream = m_frameBuffer;
	xvid_enc_frame.length = -1;

	// Initialize input image fields
	if (frame.pixels != NULL) {
		xvid_enc_frame.input.plane[0] = frame.pixels;
		xvid_enc_frame.input.csp = GetXvidPixelFormat(frame.pixelFormat);
		xvid_enc_frame.input.stride[0] = frame.width * frame.bytesPerPixel;
	} else {
		xvid_enc_frame.input.csp = XVID_CSP_NULL;
	}

	// Set up core's general features
	xvid_enc_frame.vol_flags = 0;

	// Set up core's general features
    int quality = GetXvidQuality(m_params.quality);
	xvid_enc_frame.vop_flags = vop_presets[quality];

	// Frame type -- let core decide for us
	xvid_enc_frame.type = XVID_TYPE_AUTO;

	// Force the right quantizer -- It is internally managed by RC plugins
	xvid_enc_frame.quant = 0;

	// Set up motion estimation flags
	xvid_enc_frame.motion = motion_presets[quality];

	// We don't use special matrices
	xvid_enc_frame.quant_intra_matrix = NULL;
	xvid_enc_frame.quant_inter_matrix = NULL;

	// Encode the frame
	int frameBytes = xvid_encore(m_xvidEncoderHandle, XVID_ENC_ENCODE,
        &xvid_enc_frame, &xvid_enc_stats);

    // Write the frame to the output stream.
    AVI_write_frame(m_outFile, m_frameBuffer, frameBytes);
    m_totalOutBytes += frameBytes;
    if (frameSize != NULL)
        *frameSize = frameBytes;

	return REVEL_ERR_NONE;
}


Revel_Error Revel_XvidEncoder::EncodeEnd(int *totalSize)
{
    Revel_Error revError = Revel_BaseEncoder::EncodeEnd(totalSize);
    if (revError != REVEL_ERR_NONE)
        return revError;

    return REVEL_ERR_NONE;
}
