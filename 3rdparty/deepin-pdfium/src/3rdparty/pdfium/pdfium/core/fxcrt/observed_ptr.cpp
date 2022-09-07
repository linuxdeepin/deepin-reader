// Copyright 2019 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "core/fxcrt/observed_ptr.h"

#include "third_party/base/stl_util.h"

namespace fxcrt {

Observable::Observable() = default;

Observable::~Observable() {
  NotifyObservers();
}

void Observable::AddObserver(ObserverIface* pObserver) {
  ASSERT(!pdfium::Contains(m_Observers, pObserver));
  m_Observers.insert(pObserver);
}

void Observable::RemoveObserver(ObserverIface* pObserver) {
  ASSERT(pdfium::Contains(m_Observers, pObserver));
  m_Observers.erase(pObserver);
}

void Observable::NotifyObservers() {
  for (auto* pObserver : m_Observers)
    pObserver->OnObservableDestroyed();
  m_Observers.clear();
}

}  // namespace fxcrt
