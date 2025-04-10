
; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-pso < %s | FileCheck %s -check-prefix=pso
; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-pso,fence-opt < %s | FileCheck %s -check-prefix=opt

@x = global i32 0, align 4
@y = global i32 0, align 4
@r1 = global i32 0, align 4
@r2 = global i32 0, align 4

define void @thread1() {
; pso-LABEL: define void @thread1
; opt-LABEL: define void @thread1
  store atomic i32 1, ptr @x monotonic, align 4
; pso: store atomic i32 1, ptr @x monotonic,
; opt: store atomic i32 1, ptr @x monotonic,
  %ly = load atomic i32, ptr @y monotonic, align 4
; pso-NEXT: %ly = load atomic i32, ptr @y monotonic, align 4
; opt-NEXT: %ly = load atomic i32, ptr @y monotonic, align 4
; pso-NEXT: fence seq_cst
; pso-NEXT: fence seq_cst
; opt-NEXT: fence seq_cst
; opt-NOT: fence seq_cst
  store i32 %ly, ptr @r1, align 4
; pso: store i32 %ly, ptr @r1, align 4
; opt: store i32 %ly, ptr @r1, align 4
  ret void
; pso: ret void
; opt: ret void
}

define void @thread2() {
; pso-LABEL: define void @thread2
; opt-LABEL: define void @thread2
  store atomic i32 1, ptr @y monotonic, align 4
; pso: store atomic i32 1, ptr @y monotonic,
; opt: store atomic i32 1, ptr @y monotonic,
  %lx = load atomic i32, ptr @x monotonic, align 4
; pso-NEXT: %lx = load atomic i32, ptr @x monotonic, align 4
; opt-NEXT: %lx = load atomic i32, ptr @x monotonic, align 4
; pso-NEXT: fence seq_cst
; pso-NEXT: fence seq_cst
; opt-NEXT: fence seq_cst
; opt-NOT: fence seq_cst
  store i32 %lx, ptr @r2, align 4
; pso: store i32 %lx, ptr @r2, align 4
; opt: store i32 %lx, ptr @r2, align 4
  ret void
; pso: ret void
; opt: ret void
}
