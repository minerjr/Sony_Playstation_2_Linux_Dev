/** @file Screenshot.h
	 Original file taken from sps2demo freely downloadable from linux kit project pages.
	 http://playstation2-linux.com/projects/sps2demo/
	 This file is only an adaptation to work separately from other file in sps2demo
*/
#ifndef _SCREENSHOT_H__
#define _SCREENSHOT_H__

namespace NablaPS2Utils {

	enum EScreenType {
	
		NTSC,
		PAL
	};

	class ScreenShot {
	
		//! SPS2 descriptor is needed to allocate memory.
		int m_SPS2Descriptor;
		//! Internal pointer allocated by the ScreenShot constructor
		sps2Memory_t *m_memory;
		//! This is the function that physically grab memory from front buffer
		void DownloadVram(void *virtPage, int physPage, int addr, int bufw, int x, int y, int w, int h, int eventfd);
	public:

		//! Constructor needs only to know SPS2 descriptor.
		ScreenShot(int SPS2Descriptor);
		//! Destructor to deallocate the allocated ram
		~ScreenShot();
		//! This is the function that really do the shot grabbing the frame buffer to a targa image
		void TakeShot(char *FileName, EScreenType ScreenType = NTSC);
	};	
};
#endif /* SCREENSHOT_H */
