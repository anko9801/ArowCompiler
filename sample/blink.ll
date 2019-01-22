; ModuleID = 'blink.arow'
source_filename = "blink.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

declare i1 @confirm()

declare i1 @Close()

declare i1 @gpioMode(i32, i32)

declare i1 @gpioWrite(i32, i32)

declare i1 @gpioRead(i32)

define i1 @sleep(i32 %i_arg) {
entry:
  %mul_tmp = mul i32 %i_arg, 1000
  %call_tmp = call i1 @wait(i32 %mul_tmp)
  ret i1 false
}

define i32 @main() {
entry:
  %call_tmp = call i1 @confirm()
  %call_tmp1 = call i1 @gpioMode(i32 23, i32 0)
  %ifcond = icmp ne i32 0, 5
  br i1 %ifcond, label %loop, label %afterloop

loop:                                             ; preds = %loop, %entry
  %i.0 = phi i32 [ 0, %entry ], [ %add_tmp, %loop ]
  %add_tmp = add i32 %i.0, 1
  %call_tmp4 = call i1 @gpioWrite(i32 23, i32 1)
  %call_tmp5 = call i1 @wait(i32 1000000)
  %call_tmp6 = call i1 @gpioWrite(i32 23, i32 0)
  %call_tmp7 = call i1 @wait(i32 1000000)
  %ifcond10 = icmp ne i32 %add_tmp, 5
  br i1 %ifcond10, label %loop, label %afterloop

afterloop:                                        ; preds = %loop, %entry
  %call_tmp11 = call i1 @Close()
  ret i32 0
}
