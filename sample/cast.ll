; ModuleID = 'cast.arow'
source_filename = "cast.arow"

declare i1 @printnum(i32)

declare i1 @wait(i32)

declare i32 @usclock()

define i32 @main() {
entry:
  %0 = zext i16 2 to i32
  ret i32 %0
}
