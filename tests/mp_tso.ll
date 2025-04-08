; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso < %s | FileCheck %s

@x = global i32 0, align 4   ; message variable
@y = global i32 0, align 4   ; flag variable
@r1 = global i32 0, align 4  ; record flag read by consumer
@r2 = global i32 0, align 4  ; record message read by consumer

define void @thread1() {
; CHECK-LABEL: define void @thread1
  store atomic i32 1, ptr @x monotonic, align 4
; CHECK: store atomic i32 1, ptr @x monotonic,
; CHECK: fence seq_cst
  store atomic i32 1, ptr @y monotonic, align 4
; CHECK: store atomic i32 1, ptr @y monotonic,
  ret void
}

define void @thread2() {
; CHECK-LABEL: define void @thread2
  %flag = load atomic i32, ptr @y monotonic, align 4
; CHECK: load atomic i32, ptr @y monotonic,
; CHECK: fence seq_cst
  %msg = load atomic i32, ptr @x monotonic, align 4
; CHECK: load atomic i32, ptr @x monotonic,
  store i32 %flag, ptr @r1, align 4
  store i32 %msg, ptr @r2, align 4
  ret void
}
