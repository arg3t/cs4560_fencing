; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso < %s | FileCheck %s -check-prefix=tso
; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso,fence-opt < %s | FileCheck %s -check-prefix=opt

@x = global i32 0, align 4   ; message variable
@y = global i32 0, align 4   ; flag variable
@r1 = global i32 0, align 4  ; record flag read by consumer
@r2 = global i32 0, align 4  ; record message read by consumer

define void @thread1() {
; tso-LABEL: define void @thread1
; opt-LABEL: define void @thread1
; tso-NEXT: fence seq_cst
  store atomic i32 1, ptr @x monotonic, align 4
; tso-NEXT: store atomic i32 1, ptr @x monotonic,
; opt-NEXT: store atomic i32 1, ptr @x monotonic,
; tso-NEXT: fence seq_cst
; tso-NEXT: fence seq_cst
; opt-NEXT: fence seq_cst
; opt-NOT: fence seq_cst
  store atomic i32 1, ptr @y monotonic, align 4
; tso: store atomic i32 1, ptr @y monotonic,
; opt: store atomic i32 1, ptr @y monotonic,
  ret void
; tso: ret void
; opt: ret void
}

define void @thread2() {
; tso-LABEL: define void @thread2
; opt-LABEL: define void @thread2
; tso-NEXT: fence seq_cst
  %flag = load atomic i32, ptr @y monotonic, align 4
; tso-NEXT: %flag = load atomic i32, ptr @y monotonic, align 4
; opt-NEXT: %flag = load atomic i32, ptr @y monotonic, align 4
; tso-NEXT: fence seq_cst
; tso-NEXT: fence seq_cst
; opt-NEXT: fence seq_cst
; opt-NOT: fence seq_cst
  %msg = load atomic i32, ptr @x monotonic, align 4
; tso-NEXT: load atomic i32, ptr @x monotonic,
; opt-NEXT: load atomic i32, ptr @x monotonic,
  store i32 %flag, ptr @r1, align 4
; tso-NEXT: store i32 %flag, ptr @r1, align 4
; opt-NEXT: store i32 %flag, ptr @r1, align 4
  store i32 %msg, ptr @r2, align 4
; tso: store i32 %msg, ptr @r2, align 4
; opt: store i32 %msg, ptr @r2, align 4
  ret void
; tso: ret void
; opt: ret void
}
