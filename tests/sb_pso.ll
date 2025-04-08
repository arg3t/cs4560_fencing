
; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-pso < %s | FileCheck %s

@x = global i32 0, align 4
@y = global i32 0, align 4
@r1 = global i32 0, align 4
@r2 = global i32 0, align 4

define void @thread1() {
; CHECK-LABEL: define void @thread1
  store atomic i32 1, ptr @x monotonic, align 4
; CHECK: store atomic i32 1, ptr @x monotonic,
  %ly = load atomic i32, ptr @y monotonic, align 4
; CHECK: load atomic i32, ptr @y monotonic,
  store i32 %ly, ptr @r1, align 4
  ret void
}

define void @thread2() {
; CHECK-LABEL: define void @thread2
  store atomic i32 1, ptr @y monotonic, align 4
; CHECK: store atomic i32 1, ptr @y monotonic,
  %lx = load atomic i32, ptr @x monotonic, align 4
; CHECK: load atomic i32, ptr @x monotonic,
  store i32 %lx, ptr @r2, align 4
  ret void
}
