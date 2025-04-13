; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-pso < %s | FileCheck %s -check-prefix=tso
; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-pso,fence-opt < %s | FileCheck %s -check-prefix=opt

@x = global i32 0, align 4
@y = global i32 0, align 4

define void @thread0() {
; pso-LABEL: @thread0(
entry:
; pso: fence seq_cst
; pso: store atomic i32 1, ptr @x monotonic, align 4
  store atomic i32 1, ptr @x monotonic, align 4
; pso: ret void
  ret void
}

define void @thread1() {
; pso-LABEL: @thread1(
entry:
; pso: fence seq_cst
; pso: store atomic i32 1, ptr @y monotonic, align 4
  store atomic i32 1, ptr @y monotonic, align 4
; pso: ret void
  ret void
}

define void @thread2(ptr %r1, ptr %r2) {
; pso-LABEL: @thread2(
entry:
; pso: fence seq_cst
; pso: %loadx = load atomic i32, ptr @x monotonic, align 4
  %loadx = load atomic i32, ptr @x monotonic, align 4
; pso: fence seq_cst
; pso: store i32 %loadx, ptr %r1, align 4
  store i32 %loadx, ptr %r1, align 4
; pso: fence seq_cst
; pso: %loady = load atomic i32, ptr @y monotonic, align 4
  %loady = load atomic i32, ptr @y monotonic, align 4
; pso: store i32 %loady, ptr %r2, align 4
  store i32 %loady, ptr %r2, align 4
; pso: ret void
  ret void
}

define void @thread3(ptr %r3, ptr %r4) {
; pso-LABEL: @thread3(
entry:
; pso: fence seq_cst
; pso: %loady = load atomic i32, ptr @y monotonic, align 4
  %loady = load atomic i32, ptr @y monotonic, align 4
; pso: fence seq_cst
; pso: store i32 %loady, ptr %r3, align 4
  store i32 %loady, ptr %r3, align 4
; pso: fence seq_cst
; pso: %loadx = load atomic i32, ptr @x monotonic, align 4
  %loadx = load atomic i32, ptr @x monotonic, align 4
; pso: store i32 %loadx, ptr %r4, align 4
  store i32 %loadx, ptr %r4, align 4
; pso: ret void
  ret void
}
