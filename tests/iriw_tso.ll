; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso < %s | FileCheck %s -check-prefix=tso
; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso,fence-opt < %s | FileCheck %s -check-prefix=opt

@x = global i32 0, align 4
@y = global i32 0, align 4

define void @thread0() {
; tso-LABEL: @thread0(
entry:
; tso: fence seq_cst
; opt: fence seq_cst
; tso: store atomic i32 1, ptr @x monotonic, align 4
  store atomic i32 1, ptr @x monotonic, align 4
; tso: ret void
  ret void
}

define void @thread1() {
; tso-LABEL: @thread1(
entry:
; tso: fence seq_cst
; opt: fence seq_cst
; tso: store atomic i32 1, ptr @y monotonic, align 4
  store atomic i32 1, ptr @y monotonic, align 4
; tso: ret void
  ret void
}

define void @thread2(ptr %r1, ptr %r2) {
; tso-LABEL: @thread2(
entry:
; tso: fence seq_cst
; opt: fence seq_cst
; tso: %loadx = load atomic i32, ptr @x monotonic, align 4
  %loadx = load atomic i32, ptr @x monotonic, align 4
; tso: fence seq_cst
; opt: fence seq_cst
; tso: store i32 %loadx, ptr %r1, align 4
  store i32 %loadx, ptr %r1, align 4
; tso: fence seq_cst
; opt-NOT: fence seq_cst
; tso: %loady = load atomic i32, ptr @y monotonic, align 4
  %loady = load atomic i32, ptr @y monotonic, align 4
; tso: store i32 %loady, ptr %r2, align 4
  store i32 %loady, ptr %r2, align 4
; tso: ret void
  ret void
}

define void @thread3(ptr %r3, ptr %r4) {
; tso-LABEL: @thread3(
entry:
; tso: fence seq_cst
; opt: fence seq_cst
; tso: %loady = load atomic i32, ptr @y monotonic, align 4
  %loady = load atomic i32, ptr @y monotonic, align 4
; tso: fence seq_cst
; opt: fence seq_cst
; tso: store i32 %loady, ptr %r3, align 4
  store i32 %loady, ptr %r3, align 4
; tso: fence seq_cst
; opt-NOT: fence seq_cst
; tso: %loadx = load atomic i32, ptr @x monotonic, align 4
  %loadx = load atomic i32, ptr @x monotonic, align 4
; tso: store i32 %loadx, ptr %r4, align 4
  store i32 %loadx, ptr %r4, align 4
; tso: ret void
  ret void
}
