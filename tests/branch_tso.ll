; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso  < %s | FileCheck %s

@x = global i32 0, align 4
@y = global i32 0, align 4

define void @check_and_update(i32 %param) {
entry:
  ; CHECK: icmp sgt i32 %param, 2
  %cmp = icmp sgt i32 %param, 2
  ; CHECK: br i1 %cmp, label %then, label %else
  br i1 %cmp, label %then, label %else

then:
  ; CHECK: load atomic i32, ptr @x monotonic, align 4
  %r1_load = load atomic i32, ptr @x monotonic, align 4
  br label %merge

else:
  br label %merge

merge:
  ; CHECK: %r1_val = phi i32 [ %r1_load, %then ], [ %param, %else ]
  %r1_val = phi i32 [ %r1_load, %then ], [ %param, %else ]
  ; CHECK: fence seq_cst
  ; CHECK: store atomic i32 %r1_val, ptr @y monotonic, align 4
  store atomic i32 %r1_val, ptr @y monotonic, align 4
  ret void
}
