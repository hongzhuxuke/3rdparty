#ifndef __VH_ERROR__H_INCLUDED__
#define __VH_ERROR__H_INCLUDED__

#pragma once
#include "VH_Macro.h"

//�ɹ�
#define CRE_OK								((HRESULT)0x00000000L)

//ʧ��
#define CRE_FALSE							((HRESULT)0x00000001L)

//���õ�
#define CRE_ENABLE							((HRESULT)0x00000002L)

//���õ�
#define CRE_DISABLE							((HRESULT)0x00000003L)

//�ɼ�
#define CRE_VISIBLE 						((HRESULT)0x00000004L)

//���ɼ�
#define CRE_NOTVISIBLE 						((HRESULT)0x00000005L)

//ֻ��
#define CRE_ONLYREAD	 					((HRESULT)0x00000006L)

//��д��
#define CRE_READWRITE	 					((HRESULT)0x00000007L)

//�Ƿ�����
#define CRE_INVALIDARG 						((HRESULT)0x00000101L)

//ָ���Ľӿ�û��ʵ��
#define CRE_NOINTERFACE 					((HRESULT)0x00000102L)

//Ŀ��û���ҵ�
#define CRE_OBJECT_NOTFOUND 				((HRESULT)0x00000103L)

//Ŀ���Ѵ���
#define CRE_OBJECT_ALREADY_EXISTS 			((HRESULT)0x00000104L)

//δ��ʼ��
#define CRE_OBJECT_UNINITIALIZED  			((HRESULT)0x00000105L)

//�ѳ�ʼ��
#define CRE_OBJECT_ALREADY_INITIALIZED 		((HRESULT)0x00000106L)

//δ��
#define CRE_NO_BINDINGS						((HRESULT)0x00000107L)

//����û��ʵ��
#define CRE_NOTIMPL							((HRESULT)0x00000108L)

//����δע��
#define CRE_OBJECT_UNREGISTERED  			((HRESULT)0x00000109L)

//������ע��
#define CRE_OBJECT_ALREADY_REGISTERED	 	((HRESULT)0x00000110L)

//�����ڴ�ʧ��
#define CRE_MEMORY_ALLOCATION_FAILED	 	((HRESULT)0x00000111L)

//�ڴ���� 
#define CRE_OUTOFMEMORY				 		((HRESULT)0x00000112L)

//��ʱ
#define CRE_TIMEOUT			 				((HRESULT)0x00000113L)

//δ�ҵ�
#define CRE_NOTFOUND			 			((HRESULT)0x00000114L)

#endif //__VH_ERROR__H_INCLUDED__