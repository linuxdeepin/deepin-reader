// Copyright 2017 PDFium Authors. All rights reserved.
// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXJS_JS_RESOURCES_H_
#define FXJS_JS_RESOURCES_H_

#include "core/fxcrt/widestring.h"

enum class JSMessage {
  kAlert = 1,
  kParamError,
  kInvalidInputError,
  kParamTooLongError,
  kParseDateError,
  kRangeBetweenError,
  kRangeGreaterError,
  kRangeLessError,
  kNotSupportedError,
  kBusyError,
  kDuplicateEventError,
  kSecondParamNotDateError,
  kSecondParamInvalidDateError,
  kGlobalNotFoundError,
  kReadOnlyError,
  kTypeError,
  kValueError,
  kPermissionError,
  kBadObjectError,
  kObjectTypeError,
  kUnknownProperty,
  kInvalidSetError,
  kUserGestureRequiredError,
  kTooManyOccurances,
  kUnknownMethod,
};

WideString JSGetStringFromID(JSMessage msg);
WideString JSFormatErrorString(const char* class_name,
                               const char* property_name,
                               const WideString& details);

#endif  // FXJS_JS_RESOURCES_H_
