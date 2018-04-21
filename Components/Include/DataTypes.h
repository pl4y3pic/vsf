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

#include <time.h>
#include <memory.h>
#include <wx/wx.h>

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned __int64	u64;
typedef __int64	            s64;

template <typename T>
class custom_buffer
{
public:
	T *m_pData;
	int m_size;
	bool m_need_to_release;

	~custom_buffer()
	{
		if (m_need_to_release)
		{
			delete[] m_pData;			
		}
		m_pData = NULL;
		m_size = 0;
		m_need_to_release = false;
	}

	custom_buffer(int size)
	{
		wxASSERT_MSG(size > 0, "custom_buffer(int size): not: size > 0");

		m_pData = new T[size];
		m_size = size;
		m_need_to_release = true;
	}

	custom_buffer(int size, T val)
	{
		wxASSERT_MSG(size > 0, "custom_buffer(int size, T val): not: size > 0");

		m_pData = new T[size];
		m_size = size;
		m_need_to_release = true;
		
		for (int i = 0; i < m_size; i++)
		{
			m_pData[i] = val;
		}		
	}

	custom_buffer()
	{
		m_pData = NULL;
		m_size = 0;
		m_need_to_release = false;
	}

	custom_buffer(T *pData, int size)
	{
		m_pData = pData;
		m_size = size;
		m_need_to_release = false;
	}

	custom_buffer(const custom_buffer<T> &obj)
	{
		m_size = obj.m_size;
		m_need_to_release = obj.m_need_to_release;

		if (m_need_to_release)
		{
			m_pData = new T[m_size];
			for (int i = 0; i < m_size; i++)
			{
				m_pData[i] = obj.m_pData[i];
			}
		}
		else
		{
			m_pData = obj.m_pData;
		}
	}

	custom_buffer<T>& operator= (const custom_buffer<T> &obj)
	{
		if (m_need_to_release)
		{
			delete[] m_pData;
		}

		m_size = obj.m_size;
		m_need_to_release = obj.m_need_to_release;

		if (m_need_to_release)
		{
			m_pData = new T[m_size];
			for (int i = 0; i < m_size; i++)
			{
				m_pData[i] = obj.m_pData[i];
			}
		}
		else
		{
			m_pData = obj.m_pData;
		}

		return *this;
	}

	custom_buffer<T> get_sub_buffer(int offset)
	{
		wxASSERT_MSG(offset < m_size, "get_sub_buffer(int offset): not: offset < m_size");

		return custom_buffer<T>(m_pData + offset, m_size - offset);
	}

	inline int size()
	{
		return m_size;
	}

	inline T& operator[](int idx)
	{
		wxASSERT_MSG(idx >= 0, "operator[](int idx): not: idx >= 0");
		wxASSERT_MSG(idx < m_size, "operator[](int idx): not: idx < m_size");

		return m_pData[idx];
	}
};

template<>
custom_buffer<int>::custom_buffer(int size, int val)
{
	wxASSERT_MSG(size > 0, "custom_buffer(int size, T val): not: size > 0");
	m_pData = new int[size];
	m_size = size;
	m_need_to_release = true;

	if (val == 0)
	{
		memset(m_pData, 0, m_size*sizeof(int));
	}
	else
	{
		for (int i = 0; i < m_size; i++)
		{
			m_pData[i] = val;
		}
	}
}

template<>
custom_buffer<s64>::custom_buffer(int size, s64 val)
{
	wxASSERT_MSG(size > 0, "custom_buffer(int size, T val): not: size > 0");
	m_pData = new s64[size];
	m_size = size;
	m_need_to_release = true;

	if (val == 0)
	{
		memset(m_pData, 0, m_size*sizeof(s64));
	}
	else
	{
		for (int i = 0; i < m_size; i++)
		{
			m_pData[i] = val;
		}
	}
}

template<>
custom_buffer<int>& custom_buffer<int>::operator= (const custom_buffer<int> &obj)
{
	if (m_need_to_release)
	{
		delete[] m_pData;
	}

	m_size = obj.m_size;
	m_need_to_release = obj.m_need_to_release;

	if (m_need_to_release)
	{
		m_pData = new int[m_size];
		memcpy(m_pData, obj.m_pData, m_size*sizeof(int));
	}
	else
	{
		m_pData = obj.m_pData;
	}

	return *this;
}

template<>
custom_buffer<s64>& custom_buffer<s64>::operator= (const custom_buffer<s64> &obj)
{
	if (m_need_to_release)
	{
		delete[] m_pData;
	}

	m_size = obj.m_size;
	m_need_to_release = obj.m_need_to_release;

	if (m_need_to_release)
	{
		m_pData = new s64[m_size];
		memcpy(m_pData, obj.m_pData, m_size*sizeof(s64));
	}
	else
	{
		m_pData = obj.m_pData;
	}

	return *this;
}