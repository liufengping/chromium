// Copyright (c) 2009 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/views/browser_bubble.h"

#include "app/l10n_util_win.h"
#include "chrome/browser/views/frame/browser_view.h"
#include "views/widget/root_view.h"
#include "views/widget/widget_win.h"
#include "views/window/window.h"

class BubbleWidget : public views::WidgetWin
{
public:
  BubbleWidget(BrowserBubble* bubble) : bubble_(bubble), closed_(false) {
  }

  void Show(bool activate) {
    if (activate)
      ShowWindow(SW_SHOW);
    else
      views::WidgetWin::Show();
  }

  void Close() {
    if (closed_)
      return;
    closed_ = true;
    views::WidgetWin::Close();
  }

  void OnActivate(UINT action, BOOL minimized, HWND window) {
    BrowserBubble::Delegate* delegate = bubble_->delegate();
    if (!delegate)
      return;

    if (action == WA_INACTIVE && !closed_) {
      delegate->BubbleLostFocus(bubble_);
    } else if (action == WA_ACTIVE) {
      delegate->BubbleGotFocus(bubble_);
    }
  }

private:
  bool closed_;
  BrowserBubble* bubble_;
};

void BrowserBubble::InitPopup() {
  gfx::NativeWindow native_window = frame_->GetWindow()->GetNativeWindow();

  // popup_ is a Widget, but we need to do some WidgetWin stuff first, then
  // we'll assign it into popup_.
  views::WidgetWin* pop = new BubbleWidget(this);
  pop->set_window_style(WS_POPUP);

#if 0
  // TODO(erikkay) Layered windows don't draw child windows.
  // Apparently there's some tricks you can do to handle that.
  // Do the research so we can use this.
  pop->set_window_ex_style(WS_EX_LAYERED |
                           l10n_util::GetExtendedTooltipStyles());
  pop->SetOpacity(0xFF);
#endif

  // A focus manager is necessary if you want to be able to handle various
  // mouse events properly.
  pop->Init(frame_native_view_, bounds_);
  pop->SetContentsView(view_);

  popup_ = pop;
  Reposition();
  AttachToBrowser();
}

void BrowserBubble::MovePopup(int x, int y, int w, int h) {
  views::WidgetWin* pop = static_cast<views::WidgetWin*>(popup_);
  pop->MoveWindow(x, y, w, h);
}

void BrowserBubble::Show(bool activate) {
  if (visible_)
    return;
  BubbleWidget* pop = static_cast<BubbleWidget*>(popup_);
  pop->Show(activate);
  visible_ = true;
}

void BrowserBubble::Hide() {
  if (!visible_)
    return;
  views::WidgetWin* pop = static_cast<views::WidgetWin*>(popup_);
  pop->Hide();
  visible_ = false;
}
