// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME deedict

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;
using namespace ROOT::Internal;

// Header files passed as explicit arguments
#include "MDEEFrame.h"
#include "MTree.h"
#include "MFitParam.h"
#include "MGraphFrame.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void delete_MDEEFrame(void *p);
   static void deleteArray_MDEEFrame(void *p);
   static void destruct_MDEEFrame(void *p);
   static void streamer_MDEEFrame(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MDEEFrame*)
   {
      ::MDEEFrame *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MDEEFrame >(0);
      static ::ROOT::TGenericClassInfo
         instance("MDEEFrame", ::MDEEFrame::Class_Version(), "MDEEFrame.h", 73,
                  typeid(::MDEEFrame), DefineBehavior(ptr, ptr),
                  &::MDEEFrame::Dictionary, isa_proxy, 16,
                  sizeof(::MDEEFrame) );
      instance.SetDelete(&delete_MDEEFrame);
      instance.SetDeleteArray(&deleteArray_MDEEFrame);
      instance.SetDestructor(&destruct_MDEEFrame);
      instance.SetStreamerFunc(&streamer_MDEEFrame);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MDEEFrame*)
   {
      return GenerateInitInstanceLocal((::MDEEFrame*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::MDEEFrame*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_MTree(void *p = 0);
   static void *newArray_MTree(Long_t size, void *p);
   static void delete_MTree(void *p);
   static void deleteArray_MTree(void *p);
   static void destruct_MTree(void *p);
   static void streamer_MTree(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MTree*)
   {
      ::MTree *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MTree >(0);
      static ::ROOT::TGenericClassInfo
         instance("MTree", ::MTree::Class_Version(), "MTree.h", 19,
                  typeid(::MTree), DefineBehavior(ptr, ptr),
                  &::MTree::Dictionary, isa_proxy, 16,
                  sizeof(::MTree) );
      instance.SetNew(&new_MTree);
      instance.SetNewArray(&newArray_MTree);
      instance.SetDelete(&delete_MTree);
      instance.SetDeleteArray(&deleteArray_MTree);
      instance.SetDestructor(&destruct_MTree);
      instance.SetStreamerFunc(&streamer_MTree);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MTree*)
   {
      return GenerateInitInstanceLocal((::MTree*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::MTree*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_MFitParam(void *p = 0);
   static void *newArray_MFitParam(Long_t size, void *p);
   static void delete_MFitParam(void *p);
   static void deleteArray_MFitParam(void *p);
   static void destruct_MFitParam(void *p);
   static void streamer_MFitParam(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MFitParam*)
   {
      ::MFitParam *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MFitParam >(0);
      static ::ROOT::TGenericClassInfo
         instance("MFitParam", ::MFitParam::Class_Version(), "MFitParam.h", 10,
                  typeid(::MFitParam), DefineBehavior(ptr, ptr),
                  &::MFitParam::Dictionary, isa_proxy, 16,
                  sizeof(::MFitParam) );
      instance.SetNew(&new_MFitParam);
      instance.SetNewArray(&newArray_MFitParam);
      instance.SetDelete(&delete_MFitParam);
      instance.SetDeleteArray(&deleteArray_MFitParam);
      instance.SetDestructor(&destruct_MFitParam);
      instance.SetStreamerFunc(&streamer_MFitParam);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MFitParam*)
   {
      return GenerateInitInstanceLocal((::MFitParam*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::MFitParam*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_MGraphFrame(void *p = 0);
   static void *newArray_MGraphFrame(Long_t size, void *p);
   static void delete_MGraphFrame(void *p);
   static void deleteArray_MGraphFrame(void *p);
   static void destruct_MGraphFrame(void *p);
   static void streamer_MGraphFrame(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::MGraphFrame*)
   {
      ::MGraphFrame *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::MGraphFrame >(0);
      static ::ROOT::TGenericClassInfo
         instance("MGraphFrame", ::MGraphFrame::Class_Version(), "MGraphFrame.h", 43,
                  typeid(::MGraphFrame), DefineBehavior(ptr, ptr),
                  &::MGraphFrame::Dictionary, isa_proxy, 16,
                  sizeof(::MGraphFrame) );
      instance.SetNew(&new_MGraphFrame);
      instance.SetNewArray(&newArray_MGraphFrame);
      instance.SetDelete(&delete_MGraphFrame);
      instance.SetDeleteArray(&deleteArray_MGraphFrame);
      instance.SetDestructor(&destruct_MGraphFrame);
      instance.SetStreamerFunc(&streamer_MGraphFrame);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::MGraphFrame*)
   {
      return GenerateInitInstanceLocal((::MGraphFrame*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstanceLocal((const ::MGraphFrame*)0x0); R__UseDummy(_R__UNIQUE_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr MDEEFrame::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *MDEEFrame::Class_Name()
{
   return "MDEEFrame";
}

//______________________________________________________________________________
const char *MDEEFrame::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MDEEFrame*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int MDEEFrame::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MDEEFrame*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MDEEFrame::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MDEEFrame*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MDEEFrame::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MDEEFrame*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr MTree::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *MTree::Class_Name()
{
   return "MTree";
}

//______________________________________________________________________________
const char *MTree::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MTree*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int MTree::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MTree*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MTree::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MTree*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MTree::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MTree*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr MFitParam::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *MFitParam::Class_Name()
{
   return "MFitParam";
}

//______________________________________________________________________________
const char *MFitParam::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MFitParam*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int MFitParam::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MFitParam*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MFitParam::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MFitParam*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MFitParam::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MFitParam*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr MGraphFrame::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *MGraphFrame::Class_Name()
{
   return "MGraphFrame";
}

//______________________________________________________________________________
const char *MGraphFrame::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MGraphFrame*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int MGraphFrame::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::MGraphFrame*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *MGraphFrame::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MGraphFrame*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *MGraphFrame::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD2(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::MGraphFrame*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void MDEEFrame::Streamer(TBuffer &R__b)
{
   // Stream an object of class MDEEFrame.

   TGMainFrame::Streamer(R__b);
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_MDEEFrame(void *p) {
      delete ((::MDEEFrame*)p);
   }
   static void deleteArray_MDEEFrame(void *p) {
      delete [] ((::MDEEFrame*)p);
   }
   static void destruct_MDEEFrame(void *p) {
      typedef ::MDEEFrame current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_MDEEFrame(TBuffer &buf, void *obj) {
      ((::MDEEFrame*)obj)->::MDEEFrame::Streamer(buf);
   }
} // end of namespace ROOT for class ::MDEEFrame

//______________________________________________________________________________
void MTree::Streamer(TBuffer &R__b)
{
   // Stream an object of class MTree.

   ::Error("MTree::Streamer", "version id <=0 in ClassDef, dummy Streamer() called"); if (R__b.IsReading()) { }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_MTree(void *p) {
      return  p ? new(p) ::MTree : new ::MTree;
   }
   static void *newArray_MTree(Long_t nElements, void *p) {
      return p ? new(p) ::MTree[nElements] : new ::MTree[nElements];
   }
   // Wrapper around operator delete
   static void delete_MTree(void *p) {
      delete ((::MTree*)p);
   }
   static void deleteArray_MTree(void *p) {
      delete [] ((::MTree*)p);
   }
   static void destruct_MTree(void *p) {
      typedef ::MTree current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_MTree(TBuffer &buf, void *obj) {
      ((::MTree*)obj)->::MTree::Streamer(buf);
   }
} // end of namespace ROOT for class ::MTree

//______________________________________________________________________________
void MFitParam::Streamer(TBuffer &R__b)
{
   // Stream an object of class MFitParam.

   ::Error("MFitParam::Streamer", "version id <=0 in ClassDef, dummy Streamer() called"); if (R__b.IsReading()) { }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_MFitParam(void *p) {
      return  p ? new(p) ::MFitParam : new ::MFitParam;
   }
   static void *newArray_MFitParam(Long_t nElements, void *p) {
      return p ? new(p) ::MFitParam[nElements] : new ::MFitParam[nElements];
   }
   // Wrapper around operator delete
   static void delete_MFitParam(void *p) {
      delete ((::MFitParam*)p);
   }
   static void deleteArray_MFitParam(void *p) {
      delete [] ((::MFitParam*)p);
   }
   static void destruct_MFitParam(void *p) {
      typedef ::MFitParam current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_MFitParam(TBuffer &buf, void *obj) {
      ((::MFitParam*)obj)->::MFitParam::Streamer(buf);
   }
} // end of namespace ROOT for class ::MFitParam

//______________________________________________________________________________
void MGraphFrame::Streamer(TBuffer &R__b)
{
   // Stream an object of class MGraphFrame.

   TGTransientFrame::Streamer(R__b);
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_MGraphFrame(void *p) {
      return  p ? new(p) ::MGraphFrame : new ::MGraphFrame;
   }
   static void *newArray_MGraphFrame(Long_t nElements, void *p) {
      return p ? new(p) ::MGraphFrame[nElements] : new ::MGraphFrame[nElements];
   }
   // Wrapper around operator delete
   static void delete_MGraphFrame(void *p) {
      delete ((::MGraphFrame*)p);
   }
   static void deleteArray_MGraphFrame(void *p) {
      delete [] ((::MGraphFrame*)p);
   }
   static void destruct_MGraphFrame(void *p) {
      typedef ::MGraphFrame current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_MGraphFrame(TBuffer &buf, void *obj) {
      ((::MGraphFrame*)obj)->::MGraphFrame::Streamer(buf);
   }
} // end of namespace ROOT for class ::MGraphFrame

namespace {
  void TriggerDictionaryInitialization_deedict_Impl() {
    static const char* headers[] = {
"MDEEFrame.h",
"MTree.h",
"MFitParam.h",
"MGraphFrame.h",
0
    };
/*    static const char* includePaths[] = {
"/mnt/misc/sw/x86_64/Debian/8/root/gnu/6.04.02/include/root",
"/projects/hira/Daniele/DEEFIT/",
0
}; */
     static const char* includePaths[] = {
"/home/sea/Packages/root6.18/root_install/include/ROOT",
"/home/sea/Fission2019-Unified-Analysis-Framework/Step4_SSD-Analysis-Framework/Step4.3_CsIEnergyCali/DEEFIT",
0
};
    static const char* fwdDeclCode =
R"DICTFWDDCLS(
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$MDEEFrame.h")))  MDEEFrame;
class __attribute__((annotate("$clingAutoload$MDEEFrame.h")))  MTree;
class __attribute__((annotate("$clingAutoload$MDEEFrame.h")))  MFitParam;
class __attribute__((annotate(R"ATTRDUMP(spectra selection dialog)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$MGraphFrame.h")))  MGraphFrame;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(

#ifndef G__VECTOR_HAS_CLASS_ITERATOR
  #define G__VECTOR_HAS_CLASS_ITERATOR 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
#include "MDEEFrame.h"
#include "MTree.h"
#include "MFitParam.h"
#include "MGraphFrame.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"MDEEFrame", payloadCode, "@",
"MFitParam", payloadCode, "@",
"MGraphFrame", payloadCode, "@",
"MTree", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("deedict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_deedict_Impl, {}, classesHeaders);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_deedict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_deedict() {
  TriggerDictionaryInitialization_deedict_Impl();
}
