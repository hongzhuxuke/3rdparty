/********************************************************************************
 Copyright (C) 2012 Hugh Bailey <obs.jim@gmail.com>

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
#include "VH_ConstDeff.h"

//-------------------------------------------------------------------

class BASE_EXPORT ImageSource {
public:
   
   virtual ~ImageSource() {interactiveResize=true;}
   virtual void Hide(bool bHide) {m_bHide = bHide;}
   virtual void SetDeviceExist(bool){};
   virtual bool Preprocess() {return true;}
   virtual void Tick(float fSeconds) { fSeconds; }
   virtual void Render(const Vect2 &pos, const Vect2 &size,SceneRenderType type) = 0;
   virtual void SetInteractiveResize(bool ok){interactiveResize=ok;}
   virtual bool GetInteractiveResize(){return interactiveResize;}
   virtual Vect2 GetSize() const = 0;
   virtual void Reload(){}
   virtual void UpdateSettings() {}

   virtual void BeginScene() {}
   virtual void EndScene() {}

   virtual void GlobalSourceLeaveScene() {}
   virtual void GlobalSourceEnterScene() {}

   virtual void SetFloat(CTSTR lpName, float fValue) { lpName; fValue; }
   virtual void SetInt(CTSTR lpName, int iValue) { lpName; iValue; }
   virtual void SetString(CTSTR lpName, CTSTR lpVal) { lpName; lpVal; }
   virtual void SetVector(CTSTR lpName, const Vect &value) { lpName; value; }
   virtual void SetVector2(CTSTR lpName, const Vect2 &value) { lpName; value; }
   virtual void SetVector4(CTSTR lpName, const Vect4 &value) { lpName; value; }
   virtual void SetMatrix(CTSTR lpName, const Matrix &mat) { lpName; mat; }

   inline  void SetColor(CTSTR lpName, const Color4 &value) { SetVector4(lpName, value); }
   inline  void SetColor(CTSTR lpName, float fR, float fB, float fG, float fA = 1.0f) { SetVector4(lpName, Color4(fR, fB, fG, fA)); }
   inline  void SetColor(CTSTR lpName, DWORD color) { SetVector4(lpName, RGBA_to_Vect4(color)); }

   inline  void SetColor3(CTSTR lpName, const Color3 &value) { SetVector(lpName, value); }
   inline  void SetColor3(CTSTR lpName, float fR, float fB, float fG) { SetVector(lpName, Color3(fR, fB, fG)); }
   inline  void SetColor3(CTSTR lpName, DWORD color) { SetVector(lpName, RGB_to_Vect(color)); }
   inline  void SetVector4(CTSTR lpName, float fX, float fY, float fZ, float fW) { SetVector4(lpName, Vect4(fX, fY, fZ, fW)); }
   inline  void SetVector(CTSTR lpName, float fX, float fY, float fZ) { SetVector(lpName, Vect(fX, fY, fZ)); }

   //-------------------------------------------------------------

   virtual bool GetFloat(CTSTR lpName, float &fValue)   const { lpName; fValue; return false; }
   virtual bool GetInt(CTSTR lpName, int &iValue)       const { lpName; iValue; return false; }
   virtual bool GetString(CTSTR lpName, String &strVal) const { lpName; strVal; return false; }
   virtual bool GetVector(CTSTR lpName, Vect &value)    const { lpName; value; return false; }
   virtual bool GetVector2(CTSTR lpName, Vect2 &value)  const { lpName; value; return false; }
   virtual bool GetVector4(CTSTR lpName, Vect4 &value)  const { lpName; value; return false; }
   virtual bool GetMatrix(CTSTR lpName, Matrix &mat)    const { lpName; mat; return false; }
   virtual bool GetIsHide() {return m_bHide;}
private:
   bool interactiveResize;
   bool m_bHide = false;
};


//====================================================================================

class BASE_EXPORT SceneItem {
   friend class Scene;
   friend class OBS;
   friend class CGraphics;

   Scene *parent = nullptr;

   ImageSource *source = nullptr;
   XElement *mpElement = nullptr;

   Vect2 startPos, startSize;
   Vect2 pos, size;
   Vect4 crop;

   bool bSelected = false;
   bool bRender = false;
   int mSourceType;

public:
   inline SceneItem() :mSourceType(-1) {}
   ~SceneItem();
   inline int GetSourceType() {
      return mSourceType;
   }
   inline void SetSourceType(int sourceType) {
      mSourceType = sourceType;
   }
   inline CTSTR GetName() const { return mpElement->GetName(); }
   inline ImageSource* GetSource() const { return source; }
   inline XElement* GetElement() const { return mpElement; }

   inline Vect2 GetPos() const { return pos; }
   inline Vect2 GetSize() const { return size; }
   inline Vect2 GetScale() const { return source ? (source->GetSize() / size) : Vect2(1.0f, 1.0f); }

   inline bool IsCropped() const { return !(CloseFloat(crop.w, 0.0f) && CloseFloat(crop.x, 0.0f) && CloseFloat(crop.y, 0.0f) && CloseFloat(crop.z, 0.0f)); }

   inline void SetPos(const Vect2 &newPos) { pos = newPos; }
   inline void SetSize(const Vect2 &newSize) { size = newSize; }

   inline void Select(bool bSelect) { bSelected = bSelect; }
   inline bool IsSelected() const { return bSelected; }

   inline UINT GetID();

   void SetName(CTSTR lpNewName);
   void SetRender(bool render);
   Vect4 GetCrop();
   Vect2 GetCropTL();
   Vect2 GetCropTR();
   Vect2 GetCropBR();
   Vect2 GetCropBL();

   void Update();

   void MoveUp();
   void MoveDown();
   void MoveToTop();
   void MoveToBottom();
};

//====================================================================================

class BASE_EXPORT Scene {
   friend class SceneItem;
   friend class OBS;

   bool bSceneStarted = false;
   bool bMissingSources = false;
   bool bIsSetSourceVisible = false;
   List<SceneItem*> sceneItems;
   DWORD bkColor;

   UINT hotkeyID = 0;
   bool mIsRestart = false;


public:
   Scene();
   virtual ~Scene();
   void SetIsRestart(bool isRestart);

   virtual SceneItem* InsertImageSource(UINT pos, XElement *sourceElement, SOURCE_TYPE sourceType);
   virtual SceneItem* AddImageSource(XElement *sourceElement);

   virtual void RemoveImageSource(SceneItem *item);
   void RemoveImageSourceSaveElement(SceneItem *item);
   void RemoveImageSource(CTSTR lpName);

   virtual void Tick(float fSeconds);
   virtual void Preprocess();
   virtual void Render(SceneRenderType);

   virtual void UpdateSettings() {}

   virtual void RenderSelections(Shader *solidPixelShader);

   virtual void BeginScene();
   virtual void EndScene();
   bool SetSourceVisible(wchar_t *sourceName,bool isVisible);
   bool FitItemsToScreen(SceneItem *item);

   //--------------------------------

   inline bool HasMissingSources() const { return bMissingSources; }

   inline void GetItemsOnPoint(const Vect2 &pos, List<SceneItem*> &items) const {
      items.Clear();

      for (int i = sceneItems.Num() - 1; i >= 0; i--) {
         SceneItem *item = sceneItems[i];
         int persistent = item->GetElement()->GetInt(TEXT("persistent"), 0);
         if(persistent==1)
         {
            continue;
         }
         Vect2 scale = (item->GetSource() ? item->GetSource()->GetSize() : item->GetSize()) / item->GetSize();

         Vect2 upperLeft = item->GetPos();
         upperLeft += item->GetCropTL();
         Vect2 lowerRight = upperLeft + item->GetSize();
         lowerRight += item->GetCropBR();
         if (item->bRender && pos.x >= upperLeft.x && pos.y >= upperLeft.y && pos.x <= lowerRight.x && pos.y <= lowerRight.y)
            items << item;
      }
   }

   inline void DeselectAll() {
      for (UINT i = 0; i < sceneItems.Num(); i++) {
         if (sceneItems[i]) {
            sceneItems[i]->bSelected = false;
         }
      }
   }
   //--------------------------------

   inline UINT NumSceneItems() const { return sceneItems.Num(); }
   inline SceneItem* GetSceneItem(UINT id) const { return sceneItems[id]; }

   inline SceneItem* GetSceneItem(CTSTR lpName) const {
      for (UINT i = 0; i < sceneItems.Num(); i++) {
         if (sceneItems[i] != NULL) {
            if (scmpi(sceneItems[i]->GetName(), lpName) == 0)
               return sceneItems[i];
         }
      }
      return NULL;
   }
   inline SceneItem *GetSceneItemBySourceType(int type) {
      for (UINT i = 0; i < sceneItems.Num(); i++) {
         SceneItem *item = sceneItems[i];
         if (item) {
            if (item->GetSourceType() == type) {
               return item;
            }
         }
      }
      return NULL;
   }
   inline void SceneHideSource(int type,bool bHide) {
      for (UINT i = 0; i < sceneItems.Num(); i++) {
         SceneItem *item = sceneItems[i];
         if (item) {
            if (item->GetSourceType() == type) {
               item->source->Hide(bHide);
            }
            item->mpElement->AddInt(L"sourceHide", bHide ? 1 : 0);
         }
      }
   }

   inline void GetSelectedItems(List<SceneItem*> &items) const {
      items.Clear();
      if (sceneItems.Num() > 0) {
         for (UINT i = 0; i < sceneItems.Num(); i++) {
            if (sceneItems[i] != NULL && sceneItems[i]->GetElement()) {
               int persistent = sceneItems[i]->GetElement()->GetInt(TEXT("persistent"), 0);
               if (sceneItems[i]->bSelected&&persistent == 0)
                  items << sceneItems[i];
            }
         }
      }
   }
   inline void GetAllItems(List<SceneItem*> &items) const {
      items.Clear();
      if (sceneItems.Num() > 0) {
         for (UINT i = 0; i < sceneItems.Num(); i++) {
            if (sceneItems[i] != NULL && sceneItems[i]->GetElement()) {
               int persistent = sceneItems[i]->GetElement()->GetInt(TEXT("persistent"), 0);
               if (persistent == 0)
                  items << sceneItems[i];
            }
         }
      }
   }
};


inline UINT SceneItem::GetID() {
   SceneItem *thisItem = this;
   return parent->sceneItems.FindValueIndex(thisItem);
}
