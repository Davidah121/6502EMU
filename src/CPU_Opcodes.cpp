#include "CPU.h"

//Note that {0,0,"INVALID"} means null in this case. It is an operation with no information or illegal
//Even though the operations do work and are used in certain applications, the emulator will consider them all invalid

InstructionInfo CPU::OpCodes[256] = {
    {0,7,"BRK"},            //$00 : BRK
    {1,6,"ORA ($%02x,X)"},    //$01 : ORA
    {0,0,"INVALID"},        //$02 : KIL
    {0,0,"INVALID"},        //$03 : SLO
    {0,0,"INVALID"},        //$04 : NOP
    {1,3,"ORA $%02x"},        //$05 : ORA
    {1,5,"ASL $%02x"},        //$06 : ASL
    {0,0,"INVALID"},        //$07 : SLO
    {0,3,"PHP"},            //$08 : PHP
    {1,2,"ORA #$%02x"},       //$09 : ORA
    {0,2,"ASL A"},          //$0A : ASL
    {0,0,"INVALID"},        //$0B : ANC
    {0,0,"INVALID"},        //$0C : NOP
    {2,4,"ORA $%02x%02x"},      //$0D : ORA
    {2,6,"ASL $%02x%02x"},      //$0E : ASL
    {0,0,"INVALID"},        //$0F : SLO

    {1,2,"BPL %02x"},         //$10 : BPL
    {1,5,"ORA ($%02x),Y"},    //$11 : ORA
    {0,0,"INVALID"},        //$12 : KIL
    {0,0,"INVALID"},        //$13 : SLO
    {0,0,"INVALID"},        //$14 : NOP
    {1,4,"ORA $%02x,X"},      //$15 : ORA
    {1,6,"ASL $%02x,X"},      //$16 : ASL
    {0,0,"INVALID"},        //$17 : SLO
    {0,2,"CLC"},            //$18 : CLC
    {2,4,"ORA $%02x%02x,Y"},    //$19 : ORA
    {0,0,"INVALID"},        //$1A : NOP
    {0,0,"INVALID"},        //$1B : SLO
    {0,0,"INVALID"},        //$1C : NOP
    {2,4,"ORA $%02x%02x,X"},    //$1D : ORA
    {2,7,"ASL $%02x%02x,X"},    //$1E : ASL
    {0,0,"INVALID"},        //$1F : SLO

    {2,6,"JSR $%02x%02x"},      //$20 : JSR
    {1,6,"AND $%02x%02x,X"},    //$21 : AND
    {0,0,"INVALID"},        //$22 : KIL
    {0,0,"INVALID"},        //$23 : RLA
    {1,3,"BIT $%02x"},        //$24 : BIT
    {1,3,"AND $%02x"},        //$25 : AND
    {1,5,"ROL $%02x"},        //$26 : ROL
    {0,0,"INVALID"},        //$27 : RLA
    {0,4,"PLP"},            //$28 : PLP
    {1,2,"AND #$%02x"},       //$29 : AND
    {0,2,"ROL A"},          //$2A : ROL
    {0,0,"INVALID"},        //$2B : ANC
    {2,4,"BIT $%02x%02x"},      //$2C : BIT
    {2,4,"AND $%02x%02x"},      //$2D : AND
    {2,6,"ROL $%02x%02x"},      //$2E : ROL
    {0,0,"INVALID"},        //$2F : RLA

    {1,2,"BMI %02x"},         //$30 : BMI
    {1,5,"AND ($%02x),Y"},    //$31 : AND
    {0,0,"INVALID"},        //$32 : KIL
    {0,0,"INVALID"},        //$33 : RLA
    {0,0,"INVALID"},        //$34 : NOP
    {1,4,"AND $%02x,X"},      //$35 : AND
    {1,6,"ROL $%02x,X"},      //$36 : ROL
    {0,0,"INVALID"},        //$37 : RLA
    {0,2,"SEC"},            //$38 : SEC
    {2,4,"AND $%02x%02x,Y"},    //$39 : AND
    {0,0,"INVALID"},        //$3A : NOP
    {0,0,"INVALID"},        //$3B : RLA
    {0,0,"INVALID"},        //$3C : NOP
    {2,4,"AND $%02x%02x,X"},    //$3D : AND
    {2,7,"ROL $%02x%02x,X"},    //$3E : ROL
    {0,0,"INVALID"},        //$3F : RLA

    {0,6,"RTI"},            //$40 : RTI
    {1,6,"EOR ($%02x,X)"},    //$41 : EOR
    {0,0,"INVALID"},        //$42 : KIL
    {0,0,"INVALID"},        //$43 : SRE
    {0,0,"INVALID"},        //$44 : NOP
    {1,3,"EOR $%02x"},        //$45 : EOR
    {1,5,"LSR $%02x"},        //$46 : LSR
    {0,0,"INVALID"},        //$47 : SRE
    {0,3,"PHA"},            //$48 : PHA
    {1,2,"EOR #$%02x"},       //$49 : EOR
    {0,2,"LSR A"},          //$4A : LSR
    {0,0,"INVALID"},        //$4B : ALR
    {2,3,"JMP $%02x%02x"},      //$4C : JMP
    {2,4,"EOR $%02x%02x"},      //$4D : EOR
    {2,6,"LSR $%02x%02x"},      //$4E : LSR
    {0,0,"INVALID"},        //$4F : SRE

    {1,2,"BVC %02x"},         //$50 : BVC
    {1,5,"EOR ($%02x),Y"},    //$51 : EOR
    {0,0,"INVALID"},        //$52 : KIL
    {0,0,"INVALID"},        //$53 : SRE
    {0,0,"INVALID"},        //$54 : NOP
    {1,4,"EOR $%02x,X"},      //$55 : EOR
    {1,6,"LSR $%02x,X"},      //$56 : LSR
    {0,0,"INVALID"},        //$57 : SRE
    {0,2,"CLI"},            //$58 : CLI
    {2,4,"EOR $%02x%02x,Y"},    //$59 : EOR
    {0,0,"INVALID"},        //$5A : NOP
    {0,0,"INVALID"},        //$5B : SRE
    {0,0,"INVALID"},        //$5C : NOP
    {2,4,"EOR $%02x%02x,X"},    //$5D : EOR
    {2,7,"LSR $%02x%02x,X"},    //$5E : LSR
    {0,0,"INVALID"},        //$5F : SRE

    {0,6,"RTS"},            //$60 : RTS
    {1,6,"ADC ($%02x,X)"},    //$61 : ADC
    {0,0,"INVALID"},        //$62 : KIL
    {0,0,"INVALID"},        //$63 : RRA
    {0,0,"INVALID"},        //$64 : NOP
    {1,3,"ADC $%02x"},        //$65 : ADC
    {1,5,"ROR $%02x"},        //$66 : ROR
    {0,0,"INVALID"},        //$67 : RRA
    {0,4,"PLA"},            //$68 : PLA
    {1,2,"ADC #$%02x"},       //$69 : ADC
    {0,2,"ROR A"},          //$6A : ROR
    {0,0,"INVALID"},        //$6B : ARR
    {2,5,"JMP ($%02x%02x)"},    //$6C : JMP
    {2,4,"ADC $%02x%02x"},      //$6D : ADC
    {2,6,"ROR $%02x%02x"},      //$6E : ROR
    {0,0,"INVALID"},        //$6F : RRA

    {1,2,"BVS %02x"},         //$70 : BVS
    {1,5,"ADC ($%02x),Y"},    //$71 : ADC
    {0,0,"INVALID"},        //$72 : KIL
    {0,0,"INVALID"},        //$73 : RRA
    {0,0,"INVALID"},        //$74 : NOP
    {1,4,"ADC $%02x,X"},      //$75 : ADC
    {1,6,"ROR $%02x,X"},      //$76 : ROR
    {0,0,"INVALID"},        //$77 : RRA
    {0,2,"SEI"},            //$78 : SEI
    {2,4,"ADC $%02x%02x,Y"},    //$79 : ADC
    {0,0,"INVALID"},        //$7A : NOP
    {0,0,"INVALID"},        //$7B : RRA
    {0,0,"INVALID"},        //$7C : NOP
    {2,4,"ADC $%02x%02x,X"},    //$7D : ADC
    {2,7,"ROR $%02x%02x,X"},    //$7E : ROR
    {0,0,"INVALID"},        //$7F : RRA

    {0,0,"INVALID"},        //$80 : NOP
    {1,6,"STA ($%02x,X)"},    //$81 : STA
    {0,0,"INVALID"},        //$82 : NOP
    {0,0,"INVALID"},        //$83 : SAX
    {1,3,"STY $%02x"},        //$84 : STY
    {1,3,"STA $%02x"},        //$85 : STA
    {1,3,"STX $%02x"},        //$86 : STX
    {0,0,"INVALID"},        //$87 : SAX
    {0,2,"DEY"},            //$88 : DEY
    {0,0,"INVALID"},        //$89 : NOP
    {0,2,"TXA"},            //$8A : TXA
    {0,0,"INVALID"},        //$8B : XAA
    {2,4,"STY $%02x%02x"},      //$8C : STY
    {2,4,"STA $%02x%02x"},      //$8D : STA
    {2,4,"STX $%02x%02x"},      //$8E : STX
    {0,0,"INVALID"},        //$8F : SAX

    {1,2,"BCC %02x"},         //$90 : BCC
    {1,6,"STA ($%02x),Y"},    //$91 : STA
    {0,0,"INVALID"},        //$92 : KIL
    {0,0,"INVALID"},        //$93 : AHX
    {1,4,"STY $%02x,X"},      //$94 : STY
    {1,4,"STA $%02x,X"},      //$95 : STA
    {1,4,"STA $%02x,Y"},      //$96 : STX
    {0,0,"INVALID"},        //$97 : SAX
    {0,2,"TYA"},            //$98 : TYA
    {2,5,"STA $%02x%02x,Y"},    //$99 : STA
    {0,2,"TXS"},            //$9A : TXS
    {0,0,"INVALID"},        //$9B : TAS
    {0,0,"INVALID"},        //$9C : SHY
    {2,5,"STA $%02x%02x,X"},    //$9D : STA
    {0,0,"INVALID"},        //$9E : SHX
    {0,0,"INVALID"},        //$9F : AHX

    {1,2,"LDY #$%02x"},       //$A0 : LDY
    {1,6,"LDA ($%02x,X)"},    //$A1 : LDA
    {1,2,"LDX #$%02x"},       //$A2 : LDX
    {0,0,"INVALID"},        //$A3 : LAX
    {1,3,"LDY $%02x"},        //$A4 : LDY
    {1,3,"LDA $%02x"},        //$A5 : LDA
    {1,3,"LDX $%02x"},        //$A6 : LDX
    {0,0,"INVALID"},        //$A7 : LAX
    {0,2,"TAY"},            //$A8 : TAY
    {1,2,"LDA #$%02x"},       //$A9 : LDA
    {0,2,"TAX"},            //$AA : TAX
    {0,0,"INVALID"},        //$AB : LAX
    {2,4,"LDY $%02x%02x"},      //$AC : LDY
    {2,4,"LDA $%02x%02x"},      //$AD : LDA
    {2,4,"LDX $%02x%02x"},      //$AE : LDX
    {0,0,"INVALID"},        //$AF : LAX

    {1,2,"BCS %02x"},         //$B0 : BCS
    {1,5,"LDA ($%02x),Y"},    //$B1 : LDA
    {0,0,"INVALID"},        //$B2 : KIL
    {0,0,"INVALID"},        //$B3 : LAX
    {1,4,"LDY $%02x,X"},      //$B4 : LDY
    {1,4,"LDA $%02x,X"},      //$B5 : LDA
    {1,4,"LDX $%02x,Y"},      //$B6 : LDX
    {0,0,"INVALID"},        //$B7 : LAX
    {0,2,"CLV"},            //$B8 : CLV
    {2,4,"LDA $%02x%02x,Y"},    //$B9 : LDA
    {0,2,"TSX"},            //$BA : TSX
    {0,0,"INVALID"},        //$BB : LAS
    {2,4,"LDY $%02x%02x,X"},    //$BC : LDY
    {2,4,"LDA $%02x%02x,X"},    //$BD : LDA
    {2,4,"LDX $%02x%02x,Y"},    //$BE : LDX
    {0,0,"INVALID"},        //$BF : LAX

    {1,2,"CPY #$%02x"},       //$C0 : CPY
    {1,6,"CMP ($%02x,X)"},    //$C1 : CMP
    {0,0,"INVALID"},        //$C2 : NOP
    {0,0,"INVALID"},        //$C3 : DCP
    {1,3,"CPY $%02x"},        //$C4 : CPY
    {1,3,"CMP $%02x"},        //$C5 : CMP
    {1,5,"DEC $%02x"},        //$C6 : DEC
    {0,0,"INVALID"},        //$C7 : DCP
    {0,2,"INY"},            //$C8 : INY
    {1,2,"CMP #$%02x"},       //$C9 : CMP
    {0,2,"DEX"},            //$CA : DEX
    {0,0,"INVALID"},        //$CB : AXS
    {2,4,"CPY $%02x%02x"},      //$CC : CPY
    {2,4,"CMP $%02x%02x"},      //$CD : CMP
    {2,6,"DEC $%02x%02x"},      //$CE : DEC
    {0,0,"INVALID"},        //$CF : DCP

    {1,2,"BNE %02x"},         //$D0 : BNE
    {1,5,"CMP ($%02x),Y"},    //$D1 : CMP
    {0,0,"INVALID"},        //$D2 : KIL
    {0,0,"INVALID"},        //$D3 : DCP
    {0,0,"INVALID"},        //$D4 : NOP
    {1,4,"CMP $%02x,X"},      //$D5 : CMP
    {1,6,"DEC $%02x,X"},      //$D6 : DEC
    {0,0,"INVALID"},        //$D7 : DCP
    {0,2,"CLD"},            //$D8 : CLD
    {2,4,"CMP $%02x%02x,Y"},    //$D9 : CMP
    {0,0,"INVALID"},        //$DA : NOP
    {0,0,"INVALID"},        //$DB : DCP
    {0,0,"INVALID"},        //$DC : NOP
    {2,4,"CMP $%02x%02x,X"},    //$DD : CMP
    {2,7,"DEC $%02x%02x,X"},    //$DE : DEC
    {0,0,"INVALID"},        //$DF : DCP

    {1,2,"CPX #$%02x"},       //$E0 : CPX
    {1,6,"SBC ($%02x,X)"},    //$E1 : SBC
    {0,0,"INVALID"},        //$E2 : NOP
    {0,0,"INVALID"},        //$E3 : ISC
    {1,3,"CPX $%02x"},        //$E4 : CPX
    {1,3,"SBC $%02x"},        //$E5 : SBC
    {1,5,"INC $%02x"},        //$E6 : INC 
    {0,0,"INVALID"},        //$E7 : ISC  
    {0,2,"INX"},            //$E8 : INX
    {1,2,"SBC #$%02x"},       //$E9 : SBC
    {0,2,"NOP"},            //$EA : NOP
    {0,0,"INVALID"},        //$EB : SBC
    {2,4,"CPX $%02x%02x"},      //$EC : CPX
    {2,4,"SBC $%02x%02x"},      //$ED : SBC
    {2,6,"INC $%02x%02x"},      //$EE : INC
    {0,0,"INVALID"},        //$EF : ISC

    {1,2,"BEQ %02x"},         //$F0 : BEQ
    {1,5,"SBC ($%02x),Y"},    //$F1 : SBC
    {0,0,"INVALID"},        //$F2 : KIL
    {0,0,"INVALID"},        //$F3 : ISC
    {0,0,"INVALID"},        //$F4 : NOP
    {1,4,"SBC $%02x,X"},      //$F5 : SBC
    {1,6,"INC $%02x,X"},      //$F6 : INC
    {0,0,"INVALID"},        //$F7 : ISC
    {0,2,"SED"},            //$F8 : SED
    {2,4,"SBC $%02x%02x,Y"},    //$F9 : SBC
    {0,0,"INVALID"},        //$FA : NOP
    {0,0,"INVALID"},        //$FB : ISC
    {0,0,"INVALID"},        //$FC : NOP
    {2,4,"SBC $%02x%02x,X"},    //$FD : SBC
    {2,7,"INC $%02x,%02x,X"},   //$FE : INC
    {0,0,"INVALID"}         //$FF : ISC
};

