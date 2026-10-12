/****************************************************************************\
|* VU code manager class
|* =====================
|*
|* Simple PS2 C++ framework on which to build a 3D/2.5D engine. 
|* 
|* This class handles all the VU codecs. Any file in libexec/vu<N>/... 
|* will be loaded in and can be uploaded to vu<N> at any time.
|*
|* Author					: SjG, various others
|* First created			: Sun Jul 21 13:07:48 BST 2002
|* Modification history		: See class header file
|*
\****************************************************************************/
#include "vuManager.h"

/****************************************************************************\
|* Constructor
\****************************************************************************/
VUManager::VUManager(int idx, ps2_vpu *dev)
	{
	_dev			= dev;
	_vu 			= idx;
	
	_codeSizes[0] 	=  4*1024;
	_codeSizes[1] 	= 16*1024;
	
	_codecs 		= NULL;
	_names			= NULL;
	_current		= -1;
	_num			= 0;
	
	initialise();
	}
	
/****************************************************************************\
|* Destructor
\****************************************************************************/
VUManager::~VUManager(void)
	{
	if (_codecs)
		{
		for (int i=0; i<_num; i++)
			{
			if (_codecs[i]) delete [] _codecs[i];
			if (_names [i]) delete [] _names [i];
			}
		delete [] _codecs;
		}
	}
	
/****************************************************************************\
|* Method - initialise the codec library
\****************************************************************************/
void VUManager::initialise(void)
	{
	char dirname[1024];
	sprintf(dirname, "libexec/vu%d", _vu);
	
	DIR *codecs = opendir(dirname);
	if (codecs)
		{
		struct dirent *file;
		
		/********************************************************************\
		|* Discover the number of codecs in the directory
		\********************************************************************/
		while ((file = readdir(codecs)) != NULL)
			if (strstr(file->d_name, ".vu") != NULL)
				_num ++;
		
		/********************************************************************\
		|* Reserve space for the codecs in ram
		\********************************************************************/
		_codecs = new unsigned char *[_num];
		_names  = new          char *[_num];
		if (_codecs == NULL || _names == NULL)
			{
			_log << "Cannot allocate memory for VU codec lists\n";
			return;
			}
				
		
		/********************************************************************\
		|* Load the codecs 
		\********************************************************************/
		rewinddir(codecs);
		int count = 0;
		while ((file = readdir(codecs)) != NULL)
			if (strstr(file->d_name, ".vu") != NULL)
				{
				_codecs[count] = new unsigned char[_codeSizes[_vu]];
				_names[count]  = new char[strlen(file->d_name)+1];
				
				if (_codecs[count] == NULL || _names[count] == NULL)
					{
					_log << "Cannot allocate memory for VU codecs\n";
					return;
					}
				
				memset(_codecs[count], 0, _codeSizes[_vu]);
				FILE *fp = fopen(file->d_name, "r");
				if (fp)
					{
					fread(_codecs[count], _codeSizes[_vu], 1, fp);
					fclose(fp);
					_log << "Read codec '" << dirname << "/"
					    << file->d_name << "'\n";
					}
				else
					{
					_log << "Failed to open '" << dirname << "/"
					    << file->d_name << "'\n";
					return;
					}
					
				strcpy(_names[count], file->d_name);
				count ++;
				}
		
		/********************************************************************\
		|* Tidy up
		\********************************************************************/
		closedir(codecs);
		}
	
	if (_num > 0)
		ensureActive(0);
	}

/****************************************************************************\
|* Method - ensure the current renderer is the desired one
\****************************************************************************/
void VUManager::ensureActive(int idx)
	{
	if (_current == idx)
		return;
	
	if (idx < 0 || idx >= _num)
		{
		_log << "Attempt to switch to oub codec '" << idx<< "'\n";
		return;
		}
		
	memcpy(ps2_vpu_text(_dev), _codecs[idx], _codeSizes[_vu]);
	}

/****************************************************************************\
|* Method - ensure the current renderer is the desired one
\****************************************************************************/
void VUManager::ensureActive(char *name)
	{
	for (int i=0; i<_num; i++)
		if (strcasecmp(name, _names[i]) == 0)
			{
			ensureActive(i);
			return;
			}
	}


