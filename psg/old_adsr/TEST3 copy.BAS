# R ˛ $421        : DATA STORE G I ˛ RĒ4         : INSTALL FLAG j W ˛ RĒ5         : ON/OFF FLAG  E ˛ RĒ6         : ENVELOPES Š V˛RĒ10 :V0˛RĒ11 : VOLUME Į A˛RĒ46 :A0˛RĒ50 : ATTACK ä D˛RĒ38 :D0˛RĒ42 : DECAY 	 S˛RĒ30 :S0˛RĒ34 : SUSTAIN "		 L˛RĒ18 :L0˛RĒ22 : RELEASE M	P  Â($400)ą0 §  "DRIVER PRESENT" : 90 g	R "ADSR3.PRG",8,1,$400 z	S  $400 : INIT ŗ	Z Î 1, $F9C1, 3   :Î 1, $F9C2, $F0 :Î 1, $F9C3, $C0 ŧ	c "" Į	d "" 
n  ? "  STORE:", PEEK(R) PEEK(R+1) PEEK(R+2) PEEK(R+3) 
x  "  INST :", Â(I) /
  "  WORK :", Â(W) F
  "  ENV  :", Â(E) w
  "  VOL  :          ", Â(V) Â(V0) 
  "  V-SUS:", Â(RĒ54) Â(RĒ58) 
  Ē
 " "; Ā
  Â(E)˛1 § "*"; ņ
   " ATT  :          ", Â(A) Â(A0) ü
¨ " "; Š  Â(E)˛2 § "*"; CĒ  " DEC  :          ", Â(D) Â(D0) N˛ " "; dŗ  Â(E)˛3 § "*"; ´  " SUS  :          ", Â(S) Â(S0)  ŧ " "; ļŊ  Â(E)˛4 § "*"; įž  " RLS  :          ", Â(L) Â(L0) íČ  Ō  "   0  ADVANCES ENVELOPE 0 STATE" 0Ķ  "   G  RESUMES WORK" MÜ  "   V  UPDATES VOLUME" jŪ  "   A  UPDATES ATTACK" ß  "   D  UPDATES DECAY" ¤ā  "   S  UPDATES SUSTAIN" Âá  "   L  UPDATES RELEASE" Ëæ Ą A$ éđ  A$ą˛"0" ¯ A$ŗ˛"3"  300 ūņ  A$ ˛ "G"  400 ō  A$ ˛ "V" §  V, Â(V)Ē1 8ķ  A$ ˛ "Ö" §  V, Â(V)Ģ1 Wô  A$ ˛ "A" §  A0, Â(A0)Ē1 võ  A$ ˛ "Á" §  A0, Â(A0)Ģ1 ö  A$ ˛ "D" §  D0, Â(D0)Ē1 ´÷  A$ ˛ "Ä" §  D0, Â(D0)Ģ1 Ķø  A$ ˛ "S" §  S0, Â(S0)Ē1 ōų  A$ ˛ "Ķ" §  S0, Â(S0)Ģ1 ú  A$ ˛ "L" §  L0, Â(L0)Ē1 0û  A$ ˛ "Ė" §  L0, Â(L0)Ģ1 :+ 100 Q, ADVANCE ENVELOPE l6 W, 0 : STOP WORKING @ EĒÅ(A$), Â(EĒÅ(A$))Ē1  100 ¯ W,1  : RESUME WORKING šķ 100   