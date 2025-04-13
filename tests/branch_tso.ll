; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso  < %s | FileCheck %s -check-prefix=tso

; RUN: opt -load-pass-plugin ../build/fencing/FencingPass.so -S -passes=fence-tso,fence-opt  < %s | FileCheck %s -check-prefix=opt

@x = global i32 0, align 4
@y = global i32 0, align 4

define void @check_and_update(i32 %param) {
entry:
  ; tso: icmp sgt i32 %param, 2
  ; opt: icmp sgt i32 %param, 2
  %cmp = icmp sgt i32 %param, 2
  ; tso-NEXT: br i1 %cmp, label %then, label %else
  ; opt-NEXT: br i1 %cmp, label %then, label %else
  br i1 %cmp, label %then, label %else
then:
  ; tso: %r1_load = load atomic i32, ptr @x monotonic, align 4
  ; opt: %r1_load = load atomic i32, ptr @x monotonic, align 4
  %r1_load = load atomic i32, ptr @x monotonic, align 4
  ; tso: fence seq_cst
  ; opt-NOT: fence seq_cst
  br label %merge
  ; tso: br label %merge
  ; opt: br label %merge
else:
  br label %merge
  ; tso: br label %merge
  ; opt: br label %merge
merge:
  ; tso: %r1_val = phi i32 [ %r1_load, %then ], [ %param, %else ]
  ; opt: %r1_val = phi i32 [ %r1_load, %then ], [ %param, %else ]
  %r1_val = phi i32 [ %r1_load, %then ], [ %param, %else ]
  ; tso-NEXT: fence seq_cst
  ; opt-NEXT: fence seq_cst
  ; tso-NEXT: store atomic i32 %r1_val, ptr @y monotonic, align 4
  ; opt-NEXT: store atomic i32 %r1_val, ptr @y monotonic, align 4
  store atomic i32 %r1_val, ptr @y monotonic, align 4
  ret void
}
