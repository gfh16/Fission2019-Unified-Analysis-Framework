// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME classDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
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

// The generated code does not explicitly qualifies STL entities
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "TBoard.h"
#include "TControl.h"
#include "TModV785.h"
#include "TModV785N.h"
#include "TModV775.h"
#include "TModV775N.h"
#include "TModV830AC.h"
#include "TModV792.h"
#include "TEvtBuilder.h"
#include "TClientEvtBuilder.h"
#include "TControlFrame.h"
#include "TDataFileBuilder.h"
#include "TMasterTask.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static void delete_TBoard(void *p);
   static void deleteArray_TBoard(void *p);
   static void destruct_TBoard(void *p);
   static void streamer_TBoard(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TBoard*)
   {
      ::TBoard *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TBoard >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TBoard", ::TBoard::Class_Version(), "TBoard.h", 21,
                  typeid(::TBoard), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TBoard::Dictionary, isa_proxy, 16,
                  sizeof(::TBoard) );
      instance.SetDelete(&delete_TBoard);
      instance.SetDeleteArray(&deleteArray_TBoard);
      instance.SetDestructor(&destruct_TBoard);
      instance.SetStreamerFunc(&streamer_TBoard);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TBoard*)
   {
      return GenerateInitInstanceLocal((::TBoard*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TBoard*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void delete_TControl(void *p);
   static void deleteArray_TControl(void *p);
   static void destruct_TControl(void *p);
   static void streamer_TControl(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TControl*)
   {
      ::TControl *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TControl >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TControl", ::TControl::Class_Version(), "TControl.h", 46,
                  typeid(::TControl), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TControl::Dictionary, isa_proxy, 16,
                  sizeof(::TControl) );
      instance.SetDelete(&delete_TControl);
      instance.SetDeleteArray(&deleteArray_TControl);
      instance.SetDestructor(&destruct_TControl);
      instance.SetStreamerFunc(&streamer_TControl);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TControl*)
   {
      return GenerateInitInstanceLocal((::TControl*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TControl*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_TModV785(void *p = 0);
   static void *newArray_TModV785(Long_t size, void *p);
   static void delete_TModV785(void *p);
   static void deleteArray_TModV785(void *p);
   static void destruct_TModV785(void *p);
   static void streamer_TModV785(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TModV785*)
   {
      ::TModV785 *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TModV785 >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TModV785", ::TModV785::Class_Version(), "TModV785.h", 17,
                  typeid(::TModV785), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TModV785::Dictionary, isa_proxy, 16,
                  sizeof(::TModV785) );
      instance.SetNew(&new_TModV785);
      instance.SetNewArray(&newArray_TModV785);
      instance.SetDelete(&delete_TModV785);
      instance.SetDeleteArray(&deleteArray_TModV785);
      instance.SetDestructor(&destruct_TModV785);
      instance.SetStreamerFunc(&streamer_TModV785);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TModV785*)
   {
      return GenerateInitInstanceLocal((::TModV785*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TModV785*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_TModV785N(void *p = 0);
   static void *newArray_TModV785N(Long_t size, void *p);
   static void delete_TModV785N(void *p);
   static void deleteArray_TModV785N(void *p);
   static void destruct_TModV785N(void *p);
   static void streamer_TModV785N(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TModV785N*)
   {
      ::TModV785N *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TModV785N >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TModV785N", ::TModV785N::Class_Version(), "TModV785N.h", 17,
                  typeid(::TModV785N), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TModV785N::Dictionary, isa_proxy, 16,
                  sizeof(::TModV785N) );
      instance.SetNew(&new_TModV785N);
      instance.SetNewArray(&newArray_TModV785N);
      instance.SetDelete(&delete_TModV785N);
      instance.SetDeleteArray(&deleteArray_TModV785N);
      instance.SetDestructor(&destruct_TModV785N);
      instance.SetStreamerFunc(&streamer_TModV785N);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TModV785N*)
   {
      return GenerateInitInstanceLocal((::TModV785N*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TModV785N*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_TModV775(void *p = 0);
   static void *newArray_TModV775(Long_t size, void *p);
   static void delete_TModV775(void *p);
   static void deleteArray_TModV775(void *p);
   static void destruct_TModV775(void *p);
   static void streamer_TModV775(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TModV775*)
   {
      ::TModV775 *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TModV775 >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TModV775", ::TModV775::Class_Version(), "TModV775.h", 17,
                  typeid(::TModV775), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TModV775::Dictionary, isa_proxy, 16,
                  sizeof(::TModV775) );
      instance.SetNew(&new_TModV775);
      instance.SetNewArray(&newArray_TModV775);
      instance.SetDelete(&delete_TModV775);
      instance.SetDeleteArray(&deleteArray_TModV775);
      instance.SetDestructor(&destruct_TModV775);
      instance.SetStreamerFunc(&streamer_TModV775);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TModV775*)
   {
      return GenerateInitInstanceLocal((::TModV775*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TModV775*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_TModV775N(void *p = 0);
   static void *newArray_TModV775N(Long_t size, void *p);
   static void delete_TModV775N(void *p);
   static void deleteArray_TModV775N(void *p);
   static void destruct_TModV775N(void *p);
   static void streamer_TModV775N(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TModV775N*)
   {
      ::TModV775N *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TModV775N >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TModV775N", ::TModV775N::Class_Version(), "TModV775N.h", 17,
                  typeid(::TModV775N), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TModV775N::Dictionary, isa_proxy, 16,
                  sizeof(::TModV775N) );
      instance.SetNew(&new_TModV775N);
      instance.SetNewArray(&newArray_TModV775N);
      instance.SetDelete(&delete_TModV775N);
      instance.SetDeleteArray(&deleteArray_TModV775N);
      instance.SetDestructor(&destruct_TModV775N);
      instance.SetStreamerFunc(&streamer_TModV775N);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TModV775N*)
   {
      return GenerateInitInstanceLocal((::TModV775N*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TModV775N*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_TModV830AC(void *p = 0);
   static void *newArray_TModV830AC(Long_t size, void *p);
   static void delete_TModV830AC(void *p);
   static void deleteArray_TModV830AC(void *p);
   static void destruct_TModV830AC(void *p);
   static void streamer_TModV830AC(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TModV830AC*)
   {
      ::TModV830AC *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TModV830AC >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TModV830AC", ::TModV830AC::Class_Version(), "TModV830AC.h", 17,
                  typeid(::TModV830AC), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TModV830AC::Dictionary, isa_proxy, 16,
                  sizeof(::TModV830AC) );
      instance.SetNew(&new_TModV830AC);
      instance.SetNewArray(&newArray_TModV830AC);
      instance.SetDelete(&delete_TModV830AC);
      instance.SetDeleteArray(&deleteArray_TModV830AC);
      instance.SetDestructor(&destruct_TModV830AC);
      instance.SetStreamerFunc(&streamer_TModV830AC);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TModV830AC*)
   {
      return GenerateInitInstanceLocal((::TModV830AC*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TModV830AC*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_TModV792(void *p = 0);
   static void *newArray_TModV792(Long_t size, void *p);
   static void delete_TModV792(void *p);
   static void deleteArray_TModV792(void *p);
   static void destruct_TModV792(void *p);
   static void streamer_TModV792(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TModV792*)
   {
      ::TModV792 *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TModV792 >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TModV792", ::TModV792::Class_Version(), "TModV792.h", 17,
                  typeid(::TModV792), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TModV792::Dictionary, isa_proxy, 16,
                  sizeof(::TModV792) );
      instance.SetNew(&new_TModV792);
      instance.SetNewArray(&newArray_TModV792);
      instance.SetDelete(&delete_TModV792);
      instance.SetDeleteArray(&deleteArray_TModV792);
      instance.SetDestructor(&destruct_TModV792);
      instance.SetStreamerFunc(&streamer_TModV792);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TModV792*)
   {
      return GenerateInitInstanceLocal((::TModV792*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TModV792*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static TClass *TEvtBuilder_Dictionary();
   static void TEvtBuilder_TClassManip(TClass*);
   static void delete_TEvtBuilder(void *p);
   static void deleteArray_TEvtBuilder(void *p);
   static void destruct_TEvtBuilder(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TEvtBuilder*)
   {
      ::TEvtBuilder *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::TEvtBuilder));
      static ::ROOT::TGenericClassInfo 
         instance("TEvtBuilder", "TEvtBuilder.h", 22,
                  typeid(::TEvtBuilder), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &TEvtBuilder_Dictionary, isa_proxy, 0,
                  sizeof(::TEvtBuilder) );
      instance.SetDelete(&delete_TEvtBuilder);
      instance.SetDeleteArray(&deleteArray_TEvtBuilder);
      instance.SetDestructor(&destruct_TEvtBuilder);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TEvtBuilder*)
   {
      return GenerateInitInstanceLocal((::TEvtBuilder*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TEvtBuilder*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *TEvtBuilder_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::TEvtBuilder*)0x0)->GetClass();
      TEvtBuilder_TClassManip(theClass);
   return theClass;
   }

   static void TEvtBuilder_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *TClientEvtBuilder_Dictionary();
   static void TClientEvtBuilder_TClassManip(TClass*);
   static void delete_TClientEvtBuilder(void *p);
   static void deleteArray_TClientEvtBuilder(void *p);
   static void destruct_TClientEvtBuilder(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TClientEvtBuilder*)
   {
      ::TClientEvtBuilder *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::TClientEvtBuilder));
      static ::ROOT::TGenericClassInfo 
         instance("TClientEvtBuilder", "TClientEvtBuilder.h", 21,
                  typeid(::TClientEvtBuilder), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &TClientEvtBuilder_Dictionary, isa_proxy, 0,
                  sizeof(::TClientEvtBuilder) );
      instance.SetDelete(&delete_TClientEvtBuilder);
      instance.SetDeleteArray(&deleteArray_TClientEvtBuilder);
      instance.SetDestructor(&destruct_TClientEvtBuilder);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TClientEvtBuilder*)
   {
      return GenerateInitInstanceLocal((::TClientEvtBuilder*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TClientEvtBuilder*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *TClientEvtBuilder_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::TClientEvtBuilder*)0x0)->GetClass();
      TClientEvtBuilder_TClassManip(theClass);
   return theClass;
   }

   static void TClientEvtBuilder_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static void delete_TControlFrame(void *p);
   static void deleteArray_TControlFrame(void *p);
   static void destruct_TControlFrame(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TControlFrame*)
   {
      ::TControlFrame *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TControlFrame >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TControlFrame", ::TControlFrame::Class_Version(), "TControlFrame.h", 46,
                  typeid(::TControlFrame), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TControlFrame::Dictionary, isa_proxy, 4,
                  sizeof(::TControlFrame) );
      instance.SetDelete(&delete_TControlFrame);
      instance.SetDeleteArray(&deleteArray_TControlFrame);
      instance.SetDestructor(&destruct_TControlFrame);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TControlFrame*)
   {
      return GenerateInitInstanceLocal((::TControlFrame*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TControlFrame*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void delete_TMasterTask(void *p);
   static void deleteArray_TMasterTask(void *p);
   static void destruct_TMasterTask(void *p);
   static void streamer_TMasterTask(TBuffer &buf, void *obj);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::TMasterTask*)
   {
      ::TMasterTask *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::TMasterTask >(0);
      static ::ROOT::TGenericClassInfo 
         instance("TMasterTask", ::TMasterTask::Class_Version(), "TMasterTask.h", 30,
                  typeid(::TMasterTask), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::TMasterTask::Dictionary, isa_proxy, 16,
                  sizeof(::TMasterTask) );
      instance.SetDelete(&delete_TMasterTask);
      instance.SetDeleteArray(&deleteArray_TMasterTask);
      instance.SetDestructor(&destruct_TMasterTask);
      instance.SetStreamerFunc(&streamer_TMasterTask);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::TMasterTask*)
   {
      return GenerateInitInstanceLocal((::TMasterTask*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::TMasterTask*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr TBoard::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TBoard::Class_Name()
{
   return "TBoard";
}

//______________________________________________________________________________
const char *TBoard::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TBoard*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TBoard::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TBoard*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TBoard::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TBoard*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TBoard::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TBoard*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr TControl::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TControl::Class_Name()
{
   return "TControl";
}

//______________________________________________________________________________
const char *TControl::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TControl*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TControl::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TControl*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TControl::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TControl*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TControl::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TControl*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr TModV785::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TModV785::Class_Name()
{
   return "TModV785";
}

//______________________________________________________________________________
const char *TModV785::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TModV785*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TModV785::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TModV785*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TModV785::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TModV785*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TModV785::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TModV785*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr TModV785N::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TModV785N::Class_Name()
{
   return "TModV785N";
}

//______________________________________________________________________________
const char *TModV785N::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TModV785N*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TModV785N::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TModV785N*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TModV785N::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TModV785N*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TModV785N::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TModV785N*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr TModV775::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TModV775::Class_Name()
{
   return "TModV775";
}

//______________________________________________________________________________
const char *TModV775::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TModV775*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TModV775::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TModV775*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TModV775::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TModV775*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TModV775::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TModV775*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr TModV775N::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TModV775N::Class_Name()
{
   return "TModV775N";
}

//______________________________________________________________________________
const char *TModV775N::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TModV775N*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TModV775N::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TModV775N*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TModV775N::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TModV775N*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TModV775N::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TModV775N*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr TModV830AC::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TModV830AC::Class_Name()
{
   return "TModV830AC";
}

//______________________________________________________________________________
const char *TModV830AC::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TModV830AC*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TModV830AC::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TModV830AC*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TModV830AC::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TModV830AC*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TModV830AC::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TModV830AC*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr TModV792::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TModV792::Class_Name()
{
   return "TModV792";
}

//______________________________________________________________________________
const char *TModV792::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TModV792*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TModV792::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TModV792*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TModV792::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TModV792*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TModV792::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TModV792*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr TControlFrame::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TControlFrame::Class_Name()
{
   return "TControlFrame";
}

//______________________________________________________________________________
const char *TControlFrame::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TControlFrame*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TControlFrame::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TControlFrame*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TControlFrame::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TControlFrame*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TControlFrame::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TControlFrame*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr TMasterTask::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *TMasterTask::Class_Name()
{
   return "TMasterTask";
}

//______________________________________________________________________________
const char *TMasterTask::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TMasterTask*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TMasterTask::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::TMasterTask*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *TMasterTask::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TMasterTask*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *TMasterTask::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::TMasterTask*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
void TBoard::Streamer(TBuffer &R__b)
{
   // Stream an object of class TBoard.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      R__b >> fGeo;
      R__b >> fCrateNum;
      R__b >> MaxChannel;
      R__b.CheckByteCount(R__s, R__c, TBoard::IsA());
   } else {
      R__c = R__b.WriteVersion(TBoard::IsA(), kTRUE);
      R__b << fGeo;
      R__b << fCrateNum;
      R__b << MaxChannel;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_TBoard(void *p) {
      delete ((::TBoard*)p);
   }
   static void deleteArray_TBoard(void *p) {
      delete [] ((::TBoard*)p);
   }
   static void destruct_TBoard(void *p) {
      typedef ::TBoard current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_TBoard(TBuffer &buf, void *obj) {
      ((::TBoard*)obj)->::TBoard::Streamer(buf);
   }
} // end of namespace ROOT for class ::TBoard

//______________________________________________________________________________
void TControl::Streamer(TBuffer &R__b)
{
   // Stream an object of class TControl.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TQObject::Streamer(R__b);
      R__b.CheckByteCount(R__s, R__c, TControl::IsA());
   } else {
      R__c = R__b.WriteVersion(TControl::IsA(), kTRUE);
      TQObject::Streamer(R__b);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_TControl(void *p) {
      delete ((::TControl*)p);
   }
   static void deleteArray_TControl(void *p) {
      delete [] ((::TControl*)p);
   }
   static void destruct_TControl(void *p) {
      typedef ::TControl current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_TControl(TBuffer &buf, void *obj) {
      ((::TControl*)obj)->::TControl::Streamer(buf);
   }
} // end of namespace ROOT for class ::TControl

//______________________________________________________________________________
void TModV785::Streamer(TBuffer &R__b)
{
   // Stream an object of class TModV785.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TBoard::Streamer(R__b);
      R__b.ReadStaticArray((unsigned short*)chdata);
      R__b.CheckByteCount(R__s, R__c, TModV785::IsA());
   } else {
      R__c = R__b.WriteVersion(TModV785::IsA(), kTRUE);
      TBoard::Streamer(R__b);
      R__b.WriteArray(chdata, 32);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TModV785(void *p) {
      return  p ? new(p) ::TModV785 : new ::TModV785;
   }
   static void *newArray_TModV785(Long_t nElements, void *p) {
      return p ? new(p) ::TModV785[nElements] : new ::TModV785[nElements];
   }
   // Wrapper around operator delete
   static void delete_TModV785(void *p) {
      delete ((::TModV785*)p);
   }
   static void deleteArray_TModV785(void *p) {
      delete [] ((::TModV785*)p);
   }
   static void destruct_TModV785(void *p) {
      typedef ::TModV785 current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_TModV785(TBuffer &buf, void *obj) {
      ((::TModV785*)obj)->::TModV785::Streamer(buf);
   }
} // end of namespace ROOT for class ::TModV785

//______________________________________________________________________________
void TModV785N::Streamer(TBuffer &R__b)
{
   // Stream an object of class TModV785N.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TBoard::Streamer(R__b);
      R__b.ReadStaticArray((unsigned short*)chdata);
      R__b.CheckByteCount(R__s, R__c, TModV785N::IsA());
   } else {
      R__c = R__b.WriteVersion(TModV785N::IsA(), kTRUE);
      TBoard::Streamer(R__b);
      R__b.WriteArray(chdata, 16);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TModV785N(void *p) {
      return  p ? new(p) ::TModV785N : new ::TModV785N;
   }
   static void *newArray_TModV785N(Long_t nElements, void *p) {
      return p ? new(p) ::TModV785N[nElements] : new ::TModV785N[nElements];
   }
   // Wrapper around operator delete
   static void delete_TModV785N(void *p) {
      delete ((::TModV785N*)p);
   }
   static void deleteArray_TModV785N(void *p) {
      delete [] ((::TModV785N*)p);
   }
   static void destruct_TModV785N(void *p) {
      typedef ::TModV785N current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_TModV785N(TBuffer &buf, void *obj) {
      ((::TModV785N*)obj)->::TModV785N::Streamer(buf);
   }
} // end of namespace ROOT for class ::TModV785N

//______________________________________________________________________________
void TModV775::Streamer(TBuffer &R__b)
{
   // Stream an object of class TModV775.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TBoard::Streamer(R__b);
      R__b.ReadStaticArray((unsigned short*)chdata);
      R__b.CheckByteCount(R__s, R__c, TModV775::IsA());
   } else {
      R__c = R__b.WriteVersion(TModV775::IsA(), kTRUE);
      TBoard::Streamer(R__b);
      R__b.WriteArray(chdata, 32);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TModV775(void *p) {
      return  p ? new(p) ::TModV775 : new ::TModV775;
   }
   static void *newArray_TModV775(Long_t nElements, void *p) {
      return p ? new(p) ::TModV775[nElements] : new ::TModV775[nElements];
   }
   // Wrapper around operator delete
   static void delete_TModV775(void *p) {
      delete ((::TModV775*)p);
   }
   static void deleteArray_TModV775(void *p) {
      delete [] ((::TModV775*)p);
   }
   static void destruct_TModV775(void *p) {
      typedef ::TModV775 current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_TModV775(TBuffer &buf, void *obj) {
      ((::TModV775*)obj)->::TModV775::Streamer(buf);
   }
} // end of namespace ROOT for class ::TModV775

//______________________________________________________________________________
void TModV775N::Streamer(TBuffer &R__b)
{
   // Stream an object of class TModV775N.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TBoard::Streamer(R__b);
      R__b.ReadStaticArray((unsigned short*)chdata);
      R__b.CheckByteCount(R__s, R__c, TModV775N::IsA());
   } else {
      R__c = R__b.WriteVersion(TModV775N::IsA(), kTRUE);
      TBoard::Streamer(R__b);
      R__b.WriteArray(chdata, 16);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TModV775N(void *p) {
      return  p ? new(p) ::TModV775N : new ::TModV775N;
   }
   static void *newArray_TModV775N(Long_t nElements, void *p) {
      return p ? new(p) ::TModV775N[nElements] : new ::TModV775N[nElements];
   }
   // Wrapper around operator delete
   static void delete_TModV775N(void *p) {
      delete ((::TModV775N*)p);
   }
   static void deleteArray_TModV775N(void *p) {
      delete [] ((::TModV775N*)p);
   }
   static void destruct_TModV775N(void *p) {
      typedef ::TModV775N current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_TModV775N(TBuffer &buf, void *obj) {
      ((::TModV775N*)obj)->::TModV775N::Streamer(buf);
   }
} // end of namespace ROOT for class ::TModV775N

//______________________________________________________________________________
void TModV830AC::Streamer(TBuffer &R__b)
{
   // Stream an object of class TModV830AC.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TBoard::Streamer(R__b);
      R__b.ReadStaticArray((unsigned int*)chdata);
      R__b.CheckByteCount(R__s, R__c, TModV830AC::IsA());
   } else {
      R__c = R__b.WriteVersion(TModV830AC::IsA(), kTRUE);
      TBoard::Streamer(R__b);
      R__b.WriteArray(chdata, 32);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TModV830AC(void *p) {
      return  p ? new(p) ::TModV830AC : new ::TModV830AC;
   }
   static void *newArray_TModV830AC(Long_t nElements, void *p) {
      return p ? new(p) ::TModV830AC[nElements] : new ::TModV830AC[nElements];
   }
   // Wrapper around operator delete
   static void delete_TModV830AC(void *p) {
      delete ((::TModV830AC*)p);
   }
   static void deleteArray_TModV830AC(void *p) {
      delete [] ((::TModV830AC*)p);
   }
   static void destruct_TModV830AC(void *p) {
      typedef ::TModV830AC current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_TModV830AC(TBuffer &buf, void *obj) {
      ((::TModV830AC*)obj)->::TModV830AC::Streamer(buf);
   }
} // end of namespace ROOT for class ::TModV830AC

//______________________________________________________________________________
void TModV792::Streamer(TBuffer &R__b)
{
   // Stream an object of class TModV792.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TBoard::Streamer(R__b);
      R__b.ReadStaticArray((unsigned short*)chdata);
      R__b.CheckByteCount(R__s, R__c, TModV792::IsA());
   } else {
      R__c = R__b.WriteVersion(TModV792::IsA(), kTRUE);
      TBoard::Streamer(R__b);
      R__b.WriteArray(chdata, 32);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_TModV792(void *p) {
      return  p ? new(p) ::TModV792 : new ::TModV792;
   }
   static void *newArray_TModV792(Long_t nElements, void *p) {
      return p ? new(p) ::TModV792[nElements] : new ::TModV792[nElements];
   }
   // Wrapper around operator delete
   static void delete_TModV792(void *p) {
      delete ((::TModV792*)p);
   }
   static void deleteArray_TModV792(void *p) {
      delete [] ((::TModV792*)p);
   }
   static void destruct_TModV792(void *p) {
      typedef ::TModV792 current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_TModV792(TBuffer &buf, void *obj) {
      ((::TModV792*)obj)->::TModV792::Streamer(buf);
   }
} // end of namespace ROOT for class ::TModV792

namespace ROOT {
   // Wrapper around operator delete
   static void delete_TEvtBuilder(void *p) {
      delete ((::TEvtBuilder*)p);
   }
   static void deleteArray_TEvtBuilder(void *p) {
      delete [] ((::TEvtBuilder*)p);
   }
   static void destruct_TEvtBuilder(void *p) {
      typedef ::TEvtBuilder current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TEvtBuilder

namespace ROOT {
   // Wrapper around operator delete
   static void delete_TClientEvtBuilder(void *p) {
      delete ((::TClientEvtBuilder*)p);
   }
   static void deleteArray_TClientEvtBuilder(void *p) {
      delete [] ((::TClientEvtBuilder*)p);
   }
   static void destruct_TClientEvtBuilder(void *p) {
      typedef ::TClientEvtBuilder current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TClientEvtBuilder

//______________________________________________________________________________
void TControlFrame::Streamer(TBuffer &R__b)
{
   // Stream an object of class TControlFrame.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(TControlFrame::Class(),this);
   } else {
      R__b.WriteClassBuffer(TControlFrame::Class(),this);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_TControlFrame(void *p) {
      delete ((::TControlFrame*)p);
   }
   static void deleteArray_TControlFrame(void *p) {
      delete [] ((::TControlFrame*)p);
   }
   static void destruct_TControlFrame(void *p) {
      typedef ::TControlFrame current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TControlFrame

//______________________________________________________________________________
void TMasterTask::Streamer(TBuffer &R__b)
{
   // Stream an object of class TMasterTask.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TControlFrame::Streamer(R__b);
      R__b.CheckByteCount(R__s, R__c, TMasterTask::IsA());
   } else {
      R__c = R__b.WriteVersion(TMasterTask::IsA(), kTRUE);
      TControlFrame::Streamer(R__b);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

namespace ROOT {
   // Wrapper around operator delete
   static void delete_TMasterTask(void *p) {
      delete ((::TMasterTask*)p);
   }
   static void deleteArray_TMasterTask(void *p) {
      delete [] ((::TMasterTask*)p);
   }
   static void destruct_TMasterTask(void *p) {
      typedef ::TMasterTask current_t;
      ((current_t*)p)->~current_t();
   }
   // Wrapper around a custom streamer member function.
   static void streamer_TMasterTask(TBuffer &buf, void *obj) {
      ((::TMasterTask*)obj)->::TMasterTask::Streamer(buf);
   }
} // end of namespace ROOT for class ::TMasterTask

namespace {
  void TriggerDictionaryInitialization_classDict_Impl() {
    static const char* headers[] = {
"TBoard.h",
"TControl.h",
"TModV785.h",
"TModV785N.h",
"TModV775.h",
"TModV775N.h",
"TModV830AC.h",
"TModV792.h",
"TEvtBuilder.h",
"TClientEvtBuilder.h",
"TControlFrame.h",
"TDataFileBuilder.h",
"TMasterTask.h",
0
    };
    static const char* includePaths[] = {
"/home/sea/Packages/root6.18/root_install/include",
"/home/sea/Fission2019-Unified-Analysis-Framework/DataConversion/RIBLLVMEDAQ/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "classDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_Autoloading_Map;
class __attribute__((annotate("$clingAutoload$TBoard.h")))  TBoard;
class __attribute__((annotate("$clingAutoload$TControl.h")))  TControl;
class __attribute__((annotate("$clingAutoload$TModV785.h")))  TModV785;
class __attribute__((annotate("$clingAutoload$TModV785N.h")))  TModV785N;
class __attribute__((annotate("$clingAutoload$TModV775.h")))  TModV775;
class __attribute__((annotate("$clingAutoload$TModV775N.h")))  TModV775N;
class __attribute__((annotate("$clingAutoload$TModV830AC.h")))  TModV830AC;
class __attribute__((annotate("$clingAutoload$TModV792.h")))  TModV792;
class __attribute__((annotate("$clingAutoload$TEvtBuilder.h")))  TEvtBuilder;
class __attribute__((annotate("$clingAutoload$TClientEvtBuilder.h")))  TClientEvtBuilder;
class __attribute__((annotate("$clingAutoload$TControlFrame.h")))  TControlFrame;
class __attribute__((annotate("$clingAutoload$TMasterTask.h")))  TMasterTask;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "classDict dictionary payload"

#ifndef LINUX
  #define LINUX 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "TBoard.h"
#include "TControl.h"
#include "TModV785.h"
#include "TModV785N.h"
#include "TModV775.h"
#include "TModV775N.h"
#include "TModV830AC.h"
#include "TModV792.h"
#include "TEvtBuilder.h"
#include "TClientEvtBuilder.h"
#include "TControlFrame.h"
#include "TDataFileBuilder.h"
#include "TMasterTask.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[]={
"TBoard", payloadCode, "@",
"TClientEvtBuilder", payloadCode, "@",
"TControl", payloadCode, "@",
"TControlFrame", payloadCode, "@",
"TEvtBuilder", payloadCode, "@",
"TMasterTask", payloadCode, "@",
"TModV775", payloadCode, "@",
"TModV775N", payloadCode, "@",
"TModV785", payloadCode, "@",
"TModV785N", payloadCode, "@",
"TModV792", payloadCode, "@",
"TModV830AC", payloadCode, "@",
nullptr};

    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("classDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_classDict_Impl, {}, classesHeaders, /*has no C++ module*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_classDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_classDict() {
  TriggerDictionaryInitialization_classDict_Impl();
}
