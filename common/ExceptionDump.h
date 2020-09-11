#ifndef __EXCEPTION_DUMP_H_
#define __EXCEPTION_DUMP_H_

#include <Windows.h>
#include <DbgHelp.h>

#pragma comment(lib, "DbgHelp.lib")

class CExceptionDump
{
public:
    CExceptionDump(void);
    ~CExceptionDump(void);

private:
    //��ʼ��
    void Init();

    //����ʼ��
    void UnInit();

private:
    LPTOP_LEVEL_EXCEPTION_FILTER m_pSystemFilter;
};

#endif //__EXCEPTION_DUMP_H_