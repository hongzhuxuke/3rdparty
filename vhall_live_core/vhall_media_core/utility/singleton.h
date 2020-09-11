//
//  Singleton.hpp
//  VhallLiveApi
//
//  Created by ilong on 2017/9/28.
//  Copyright © 2017年 vhall. All rights reserved.
//

#ifndef Singleton_h
#define Singleton_h

#include <stdio.h>

#ifndef WIN32
#include <pthread.h>

template <typename T>
class Singleton {
   
public:
   static T& instance(){
      pthread_once(&mPonce,&Singleton::Init);
      return mValue;
   }
private:
   Singleton(){};
   ~Singleton(){};
   Singleton(const Singleton& )=delete;//禁用copy方法
   const Singleton& operator=( const Singleton& )=delete;//禁用赋值方法
   static void Init(){
      mValue = new T();
   }
private:
   static pthread_once_t mPonce;
   static T* mValue;
};

template <typename T>
pthread_once_t Singleton<T>::mPonce = PTHREAD_ONCE_INIT;

template <typename T>
T * Singleton<T>::mValue = NULL;

#endif
//Instructions for use
//Foo * foo = Singleton<Foo>::Instance();

#endif /* Singleton_hpp */
