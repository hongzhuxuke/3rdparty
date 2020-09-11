#ifndef __CR_PLUGIN_DEFF__H_INCLUDE__
#define __CR_PLUGIN_DEFF__H_INCLUDE__

#ifndef INITGUID
#define INITGUID
#endif

#pragma once
#include <Guiddef.h>
#include <initguid.h>

//------------------------------------------------------BasePlug----------------------------------------------------------

// MainUI插件
// {8352D856-A907-4293-BEF5-C827FAB4B245}
DEFINE_GUID(PID_IMainUI,
            0x8352d856, 0xa907, 0x4293, 0xbe, 0xf5, 0xc8, 0x27, 0xfa, 0xb4, 0xb2, 0x45);

// CommonToolKit插件
// {92EF96A0-76CA-4A73-B7CD-8CE275E51239}
DEFINE_GUID(PID_ICommonToolKit,
                    0x92ef96a0, 0x76ca, 0x4a73, 0xb7, 0xcd, 0x8c, 0xe2, 0x75, 0xe5, 0x12, 0x39);

// OBSControl插件
// {E4F940BF-5E8E-4443-B59A-80635D5B89B5}
DEFINE_GUID(PID_IOBSControl,
            0xe4f940bf, 0x5e8e, 0x4443, 0xb5, 0x9a, 0x80, 0x63, 0x5d, 0x5b, 0x89, 0xb5);

//VHallRightExtraWidget插件
//{c1071c82 - 2a3e-4a61 - 8caa - bc00fc2e7d27}
DEFINE_GUID(PID_IVhallRightExtraWidget,
            0xc1071c82, 0x2a3e, 0x4a61, 0x8c, 0xaa, 0xbc, 0x00, 0xfc, 0x2e, 0x7d, 0x27);
#endif // __CR_PLUGIN_DEFF__H_INCLUDE__