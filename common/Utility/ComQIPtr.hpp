#pragma once
#include "ComPtr.hpp"
template<class T> class ComQIPtr : public ComPtr<T> {

public:
	inline ComQIPtr(IUnknown *unk)
	{
		this->ptr = nullptr;
		unk->QueryInterface(__uuidof(T), (void**)&this->ptr);
	}

	inline ComPtr<T> &operator=(IUnknown *unk)
	{
		ComPtr<T>::Clear();
		unk->QueryInterface(__uuidof(T), (void**)&this->ptr);
		return *this;
	}
};

