#ifndef H_CSINGLETON_H
#define H_CSINGLETON_H



#include "CriticalSection.h"

template< typename T >
//单例模式
class CSingleton
{
public:
	static T& Instance();

private:
	static void MakeInstance();
	static void DestroySingleton();

	static T* m_pInstance;

private:
	CSingleton();
	CSingleton(const CSingleton&);

	//临界区
	static CCriticalSection  m_oCriSection;
};

template<class T> CCriticalSection CSingleton<T>::m_oCriSection;
template<class T> T* CSingleton<T>::m_pInstance = 0;

template <class T> 
T& CSingleton<T>::Instance()
{
	if ( !m_pInstance)
	{
		MakeInstance();
	}

	return *m_pInstance;
}

template <class T>
inline void CSingleton<T>::MakeInstance()
{
	CCriticalAutoLock loGuard(m_oCriSection);

	if(!m_pInstance)
	{
		m_pInstance = new T;
		::atexit(DestroySingleton);
	}
}

template <class T>
void CSingleton<T>::DestroySingleton()
{
	delete m_pInstance;
	m_pInstance = 0;
}
#endif // !H_CSINGLETON_H