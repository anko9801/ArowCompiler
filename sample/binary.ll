; ModuleID = 'binary.arow'
source_filename = "binary.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %call_tmp = call i1 @printnum(i32 5)
  ret i32 0
}
