//
// Copyright (c) 2014-present, Facebook, Inc.
// All rights reserved.
//
// This source code is licensed under the University of Illinois/NCSA Open
// Source License found in the LICENSE file in the root directory of this
// source tree. An additional grant of patent rights can be found in the
// PATENTS file in the same directory.
//

#ifndef __DebugServer2_GDBRemote_Types_h
#define __DebugServer2_GDBRemote_Types_h

#include "DebugServer2/Architecture/RegisterLayout.h"
#include "DebugServer2/GDBRemote/Base.h"
#include "DebugServer2/Types.h"
#include "JSObjects/JSObjects.h"

#include <set>

namespace ds2 {
namespace GDBRemote {

struct ProcessThreadId : public ds2::ProcessThreadId {
  ProcessThreadId(ProcessId pid = kAnyProcessId, ThreadId tid = kAnyThreadId)
      : ds2::ProcessThreadId(pid, tid) {}
  bool parse(std::string const &string, CompatibilityMode mode);
  std::string encode(CompatibilityMode mode) const;
};

struct MemoryRegionInfo : public ds2::MemoryRegionInfo {
  std::string encode() const;
};

struct StopInfo : public ds2::StopInfo {
  ProcessThreadId ptid;
  std::string threadName;
  Architecture::GPRegisterStopMap registers;
  std::set<ThreadId> threads;

public:
  std::string encode(CompatibilityMode mode) const;
  std::string encodeWithAllThreads(CompatibilityMode mode,
                                   const JSArray &threadsStopInfo) const;
  JSDictionary *encodeJson() const;
  JSDictionary *encodeBriefJson() const;

private:
  std::string reasonToString() const;
  std::string encodeInfo(CompatibilityMode mode) const;
  void encodeRegisters(std::map<std::string, std::string> &regs,
                       bool hexIndex) const;
  std::string encodeRegisters() const;
};

enum ResumeAction {
  kResumeActionInvalid,

  kResumeActionSingleStep,
  kResumeActionSingleStepWithSignal,
  kResumeActionSingleStepCycle,
  kResumeActionSingleStepCycleWithSignal,
  kResumeActionContinue,
  kResumeActionContinueWithSignal,
  kResumeActionBackwardStep,
  kResumeActionBackwardContinue,
  kResumeActionStop
};

struct ThreadResumeAction {
  typedef std::vector<ThreadResumeAction> Collection;

  ProcessThreadId ptid;
  ResumeAction action;
  Address address;
  int signal;
  uint32_t ncycles;

  ThreadResumeAction() : action(kResumeActionInvalid), signal(0), ncycles(0) {}
};

struct Feature {
  typedef std::vector<Feature> Collection;

  enum Flag { kNotSupported, kSupported, kQuerySupported };

  std::string name;
  std::string value;
  Flag flag;

  Feature() : flag(kNotSupported) {}

  Feature(std::string const &string) : flag(kNotSupported) { parse(string); }

  bool parse(std::string const &string);
};

struct RegisterInfo {
  enum Encoding {
    kEncodingNone,
    kEncodingUInt,
    kEncodingSInt,
    kEncodingIEEE754,
    kEncodingVector
  };

  enum Format {
    kFormatNone,
    kFormatBinary,
    kFormatDecimal,
    kFormatHex,
    kFormatFloat,
    kFormatVectorUInt8,
    kFormatVectorSInt8,
    kFormatVectorUInt16,
    kFormatVectorSInt16,
    kFormatVectorUInt32,
    kFormatVectorSInt32,
    kFormatVectorUInt128,
    kFormatVectorFloat32
  };

  std::string setName;
  std::string registerName;
  std::string alternateName;
  std::string genericName;
  size_t bitSize;
  ssize_t byteOffset;
  ssize_t gccRegisterIndex;
  ssize_t dwarfRegisterIndex;
  uint32_t regno;
  Encoding encoding;
  Format format;
  std::vector<uint32_t> containerRegisters;
  std::vector<uint32_t> invalidateRegisters;

  RegisterInfo()
      : bitSize(0), byteOffset(-1), gccRegisterIndex(-1),
        encoding(kEncodingNone), format(kFormatNone) {}

  // xmlSet is the index of this set of registers in the
  // list of register groups. If < 0, don't encode as xml
  std::string encode(int xmlSet = -1) const;
};

struct HostInfo : public ds2::HostInfo {
  bool watchpointExceptionsReceivedBefore;

  HostInfo() : ds2::HostInfo(), watchpointExceptionsReceivedBefore(false) {}

  std::string encode() const;
};

struct ProcessInfo : public ds2::ProcessInfo {
  ProcessInfo() : ds2::ProcessInfo() {}
  std::string encode(CompatibilityMode mode,
                     bool alternateVersion = false) const;
};

struct ProcessInfoMatch : public ProcessInfo {
  std::string nameMatch;
  std::string triple;
  bool allUsers;
  StringCollection keys;
};

struct ServerVersion {
  std::string name;
  std::string version;
  std::string patchLevel;
  std::string releaseName;
  uint32_t majorVersion;
  uint32_t minorVersion;
  uint32_t buildNumber;

  std::string encode() const;
};

struct ProgramResult {
  int status; // exit code
  int signal;
  std::string output;

  ProgramResult() { clear(); }

  inline void clear() {
    status = 0;
    signal = 0;
    output.clear();
  }

  std::string encode() const;
};

template <class T> struct IterationState {
  std::vector<T> vals;
  typename std::vector<T>::iterator it;
};
}
}

#endif // !__DebugServer2_GDBRemote_Types_h
