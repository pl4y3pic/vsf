                              //ImageBox.h//                                
//////////////////////////////////////////////////////////////////////////////////
//																				//
// Author:  Simeon Kosnitsky													//
//          skosnits@gmail.com													//
//																				//
// License:																		//
//     This software is released into the public domain.  You are free to use	//
//     it in any way you like, except that you may not sell this source code.	//
//																				//
//     This software is provided "as is" with no expressed or implied warranty.	//
//     I accept no liability for any damage or loss of business that this		//
//     software may cause.														//
//																				//
//////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <wx/minifram.h>
#include <wx/image.h>
#include "MyResource.h"
#include "MainFrm.h"
#include "StaticText.h"
#include "ResizableWindow.h"

class CImageBox;

class CImageWnd : public wxWindow
{
public:
	CImageWnd(CImageBox *pIB);
	~CImageWnd();
	
	CImageBox *m_pIB;

public:
	void OnPaint( wxPaintEvent &event );

private:
   DECLARE_EVENT_TABLE()
};

class CImageBox : public CResizableWindow
{
public:
	CImageBox(CMainFrame* pMF);           // protected constructor used by dynamic creation
	~CImageBox();

	wxTextCtrl	*m_pLog;
	CStaticText	*m_plblIB;
	CImageWnd   *m_pIW;

	wxColour	m_IBColor;

	bool		m_WasInited;

	wxImage		*m_pImage;

	CMainFrame*	m_pMF;

public:
	void Init();
	void ResizeControls();

	void ViewRGBImage(simple_buffer<int> &Im, int w, int h);
	void ViewGrayscaleImage(simple_buffer<u8> &Im, int w, int h);
	void ViewImage(simple_buffer<int> &Im, int w, int h);
	void ViewBGRImage(simple_buffer<u8>& ImBGR, int w, int h);
	void ClearScreen();

public:
	void OnSize( wxSizeEvent& event );

private:
	std::mutex m_view_mutex;
	
	DECLARE_EVENT_TABLE()
};


