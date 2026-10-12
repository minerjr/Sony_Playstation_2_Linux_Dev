/** @file TextureManager.h
	 @author Alkampfer
	 @version 0.2
*/
#ifndef _TEX_MANAGER__
#define _TEX_MANAGER__

#include <sps2util.h>
#include <sps2tags.h>

/** This namespace is used to avoid name clashing.
*/
namespace NablaPS2Utils {

/** Structure placed at the header of texture file.
	 Remember to add the packed attribute, if not the structure cannot
	 be read from file with a single fread call.
*/
struct TextureFileHeader {
	
	int	HeaderSize;
	char	FileVersion;
	int	Width;
	int	Height;
	PSM_t PixelFormat;
	char	Swizzled;
	int	DataSize;
}  __attribute__((packed));

/** Class used to manage texture on PS2.
	 This is a simple class used to mange a single texture, a real
	 texture manager should manage multiple texture in GS memory.
	 If such a class is needed it can be build using TextureManger
	 to handle single texture, for this reason the function used to 
	 send texture to GS accept the address where upload the texture.
	 @author Alkampfer
	 @version 0.2
*/
class TextureManager {

	//! Format of the pixel, used to determine if CLUT is present
	PSM_t	m_pixelFormat;
	int	m_width;       //!< Image width
	int	m_height;		//!< Image height
	bool	m_swizzled;		//!< Image is swizzled (True/False)
	int   m_numMemBlocks;//!< Number of allocated blocks (4KB each)
	int   m_imageSize;	//!< Size of the image in bytes
	sps2Memory_t  *m_imageData;  //!< Pixel of the image, raw data read from file
	sps2Memory_t  *m_clutData;	//!< Clut, raw data read from file.

	//! True if TextureManger object can be used, false otherwise
	bool  m_status;
	//! Last address where the texture was uploaded
	int   m_UploadAddress;
	//! Used to update data of the image
	void UpdateImageData(const TextureFileHeader &TFH);
	
	//! Descriptor of the SPS2 library used to manage memory
	int m_SPS2Desc;
public:

	//! Create a textureManger object reading a filename
	TextureManager(const char *FileName, int SPS2Descriptor);
	//! Deallocate all memory used
	~TextureManager();
	//! Send texture to GS
	void SendTexture(sps2uint32 GSAddress, sps2Memory_t *DMAMemory);
	//! Set TEX0 register
	sps2uint64 BuildTEX0(int TextureFunction) const;
	//! Set TEX1 register
	sps2uint64 BuildTEX1(int FilteringMode) const;

	//! Access height of the image
	int  Height() const {return m_height;}
	//! Access width of the image
	int  Width() const {return m_width;}
	//! Convert to void * for testing
	operator void *() const {return (void *) (m_status);}
}; //End of TetureManager Class

int GetPixelSizeInBytes(PSM_t Format);
}; //End of namespace.
#endif