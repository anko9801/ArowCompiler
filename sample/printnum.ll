; ModuleID = 'printnum.arow'
source_filename = "printnum.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %array = alloca [2 x double]*, i32 2
  %call_tmp = call i1 @printnum(i32 1)
  %call_tmp1 = call i1 @printnum(i32 3)
  %call_tmp2 = call i1 @printnum(i32 3)
  ret i32 0
}
