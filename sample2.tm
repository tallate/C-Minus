* C- Compilation to TM Code
* File: sample2.tm
* Standard prelude
  0:     LD  6,0(0)   load gp with maxaddress
  1:    LDA  5,-15(6)   copy offset to fp
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
 14:    LDA  0,-7(5)   varE: cal address in local
* <- VarE
 15:     ST  0,0(4)   assignE: push left
* -> VarE
* varE: address of arr
 16:    LDA  0,-3(5)   varE: cal address in local
* varE: if ! isAddress load the value
 17:     LD  0,0(0)   varE: load value to ac
* <- VarE
 18:     LD  1,0(4)   assignE: load left
 19:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
 20:    LDA  0,-6(5)   varE: cal address in local
* <- VarE
 21:     ST  0,1(4)   assignE: push left
* -> ArrE
* arrE: cal address of arr(base + index)
* -> VarE
* varE: address of arr
 22:    LDA  0,-3(5)   varE: cal address in local
* varE: if ! isAddress load the value
 23:     LD  0,0(0)   varE: load value to ac
* <- VarE
 24:    LDA  1,-2(5)   arrE: cal the base address in local
 25:     LD  1,0(1)   arrE: redirect
 26:    SUB  0,1,0   arrE: cal the absolute address
* arrE: if ! isAddress load the value
 27:     LD  0,0(0)   arrE: put value in ac
* <- ArrE
 28:     LD  1,1(4)   assignE: load left
 29:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
 30:    LDA  0,-5(5)   varE: cal address in local
* <- VarE
 31:     ST  0,2(4)   assignE: push left
* -> OpE
* -> VarE
* varE: address of arr
 32:    LDA  0,-3(5)   varE: cal address in local
* varE: if ! isAddress load the value
 33:     LD  0,0(0)   varE: load value to ac
* <- VarE
 34:     ST  0,3(4)   opE: push left
* -> ConstE
 35:    LDC  0,1(0)   constE: load const
* <- ConstE
 36:     LD  1,3(4)   opE: load left
 37:    ADD  0,1,0   opE +
* <- OpE
 38:     LD  1,2(4)   assignE: load left
 39:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> IteS
* -> OpE
* -> VarE
* varE: address of arr
 40:    LDA  0,-5(5)   varE: cal address in local
* varE: if ! isAddress load the value
 41:     LD  0,0(0)   varE: load value to ac
* <- VarE
 42:     ST  0,4(4)   opE: push left
* -> VarE
* varE: address of arr
 43:    LDA  0,-4(5)   varE: cal address in local
* varE: if ! isAddress load the value
 44:     LD  0,0(0)   varE: load value to ac
* <- VarE
 45:     LD  1,4(4)   opE: load left
 46:    SUB  0,1,0   op <
 47:    JLT  0,2(7)   br if true
 48:    LDC  0,0(0)   false case
 49:    LDA  7,1(7)   unconditional jmp
 50:    LDC  0,1(0)   true case
* <- OpE
* iteS: jump to end belongs here
* -> ComS
* -> SelS
* -> OpE
* -> ArrE
* arrE: cal address of arr(base + index)
* -> VarE
* varE: address of arr
 52:    LDA  0,-5(5)   varE: cal address in local
* varE: if ! isAddress load the value
 53:     LD  0,0(0)   varE: load value to ac
* <- VarE
 54:    LDA  1,-2(5)   arrE: cal the base address in local
 55:     LD  1,0(1)   arrE: redirect
 56:    SUB  0,1,0   arrE: cal the absolute address
* arrE: if ! isAddress load the value
 57:     LD  0,0(0)   arrE: put value in ac
* <- ArrE
 58:     ST  0,5(4)   opE: push left
* -> VarE
* varE: address of arr
 59:    LDA  0,-6(5)   varE: cal address in local
* varE: if ! isAddress load the value
 60:     LD  0,0(0)   varE: load value to ac
* <- VarE
 61:     LD  1,5(4)   opE: load left
 62:    SUB  0,1,0   op <
 63:    JLT  0,2(7)   br if true
 64:    LDC  0,0(0)   false case
 65:    LDA  7,1(7)   unconditional jmp
 66:    LDC  0,1(0)   true case
* <- OpE
* selS: jump to else belongs here
* -> ComS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
 68:    LDA  0,-6(5)   varE: cal address in local
* <- VarE
 69:     ST  0,6(4)   assignE: push left
* -> ArrE
* arrE: cal address of arr(base + index)
* -> VarE
* varE: address of arr
 70:    LDA  0,-5(5)   varE: cal address in local
* varE: if ! isAddress load the value
 71:     LD  0,0(0)   varE: load value to ac
* <- VarE
 72:    LDA  1,-2(5)   arrE: cal the base address in local
 73:     LD  1,0(1)   arrE: redirect
 74:    SUB  0,1,0   arrE: cal the absolute address
* arrE: if ! isAddress load the value
 75:     LD  0,0(0)   arrE: put value in ac
* <- ArrE
 76:     LD  1,6(4)   assignE: load left
 77:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
 78:    LDA  0,-7(5)   varE: cal address in local
* <- VarE
 79:     ST  0,7(4)   assignE: push left
* -> VarE
* varE: address of arr
 80:    LDA  0,-5(5)   varE: cal address in local
* varE: if ! isAddress load the value
 81:     LD  0,0(0)   varE: load value to ac
* <- VarE
 82:     LD  1,7(4)   assignE: load left
 83:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* <- ComS
* selS: jump to end belongs here
 67:    JEQ  0,17(7)   selS: jmp to else
 84:    LDA  7,0(7)   selS: jmp to end
* <- SelS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
 85:    LDA  0,-5(5)   varE: cal address in local
* <- VarE
 86:     ST  0,8(4)   assignE: push left
* -> OpE
* -> VarE
* varE: address of arr
 87:    LDA  0,-5(5)   varE: cal address in local
* varE: if ! isAddress load the value
 88:     LD  0,0(0)   varE: load value to ac
* <- VarE
 89:     ST  0,9(4)   opE: push left
* -> ConstE
 90:    LDC  0,1(0)   constE: load const
* <- ConstE
 91:     LD  1,9(4)   opE: load left
 92:    ADD  0,1,0   opE +
* <- OpE
 93:     LD  1,8(4)   assignE: load left
 94:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* <- ComS
 95:    LDA  7,-56(7)   jmp to begin
* iteS: jump to begin belongs here
 51:    JEQ  0,44(7)   jmp to end
* <- IteS
* -> RetS
* -> VarE
* varE: address of arr
 96:    LDA  0,-7(5)   varE: cal address in local
* varE: if ! isAddress load the value
 97:     LD  0,0(0)   varE: load value to ac
* <- VarE
 98:     LD  7,-1(5)   retS: return
* <- RetS
* <- ComS
 99:     LD  7,-1(5)   funD: return
* <- FunD
* -> FunD
100:     ST  0,-1(5)   funD: store return address to retFO
* -> ComS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
101:    LDA  0,-5(5)   varE: cal address in local
* <- VarE
102:     ST  0,10(4)   assignE: push left
* -> VarE
* varE: address of arr
103:    LDA  0,-3(5)   varE: cal address in local
* varE: if ! isAddress load the value
104:     LD  0,0(0)   varE: load value to ac
* <- VarE
105:     LD  1,10(4)   assignE: load left
106:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> IteS
* -> OpE
* -> VarE
* varE: address of arr
107:    LDA  0,-5(5)   varE: cal address in local
* varE: if ! isAddress load the value
108:     LD  0,0(0)   varE: load value to ac
* <- VarE
109:     ST  0,11(4)   opE: push left
* -> OpE
* -> VarE
* varE: address of arr
110:    LDA  0,-4(5)   varE: cal address in local
* varE: if ! isAddress load the value
111:     LD  0,0(0)   varE: load value to ac
* <- VarE
112:     ST  0,12(4)   opE: push left
* -> ConstE
113:    LDC  0,1(0)   constE: load const
* <- ConstE
114:     LD  1,12(4)   opE: load left
115:    SUB  0,1,0   opE -
* <- OpE
116:     LD  1,11(4)   opE: load left
117:    SUB  0,1,0   op <
118:    JLT  0,2(7)   br if true
119:    LDC  0,0(0)   false case
120:    LDA  7,1(7)   unconditional jmp
121:    LDC  0,1(0)   true case
* <- OpE
* iteS: jump to end belongs here
* -> ComS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
123:    LDA  0,-6(5)   varE: cal address in local
* <- VarE
124:     ST  0,13(4)   assignE: push left
* -> CallE
* callE: cal every arguments
* -> VarE
* varE: address of arr
125:    LDA  0,-2(5)   varE: cal address in local
* varE: if ! isAddress load the value
126:     LD  0,0(0)   varE: load value to ac
* <- VarE
127:     ST  0,-10(5)   callE: store argument
* -> VarE
* varE: address of arr
128:    LDA  0,-5(5)   varE: cal address in local
* varE: if ! isAddress load the value
129:     LD  0,0(0)   varE: load value to ac
* <- VarE
130:     ST  0,-11(5)   callE: store argument
* -> VarE
* varE: address of arr
131:    LDA  0,-4(5)   varE: cal address in local
* varE: if ! isAddress load the value
132:     LD  0,0(0)   varE: load value to ac
* <- VarE
133:     ST  0,-12(5)   callE: store argument
134:     ST  5,-8(5)   store current fp
135:    LDA  5,-8(5)   push new frame
136:    LDA  0,1(7)   save return in ac
137:    LDC  7,13(0)   jump to function entry
138:     LD  5,0(5)   pop current frame
* <- CallE
139:     LD  1,13(4)   assignE: load left
140:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
141:    LDA  0,-7(5)   varE: cal address in local
* <- VarE
142:     ST  0,14(4)   assignE: push left
* -> ArrE
* arrE: cal address of arr(base + index)
* -> VarE
* varE: address of arr
143:    LDA  0,-6(5)   varE: cal address in local
* varE: if ! isAddress load the value
144:     LD  0,0(0)   varE: load value to ac
* <- VarE
145:    LDA  1,-2(5)   arrE: cal the base address in local
146:     LD  1,0(1)   arrE: redirect
147:    SUB  0,1,0   arrE: cal the absolute address
* arrE: if ! isAddress load the value
148:     LD  0,0(0)   arrE: put value in ac
* <- ArrE
149:     LD  1,14(4)   assignE: load left
150:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> ExpS
* -> AssignE
* -> ArrE
* arrE: cal address of arr(base + index)
* -> VarE
* varE: address of arr
151:    LDA  0,-6(5)   varE: cal address in local
* varE: if ! isAddress load the value
152:     LD  0,0(0)   varE: load value to ac
* <- VarE
153:    LDA  1,-2(5)   arrE: cal the base address in local
154:     LD  1,0(1)   arrE: redirect
155:    SUB  0,1,0   arrE: cal the absolute address
* <- ArrE
156:     ST  0,15(4)   assignE: push left
* -> ArrE
* arrE: cal address of arr(base + index)
* -> VarE
* varE: address of arr
157:    LDA  0,-5(5)   varE: cal address in local
* varE: if ! isAddress load the value
158:     LD  0,0(0)   varE: load value to ac
* <- VarE
159:    LDA  1,-2(5)   arrE: cal the base address in local
160:     LD  1,0(1)   arrE: redirect
161:    SUB  0,1,0   arrE: cal the absolute address
* arrE: if ! isAddress load the value
162:     LD  0,0(0)   arrE: put value in ac
* <- ArrE
163:     LD  1,15(4)   assignE: load left
164:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> ExpS
* -> AssignE
* -> ArrE
* arrE: cal address of arr(base + index)
* -> VarE
* varE: address of arr
165:    LDA  0,-5(5)   varE: cal address in local
* varE: if ! isAddress load the value
166:     LD  0,0(0)   varE: load value to ac
* <- VarE
167:    LDA  1,-2(5)   arrE: cal the base address in local
168:     LD  1,0(1)   arrE: redirect
169:    SUB  0,1,0   arrE: cal the absolute address
* <- ArrE
170:     ST  0,16(4)   assignE: push left
* -> VarE
* varE: address of arr
171:    LDA  0,-7(5)   varE: cal address in local
* varE: if ! isAddress load the value
172:     LD  0,0(0)   varE: load value to ac
* <- VarE
173:     LD  1,16(4)   assignE: load left
174:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
175:    LDA  0,-5(5)   varE: cal address in local
* <- VarE
176:     ST  0,17(4)   assignE: push left
* -> OpE
* -> VarE
* varE: address of arr
177:    LDA  0,-5(5)   varE: cal address in local
* varE: if ! isAddress load the value
178:     LD  0,0(0)   varE: load value to ac
* <- VarE
179:     ST  0,18(4)   opE: push left
* -> ConstE
180:    LDC  0,1(0)   constE: load const
* <- ConstE
181:     LD  1,18(4)   opE: load left
182:    ADD  0,1,0   opE +
* <- OpE
183:     LD  1,17(4)   assignE: load left
184:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* <- ComS
185:    LDA  7,-79(7)   jmp to begin
* iteS: jump to begin belongs here
122:    JEQ  0,63(7)   jmp to end
* <- IteS
* <- ComS
186:     LD  7,-1(5)   funD: return
* <- FunD
* -> FunD
187:     ST  0,-1(5)   funD: store return address to retFO
* -> ComS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
188:    LDA  0,-2(5)   varE: cal address in local
* <- VarE
189:     ST  0,19(4)   assignE: push left
* -> ConstE
190:    LDC  0,0(0)   constE: load const
* <- ConstE
191:     LD  1,19(4)   assignE: load left
192:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> IteS
* -> OpE
* -> VarE
* varE: address of arr
193:    LDA  0,-2(5)   varE: cal address in local
* varE: if ! isAddress load the value
194:     LD  0,0(0)   varE: load value to ac
* <- VarE
195:     ST  0,20(4)   opE: push left
* -> ConstE
196:    LDC  0,10(0)   constE: load const
* <- ConstE
197:     LD  1,20(4)   opE: load left
198:    SUB  0,1,0   op <
199:    JLT  0,2(7)   br if true
200:    LDC  0,0(0)   false case
201:    LDA  7,1(7)   unconditional jmp
202:    LDC  0,1(0)   true case
* <- OpE
* iteS: jump to end belongs here
* -> ComS
* -> ExpS
* -> AssignE
* -> ArrE
* arrE: cal address of arr(base + index)
* -> VarE
* varE: address of arr
204:    LDA  0,-2(5)   varE: cal address in local
* varE: if ! isAddress load the value
205:     LD  0,0(0)   varE: load value to ac
* <- VarE
206:    LDA  1,-2(6)   arrE: cal the base address in global
207:    SUB  0,1,0   arrE: cal the absolute address
* <- ArrE
208:     ST  0,21(4)   assignE: push left
* -> CallE
* callE: cal every arguments
209:     ST  5,-3(5)   store current fp
210:    LDA  5,-3(5)   push new frame
211:    LDA  0,1(7)   save return in ac
212:    LDC  7,6(0)   jump to function entry
213:     LD  5,0(5)   pop current frame
* <- CallE
214:     LD  1,21(4)   assignE: load left
215:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
216:    LDA  0,-2(5)   varE: cal address in local
* <- VarE
217:     ST  0,22(4)   assignE: push left
* -> OpE
* -> VarE
* varE: address of arr
218:    LDA  0,-2(5)   varE: cal address in local
* varE: if ! isAddress load the value
219:     LD  0,0(0)   varE: load value to ac
* <- VarE
220:     ST  0,23(4)   opE: push left
* -> ConstE
221:    LDC  0,1(0)   constE: load const
* <- ConstE
222:     LD  1,23(4)   opE: load left
223:    ADD  0,1,0   opE +
* <- OpE
224:     LD  1,22(4)   assignE: load left
225:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* <- ComS
226:    LDA  7,-34(7)   jmp to begin
* iteS: jump to begin belongs here
203:    JEQ  0,23(7)   jmp to end
* <- IteS
* -> ExpS
* -> CallE
* callE: cal every arguments
* -> VarE
* varE: address of arr
227:    LDA  0,-2(6)   varE: cal address in global
* <- VarE
228:     ST  0,-5(5)   callE: store argument
* -> ConstE
229:    LDC  0,0(0)   constE: load const
* <- ConstE
230:     ST  0,-6(5)   callE: store argument
* -> ConstE
231:    LDC  0,10(0)   constE: load const
* <- ConstE
232:     ST  0,-7(5)   callE: store argument
233:     ST  5,-3(5)   store current fp
234:    LDA  5,-3(5)   push new frame
235:    LDA  0,1(7)   save return in ac
236:    LDC  7,100(0)   jump to function entry
237:     LD  5,0(5)   pop current frame
* <- CallE
* <- ExpS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
238:    LDA  0,-2(5)   varE: cal address in local
* <- VarE
239:     ST  0,24(4)   assignE: push left
* -> ConstE
240:    LDC  0,0(0)   constE: load const
* <- ConstE
241:     LD  1,24(4)   assignE: load left
242:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* -> IteS
* -> OpE
* -> VarE
* varE: address of arr
243:    LDA  0,-2(5)   varE: cal address in local
* varE: if ! isAddress load the value
244:     LD  0,0(0)   varE: load value to ac
* <- VarE
245:     ST  0,25(4)   opE: push left
* -> ConstE
246:    LDC  0,10(0)   constE: load const
* <- ConstE
247:     LD  1,25(4)   opE: load left
248:    SUB  0,1,0   op <
249:    JLT  0,2(7)   br if true
250:    LDC  0,0(0)   false case
251:    LDA  7,1(7)   unconditional jmp
252:    LDC  0,1(0)   true case
* <- OpE
* iteS: jump to end belongs here
* -> ComS
* -> ExpS
* -> CallE
* callE: cal every arguments
* -> ArrE
* arrE: cal address of arr(base + index)
* -> VarE
* varE: address of arr
254:    LDA  0,-2(5)   varE: cal address in local
* varE: if ! isAddress load the value
255:     LD  0,0(0)   varE: load value to ac
* <- VarE
256:    LDA  1,-2(6)   arrE: cal the base address in global
257:    SUB  0,1,0   arrE: cal the absolute address
* arrE: if ! isAddress load the value
258:     LD  0,0(0)   arrE: put value in ac
* <- ArrE
259:     ST  0,-5(5)   callE: store argument
260:     ST  5,-3(5)   store current fp
261:    LDA  5,-3(5)   push new frame
262:    LDA  0,1(7)   save return in ac
263:    LDC  7,9(0)   jump to function entry
264:     LD  5,0(5)   pop current frame
* <- CallE
* <- ExpS
* -> ExpS
* -> AssignE
* -> VarE
* varE: address of arr
265:    LDA  0,-2(5)   varE: cal address in local
* <- VarE
266:     ST  0,26(4)   assignE: push left
* -> OpE
* -> VarE
* varE: address of arr
267:    LDA  0,-2(5)   varE: cal address in local
* varE: if ! isAddress load the value
268:     LD  0,0(0)   varE: load value to ac
* <- VarE
269:     ST  0,27(4)   opE: push left
* -> ConstE
270:    LDC  0,1(0)   constE: load const
* <- ConstE
271:     LD  1,27(4)   opE: load left
272:    ADD  0,1,0   opE +
* <- OpE
273:     LD  1,26(4)   assignE: load left
274:     ST  0,0(1)   assignE: store right to left
* <- assign
* <- ExpS
* <- ComS
275:    LDA  7,-33(7)   jmp to begin
* iteS: jump to begin belongs here
253:    JEQ  0,22(7)   jmp to end
* <- IteS
* <- ComS
276:     LD  7,-1(5)   funD: return
* <- FunD
  3:    LDC  0,277(0)   main: load return address to ac
  4:     ST  0,-1(5)   main: store return address to retFO
  5:    LDC  7,187(0)   jump to main
* End of execution.
277:   HALT  0,0,0   
