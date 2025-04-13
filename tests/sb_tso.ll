; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso < %s | FileCheck %s -check-prefix=tso
; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso,fence-opt < %s | FileCheck %s -check-prefix=opt

@x = global i32 0, align 4
@y = global i32 0, align 4
@r1 = global i32 0, align 4
@r2 = global i32 0, align 4

define void @thread1() {
; tso-LABEL: define void @thread1
; opt-LABEL: define void @thread1
; tso-NEXT: fence seq_cst
  store atomic i32 1, ptr @x monotonic, align 4
; tso: store atomic i32 1, ptr @x monotonic, align 4
; opt: store atomic i32 1, ptr @x monotonic, align 4
  %ly = load atomic i32, ptr @y monotonic, align 4
; tso-NEXT: %ly = load atomic i32, ptr @y monotonic, align 4
; opt-NEXT: %ly = load atomic i32, ptr @y monotonic, align 4
; opt-NEXT: fence seq_cst
; opt-NOT: fence seq_cst
  store i32 %ly, ptr @r1, align 4
; tso-NEXT: store i32 %ly, ptr @r1, align 4
; opt-NEXT: store i32 %ly, ptr @r1, align 4
  ret void
; tso: ret void
; opt: ret void
}

define void @thread2() {
; tso-LABEL: define void @thread2
; opt-LABEL: define void @thread2
; tso-NEXT: fence seq_cst
  store atomic i32 1, ptr @y monotonic, align 4
; tso: store atomic i32 1, ptr @y monotonic,
; opt: store atomic i32 1, ptr @y monotonic,
  %lx = load atomic i32, ptr @x monotonic, align 4
; tso-NEXT: %lx = load atomic i32, ptr @x monotonic, align 4
; opt-NEXT: %lx = load atomic i32, ptr @x monotonic, align 4
; opt-NEXT: fence seq_cst
; opt-NOT: fence seq_cst
  store i32 %lx, ptr @r2, align 4
; tso: store i32 %lx, ptr @r2, align 4
; opt: store i32 %lx, ptr @r2, align 4
  ret void
; tso: ret void
; opt: ret void
}
