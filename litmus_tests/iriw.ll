; ModuleID = 'iriw.cpp'
source_filename = "iriw.cpp"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

module asm ".globl _ZSt21ios_base_library_initv"

%"class.std::basic_ostream" = type { ptr, %"class.std::basic_ios" }
%"class.std::basic_ios" = type { %"class.std::ios_base", ptr, i8, i8, ptr, ptr, ptr, ptr }
%"class.std::ios_base" = type { ptr, i64, i64, i32, i32, i32, ptr, %"struct.std::ios_base::_Words", [8 x %"struct.std::ios_base::_Words"], i32, ptr, %"class.std::locale" }
%"struct.std::ios_base::_Words" = type { ptr, i64 }
%"class.std::locale" = type { ptr }
%"class.std::unique_ptr" = type { %"struct.std::__uniq_ptr_data" }
%"struct.std::__uniq_ptr_data" = type { %"class.std::__uniq_ptr_impl" }
%"class.std::__uniq_ptr_impl" = type { %"class.std::tuple" }
%"class.std::tuple" = type { %"struct.std::_Tuple_impl" }
%"struct.std::_Tuple_impl" = type { %"struct.std::_Head_base.1" }
%"struct.std::_Head_base.1" = type { ptr }
%"class.std::thread" = type { %"class.std::thread::id" }
%"class.std::thread::id" = type { i64 }

$_ZNSt6thread24_M_thread_deps_never_runEv = comdat any

$_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED0Ev = comdat any

$_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEE6_M_runEv = comdat any

$_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE = comdat any

$_ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE = comdat any

$_ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE = comdat any

@x = dso_local local_unnamed_addr global { i32 } zeroinitializer, align 4
@y = dso_local local_unnamed_addr global { i32 } zeroinitializer, align 4
@r1 = dso_local local_unnamed_addr global i32 0, align 4
@r2 = dso_local local_unnamed_addr global i32 0, align 4
@r3 = dso_local local_unnamed_addr global i32 0, align 4
@r4 = dso_local local_unnamed_addr global i32 0, align 4
@_ZSt4cout = external global %"class.std::basic_ostream", align 8
@.str = private unnamed_addr constant [4 x i8] c"r1=\00", align 1
@.str.1 = private unnamed_addr constant [5 x i8] c" r2=\00", align 1
@.str.2 = private unnamed_addr constant [5 x i8] c" r3=\00", align 1
@.str.3 = private unnamed_addr constant [5 x i8] c" r4=\00", align 1
@.str.4 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE = linkonce_odr dso_local unnamed_addr constant { [5 x ptr] } { [5 x ptr] [ptr null, ptr @_ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE, ptr @_ZNSt6thread6_StateD2Ev, ptr @_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED0Ev, ptr @_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEE6_M_runEv] }, comdat, align 8
@_ZTVN10__cxxabiv120__si_class_type_infoE = external global [0 x ptr]
@_ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE = linkonce_odr dso_local constant [59 x i8] c"NSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE\00", comdat, align 1
@_ZTINSt6thread6_StateE = external constant ptr
@_ZTINSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE = linkonce_odr dso_local constant { ptr, ptr, ptr } { ptr getelementptr inbounds (ptr, ptr @_ZTVN10__cxxabiv120__si_class_type_infoE, i64 2), ptr @_ZTSNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE, ptr @_ZTINSt6thread6_StateE }, comdat, align 8

; Function Attrs: mustprogress nofree norecurse nounwind sspstrong willreturn memory(readwrite, argmem: none, inaccessiblemem: none) uwtable
define dso_local void @_Z7writer1v() #0 {
  store atomic i32 1, ptr @x monotonic, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nounwind sspstrong willreturn memory(readwrite, argmem: none, inaccessiblemem: none) uwtable
define dso_local void @_Z7writer2v() #0 {
  store atomic i32 1, ptr @y monotonic, align 4
  ret void
}

; Function Attrs: mustprogress nofree norecurse nounwind sspstrong willreturn memory(readwrite, argmem: none, inaccessiblemem: none) uwtable
define dso_local void @_Z7reader1v() #0 {
  %1 = load atomic i32, ptr @x monotonic, align 4
  store i32 %1, ptr @r1, align 4, !tbaa !5
  %2 = load atomic i32, ptr @y monotonic, align 4
  store i32 %2, ptr @r2, align 4, !tbaa !5
  ret void
}

; Function Attrs: mustprogress nofree norecurse nounwind sspstrong willreturn memory(readwrite, argmem: none, inaccessiblemem: none) uwtable
define dso_local void @_Z7reader2v() #0 {
  %1 = load atomic i32, ptr @y monotonic, align 4
  store i32 %1, ptr @r3, align 4, !tbaa !5
  %2 = load atomic i32, ptr @x monotonic, align 4
  store i32 %2, ptr @r4, align 4, !tbaa !5
  ret void
}

; Function Attrs: mustprogress norecurse sspstrong uwtable
define dso_local noundef i32 @main() local_unnamed_addr #1 personality ptr @__gxx_personality_v0 {
  %1 = alloca %"class.std::unique_ptr", align 8
  %2 = alloca %"class.std::unique_ptr", align 8
  %3 = alloca %"class.std::unique_ptr", align 8
  %4 = alloca %"class.std::unique_ptr", align 8
  %5 = alloca %"class.std::thread", align 8
  %6 = alloca %"class.std::thread", align 8
  %7 = alloca %"class.std::thread", align 8
  %8 = alloca %"class.std::thread", align 8
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %5) #11
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %4)
  store i64 0, ptr %5, align 8, !tbaa !9
  %9 = tail call noalias noundef nonnull dereferenceable(16) ptr @_Znwm(i64 noundef 16) #12
  store ptr getelementptr inbounds inrange(-16, 24) (i8, ptr @_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE, i64 16), ptr %9, align 8, !tbaa !12
  %10 = getelementptr inbounds i8, ptr %9, i64 8
  store ptr @_Z7writer1v, ptr %10, align 8, !tbaa !14
  store ptr %9, ptr %4, align 8, !tbaa !17
  invoke void @_ZNSt6thread15_M_start_threadESt10unique_ptrINS_6_StateESt14default_deleteIS1_EEPFvvE(ptr noundef nonnull align 8 dereferenceable(8) %5, ptr noundef nonnull %4, ptr noundef nonnull @_ZNSt6thread24_M_thread_deps_never_runEv)
          to label %11 unwind label %18

11:                                               ; preds = %0
  %12 = load ptr, ptr %4, align 8, !tbaa !17
  %13 = icmp eq ptr %12, null
  br i1 %13, label %28, label %14

14:                                               ; preds = %11
  %15 = load ptr, ptr %12, align 8, !tbaa !12
  %16 = getelementptr inbounds i8, ptr %15, i64 8
  %17 = load ptr, ptr %16, align 8
  call void %17(ptr noundef nonnull align 8 dereferenceable(8) %12) #11
  br label %28

18:                                               ; preds = %0
  %19 = landingpad { ptr, i32 }
          cleanup
  %20 = load ptr, ptr %4, align 8, !tbaa !17
  %21 = icmp eq ptr %20, null
  br i1 %21, label %26, label %22

22:                                               ; preds = %18
  %23 = load ptr, ptr %20, align 8, !tbaa !12
  %24 = getelementptr inbounds i8, ptr %23, i64 8
  %25 = load ptr, ptr %24, align 8
  call void %25(ptr noundef nonnull align 8 dereferenceable(8) %20) #11
  br label %26

26:                                               ; preds = %18, %22, %154
  %27 = phi { ptr, i32 } [ %150, %154 ], [ %19, %22 ], [ %19, %18 ]
  resume { ptr, i32 } %27

28:                                               ; preds = %11, %14
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %4)
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %6) #11
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %3)
  store i64 0, ptr %6, align 8, !tbaa !9
  %29 = invoke noalias noundef nonnull dereferenceable(16) ptr @_Znwm(i64 noundef 16) #12
          to label %30 unwind label %128

30:                                               ; preds = %28
  store ptr getelementptr inbounds inrange(-16, 24) (i8, ptr @_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE, i64 16), ptr %29, align 8, !tbaa !12
  %31 = getelementptr inbounds i8, ptr %29, i64 8
  store ptr @_Z7writer2v, ptr %31, align 8, !tbaa !14
  store ptr %29, ptr %3, align 8, !tbaa !17
  invoke void @_ZNSt6thread15_M_start_threadESt10unique_ptrINS_6_StateESt14default_deleteIS1_EEPFvvE(ptr noundef nonnull align 8 dereferenceable(8) %6, ptr noundef nonnull %3, ptr noundef nonnull @_ZNSt6thread24_M_thread_deps_never_runEv)
          to label %32 unwind label %39

32:                                               ; preds = %30
  %33 = load ptr, ptr %3, align 8, !tbaa !17
  %34 = icmp eq ptr %33, null
  br i1 %34, label %47, label %35

35:                                               ; preds = %32
  %36 = load ptr, ptr %33, align 8, !tbaa !12
  %37 = getelementptr inbounds i8, ptr %36, i64 8
  %38 = load ptr, ptr %37, align 8
  call void %38(ptr noundef nonnull align 8 dereferenceable(8) %33) #11
  br label %47

39:                                               ; preds = %30
  %40 = landingpad { ptr, i32 }
          cleanup
  %41 = load ptr, ptr %3, align 8, !tbaa !17
  %42 = icmp eq ptr %41, null
  br i1 %42, label %149, label %43

43:                                               ; preds = %39
  %44 = load ptr, ptr %41, align 8, !tbaa !12
  %45 = getelementptr inbounds i8, ptr %44, i64 8
  %46 = load ptr, ptr %45, align 8
  call void %46(ptr noundef nonnull align 8 dereferenceable(8) %41) #11
  br label %149

47:                                               ; preds = %35, %32
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %3)
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %7) #11
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %2)
  store i64 0, ptr %7, align 8, !tbaa !9
  %48 = invoke noalias noundef nonnull dereferenceable(16) ptr @_Znwm(i64 noundef 16) #12
          to label %49 unwind label %130

49:                                               ; preds = %47
  store ptr getelementptr inbounds inrange(-16, 24) (i8, ptr @_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE, i64 16), ptr %48, align 8, !tbaa !12
  %50 = getelementptr inbounds i8, ptr %48, i64 8
  store ptr @_Z7reader1v, ptr %50, align 8, !tbaa !14
  store ptr %48, ptr %2, align 8, !tbaa !17
  invoke void @_ZNSt6thread15_M_start_threadESt10unique_ptrINS_6_StateESt14default_deleteIS1_EEPFvvE(ptr noundef nonnull align 8 dereferenceable(8) %7, ptr noundef nonnull %2, ptr noundef nonnull @_ZNSt6thread24_M_thread_deps_never_runEv)
          to label %51 unwind label %58

51:                                               ; preds = %49
  %52 = load ptr, ptr %2, align 8, !tbaa !17
  %53 = icmp eq ptr %52, null
  br i1 %53, label %66, label %54

54:                                               ; preds = %51
  %55 = load ptr, ptr %52, align 8, !tbaa !12
  %56 = getelementptr inbounds i8, ptr %55, i64 8
  %57 = load ptr, ptr %56, align 8
  call void %57(ptr noundef nonnull align 8 dereferenceable(8) %52) #11
  br label %66

58:                                               ; preds = %49
  %59 = landingpad { ptr, i32 }
          cleanup
  %60 = load ptr, ptr %2, align 8, !tbaa !17
  %61 = icmp eq ptr %60, null
  br i1 %61, label %144, label %62

62:                                               ; preds = %58
  %63 = load ptr, ptr %60, align 8, !tbaa !12
  %64 = getelementptr inbounds i8, ptr %63, i64 8
  %65 = load ptr, ptr %64, align 8
  call void %65(ptr noundef nonnull align 8 dereferenceable(8) %60) #11
  br label %144

66:                                               ; preds = %54, %51
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %2)
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %8) #11
  call void @llvm.lifetime.start.p0(i64 8, ptr nonnull %1)
  store i64 0, ptr %8, align 8, !tbaa !9
  %67 = invoke noalias noundef nonnull dereferenceable(16) ptr @_Znwm(i64 noundef 16) #12
          to label %68 unwind label %132

68:                                               ; preds = %66
  store ptr getelementptr inbounds inrange(-16, 24) (i8, ptr @_ZTVNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEEE, i64 16), ptr %67, align 8, !tbaa !12
  %69 = getelementptr inbounds i8, ptr %67, i64 8
  store ptr @_Z7reader2v, ptr %69, align 8, !tbaa !14
  store ptr %67, ptr %1, align 8, !tbaa !17
  invoke void @_ZNSt6thread15_M_start_threadESt10unique_ptrINS_6_StateESt14default_deleteIS1_EEPFvvE(ptr noundef nonnull align 8 dereferenceable(8) %8, ptr noundef nonnull %1, ptr noundef nonnull @_ZNSt6thread24_M_thread_deps_never_runEv)
          to label %70 unwind label %77

70:                                               ; preds = %68
  %71 = load ptr, ptr %1, align 8, !tbaa !17
  %72 = icmp eq ptr %71, null
  br i1 %72, label %85, label %73

73:                                               ; preds = %70
  %74 = load ptr, ptr %71, align 8, !tbaa !12
  %75 = getelementptr inbounds i8, ptr %74, i64 8
  %76 = load ptr, ptr %75, align 8
  call void %76(ptr noundef nonnull align 8 dereferenceable(8) %71) #11
  br label %85

77:                                               ; preds = %68
  %78 = landingpad { ptr, i32 }
          cleanup
  %79 = load ptr, ptr %1, align 8, !tbaa !17
  %80 = icmp eq ptr %79, null
  br i1 %80, label %139, label %81

81:                                               ; preds = %77
  %82 = load ptr, ptr %79, align 8, !tbaa !12
  %83 = getelementptr inbounds i8, ptr %82, i64 8
  %84 = load ptr, ptr %83, align 8
  call void %84(ptr noundef nonnull align 8 dereferenceable(8) %79) #11
  br label %139

85:                                               ; preds = %73, %70
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %1)
  invoke void @_ZNSt6thread4joinEv(ptr noundef nonnull align 8 dereferenceable(8) %5)
          to label %86 unwind label %134

86:                                               ; preds = %85
  invoke void @_ZNSt6thread4joinEv(ptr noundef nonnull align 8 dereferenceable(8) %6)
          to label %87 unwind label %134

87:                                               ; preds = %86
  invoke void @_ZNSt6thread4joinEv(ptr noundef nonnull align 8 dereferenceable(8) %7)
          to label %88 unwind label %134

88:                                               ; preds = %87
  invoke void @_ZNSt6thread4joinEv(ptr noundef nonnull align 8 dereferenceable(8) %8)
          to label %89 unwind label %134

89:                                               ; preds = %88
  %90 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, ptr noundef nonnull @.str, i64 noundef 3)
          to label %91 unwind label %134

91:                                               ; preds = %89
  %92 = load i32, ptr @r1, align 4, !tbaa !5
  %93 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) @_ZSt4cout, i32 noundef %92)
          to label %94 unwind label %134

94:                                               ; preds = %91
  %95 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l(ptr noundef nonnull align 8 dereferenceable(8) %93, ptr noundef nonnull @.str.1, i64 noundef 4)
          to label %96 unwind label %134

96:                                               ; preds = %94
  %97 = load i32, ptr @r2, align 4, !tbaa !5
  %98 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) %93, i32 noundef %97)
          to label %99 unwind label %134

99:                                               ; preds = %96
  %100 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l(ptr noundef nonnull align 8 dereferenceable(8) %98, ptr noundef nonnull @.str.2, i64 noundef 4)
          to label %101 unwind label %134

101:                                              ; preds = %99
  %102 = load i32, ptr @r3, align 4, !tbaa !5
  %103 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) %98, i32 noundef %102)
          to label %104 unwind label %134

104:                                              ; preds = %101
  %105 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l(ptr noundef nonnull align 8 dereferenceable(8) %103, ptr noundef nonnull @.str.3, i64 noundef 4)
          to label %106 unwind label %134

106:                                              ; preds = %104
  %107 = load i32, ptr @r4, align 4, !tbaa !5
  %108 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8) %103, i32 noundef %107)
          to label %109 unwind label %134

109:                                              ; preds = %106
  %110 = invoke noundef nonnull align 8 dereferenceable(8) ptr @_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l(ptr noundef nonnull align 8 dereferenceable(8) %108, ptr noundef nonnull @.str.4, i64 noundef 1)
          to label %111 unwind label %134

111:                                              ; preds = %109
  %112 = load i64, ptr %8, align 8, !tbaa !18
  %113 = icmp eq i64 %112, 0
  br i1 %113, label %115, label %114

114:                                              ; preds = %111
  call void @_ZSt9terminatev() #13
  unreachable

115:                                              ; preds = %111
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %8) #11
  %116 = load i64, ptr %7, align 8, !tbaa !18
  %117 = icmp eq i64 %116, 0
  br i1 %117, label %119, label %118

118:                                              ; preds = %115
  call void @_ZSt9terminatev() #13
  unreachable

119:                                              ; preds = %115
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %7) #11
  %120 = load i64, ptr %6, align 8, !tbaa !18
  %121 = icmp eq i64 %120, 0
  br i1 %121, label %123, label %122

122:                                              ; preds = %119
  call void @_ZSt9terminatev() #13
  unreachable

123:                                              ; preds = %119
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %6) #11
  %124 = load i64, ptr %5, align 8, !tbaa !18
  %125 = icmp eq i64 %124, 0
  br i1 %125, label %127, label %126

126:                                              ; preds = %123
  call void @_ZSt9terminatev() #13
  unreachable

127:                                              ; preds = %123
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %5) #11
  ret i32 0

128:                                              ; preds = %28
  %129 = landingpad { ptr, i32 }
          cleanup
  br label %149

130:                                              ; preds = %47
  %131 = landingpad { ptr, i32 }
          cleanup
  br label %144

132:                                              ; preds = %66
  %133 = landingpad { ptr, i32 }
          cleanup
  br label %139

134:                                              ; preds = %109, %104, %99, %94, %89, %106, %101, %96, %91, %88, %87, %86, %85
  %135 = landingpad { ptr, i32 }
          cleanup
  %136 = load i64, ptr %8, align 8, !tbaa !18
  %137 = icmp eq i64 %136, 0
  br i1 %137, label %139, label %138

138:                                              ; preds = %134
  call void @_ZSt9terminatev() #13
  unreachable

139:                                              ; preds = %134, %132, %81, %77
  %140 = phi { ptr, i32 } [ %133, %132 ], [ %78, %81 ], [ %78, %77 ], [ %135, %134 ]
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %8) #11
  %141 = load i64, ptr %7, align 8, !tbaa !18
  %142 = icmp eq i64 %141, 0
  br i1 %142, label %144, label %143

143:                                              ; preds = %139
  call void @_ZSt9terminatev() #13
  unreachable

144:                                              ; preds = %139, %130, %62, %58
  %145 = phi { ptr, i32 } [ %131, %130 ], [ %59, %62 ], [ %59, %58 ], [ %140, %139 ]
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %7) #11
  %146 = load i64, ptr %6, align 8, !tbaa !18
  %147 = icmp eq i64 %146, 0
  br i1 %147, label %149, label %148

148:                                              ; preds = %144
  call void @_ZSt9terminatev() #13
  unreachable

149:                                              ; preds = %144, %128, %43, %39
  %150 = phi { ptr, i32 } [ %129, %128 ], [ %40, %43 ], [ %40, %39 ], [ %145, %144 ]
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %6) #11
  %151 = load i64, ptr %5, align 8, !tbaa !18
  %152 = icmp eq i64 %151, 0
  br i1 %152, label %154, label %153

153:                                              ; preds = %149
  call void @_ZSt9terminatev() #13
  unreachable

154:                                              ; preds = %149
  call void @llvm.lifetime.end.p0(i64 8, ptr nonnull %5) #11
  br label %26
}

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.start.p0(i64 immarg, ptr nocapture) #2

declare i32 @__gxx_personality_v0(...)

declare void @_ZNSt6thread4joinEv(ptr noundef nonnull align 8 dereferenceable(8)) local_unnamed_addr #3

declare noundef nonnull align 8 dereferenceable(8) ptr @_ZNSolsEi(ptr noundef nonnull align 8 dereferenceable(8), i32 noundef) local_unnamed_addr #3

; Function Attrs: mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite)
declare void @llvm.lifetime.end.p0(i64 immarg, ptr nocapture) #2

; Function Attrs: cold noreturn nounwind
declare void @_ZSt9terminatev() local_unnamed_addr #4

declare void @_ZNSt6thread15_M_start_threadESt10unique_ptrINS_6_StateESt14default_deleteIS1_EEPFvvE(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef, ptr noundef) local_unnamed_addr #3

; Function Attrs: nobuiltin allocsize(0)
declare noundef nonnull ptr @_Znwm(i64 noundef) local_unnamed_addr #5

; Function Attrs: nobuiltin nounwind
declare void @_ZdlPvm(ptr noundef, i64 noundef) local_unnamed_addr #6

; Function Attrs: mustprogress nounwind sspstrong uwtable
define linkonce_odr dso_local void @_ZNSt6thread24_M_thread_deps_never_runEv() #7 comdat align 2 {
  ret void
}

; Function Attrs: nounwind
declare void @_ZNSt6thread6_StateD2Ev(ptr noundef nonnull align 8 dereferenceable(8)) unnamed_addr #8

; Function Attrs: inlinehint mustprogress nounwind sspstrong uwtable
define linkonce_odr dso_local void @_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEED0Ev(ptr noundef nonnull align 8 dereferenceable(16) %0) unnamed_addr #9 comdat align 2 {
  tail call void @_ZNSt6thread6_StateD2Ev(ptr noundef nonnull align 8 dereferenceable(16) %0) #11
  tail call void @_ZdlPvm(ptr noundef nonnull %0, i64 noundef 16) #14
  ret void
}

; Function Attrs: mustprogress sspstrong uwtable
define linkonce_odr dso_local void @_ZNSt6thread11_State_implINS_8_InvokerISt5tupleIJPFvvEEEEEE6_M_runEv(ptr noundef nonnull align 8 dereferenceable(16) %0) unnamed_addr #10 comdat align 2 {
  %2 = getelementptr inbounds i8, ptr %0, i64 8
  %3 = load ptr, ptr %2, align 8, !tbaa !17
  tail call void %3()
  ret void
}

declare noundef nonnull align 8 dereferenceable(8) ptr @_ZSt16__ostream_insertIcSt11char_traitsIcEERSt13basic_ostreamIT_T0_ES6_PKS3_l(ptr noundef nonnull align 8 dereferenceable(8), ptr noundef, i64 noundef) local_unnamed_addr #3

attributes #0 = { mustprogress nofree norecurse nounwind sspstrong willreturn memory(readwrite, argmem: none, inaccessiblemem: none) uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { mustprogress norecurse sspstrong uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #2 = { mustprogress nocallback nofree nosync nounwind willreturn memory(argmem: readwrite) }
attributes #3 = { "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #4 = { cold noreturn nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #5 = { nobuiltin allocsize(0) "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #6 = { nobuiltin nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #7 = { mustprogress nounwind sspstrong uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #8 = { nounwind "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #9 = { inlinehint mustprogress nounwind sspstrong uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #10 = { mustprogress sspstrong uwtable "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #11 = { nounwind }
attributes #12 = { builtin allocsize(0) }
attributes #13 = { cold noreturn nounwind }
attributes #14 = { builtin nounwind }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{!"clang version 19.1.7"}
!5 = !{!6, !6, i64 0}
!6 = !{!"int", !7, i64 0}
!7 = !{!"omnipotent char", !8, i64 0}
!8 = !{!"Simple C++ TBAA"}
!9 = !{!10, !11, i64 0}
!10 = !{!"_ZTSNSt6thread2idE", !11, i64 0}
!11 = !{!"long", !7, i64 0}
!12 = !{!13, !13, i64 0}
!13 = !{!"vtable pointer", !8, i64 0}
!14 = !{!15, !16, i64 0}
!15 = !{!"_ZTSSt10_Head_baseILm0EPFvvELb0EE", !16, i64 0}
!16 = !{!"any pointer", !7, i64 0}
!17 = !{!16, !16, i64 0}
!18 = !{!11, !11, i64 0}
