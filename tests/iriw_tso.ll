; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso < %s | FileCheck %s -check-prefix=tso
; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso,fence-opt < %s | FileCheck %s -check-prefix=opt


@x = global i32 0, align 4
@y = global i32 0, align 4

define void @thread0() {
; tso-LABEL: @thread0(
  store atomic i32 1, i32* @x monotonic, align 4
; tso: store atomic i32 1, i32* @x monotonic, align 4
  ret void
}
