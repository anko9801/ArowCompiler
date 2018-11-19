; ModuleID = 'test3.dc'

declare i32 @printnum(i32)

define i32 @main() {
entry:
  %call_tmp = call i32 @printnum(i32 23)
  ret i32 0
}
