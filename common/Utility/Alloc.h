/********************************************************************************
 Copyright (C) 2001-2012 Hugh Bailey <obs.jim@gmail.com>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
********************************************************************************/


#pragma once

//========================================================
//Allocation class
class BASE_EXPORT Alloc
{
public:
    virtual ~Alloc()  {}

    virtual void * __restrict _Allocate(size_t dwSize)=0;
    virtual void * _ReAllocate(LPVOID lpData, size_t dwSize)=0;
    virtual void   _Free(LPVOID lpData)=0;
    virtual void   ErrorTermination()=0;

    inline void  *operator new(size_t dwSize)
    {
        return malloc(dwSize);
    }

    inline void operator delete(void *lpData)
    {
        free(lpData);
    }

#ifdef _DEBUG
    inline void *operator new(size_t dwSize, TCHAR *lpFile, unsigned int lpLine)
    {
         lpFile;
         lpLine;
         return malloc(dwSize);
    }

    inline void operator delete(void *lpData, TCHAR *lpFile, unsigned int lpLine)
    {
         lpFile;
         lpLine;
         free(lpData);
    }
#endif

};

__declspec(dllexport) void *Allocate(size_t);
__declspec(dllexport) void *ReAllocate(void *,size_t);
__declspec(dllexport) void Free(void *lpData) ;

//inline void* operator new(size_t dwSize)
//{
//    void* val = Allocate(dwSize);
//    zero(val, dwSize);
//
//    return val;
//}
//
//inline void operator delete(void* lpData)
//{
//    Free(lpData);
//}
//
//inline void* operator new[](size_t dwSize)
//{
//    void* val = Allocate(dwSize);
//    zero(val, dwSize);
//
//    return val;
//}
//
//inline void operator delete[](void* lpData)
//{
//    Free(lpData);
//}
//