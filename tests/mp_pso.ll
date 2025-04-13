; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-pso < %s | FileCheck %s -check-prefix=pso
; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-pso,fence-opt < %s | FileCheck %s -check-prefix=opt

@x = global i32 0, align 4   ; message variable
@y = global i32 0, align 4   ; flag variable
@r1 = global i32 0, align 4  ; record flag read by consumer
@r2 = global i32 0, align 4  ; record message read by consumer

define void @thread1() {
; pso-LABEL: define void @thread1
; opt-LABEL: define void @thread1
; pso: fence seq_cst
  store atomic i32 1, ptr @x monotonic, align 4
; pso: store atomic i32 1, ptr @x monotonic,
; opt: store atomic i32 1, ptr @x monotonic,
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
  %flag = load atomic i32, ptr @y monotonic, align 4
; pso: load atomic i32, ptr @y monotonic,
; opt: load atomic i32, ptr @y monotonic,
; pso: fence seq_cst
; pso: fence seq_cst
; opt: fence seq_cst
; opt-NOT: fence seq_cst
  %msg = load atomic i32, ptr @x monotonic, align 4
; pso: load atomic i32, ptr @x monotonic,
; opt: load atomic i32, ptr @x monotonic,
; opt-NOT: fence seq_cst
  store i32 %flag, ptr @r1, align 4
; pso: store i32 %flag, ptr @r1, align 4
; opt: store i32 %flag, ptr @r1, align 4
  store i32 %msg, ptr @r2, align 4
; pso: store i32 %msg, ptr @r2, align 4
; opt: store i32 %msg, ptr @r2, align 4
  ret void
; pso: ret void
; opt: ret void
}
