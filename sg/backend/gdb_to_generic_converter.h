// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SG_BACKEND_GDB_TO_GENERIC_CONVERTER_H_H
#define SG_BACKEND_GDB_TO_GENERIC_CONVERTER_H_H

#include "sg/backend/backend.h"
#include "sg/backend/gdb_mi_parse.h"

std::string FindStringValue(
    const std::string& key,
    const std::vector<GdbRecordResult*>& results);

const base::DictionaryValue* FindDictionaryValue(
    const std::string& key,
    const std::vector<GdbRecordResult*>& results);

StoppedAtBreakpointData StoppedAtBreakpointDataFromRecordResults(
    const std::vector<GdbRecordResult*>& results);

#endif  // SG_BACKEND_GDB_TO_GENERIC_CONVERTER_H_H
