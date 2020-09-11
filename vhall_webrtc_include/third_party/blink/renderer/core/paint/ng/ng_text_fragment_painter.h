// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_NG_NG_TEXT_FRAGMENT_PAINTER_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_NG_NG_TEXT_FRAGMENT_PAINTER_H_

#include "third_party/blink/renderer/core/style/computed_style_constants.h"
#include "third_party/blink/renderer/platform/geometry/layout_rect.h"
#include "third_party/blink/renderer/platform/wtf/allocator/allocator.h"

namespace blink {

class ComputedStyle;
class DisplayItemClient;
class LayoutObject;
class NGFragmentItem;
class NGFragmentItems;
class NGPaintFragment;
class NGPhysicalTextFragment;
struct NGTextFragmentPaintInfo;
struct PaintInfo;
struct PhysicalOffset;
struct PhysicalRect;
struct PhysicalSize;

// Text fragment painter for LayoutNG. Operates on NGPhysicalTextFragments and
// handles clipping, selection, etc. Delegates to NGTextPainter to paint the
// text itself.
class NGTextFragmentPainter {
  STACK_ALLOCATED();

 public:
  // TODO(kojii) : Remove | NGPaintFragment | once the transition is done.
  // crbug.com/982194
  explicit NGTextFragmentPainter(const NGPaintFragment&);
  NGTextFragmentPainter(const NGFragmentItem&, const NGFragmentItems&);

  void Paint(const PaintInfo&, const PhysicalOffset& paint_offset);

 private:
  void PaintItem(const PaintInfo&, const PhysicalOffset& paint_offset);

  void Paint(const NGTextFragmentPaintInfo& fragment_paint_info,
             const LayoutObject* layout_object,
             const DisplayItemClient& display_item_client,
             const ComputedStyle& style,
             PhysicalRect box_rect,
             const IntRect& visual_rect,
             bool is_ellipsis,
             bool is_symbol_marker,
             const PaintInfo& paint_info,
             const PhysicalOffset& paint_offset);

  static void PaintSymbol(const LayoutObject* layout_object,
                          const ComputedStyle& style,
                          const PhysicalSize box_size,
                          const PaintInfo& paint_info,
                          const PhysicalOffset& paint_offset);

  const NGPaintFragment* paint_fragment_ = nullptr;
  const NGPhysicalTextFragment* text_fragment_ = nullptr;

  const NGFragmentItem* item_ = nullptr;
  const NGFragmentItems* items_ = nullptr;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_PAINT_NG_NG_TEXT_FRAGMENT_PAINTER_H_
