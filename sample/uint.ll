; ModuleID = 'uint.arow'
source_filename = "uint.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i1 @sleep(i32 %i_arg) {
entry:
  %mul_tmp = mul i32 %i_arg, 1000
  %call_tmp = call i1 @wait(i32 %mul_tmp)
  ret i1 false
}

define i33 @main() {
entry:
  %0 = zext i3 -3 to i32
  %call_tmp = call i1 @printnum(i32 %0)
  %1 = zext i3 -3 to i33
  ret i33 %1
}
