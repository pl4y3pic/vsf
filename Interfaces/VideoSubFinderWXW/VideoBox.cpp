                              //VideoBox.cpp//                                
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

#include "VideoBox.h"

BEGIN_EVENT_TABLE(CVideoWnd, wxWindow)
	EVT_PAINT(CVideoWnd::OnPaint)
	EVT_SET_FOCUS(CVideoWnd::OnSetFocus)
	EVT_ERASE_BACKGROUND(CVideoWnd::OnEraseBackGround)
	EVT_LEFT_DOWN(CVideoWnd::OnLeftDown)
END_EVENT_TABLE()

CVideoWnd::CVideoWnd(CVideoWindow *pVW)
		:wxWindow( pVW, wxID_ANY )
{
	this->SetBackgroundColour(wxColor(125, 125, 125));

	m_pVW = pVW;
	m_pVB = pVW->m_pVB;
	m_filter_image = false;
}

CVideoWnd::~CVideoWnd()
{
}

void CVideoWnd::OnEraseBackGround(wxEraseEvent& event)
{
	bool do_erase = true;

	if (m_pVB)
	{
		if (m_pVB->m_pImage || m_pVB->m_pMF->m_VIsOpen)
		{
			do_erase = false;
		}
	}
	
	if (do_erase)
	{
		int w, h;
		this->GetClientSize(&w, &h);
		event.GetDC()->SetBrush(wxBrush(wxColor(125, 125, 125)));
		event.GetDC()->DrawRectangle(0, 0, w, h);
	}
}

bool CVideoWnd::CheckFilterImage()
{
	bool filter_image = false;

	if ((m_pVB->m_pMF->m_VIsOpen) || (m_pVB->m_pImage != NULL))
	{

		if (wxGetKeyState(wxKeyCode('U')) || wxGetKeyState(wxKeyCode('u')))
		{
			filter_image = true;
		}
		else 
		{
			if (g_color_ranges.size() > 0)
			{
				if (wxGetKeyState(wxKeyCode('R')) || wxGetKeyState(wxKeyCode('r')) ||
					wxGetKeyState(wxKeyCode('T')) || wxGetKeyState(wxKeyCode('t')) ||
					wxGetKeyState(wxKeyCode('Y')) || wxGetKeyState(wxKeyCode('y')))
				{
					filter_image = true;
				}
			}

			if (!filter_image)
			{
				if (g_outline_color_ranges.size() > 0)
				{
					if (wxGetKeyState(wxKeyCode('R')) || wxGetKeyState(wxKeyCode('r')) ||
						wxGetKeyState(wxKeyCode('T')) || wxGetKeyState(wxKeyCode('t')) ||
						wxGetKeyState(wxKeyCode('I')) || wxGetKeyState(wxKeyCode('i')))
					{
						filter_image = true;
					}
				}
			}
		}		
	}

	return filter_image;
}

inline void FilterImage(simple_buffer<u8> &ImBGR, simple_buffer<u8> &ImLab, const int w, const int h)
{
	bool bln_color_ranges = (g_color_ranges.size() > 0) ? true : false;
	bool bln_outline_color_ranges = (g_outline_color_ranges.size() > 0) ? true : false;	
	bool bln_r_pressed = (wxGetKeyState(wxKeyCode('R')) || wxGetKeyState(wxKeyCode('r')));
	bool bln_t_pressed = (wxGetKeyState(wxKeyCode('T')) || wxGetKeyState(wxKeyCode('t')));
	bool bln_y_pressed = (wxGetKeyState(wxKeyCode('Y')) || wxGetKeyState(wxKeyCode('y')));
	bool bln_i_pressed = (wxGetKeyState(wxKeyCode('I')) || wxGetKeyState(wxKeyCode('i')));
	int rc = GetBGRColor(ColorName::Red);
	int gc = GetBGRColor(ColorName::Green);
	int yc = GetBGRColor(ColorName::Yellow);
	int bc = 0;
	int pc;

	if (bln_y_pressed)
	{
		if ((!bln_t_pressed) && (!bln_r_pressed))
		{
			if (!bln_color_ranges)
			{
				return;
			}
		}
	}

	if (bln_i_pressed)
	{
		if ((!bln_t_pressed) && (!bln_r_pressed) && (!bln_y_pressed))
		{
			if (!bln_outline_color_ranges)
			{
				return;
			}
		}
	}

	if (bln_color_ranges || bln_outline_color_ranges)
	{
		int num_pixels = w * h;

		if (bln_r_pressed)
		{
			simple_buffer<u8> ImRes(w * h, (u8)255);
			int res = FilterImageByPixelColorIsInRange(ImRes, &ImBGR, &ImLab, w, h, wxT("FilterImage"), (u8)0, true, true);

			ImBGR.set_values(0);
			
			if (res != 0)
			{
				for (int p_id = 0; p_id < num_pixels; p_id++)
				{
					if (ImRes[p_id] != 0)
					{
						SetBGRColor(ImBGR, p_id, rc);
					}
				}
			}
		}
		else
		{
			int offset;			
			bool bln_in_color_ranges;
			bool bln_in_outline_color_ranges;

			for (int p_id = 0; p_id < num_pixels; p_id++)
			{
				bln_in_color_ranges = false;
				bln_in_outline_color_ranges = false;
				pc = -1;

				if (bln_color_ranges) bln_in_color_ranges = PixelColorIsInRange(g_color_ranges, &ImBGR, &ImLab, w, h, p_id);
				if (bln_outline_color_ranges) bln_in_outline_color_ranges = PixelColorIsInRange(g_outline_color_ranges, &ImBGR, &ImLab, w, h, p_id);

				if (bln_t_pressed)
				{
					if (bln_in_color_ranges)
					{
						if (bln_in_outline_color_ranges)
						{
							pc = yc;
						}
						else
						{
							pc = rc;
						}
					}
					else
					{
						if (bln_in_outline_color_ranges)
						{
							pc = gc;
						}
						else
						{
							pc = bc;
						}
					}
				}
				else if (bln_y_pressed)
				{
					if (!bln_in_color_ranges)
					{
						pc = bc;
					}
				}
				else if (bln_i_pressed)
				{
					if (!bln_in_outline_color_ranges)
					{
						pc = bc;
					}
				}

				if (pc != -1)
				{
					SetBGRColor(ImBGR, p_id, pc);
				}
			}
		}
	}
}

void CVideoWnd::DrawImage(simple_buffer<u8>& ImBGR, const int w, const int h)
{
	wxPaintDC dc(this);
	{
		int num_pixels = w * h;
		u8* img_data = (unsigned char*)malloc(num_pixels * 3); // auto released by wxImage

		for (int i = 0; i < num_pixels; i++)
		{
			img_data[(i * 3)] = ImBGR[(i * 3) + 2];
			img_data[(i * 3) + 1] = ImBGR[(i * 3) + 1];
			img_data[(i * 3) + 2] = ImBGR[(i * 3)];
		}

		wxImage Im(w, h, img_data);

		int cw, ch;
		this->GetClientSize(&cw, &ch);

		if ((cw != w) || (ch != h))
		{
			dc.DrawBitmap(Im.Scale(cw, ch, wxIMAGE_QUALITY_HIGH), 0, 0);
		}
		else
		{
			dc.DrawBitmap(Im, 0, 0);
		}
	}
}

void wxImageToBGR(wxImage& wxIm, simple_buffer<u8>& ImBGRRes)
{
	int num_pixels = wxIm.GetWidth() * wxIm.GetHeight();
	u8* img_data = wxIm.GetData();

	ImBGRRes.set_size(num_pixels * 3);

	for (int i = 0; i < num_pixels; i++)
	{
		ImBGRRes[(i * 3)] = img_data[(i * 3) + 2];
		ImBGRRes[(i * 3) + 1] = img_data[(i * 3) + 1];
		ImBGRRes[(i * 3) + 2] = img_data[(i * 3)];
	}
}

void CVideoWnd::OnPaint(wxPaintEvent& WXUNUSED(event))
{	
	if (m_pVB != NULL) 
	{		
		if (m_pVB->m_pMF->m_VIsOpen)
		{
			bool filter_image = CheckFilterImage();
			
			if (filter_image)
			{
				m_filter_image = true;
				int vw = m_pVB->m_pMF->m_pVideo->m_Width;
				int vh = m_pVB->m_pMF->m_pVideo->m_Height;
				int imw = vw;
				int imh = vh;
				simple_buffer<u8> ImBGR(imw * imh * 3), ImLab;
				m_pVB->m_pMF->m_pVideo->GetBGRImage(ImBGR, 0, imw - 1, 0, imh - 1);

				{
					cv::Mat cv_ImBGR, cv_ImLab;
					BGRImageToMat(ImBGR, imw, imh, cv_ImBGR);
					//cv::resize(cv_ImBGROrig, cv_ImBGR, cv::Size(0, 0), g_scale, g_scale);
					cv::cvtColor(cv_ImBGR, cv_ImLab, cv::COLOR_BGR2Lab);
					//imw *= g_scale;
					//imh *= g_scale;
					//ImBGR.set_size(imw * imh * 3);
					ImLab.set_size(imw * imh * 3);
					//BGRMatToImage(cv_ImBGR, imw, imh, ImBGR);
					BGRMatToImage(cv_ImLab, imw, imh, ImLab);
				}

				if ( !wxGetKeyState(wxKeyCode('U')) && !wxGetKeyState(wxKeyCode('u')) )
				{
					FilterImage(ImBGR, ImLab, imw, imh);
				}
				DrawImage(ImBGR, imw, imh);
			}
			else
			{
				m_filter_image = false;
				wxPaintDC dc(this);
				int cw, ch;
				this->GetClientSize(&cw, &ch);
				m_pVB->m_pMF->m_pVideo->SetVideoWindowPosition(0, 0, cw, ch, &dc);
			}
		}
		else if (m_pVB->m_pImage != NULL)
		{
			bool filter_image = CheckFilterImage();

			if (filter_image)
			{
				m_filter_image = true;
				int w = m_pVB->m_pImage->GetWidth();
				int h = m_pVB->m_pImage->GetHeight();
				simple_buffer<u8> ImBGR, ImLab;
				wxImageToBGR(*(m_pVB->m_pImage), ImBGR);

				{
					cv::Mat cv_ImBGR, cv_ImLab;
					BGRImageToMat(ImBGR, w, h, cv_ImBGR);
					//cv::resize(cv_ImBGROrig, cv_ImBGR, cv::Size(0, 0), g_scale, g_scale);
					cv::cvtColor(cv_ImBGR, cv_ImLab, cv::COLOR_BGR2Lab);
					//w *= g_scale;
					//h *= g_scale;
					//ImBGR.set_size(w * h * 3);
					ImLab.set_size(w * h * 3);
					//BGRMatToImage(cv_ImBGR, w, h, ImBGR);
					BGRMatToImage(cv_ImLab, w, h, ImLab);
				}

				if (!wxGetKeyState(wxKeyCode('U')) && !wxGetKeyState(wxKeyCode('u')))
				{
					FilterImage(ImBGR, ImLab, w, h);
				}
				DrawImage(ImBGR, w, h);
			}
			else
			{
				m_filter_image = false;
				wxPaintDC dc(this);
				int cw, ch;
				this->GetClientSize(&cw, &ch);
				dc.DrawBitmap(m_pVB->m_pImage->Scale(cw, ch), 0, 0);
			}
		}
		else
		{
			wxPaintDC dc(this);
			dc.Clear();
		}
	}
}

void CVideoWnd::OnSetFocus(wxFocusEvent& event)
{
	m_pVB->SetFocus();
}

void CVideoWnd::OnLeftDown(wxMouseEvent& event)
{
	if (m_pVB != NULL)
	{
		int cw, ch, mx, my;
		this->GetClientSize(&cw, &ch);
		event.GetPosition(&mx, &my);

		if (((mx >= 0) && (mx < cw)) &&
			((my >= 0) && (my < ch)))
		{
			if ((m_pVB->m_pMF->m_VIsOpen) || (m_pVB->m_pImage != NULL))
			{
				wxClientDC dc(this);
				wxColor clr;
				dc.GetPixel(wxPoint(mx, my), &clr);

				u8 bgr[3], lab[3], y;

				bgr[0] = clr.Blue();
				bgr[1] = clr.Green();
				bgr[2] = clr.Red();

				BGRToYUV(bgr[0], bgr[1], bgr[2], &y);
				BGRToLab(bgr[0], bgr[1], bgr[2], &(lab[0]), &(lab[1]), &(lab[2]));

				m_pVB->m_pMF->m_cfg.m_pixel_color_bgr = wxString::Format(wxT("RGB: r:%d g:%d b:%d L:%d"), (int)(bgr[2]), (int)(bgr[1]), (int)(bgr[0]), (int)y);
				m_pVB->m_pMF->m_cfg.m_pixel_color_lab = wxString::Format(wxT("Lab: l:%d a:%d b:%d"), (int)(lab[0]), (int)(lab[1]), (int)(lab[2]));
				m_pVB->m_pMF->m_pPanel->m_pSSPanel->m_pPixelColorRGB->SetLabel(m_pVB->m_pMF->m_cfg.m_pixel_color_bgr);
				m_pVB->m_pMF->m_pPanel->m_pSSPanel->m_pPixelColorLab->SetLabel(m_pVB->m_pMF->m_cfg.m_pixel_color_lab);
				m_pVB->m_pMF->m_pPanel->m_pSSPanel->m_pPixelColorExample->SetBackgroundColour(wxColour(bgr[2], bgr[1], bgr[0]));
				m_pVB->m_pMF->m_pPanel->m_pSSPanel->m_pPixelColorExample->Refresh();
			}
		}
	}

	event.Skip();
}


BEGIN_EVENT_TABLE(CVideoWindow, wxPanel)
	EVT_PAINT(CVideoWindow::OnPaint)
	EVT_SIZE(CVideoWindow::OnSize)	
END_EVENT_TABLE()

CVideoWindow::CVideoWindow(CVideoBox *pVB)
		:wxPanel( pVB, wxID_ANY )
{
	m_pVB = pVB;
	m_WasInited = false;
	m_pVideoWnd = NULL;
}

CVideoWindow::~CVideoWindow()
{
}

void CVideoWindow::Init()
{
	m_pVideoWnd = new CVideoWnd(this);

	m_pHSL1 = new CSeparatingLine(this, 200, 3, 7, 3, 100, 110, 50, 0);
	m_pHSL1->m_pos = 0;
	m_pHSL2 = new CSeparatingLine(this, 200, 3, 7, 3, 140, 150, 50, 0);
	m_pHSL2->m_pos = 1;
	m_pVSL1 = new CSeparatingLine(this, 3, 100, 3, 7, 100, 110, 50, 1);
	m_pVSL1->m_pos = 0;
	m_pVSL2 = new CSeparatingLine(this, 3, 100, 3, 7, 140, 150, 50, 1);
	m_pVSL2->m_pos = 1;

	m_pHSL1->Raise();
	m_pHSL2->Raise();
	m_pVSL1->Raise();
	m_pVSL2->Raise();

	this->SetSize(50,50,200,200);
}

void CVideoWindow::Update()
{
	m_pHSL1->m_pos_max = m_pHSL2->m_pos-0.05;
	m_pHSL2->m_pos_min = m_pHSL1->m_pos+0.05;
	m_pVSL1->m_pos_max = m_pVSL2->m_pos-0.05;
	m_pVSL2->m_pos_min = m_pVSL1->m_pos+0.05;

	if (m_pVB->m_pMF->m_VIsOpen)
	{
		if (m_pVB->m_pMF->m_vs != CMainFrame::Play)
		{
			s64 Cur;
			Cur = m_pVB->m_pMF->m_pVideo->GetPos();
			m_pVB->m_pMF->m_pVideo->SetPos(Cur);
		}
	}
}

void CVideoWindow::Refresh(bool eraseBackground,
	const wxRect* rect)
{
	bool need_to_refresh = true;

	if (m_pVideoWnd)
	{
		if (m_pVideoWnd->GetParent() == NULL)
		{
			m_pVideoWnd->Refresh(true);
			need_to_refresh = false;
		}
	}
	
	if (need_to_refresh)
	{
		wxWindow::Refresh(eraseBackground, rect);
	}
}

void CVideoWindow::OnPaint( wxPaintEvent &event )
{
	wxPaintDC dc(this);	
}

void CVideoWindow::OnSize(wxSizeEvent& event)
{
	int w, h;
	wxRect rcCL, rcVWND;

	this->GetClientSize(&w, &h);
	
	rcVWND.x = 9;
	rcVWND.y = 9;
	rcVWND.width = w - rcVWND.x*2;
	rcVWND.height = h - rcVWND.y*2;

	m_pVideoWnd->SetSize(rcVWND);

	m_pHSL1->m_offset = rcVWND.x - 2;
	m_pHSL1->m_w = rcVWND.width + 4;
	m_pHSL1->m_min = rcVWND.y;
	m_pHSL1->m_max = m_pHSL1->m_min + rcVWND.height;
	//m_pHSL1->m_pos = 0;

	m_pHSL2->m_offset = m_pHSL1->m_offset;
	m_pHSL2->m_w = m_pHSL1->m_w;
	m_pHSL2->m_min = rcVWND.y;
	m_pHSL2->m_max = m_pHSL2->m_min + rcVWND.height;
	//m_pHSL2->m_pos = 1;

	m_pVSL1->m_offset = rcVWND.y - 2;
	m_pVSL1->m_h = rcVWND.height + 4;
	m_pVSL1->m_min = rcVWND.x;
	m_pVSL1->m_max = m_pVSL1->m_min + rcVWND.width;
	//m_pVSL1->m_pos = 0;

	m_pVSL2->m_offset = m_pVSL1->m_offset;
	m_pVSL2->m_h = m_pVSL1->m_h;
	m_pVSL2->m_min = rcVWND.x;
	m_pVSL2->m_max = m_pVSL2->m_min + rcVWND.width;
	//m_VSL2->m_pos = 1;

	m_pHSL1->UpdateSL();
	m_pHSL2->UpdateSL();
	m_pVSL1->UpdateSL();
	m_pVSL2->UpdateSL();
}

BEGIN_EVENT_TABLE(CVideoBox, CResizableWindow)
	EVT_SIZE(CVideoBox::OnSize)
	EVT_MENU(ID_TB_RUN, CVideoBox::OnBnClickedRun)
	EVT_MENU(ID_TB_PAUSE, CVideoBox::OnBnClickedPause)
	EVT_MENU(ID_TB_STOP, CVideoBox::OnBnClickedStop)
	EVT_KEY_DOWN(CVideoBox::OnKeyDown)
	EVT_KEY_UP(CVideoBox::OnKeyUp)
	EVT_MOUSEWHEEL(CVideoBox::OnMouseWheel)
	EVT_SCROLL_THUMBTRACK(CVideoBox::OnHScroll)
	EVT_TIMER(TIMER_ID_VB, CVideoBox::OnTimer)
END_EVENT_TABLE()

CVideoBox::CVideoBox(CMainFrame* pMF, wxColour	bc)
		: CResizableWindow(pMF,//->GetClientWindow(),
				  wxID_ANY,
		          wxDefaultPosition, wxDefaultSize), m_timer(this, TIMER_ID_VB), m_bc (bc)
{
	m_pImage = NULL;
	m_pMF = pMF;
	m_WasInited = false;
}

CVideoBox::~CVideoBox()
{
	if (m_pImage != NULL)
	{
		delete m_pImage;
		m_pImage = NULL;
	}
}

void CVideoBox::Init()
{
	//wxString strVBClass;
	//wxString strVBXClass;
	wxBitmap bmp;

	m_VBX = wxColour(125, 125, 125);
	m_CL1 = wxColour(255, 255, 225);
	m_CL2 = wxColour(0, 0, 0);

	this->SetBackgroundColour(m_bc);

	m_pVBar = new wxToolBar(this, wxID_ANY,
                               wxDefaultPosition, wxSize(30, 30), 
							   wxTB_HORIZONTAL | wxTB_BOTTOM | 
							   /*wxTB_NO_TOOLTIPS |*/ wxTB_FLAT );
	m_pVBar->SetMargins(4, 4);

	LoadToolBarImage(bmp, g_app_dir + "/bitmaps/tb_run.bmp", m_bc);
	m_pVBar->AddTool(ID_TB_RUN, _T(""), bmp, wxNullBitmap, wxITEM_CHECK);

	LoadToolBarImage(bmp, g_app_dir + "/bitmaps/tb_pause.bmp", m_bc);
	m_pVBar->AddTool(ID_TB_PAUSE, _T(""), bmp, wxNullBitmap, wxITEM_CHECK);
	
	LoadToolBarImage(bmp, g_app_dir + "/bitmaps/tb_stop.bmp", m_bc);
	m_pVBar->AddTool(ID_TB_STOP, _T(""), bmp, wxNullBitmap, wxITEM_CHECK);	

	m_plblVB = new CStaticText( this, ID_LBL_VB, wxT("Video Box") );
	m_plblVB->SetSize(0, 0, 390, 30);
	m_plblVB->SetFont(m_pMF->m_LBLFont);
	m_plblVB->SetBackgroundColour( m_CL1 );
	
	m_plblTIME = new CStaticText( this, ID_LBL_TIME, 
									wxT("00:00:00,000/00:00:00,000   "), wxALIGN_RIGHT | wxTB_BOTTOM);
	m_plblTIME->SetSize(200, 242, 190, 26);
	m_plblTIME->SetFont(m_pMF->m_LBLFont);
	m_plblTIME->SetTextColour(*wxWHITE);
	m_plblTIME->SetBackgroundColour( m_CL2 );

	m_pVBox = new CVideoWindow(this);
	m_pVBox->Init();

	m_pSB = new CScrollBar(this, ID_TRACKBAR);
	m_pSB->SetScrollRange(0, 255);

	this->SetSize(20,20,402,300);

	m_pVBar->Realize();

	m_plblVB->Bind(wxEVT_MOTION, &CResizableWindow::OnMouseMove, this);
	m_plblVB->Bind(wxEVT_LEAVE_WINDOW, &CResizableWindow::OnMouseLeave, this);
	m_plblVB->Bind(wxEVT_LEFT_DOWN, &CResizableWindow::OnLButtonDown, this);
	m_plblVB->Bind(wxEVT_LEFT_UP, &CResizableWindow::OnLButtonUp, this);

	m_plblTIME->Bind(wxEVT_MOTION, &CResizableWindow::OnMouseMove, this);
	m_plblTIME->Bind(wxEVT_LEAVE_WINDOW, &CResizableWindow::OnMouseLeave, this);
	m_plblTIME->Bind(wxEVT_LEFT_DOWN, &CResizableWindow::OnLButtonDown, this);
	m_plblTIME->Bind(wxEVT_LEFT_UP, &CResizableWindow::OnLButtonUp, this);

	m_WasInited = true;
}

void CVideoBox::OnSize(wxSizeEvent& event)
{
	int w, h, sbw, sbh, csbw, csbh;
	wxRect rlVB, rlTIME, rcSB, rcVBOX, rcVBAR;

	this->GetClientSize(&w, &h);
	
	rlVB.x = 0;
	rlVB.width = w;
	rlVB.y = 0;
	rlVB.height = 28;

	int vbw, vbh;
	m_pVBar->GetBestSize(&vbw, &vbh);

	rcVBAR.width = vbw;
	rcVBAR.height = vbh;
	rcVBAR.x = 2;
	rcVBAR.y = h - rcVBAR.height - 2;

	m_pSB->GetClientSize(&csbw, &csbh);
	m_pSB->GetSize(&sbw, &sbh);

	rcSB.x = 2;
	rcSB.width = w - rcSB.x*2;
	rcSB.height = (sbh-csbh) + m_pSB->m_SBLA.GetHeight();
	rcSB.y = rcVBAR.y - rcSB.height - 1;	
	
	rcVBOX.x = 0;
	rcVBOX.width = w;
	rcVBOX.y = rlVB.GetBottom() + 1;
	rcVBOX.height = rcSB.y - rcVBOX.y - 2;

	m_plblVB->SetSize(rlVB);
	m_pVBox->SetSize(rcVBOX);
	m_pSB->SetSize(rcSB);
	m_pVBar->SetSize(rcVBAR);

	//m_pVBar->GetSize(&w, &h);

	rlTIME.width = w - rcVBAR.width - rcVBAR.x - 2;
	rlTIME.height = 22;
	rlTIME.x = rcVBAR.x + rcVBAR.width;
	rlTIME.y = rcVBAR.y + (rcVBAR.height - rlTIME.height)/2;

	m_plblTIME->SetSize(rlTIME);

	this->Refresh(false);
	this->Update();

	event.Skip();
}

void CVideoBox::OnBnClickedRun(wxCommandEvent& event)
{
	if (m_pMF->m_VIsOpen)
	{
		m_pVBar->ToggleTool(ID_TB_RUN, true);
		m_pVBar->ToggleTool(ID_TB_PAUSE, false);
		m_pVBar->ToggleTool(ID_TB_STOP, false);

		m_pMF->m_pVideo->Run();
		m_pMF->m_vs = m_pMF->Play;
	}
	else
	{
		m_pVBar->ToggleTool(ID_TB_RUN, false);
		m_pVBar->ToggleTool(ID_TB_PAUSE, false);
		m_pVBar->ToggleTool(ID_TB_STOP, false);
	}
}

void CVideoBox::OnBnClickedPause(wxCommandEvent& event)
{
	if (m_pMF->m_VIsOpen)
	{
		m_pVBar->ToggleTool(ID_TB_RUN, false);
		m_pVBar->ToggleTool(ID_TB_PAUSE, true);
		m_pVBar->ToggleTool(ID_TB_STOP, false);

		m_pMF->m_pVideo->Pause();		
		m_pMF->m_vs = m_pMF->Pause;
	}
	else
	{
		m_pVBar->ToggleTool(ID_TB_RUN, false);
		m_pVBar->ToggleTool(ID_TB_PAUSE, false);
		m_pVBar->ToggleTool(ID_TB_STOP, false);
	}
}

void CVideoBox::OnBnClickedStop(wxCommandEvent& event)
{
	if (m_pMF->m_VIsOpen)
	{
		wxCommandEvent cmd_event;
		m_pMF->OnStop(cmd_event);
	}
	else
	{
		m_pVBar->ToggleTool(ID_TB_RUN, false);
		m_pVBar->ToggleTool(ID_TB_PAUSE, false);
		m_pVBar->ToggleTool(ID_TB_STOP, false);
	}
}

void CVideoBox::OnKeyDown(wxKeyEvent& event)
{
	s64 Cur;
	int key_code = event.GetKeyCode();

	switch (key_code)
	{
		case 'R':
		case 'r':
		case 'T':
		case 't':
		case 'Y':
		case 'y':
		case 'U':
		case 'u':
		case 'I':
		case 'i':
			g_color_ranges = GetColorRanges(g_use_filter_color);
			g_outline_color_ranges = GetColorRanges(g_use_outline_filter_color);

			if (m_pVBox->m_pVideoWnd->CheckFilterImage())
			{
				m_pVBox->m_pVideoWnd->Reparent(NULL);
				int w = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
				int h = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
				m_pVBox->m_pVideoWnd->SetSize(0, 0, w, h);

				m_pVBox->m_pVideoWnd->Refresh(false);
				m_pVBox->m_pVideoWnd->Update();

				if (!m_timer.IsRunning())
				{
					m_timer.Start(100);
				}

				break;
			}
			break;
	}

	if (m_pMF->m_VIsOpen)
	{
		wxCommandEvent evt;

		switch ( key_code )
		{
			case WXK_RIGHT:
				m_pMF->PauseVideo();
				m_pMF->m_pVideo->OneStep();
				break;

			case WXK_UP:
				m_pMF->PauseVideo();
				m_pMF->m_pVideo->OneStep();
				break;

			case WXK_LEFT:
				m_pMF->PauseVideo();
				Cur = m_pMF->m_pVideo->GetPos();
				Cur -= m_pMF->m_dt;
				if (Cur < 0) Cur = 0;
				m_pMF->m_pVideo->SetPosFast(Cur);
				break;
			
			case WXK_DOWN:
				m_pMF->PauseVideo();
				Cur = m_pMF->m_pVideo->GetPos();
				Cur -= m_pMF->m_dt;
				if (Cur < 0) Cur = 0;
				m_pMF->m_pVideo->SetPosFast(Cur);
				break;

			case WXK_SPACE:				
 				m_pMF->OnPlayPause(evt);
				break;
		}
	}
}

void CVideoBox::OnKeyUp(wxKeyEvent& event)
{
	int key_code = event.GetKeyCode();

	switch (key_code)
	{
		case 'R':
		case 'r':
		case 'T':
		case 't':
		case 'Y':
		case 'y':
		case 'U':
		case 'u':
		case 'I':
		case 'i':
			if (m_pVBox->m_pVideoWnd->CheckFilterImage())
			{
				m_pVBox->m_pVideoWnd->Refresh(false);
				m_pVBox->m_pVideoWnd->Update();
			}
			else
			{
				if (m_timer.IsRunning())
				{
					m_timer.Stop();
				}

				m_pVBox->m_pVideoWnd->Reparent(m_pVBox);

				wxSizeEvent event;
				m_pVBox->OnSize(event);
			}
			break;
	}
}

void CVideoBox::OnMouseWheel(wxMouseEvent& event)
{
	if (m_pMF->m_VIsOpen)
	{
		s64 Cur;

		if (event.m_wheelRotation > 0)
		{
			m_pMF->PauseVideo();
			m_pMF->m_pVideo->OneStep();
		}
		else
		{
			m_pMF->PauseVideo();
			Cur = m_pMF->m_pVideo->GetPos();
			Cur -= m_pMF->m_dt;
			if (Cur < 0) Cur = 0;
			m_pMF->m_pVideo->SetPosFast(Cur);
		}
	}
	else
	{
		m_pMF->OnMouseWheel(event);
	}
}

void CVideoBox::ViewImage(simple_buffer<int> &Im, int w, int h)
{
	int num_pixels = w*h;
	u8 *color;

	unsigned char *img_data = (unsigned char*)malloc(num_pixels * 3); // auto released by wxImage

	for (int i = 0; i < num_pixels; i++)
	{
		color = (u8*)(&Im[i]);
		img_data[i * 3] = color[2]; //r
		img_data[i * 3 + 1] = color[1]; //g
		img_data[i * 3 + 2] = color[0]; //b
	}

	{
		std::lock_guard<std::mutex> guard(m_view_mutex);

		if (m_pImage != NULL) delete m_pImage;
		m_pImage = new wxImage(w, h, img_data);

		m_pVBox->m_pVideoWnd->Refresh(false);
		m_pVBox->m_pVideoWnd->Update();
	}
}

void CVideoBox::ViewGrayscaleImage(simple_buffer<u8>& Im, int w, int h)
{
	int num_pixels = w * h;

	unsigned char* img_data = (unsigned char*)malloc(num_pixels * 3); // auto released by wxImage

	for (int i = 0; i < num_pixels; i++)
	{
		img_data[i * 3] = Im[i];
		img_data[i * 3 + 1] = Im[i];
		img_data[i * 3 + 2] = Im[i];
	}

	{
		std::lock_guard<std::mutex> guard(m_view_mutex);

		if (m_pImage != NULL) delete m_pImage;
		m_pImage = new wxImage(w, h, img_data);

		m_pVBox->m_pVideoWnd->Refresh(false);
		m_pVBox->m_pVideoWnd->Update();
	}
}

void CVideoBox::ViewBGRImage(simple_buffer<u8>& ImBGR, int w, int h)
{
	int num_pixels = w * h;
	unsigned char* img_data = (unsigned char*)malloc(num_pixels * 3); // auto released by wxImage

	for (int i = 0; i < num_pixels; i++)
	{
		img_data[i * 3] = ImBGR[i * 3 + 2];
		img_data[i * 3 + 1] = ImBGR[i * 3 + 1];
		img_data[i * 3 + 2] = ImBGR[i * 3];
	}

	{
		std::lock_guard<std::mutex> guard(m_view_mutex);

		if (m_pImage != NULL) delete m_pImage;
		m_pImage = new wxImage(w, h, img_data);

		m_pVBox->m_pVideoWnd->Refresh(false);
		m_pVBox->m_pVideoWnd->Update();
	}
}

void CVideoBox::ClearScreen()
{
	if (m_pImage != NULL)
	{
		int w, h;
		m_pVBox->m_pVideoWnd->GetClientSize(&w, &h);

		delete m_pImage;
		m_pImage = new wxImage(w, h);
	}
	m_pVBox->m_pVideoWnd->Refresh(false);
}

void CVideoBox::OnHScroll(wxScrollEvent& event)
{
	s64 SP = event.GetPosition();

	if (m_pMF->m_VIsOpen) 
	{
		s64 Cur, Pos, endPos;
		
		if (SP >= 0)
		{
			m_pMF->PauseVideo();
			
			Pos = SP;

			endPos = m_pMF->m_pVideo->m_Duration;
			Cur = m_pMF->m_pVideo->GetPos();

			if (Pos != Cur)
			{
				m_pMF->m_pVideo->SetPosFast(Pos);
			}
		}
		else
		{
			if (SP == -1)
			{
				m_pMF->PauseVideo();
				m_pMF->m_pVideo->OneStep();
			}
			else
			{
				m_pMF->PauseVideo();
				Cur = m_pMF->m_pVideo->GetPos();
				Cur -= m_pMF->m_dt;
				if (Cur < 0) Cur = 0;
				m_pMF->m_pVideo->SetPosFast(Cur);				
			}
		}
	}
}

void CVideoBox::OnTimer(wxTimerEvent& event)
{
	if (!m_pVBox->m_pVideoWnd->CheckFilterImage())
	{
		m_pVBox->m_pVideoWnd->Reparent(m_pVBox);

		wxSizeEvent event;
		m_pVBox->OnSize(event);

		m_timer.Stop();
	}
}

