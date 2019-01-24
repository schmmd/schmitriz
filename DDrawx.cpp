#include "ddrawx.h"

#include "ijl.h"

#include "debug.h"

int GetMaskInfo (DWORD Bitmask, int* lpShift)
{
    int Precision, Shift;

    Precision = Shift = 0;
    //count the zeros on right hand side
    while (!(Bitmask & 0x01L))
    {
        Bitmask >>= 1;
        Shift++;
    }
	
    //count the ones on right hand side
    while (Bitmask & 0x01L)
    {
        Bitmask >>= 1;
        Precision++;
    }
    *lpShift = Shift;
    return Precision;
}

bool CDirectDraw::CreateClipper()
{
	// Create the clipper
	if (FAILED(lpdd->CreateClipper(0, &lpddclipper, NULL)))
	{
		// error
		FatalError("Could not create the clipper.");
	}

	return 1;
}

bool CDirectDraw::AttachClipper(LPDIRECTDRAWSURFACEX lpddsurface)
{
	if (FAILED(lpddsurface->SetClipper(lpddclipper)))
	{
		// error
		FatalError("Could not set the clipper to the surface.");
	}

	return 1;
}

bool CDirectDraw::SetClipperWindow	(HWND hWnd)
{
	// Associate clipper with hMainWnd
	if (FAILED(lpddclipper->SetHWnd(0, hWnd)))
	{
		// error
		FatalError("Could not attach the clipper to the window.");
	}

	return 1;
}

bool CDirectDraw::CreateDirectDraw()
{
	/*
	// Create the IDirectDrawX interface
	if (FAILED(DirectDrawCreateEx(NULL, (VOID**)&lpdd, IID_IDirectDrawX, NULL)))
	{
		// error
		FatalError("Could not create a DirectDraw interface.");
	}
	*/
	LPDIRECTDRAW lpddtemp = NULL;

	// Create IDirectDraw interface
	if (FAILED(DirectDrawCreate(NULL, &lpddtemp, NULL)))
	{
		// error
		FatalError("Could not create DirectDraw interface.");
	}

	// Query for IDirectDraw4
	if (FAILED(lpddtemp->QueryInterface(IID_IDirectDrawX, (LPVOID *)&lpdd)))
	{
		// error
		FatalError("Could not query for DirectDraw interface.");
	}

	// Release IDirectDraw
	lpddtemp->Release();

	return 1;
}

bool CDirectDraw::SetWindowCooperation(HWND hWnd)
{
	// Set the cooperation level
	if (FAILED(lpdd->SetCooperativeLevel(hWnd, DDSCL_NORMAL)))
	{
		// error
		FatalError("Could not set the cooperation level to DDSCL_NORMAL.");
	}

	return 1;
}

bool CDirectDraw::CreatePrimarySurface()
{
	DDSURFACEDESC2 ddsd;

	// Initialize DDSURFACEDEST2 structure
	memset((bool *)&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);

	// Set flags
	ddsd.dwFlags	    = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	// Create the primary surface
	if (FAILED(lpdd->CreateSurface(&ddsd, &lpddsprimary, NULL)))
	{
		// error
		FatalError("Could not create the primary surface.");
	}

	return 1;
}

bool CDirectDraw::GetPixelFormat()
{
	// Reset ddpixelformat structure
	memset((bool*)&ddpixelformat, 0, sizeof(ddpixelformat));
	ddpixelformat.dwSize = sizeof(ddpixelformat);

	// Retrieve the pixel format
	if (FAILED(lpddsback->GetPixelFormat(&ddpixelformat)))
	{
		// error
		FatalError("Could not get the pixel format.");
	}

	return 1;
}

DWORD CDirectDraw::CreateRGB(BYTE red, BYTE green, BYTE blue)
{
    int rshr, gshr, bshr; 	// bitsize of field
    int rshl, gshl, bshl;	// 0’s on left (the shift value)
	
    rshr = 8 - GetMaskInfo (ddpixelformat.dwRBitMask, &rshl);
    gshr = 8 - GetMaskInfo (ddpixelformat.dwGBitMask, &gshl);
    bshr = 8 - GetMaskInfo (ddpixelformat.dwBBitMask, &bshl);

    DWORD Color = ((blue >> bshr) << bshl) + ((green >> gshr) << gshl) + ((red >> rshr) << rshl);

	return Color;
}

bool CDirectDraw::CheckCoopLevel()
{
	HRESULT hr;

	// Check the cooperative level before rendering
    if(FAILED(hr = lpdd->TestCooperativeLevel()))
    {
        switch(hr)
        {
            case DDERR_EXCLUSIVEMODEALREADYSET:
                // Do nothing because some other app has exclusive mode
                Sleep(10);
                break;
				// return DD_OK;

            case DDERR_WRONGMODE:
                // The display mode changed on us. Update the
                // DirectDraw surfaces accordingly
                // FreeDirectDraw();
                // return InitDirectDraw( hWnd );
				break;
        }
        return false;
    }

	return true;
}

HRESULT CDirectDraw::FlipBuffer(HWND hWnd)
{
	RECT rcSourceRect, rcDestRect;

	POINT p;
	HRESULT hr;

	p.x = 0; p.y = 0;
	GetClientRect (hWnd, &rcDestRect);
	ClientToScreen(hWnd, &p);
	OffsetRect    (&rcDestRect, p.x, p.y);

	// Setup source rect
    SetRect(&rcSourceRect, 0, 0, rcDestRect.right - rcDestRect.left, rcDestRect.bottom - rcDestRect.top);
 
    if (hr = (lpddsprimary->Blt(&rcDestRect, lpddsback, &rcSourceRect, DDBLT_WAIT, NULL)) != DD_OK)
		return hr;

	return DD_OK;
}

bool CDirectDraw::ClearBackSurface(DWORD dwColor)
{
	DDBLTFX fx;
	memset(&fx, 0, sizeof(fx));
	fx.dwSize = sizeof(fx);

	fx.dwFillColor = dwColor;

	if (FAILED(lpddsback->Blt(NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx)))
		FatalError("Could not clear back surface.");

	return true;
}

HRESULT CDirectDraw::StretchBlit(LPDIRECTDRAWSURFACEX lpddssource, int x, int y, int width, int height, int destwidth, int destheight, DWORD dwExFlags)
{
	RECT rcSourceRect, rcDestRect;

	SetRect(&rcSourceRect, 0, 0, width    , height);
	SetRect(&rcDestRect,   x, y, destwidth + x, destheight + y);

	return lpddsback->Blt(&rcDestRect, lpddssource, &rcSourceRect, DDBLT_WAIT | dwExFlags, NULL);
}

HRESULT CDirectDraw::SimpleBlit(LPDIRECTDRAWSURFACEX lpddssource, int x, int y, int width, int height, DWORD dwExFlags)
{
	return DirectDraw_SimpleBlit(lpddsback, lpddssource, x, y, width, height, dwExFlags);
	/*
	HRESULT hr;
	RECT rcSourceRect, rcDestRect;

	SetRect(&rcSourceRect, 0, 0, width    , height);
	SetRect(&rcDestRect,   x, y, x + width, y + height);

	if ((hr = lpddsback->Blt(&rcDestRect, lpddssource, &rcSourceRect, DDBLT_WAIT | dwExFlags, NULL)) != DD_OK)
	{
		if (hr != DDERR_SURFACELOST)
			FatalError("Could not blit to destination.");
	}

	return hr;
	*/
}

HRESULT DirectDraw_SimpleBlit(LPDIRECTDRAWSURFACEX lpddsdest, LPDIRECTDRAWSURFACEX lpddssource, int x, int y, int width, int height, DWORD dwExFlags)
{
	HRESULT hr;
	RECT rcSourceRect, rcDestRect;

	SetRect(&rcSourceRect, 0, 0, width    , height);
	SetRect(&rcDestRect,   x, y, x + width, y + height);

	if ((hr = (lpddsdest->Blt(&rcDestRect, lpddssource, &rcSourceRect, DDBLT_WAIT | dwExFlags, NULL))) != DD_OK)
	{
		if (hr != DDERR_SURFACELOST)
			FatalError("Could not blit to destination.");
	}

	return hr;
}

bool DirectDraw_ClearSurface(LPDIRECTDRAWSURFACEX lpdds, DWORD dwColor)
{
	DDBLTFX fx;
	memset(&fx, 0, sizeof(fx));
	fx.dwSize = sizeof(fx);

	fx.dwFillColor = dwColor;

	if (FAILED(lpdds->Blt(NULL, NULL, NULL, DDBLT_WAIT | DDBLT_COLORFILL, &fx)))
		FatalError("Could not clear the surface.");

	return true;
}

DWORD DirectDraw_CreateRGB(LPDIRECTDRAWSURFACEX lpdds, BYTE red, BYTE green, BYTE blue)
{
	DDPIXELFORMAT ddtemppixelformat;

	// Reset ddpixelformat structure
	memset((bool*)&ddtemppixelformat, 0, sizeof(ddtemppixelformat));
	ddtemppixelformat.dwSize = sizeof(ddtemppixelformat);

	// Retrieve the pixel format
	lpdds->GetPixelFormat(&ddtemppixelformat);

    int rshr, gshr, bshr; 	// bitsize of field
    int rshl, gshl, bshl;	// 0’s on left (the shift value)
	
    rshr = 8 - GetMaskInfo (ddtemppixelformat.dwRBitMask, &rshl);
    gshr = 8 - GetMaskInfo (ddtemppixelformat.dwGBitMask, &gshl);
    bshr = 8 - GetMaskInfo (ddtemppixelformat.dwBBitMask, &bshl);

    DWORD Color = ((blue >> bshr) << bshl) + ((green >> gshr) << gshl) + ((red >> rshr) << rshl);

	return Color;
}

HRESULT DirectDraw_SetColorKey(LPDIRECTDRAWSURFACEX lpdds, BYTE red, BYTE green, BYTE blue)
{
    DDCOLORKEY ddck;

	DWORD dwColor = DirectDraw_CreateRGB(lpdds, red, green, blue);
    ddck.dwColorSpaceLowValue  = dwColor;
    ddck.dwColorSpaceHighValue = dwColor;

    return lpdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}

HRESULT
DDCopyBitmap(LPDIRECTDRAWSURFACEX pdds, HBITMAP hbm, int x, int y,
             int dx, int dy)
{
    HDC                     hdcImage;
    HDC                     hdc;
    BITMAP                  bm;
    DDSURFACEDESC2          ddsd;
    HRESULT                 hr;

    if (hbm == NULL || pdds == NULL)
        return E_FAIL;
    //
    // Make sure this surface is restored.
    //
    pdds->Restore();
    //
    // Select bitmap into a memoryDC so we can use it.
    //
    hdcImage = CreateCompatibleDC(NULL);
    if (!hdcImage)
        OutputDebugString("createcompatible dc failed\n");
    SelectObject(hdcImage, hbm);
    //
    // Get size of the bitmap
    //
    GetObject(hbm, sizeof(bm), &bm);
    dx = dx == 0 ? bm.bmWidth : dx;     // Use the passed size, unless zero
    dy = dy == 0 ? bm.bmHeight : dy;
    //
    // Get size of surface.
    //
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    pdds->GetSurfaceDesc(&ddsd);

    if ((hr = pdds->GetDC(&hdc)) == DD_OK)
    {
        StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, x, y,
                   dx, dy, SRCCOPY);
        pdds->ReleaseDC(hdc);
    }
    DeleteDC(hdcImage);
    return hr;
}

//-----------------------------------------------------------------------------
// Name: DDLoadBitmap()
// Desc: Create a DirectDrawSurface from a bitmap resource.
//-----------------------------------------------------------------------------
LPDIRECTDRAWSURFACEX 
DDLoadBitmap(LPDIRECTDRAWX pdd, LPCSTR szBitmap, int dx, int dy, DWORD dwColorLowValue, DWORD dwColorHighValue, DWORD memflags)
{
    HBITMAP                 hbm;
    BITMAP                  bm;
    DDSURFACEDESC2          ddsd;
    LPDIRECTDRAWSURFACEX    pdds;

    //
    //  Try to load the bitmap as a resource, if that fails, try it as a file
    //
    hbm = (HBITMAP) LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, dx,
                              dy, LR_CREATEDIBSECTION);
    if (hbm == NULL)
        hbm = (HBITMAP) LoadImage(NULL, szBitmap, IMAGE_BITMAP, dx, dy,
                                  LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbm == NULL)
        return NULL;
    //
    // Get size of the bitmap
    //
    GetObject(hbm, sizeof(bm), &bm);
    //
    // Create a DirectDrawSurface for this bitmap
    //
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | memflags;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth = bm.bmWidth;
    ddsd.dwHeight = bm.bmHeight;
	ddsd.ddckCKSrcBlt.dwColorSpaceLowValue  = dwColorLowValue;
	ddsd.ddckCKSrcBlt.dwColorSpaceHighValue = dwColorLowValue;
    if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK)
        return NULL;
    DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
    DeleteObject(hbm);
    return pdds;
}

//-----------------------------------------------------------------------------
// Name: DDLoadBitmap()
// Desc: Create a DirectDrawSurface from a bitmap resource.
//-----------------------------------------------------------------------------
LPDIRECTDRAWSURFACEX 
DDReLoadBitmap(LPDIRECTDRAWX pdd, LPDIRECTDRAWSURFACEX pdds, LPCSTR szBitmap, int dx, int dy)
{
    HBITMAP                 hbm;
    BITMAP                  bm;

    //
    //  Try to load the bitmap as a resource, if that fails, try it as a file
    //
    hbm = (HBITMAP) LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, dx,
                              dy, LR_CREATEDIBSECTION);
    if (hbm == NULL)
        hbm = (HBITMAP) LoadImage(NULL, szBitmap, IMAGE_BITMAP, dx, dy,
                                  LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbm == NULL)
        return NULL;
    //
    // Get size of the bitmap
    //
    GetObject(hbm, sizeof(bm), &bm);
    //
    // Create a DirectDrawSurface for this bitmap
    //
    DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
    DeleteObject(hbm);
    return pdds;
}

#define RGB555(r, g, b) ((r >> 3) | ((g >> 3) << 5) | ((b >> 3) << 10))
#define RGB565(r, g, b) ((r >> 3) | ((g >> 2) << 5) | ((b >> 3) << 11))

bool CDirectDraw::LoadJPEG(LPCTSTR Name, int Location, LPDIRECTDRAWSURFACEX* Surface)
{
   //Error handle
   IJLERR jerr;

   //Create the JPEG object
   JPEG_CORE_PROPERTIES jcprops;
   
   //Initialize the JPEG object
   jerr = ijlInit(&jcprops);
   if (jerr != IJL_OK)
   {
      MessageBox(NULL, "IJLInit problem", NULL, MB_OK);
      return false;
   }

   //Load from File or Resource?
   if (Location == FROM_FILE)
   {
      //Set the IJL data source as a filename
      jcprops.JPGFile = Name;

      //Read JPEG parameters from the file
      jerr = ijlRead(&jcprops, IJL_JFILE_READPARAMS);
   }
   else //(Location == FROM_RESOURCE) or integer ID of resource
   {
      //Resource variables
      BYTE* pmem;
      HGLOBAL hmem;
      DWORD size;
      HRSRC hres;
      
      //Determine whether resource ID is an int or a string
      hres = FindResource(NULL, Name, "JPEG");      
      if (hres)
      {
         size = SizeofResource(NULL, hres);
         if (!size)
         {
            MessageBox(NULL, "Error retrieving resource size",
               NULL, MB_OK);
            return false;
         }
         hmem = LoadResource(NULL, hres);
         if (hmem == NULL)
         {
            MessageBox(NULL, "Error loading resource", NULL, MB_OK);
            return false;
         }
         pmem = (BYTE *)LockResource(hmem);
         if (pmem == NULL)
         {
            MessageBox(NULL, "Error locking resource", NULL, MB_OK);
            return false;
         }
      }
      else
      {
         MessageBox(NULL, "JPEG resource not found", NULL, MB_OK);
         return false;
      }

      //Set the IJL data source as the resource buffer
      jcprops.JPGFile      = NULL;
      jcprops.JPGBytes     = pmem;
      jcprops.JPGSizeBytes = size;

      //Read JPEG parameters from the buffer
      jerr = ijlRead(&jcprops, IJL_JBUFF_READPARAMS);
   }
   
   //Make sure Parameter read was successful
   if (jerr != IJL_OK)
   {
      MessageBox(NULL, "Error reading JPEG parameters", NULL, MB_OK);
      return false;
   }

   //Prepare a 24Bit buffer to receive image data
   BYTE *buffer24;
   
   //Determine the required size
   long szbuff24 = (jcprops.JPGWidth * 24 + 7) / 8
      * jcprops.JPGHeight;

   //Resize the buffer
   buffer24 = new BYTE [szbuff24];
   if (buffer24 == NULL)
   {
      MessageBox(NULL, "Memory Allocation Error", NULL, MB_OK);
      return false;
   }
   
   //Set up the DIB specification for the JPEG decoder
   jcprops.DIBWidth    = jcprops.JPGWidth;
   jcprops.DIBHeight   = jcprops.JPGHeight; //Implies a bottom-up DIB.
   jcprops.DIBChannels = 3;
   jcprops.DIBColor    = IJL_BGR;
   jcprops.DIBPadBytes = IJL_DIB_PAD_BYTES(jcprops.JPGWidth,3);
   jcprops.DIBBytes    = reinterpret_cast<BYTE*>(buffer24);

   //Set the JPG color space ... this will always be somewhat of an
   //educated guess at best because JPEG is "color blind" (i.e.,
   //nothing in the bit stream tells you what color space the data was
   //encoded from. However, in this example we assume that we are
   //reading JFIF files which means that 3 channel images are in the
   //YCbCr color space and 1 channel images are in the Y color space.
   switch(jcprops.JPGChannels)
   {
      case 1: jcprops.JPGColor = IJL_G;
         break;
      
      case 3: jcprops.JPGColor = IJL_YCBCR;
         break;
      
      default:
         //This catches everything else, but no color twist will be
         //performed by the IJL.
         jcprops.DIBColor = (IJL_COLOR)IJL_OTHER;
         jcprops.JPGColor = (IJL_COLOR)IJL_OTHER;
         break;
   }
   
   //Read JPEG image data into our 24bit buffer
   if (Location == FROM_FILE)
      jerr = ijlRead(&jcprops, IJL_JFILE_READWHOLEIMAGE);
   else
      jerr = ijlRead(&jcprops, IJL_JBUFF_READWHOLEIMAGE);

   //Make sure the read was successful
   if (jerr != IJL_OK)
   {
      MessageBox(NULL, "Error reading JPEG image", NULL, MB_OK);
      return false;
   }

   HBITMAP hbm;

   //Convert to current CLRMODE
   if (ddpixelformat.dwRGBBitCount == 16)
   {
      //Create a 16bit buffer
      WORD *buffer16;
      long szbuff16;
      
      //determine the size of our buffer
      szbuff16 = ((jcprops.JPGWidth * 16 + 7) / 8)
         * jcprops.JPGHeight;
      
      //resize the buffer and make sure resize works
      buffer16 = new WORD [szbuff16];
      if (buffer16 == NULL)
      {
         MessageBox(NULL, "Error creating 16Bit buffer", NULL, MB_OK);
         return false;
      }

      //Start at the beginning of the buffer
      long j = 0;

	  int PixelFormat;
	  if((ddpixelformat.dwGBitMask / 32) == 31)
         PixelFormat = PF_555;
      else if((ddpixelformat.dwGBitMask / 32) == 63)
         PixelFormat = PF_565;

      //Step through the 24bit buffer
      //Retrieve 3 channels at a time and convert their values to 16bit
      for (long i = 0; i < szbuff24; i += 3)   
      {
         //Check the pixel format and write the color data
         //to the 16bit buffer. After the write, advance the
         //16bit buffer by one.
		  
         if (PixelFormat == PF_555)
            buffer16[j++] = RGB555(buffer24[i], buffer24[i + 1],
               buffer24[i + 2]);
         else
            buffer16[j++] = RGB565(buffer24[i], buffer24[i + 1],
               buffer24[i + 2]);
      }

      //Create the bitmap using the new 16bit buffer   
      hbm = CreateBitmap (jcprops.JPGWidth, jcprops.JPGHeight, 1,
         16, buffer16);
      if(hbm == NULL)
      {
         MessageBox(NULL, "Failed to create 16Bit Bitmap", NULL, MB_OK);
         return false;
      }

      //remove the new buffer
      delete buffer16;
   }
   else if (ddpixelformat.dwRGBBitCount == 24)
   {
      //The data we have from the JPEG is already 24bit
      //Just create the new bitmap from our buffer
      hbm = CreateBitmap (jcprops.JPGWidth, jcprops.JPGHeight, 1,
         24, buffer24);
      if(hbm == NULL)
      {
         MessageBox(NULL, "Failed to create 24Bit Bitmap", NULL, MB_OK);
         return false;
      }
   }
   else if (ddpixelformat.dwRGBBitCount == 32)
   {
      //Create a 32bit buffer
      BYTE *buffer32;
      long szbuff32;
      
      //determine the size of our buffer
      szbuff32 = ((jcprops.JPGWidth * 32 + 7) / 8)
         * jcprops.JPGHeight;

      //resize the buffer and make sure resize works
      buffer32 = new BYTE [szbuff32];
      if (buffer32 == NULL)
      {
         MessageBox(NULL, "Error creating 32Bit buffer", NULL, MB_OK);
         return false;
      }
      
      //Start at the beginning of the 24bit buffer
      long j = 0;

      //Step through the 32bit buffer, copying the 3 Channels from
      //the 24bit buffer. However, at the end of each channel, we
      //have to write an extra byte for the 32bit image's alpha
      //channel
      for (long i = 0; i < szbuff32; i += 4)   
      {
         buffer32[i + 0] = buffer24[j++]; //Blue channel
         buffer32[i + 1] = buffer24[j++]; //Green channel
         buffer32[i + 2] = buffer24[j++]; //Red channel
         buffer32[i + 3] = (BYTE)0x00;    //Alpha channel
      }

      //Create the bitmap using the new 32bit buffer   
      hbm = CreateBitmap (jcprops.JPGWidth, jcprops.JPGHeight, 1,
         32, buffer32);
      if(hbm == NULL)
      {
         MessageBox(NULL, "Failed to create 32Bit Bitmap",
            NULL, MB_OK);
         return false;
      }

      //remove the 32bit buffer
      delete buffer32;
   }
   else
   {
      MessageBox(NULL,
         "Palettized modes not supported. Use a 16, 24, or 32 Bit display.",
         NULL, MB_OK);
      return false;
   }

   //remove the old buffer
   delete buffer24;

   //Create a DirectDraw surface   
   DDSURFACEDESC2 ddsd;

   //If DirectDraw Surface is NULL, create it
   if (*Surface == NULL)
   {
      // Create a DirectDrawSurface for this bitmap
      ZeroMemory(&ddsd, sizeof(ddsd));
      ddsd.dwSize   = sizeof(ddsd);
      ddsd.dwFlags  = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
      ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
      ddsd.dwWidth  = jcprops.JPGWidth;
      ddsd.dwHeight = jcprops.JPGHeight;

      //Create the surface from our surface description
      HRESULT Result = lpdd->CreateSurface(&ddsd, Surface, NULL);
      if(Result != DD_OK)
      {
         MessageBox(NULL, "Could not create surface", NULL, MB_OK);
         return false;
      }
   }

   //Copy bitmap data to DirectDraw Surface
   DDCopyBitmap(*Surface, hbm, 0, 0, 0, 0);
      
   // clean up and destroy the JPEG Decompressor
   ijlFree(&jcprops);

   return true;
}