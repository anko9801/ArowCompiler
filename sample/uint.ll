; ModuleID = 'uint.arow'
source_filename = "uint.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %0 = zext i2 1 to i32
  %call_tmp = call i1 @printnum(i32 %0)
  %1 = zext i2 1 to i32
  ret i32 %1
}
