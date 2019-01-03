; ModuleID = 'benchmark.arow'
source_filename = "benchmark.arow"

declare i1 @printnum(i32)

declare i1 @sleep(i32)

declare i32 @usclock()

declare i1 @BlinkLED(i32)

declare i1 @GPIOsetup()

declare i1 @GPIOclear()

define i32 @main() {
entry:
  %call_tmp = call i32 @usclock()
  %ifcond = icmp slt i32 0, 1000000
  br label %loop

loop:                                             ; preds = %loop, %entry
  %i.0 = phi i32 [ 0, %entry ], [ %add_tmp, %loop ]
  %add_tmp = add i32 %i.0, 1
  %call_tmp3 = call i1 @printnum(i32 %add_tmp)
  br i1 %ifcond, label %loop, label %afterloop

afterloop:                                        ; preds = %loop
  %call_tmp4 = call i32 @usclock()
  %sub_tmp = sub i32 %call_tmp4, %call_tmp
  %call_tmp6 = call i1 @printnum(i32 %sub_tmp)
  ret i32 %add_tmp
}
