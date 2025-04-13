; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-pso < %s | FileCheck %s -check-prefix=pso
; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-pso,fence-opt < %s | FileCheck %s -check-prefix=opt

@x = global i32 0, align 4
@y = global i32 0, align 4
@r1 = global i32 0, align 4
@r2 = global i32 0, align 4

define void @thread1() {
  ; pso-LABEL: define void @thread1
  ; opt-LABEL: define void @thread1
  ; pso: fence seq_cst
  %lx = load atomic i32, ptr @x monotonic, align 4
  ; pso: load atomic i32, ptr @x monotonic,
  ; pso: fence seq_cst
  ; opt: fence seq_cst
  ; opt-NOT: fence seq_cst
  store i32 %lx, ptr @r1, align 4
  ; pso: store i32 %lx, ptr @r1, align 4
  ; opt: store i32 %lx, ptr @r1, align 4
  ; pso: fence seq_cst
  store atomic i32 1, ptr @y monotonic, align 4
  ; pso: store atomic i32 1, ptr @y monotonic,
  ; opt: store atomic i32 1, ptr @y monotonic,
  ret void
  ; pso: ret void
  ; opt: ret void
}

define void @thread2() {
  ; pso-LABEL: define void @thread2
  ; opt-LABEL: define void @thread2
  ; pso: fence seq_cst
  %ly = load atomic i32, ptr @y monotonic, align 4
  ; pso: load atomic i32, ptr @y monotonic,
  ; opt: load atomic i32, ptr @y monotonic,
  ; pso: fence seq_cst
  ; opt: fence seq_cst
  ; opt-NOT: fence seq_cst
  store i32 %ly, ptr @r2, align 4
  ; pso: store i32 %ly, ptr @r2, align 4
  ; opt: store i32 %ly, ptr @r2, align 4
  ; pso: fence seq_cst
  store atomic i32 1, ptr @x monotonic, align 4
  ; pso: store atomic i32 1, ptr @x monotonic,
  ; opt: store atomic i32 1, ptr @x monotonic,
  ret void
  ; pso: ret void
  ; opt: ret void
}
