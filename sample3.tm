* C- Compilation to TM Code
* File: sample3.tm
* Standard prelude
  0:     LD  6,0(0)   load gp with maxaddress
  1:    LDA  5,-4(6)   copy offset to fp
  2:     ST  0,0(0)   clear location 0
* End of standard prelude
* -> Input
  6:     ST  0,-1(5)   input: store return address to retFO
  7:     IN  0,0,0   input: read integer value
  8:     LD  7,-1(5)   input: return
* <- Input
* -> Output
  9:     ST  0,-1(5)   output: store return address to retFO
 10:     LD  0,-2(5)   output: load first arg to ac
 11:    OUT  0,0,0   output: print
 12:     LD  7,-1(5)   output: return
* <- Output
* -> FunD
 13:     ST  0,-1(5)   funD: store return address to retFO
* -> ComS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
 14:    LDA  0,-2(5)   varE: cal address in local
* <- VarE
 15:     ST  0,0(4)   assignE: push left
* -> ConstE
 16:    LDC  0,1(0)   constE: load const
* <- ConstE
 17:     LD  1,0(4)   assignE: load left
 18:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> RetS
* -> VarE
* varE: address of arr
 19:    LDA  0,-2(5)   varE: cal address in local
* varE: if ! isAddress load the value
 20:     LD  0,0(0)   varE: load value to ac
* <- VarE
 21:     LD  7,-1(5)   retS: return
* <- RetS
* <- ComS
 22:     LD  7,-1(5)   funD: return
* <- FunD
* -> FunD
 23:     ST  0,-1(5)   funD: store return address to retFO
* -> ComS
* -> ExpS
* -> CallE
* callE: cal every arguments
* -> CallE
* callE: cal every arguments
 24:     ST  5,-3(5)   store current fp
 25:    LDA  5,-3(5)   push new frame
 26:    LDA  0,1(7)   save return in ac
 27:    LDC  7,13(0)   jump to function entry
 28:     LD  5,0(5)   pop current frame
* <- CallE
 29:     ST  0,-5(5)   callE: store argument
 30:     ST  5,-3(5)   store current fp
 31:    LDA  5,-3(5)   push new frame
 32:    LDA  0,1(7)   save return in ac
 33:    LDC  7,9(0)   jump to function entry
 34:     LD  5,0(5)   pop current frame
* <- CallE
* <- ExpS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
 35:    LDA  0,-2(5)   varE: cal address in local
* <- VarE
 36:     ST  0,1(4)   assignE: push left
* -> ConstE
 37:    LDC  0,0(0)   constE: load const
* <- ConstE
 38:     LD  1,1(4)   assignE: load left
 39:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> ExpS
* -> CallE
* callE: cal every arguments
* -> CallE
* callE: cal every arguments
 40:     ST  5,-3(5)   store current fp
 41:    LDA  5,-3(5)   push new frame
 42:    LDA  0,1(7)   save return in ac
 43:    LDC  7,13(0)   jump to function entry
 44:     LD  5,0(5)   pop current frame
* <- CallE
 45:     ST  0,-5(5)   callE: store argument
 46:     ST  5,-3(5)   store current fp
 47:    LDA  5,-3(5)   push new frame
 48:    LDA  0,1(7)   save return in ac
 49:    LDC  7,9(0)   jump to function entry
 50:     LD  5,0(5)   pop current frame
* <- CallE
* <- ExpS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
 51:    LDA  0,-2(5)   varE: cal address in local
* <- VarE
 52:     ST  0,2(4)   assignE: push left
* -> CallE
* callE: cal every arguments
 53:     ST  5,-3(5)   store current fp
 54:    LDA  5,-3(5)   push new frame
 55:    LDA  0,1(7)   save return in ac
 56:    LDC  7,13(0)   jump to function entry
 57:     LD  5,0(5)   pop current frame
* <- CallE
 58:     LD  1,2(4)   assignE: load left
 59:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> ExpS
* -> CallE
* callE: cal every arguments
* -> VarE
* varE: address of arr
 60:    LDA  0,-2(5)   varE: cal address in local
* varE: if ! isAddress load the value
 61:     LD  0,0(0)   varE: load value to ac
* <- VarE
 62:     ST  0,-5(5)   callE: store argument
 63:     ST  5,-3(5)   store current fp
 64:    LDA  5,-3(5)   push new frame
 65:    LDA  0,1(7)   save return in ac
 66:    LDC  7,9(0)   jump to function entry
 67:     LD  5,0(5)   pop current frame
* <- CallE
* <- ExpS
* <- ComS
 68:     LD  7,-1(5)   funD: return
* <- FunD
  3:    LDC  0,69(0)   main: load return address to ac
  4:     ST  0,-1(5)   main: store return address to retFO
  5:    LDC  7,23(0)   jump to main
* End of execution.
 69:   HALT  0,0,0   
