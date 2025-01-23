//function SimpleFunction.test 2
(SimpleFunction.test)
@0
A=M
//push constant 0
M=0
A=A+1
//push constant 0
M=0
A=A+1
D=A
@0
M=D
//push local 0
@1
A=M
D=A
@0
A=D+A
D=M
@0
A=M
M=D
D=A+1
@0
M=D
//push local 1
@1
A=M
D=A
@1
A=D+A
D=M
@0
A=M
M=D
D=A+1
@0
M=D
//add
@0
A=M
A=A-1
D=M
A=A-1
M=D+M
A=A+1
D=A
@0
M=D
//not
@0
A=M-1
D=M
M=!D
//push argument 0
@2
A=M
D=A
@0
A=D+A
D=M
@0
A=M
M=D
D=A+1
@0
M=D
//add
@0
A=M
A=A-1
D=M
A=A-1
M=D+M
A=A+1
D=A
@0
M=D
//push argument 1
@2
A=M
D=A
@1
A=D+A
D=M
@0
A=M
M=D
D=A+1
@0
M=D
//add
@0
A=M
A=A-1
D=M
A=A-1
M=M-D
A=A+1
D=A
@0
M=D
//return
//store FRAME = LCL
@1
D=M
@FRAME
M=D
//STORE THE return address to RET
@5
D=D-A
A=D
D=M
@RET
M=D
// *ARG = POP()
@0
A=M-1
D=M
@2
A=M
M=D
// SP = ARG + 1
@2
D=M+1
@0
M=D
//copy that
@FRAME
AM=M-1
D=M
@4
M=D
//copy this
@FRAME
AM=M-1
D=M
@3
M=D
//copy ARG
@FRAME
AM=M-1
D=M
@2
M=D
//copy LCL
@FRAME
AM=M-1
D=M
@1
M=D
@RET
A=M
0;JMP
