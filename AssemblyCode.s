            TTL Lab 10
;****************************************************************
;Name:  George Colgrove
;Date:  October 24,2017
;Class:  CMPE-250
;Section:  <2, 26, 2pm>
;---------------------------------------------------------------
;Keil Template for KL46
;R. W. Melton
;September 25, 2017
;****************************************************************
;Assembler directives
            THUMB
            GBLL  MIXED_ASM_C
MIXED_ASM_C SETL  {TRUE}
			EXPORT GetChar
			EXPORT PutChar
			EXPORT GetStringSB
			EXPORT Init_UART0_IRQ
			EXPORT PutNumHex
			EXPORT PutNumUB
			EXPORT PutStringSB
			EXPORT Count
			EXPORT RunStopWatch
			EXPORT PutNumU
			EXPORT Init_PIT_IRQ
			EXPORT SetupUARTVariables
			EXPORT UART0_IRQHandler
			EXPORT CheckChar
            OPT    64  			;Turn on listing macro expansions
;****************************************************************
;Include files
            GET  MKL46Z4.s     	;Included by start.s
            OPT  1   			;Turn on listing
;****************************************************************
;EQUates
Q_REC_SZ		EQU	18
Q_BUF_SZ		EQU	16
QT_BUF_SZ		EQU	16
IN_PTR			EQU	0
OUT_PTR			EQU	4
BUF_STRT		EQU	8
BUF_PAST		EQU	12
BUF_SIZE		EQU	16
NUM_ENQD		EQU	17
WORD			EQU	4
STRING			EQU	100
	

;---------------------------------------------------------------
;PORTx_PCRn (Port x pin control register n [for pin n])
;___->10-08:Pin mux control (select 0 to 8)
;Use provided PORT_PCR_MUX_SELECT_2_MASK
;---------------------------------------------------------------
;Port A
PORT_PCR_SET_PTA1_UART0_RX  EQU  (PORT_PCR_ISF_MASK :OR: \
                                  PORT_PCR_MUX_SELECT_2_MASK)
PORT_PCR_SET_PTA2_UART0_TX  EQU  (PORT_PCR_ISF_MASK :OR: \
                                  PORT_PCR_MUX_SELECT_2_MASK)
;---------------------------------------------------------------
;SIM_SCGC4
;1->10:UART0 clock gate control (enabled)
;Use provided SIM_SCGC4_UART0_MASK
;---------------------------------------------------------------
;SIM_SCGC5
;1->09:Port A clock gate control (enabled)
;Use provided SIM_SCGC5_PORTA_MASK
;---------------------------------------------------------------
;SIM_SOPT2
;01=27-26:UART0SRC=UART0 clock source select
;         (PLLFLLSEL determines MCGFLLCLK' or MCGPLLCLK/2)
; 1=   16:PLLFLLSEL=PLL/FLL clock select (MCGPLLCLK/2)
SIM_SOPT2_UART0SRC_MCGPLLCLK  EQU  \
                                 (1 << SIM_SOPT2_UART0SRC_SHIFT)
SIM_SOPT2_UART0_MCGPLLCLK_DIV2 EQU \
    (SIM_SOPT2_UART0SRC_MCGPLLCLK :OR: SIM_SOPT2_PLLFLLSEL_MASK)
;---------------------------------------------------------------
;SIM_SOPT5
; 0->   16:UART0 open drain enable (disabled)
; 0->   02:UART0 receive data select (UART0_RX)
;00->01-00:UART0 transmit data select source (UART0_TX)
SIM_SOPT5_UART0_EXTERN_MASK_CLEAR  EQU  \
                               (SIM_SOPT5_UART0ODE_MASK :OR: \
                                SIM_SOPT5_UART0RXSRC_MASK :OR: \
                                SIM_SOPT5_UART0TXSRC_MASK)
;---------------------------------------------------------------
;UART0_BDH
;    0->  7:LIN break detect IE (disabled)
;    0->  6:RxD input active edge IE (disabled)
;    0->  5:Stop bit number select (1)
;00001->4-0:SBR[12:0] (UART0CLK / [9600 * (OSR + 1)]) 
;UART0CLK is MCGPLLCLK/2
;MCGPLLCLK is 96 MHz
;MCGPLLCLK/2 is 48 MHz
;SBR = 48 MHz / (9600 * 16) = 312.5 --> 312 = 0x138
UART0_BDH_9600  EQU  0x01
;---------------------------------------------------------------
;UART0_BDL
;0x38->7-0:SBR[7:0] (UART0CLK / [9600 * (OSR + 1)])
;UART0CLK is MCGPLLCLK/2
;MCGPLLCLK is 96 MHz
;MCGPLLCLK/2 is 48 MHz
;SBR = 48 MHz / (9600 * 16) = 312.5 --> 312 = 0x138
UART0_BDL_9600  EQU  0x38
;---------------------------------------------------------------
;UART0_C1
;0-->7:LOOPS=loops select (normal)
;0-->6:DOZEEN=doze enable (disabled)
;0-->5:RSRC=receiver source select (internal--no effect LOOPS=0)
;0-->4:M=9- or 8-bit mode select 
;        (1 start, 8 data [lsb first], 1 stop)
;0-->3:WAKE=receiver wakeup method select (idle)
;0-->2:IDLE=idle line type select (idle begins after start bit)
;0-->1:PE=parity enable (disabled)
;0-->0:PT=parity type (even parity--no effect PE=0)
UART0_C1_8N1  EQU  0x00
;---------------------------------------------------------------
;UART0_C2
;0-->7:TIE=transmit IE for TDRE (disabled)
;0-->6:TCIE=transmission complete IE for TC (disabled)
;0-->5:RIE=receiver IE for RDRF (disabled)
;0-->4:ILIE=idle line IE for IDLE (disabled)
;1-->3:TE=transmitter enable (enabled)
;1-->2:RE=receiver enable (enabled)
;0-->1:RWU=receiver wakeup control (normal)
;0-->0:SBK=send break (disabled, normal)
UART0_C2_T_R  EQU  (UART0_C2_TE_MASK :OR: UART0_C2_RE_MASK)
;---------------------------------------------------------------
;UART0_C3
;0-->7:R8T9=9th data bit for receiver (not used M=0)
;           10th data bit for transmitter (not used M10=0)
;0-->6:R9T8=9th data bit for transmitter (not used M=0)
;           10th data bit for receiver (not used M10=0)
;0-->5:TXDIR=UART_TX pin direction in single-wire mode
;            (no effect LOOPS=0)
;0-->4:TXINV=transmit data inversion (not inverted)
;0-->3:ORIE=overrun IE for OR (disabled)
;0-->2:NEIE=noise error IE for NF (disabled)
;0-->1:FEIE=framing error IE for FE (disabled)
;0-->0:PEIE=parity error IE for PF (disabled)
UART0_C3_NO_TXINV  EQU  0x00
;---------------------------------------------------------------
;UART0_C4
;    0-->  7:MAEN1=match address mode enable 1 (disabled)
;    0-->  6:MAEN2=match address mode enable 2 (disabled)
;    0-->  5:M10=10-bit mode select (not selected)
;01111-->4-0:OSR=over sampling ratio (16)
;               = 1 + OSR for 3 <= OSR <= 31
;               = 16 for 0 <= OSR <= 2 (invalid values)
UART0_C4_OSR_16           EQU  0x0F
UART0_C4_NO_MATCH_OSR_16  EQU  UART0_C4_OSR_16
;---------------------------------------------------------------
;UART0_C5
;  0-->  7:TDMAE=transmitter DMA enable (disabled)
;  0-->  6:Reserved; read-only; always 0
;  0-->  5:RDMAE=receiver full DMA enable (disabled)
;000-->4-2:Reserved; read-only; always 0
;  0-->  1:BOTHEDGE=both edge sampling (rising edge only)
;  0-->  0:RESYNCDIS=resynchronization disable (enabled)
UART0_C5_NO_DMA_SSR_SYNC  EQU  0x00
;---------------------------------------------------------------
;UART0_S1
;0-->7:TDRE=transmit data register empty flag; read-only
;0-->6:TC=transmission complete flag; read-only
;0-->5:RDRF=receive data register full flag; read-only
;1-->4:IDLE=idle line flag; write 1 to clear (clear)
;1-->3:OR=receiver overrun flag; write 1 to clear (clear)
;1-->2:NF=noise flag; write 1 to clear (clear)
;1-->1:FE=framing error flag; write 1 to clear (clear)
;1-->0:PF=parity error flag; write 1 to clear (clear)
UART0_S1_CLEAR_FLAGS  EQU  0x1F
;---------------------------------------------------------------
;UART0_S2
;1-->7:LBKDIF=LIN break detect interrupt flag (clear)
;             write 1 to clear
;1-->6:RXEDGIF=RxD pin active edge interrupt flag (clear)
;              write 1 to clear
;0-->5:(reserved); read-only; always 0
;0-->4:RXINV=receive data inversion (disabled)
;0-->3:RWUID=receive wake-up idle detect
;0-->2:BRK13=break character generation length (10)
;0-->1:LBKDE=LIN break detect enable (disabled)
;0-->0:RAF=receiver active flag; read-only
UART0_S2_NO_RXINV_BRK10_NO_LBKDETECT_CLEAR_FLAGS  EQU  0xC0
	
	
NVIC_ICPR_UART0_MASK			EQU	UART0_IRQ_MASK
UART0_IRQ_PRIORITY				EQU	3
NVIC_IPR_UART0_MASK				EQU	(3 << UART0_PRI_POS)
NVIC_IPR_UART0_PRI_3			EQU (UART0_IRQ_PRIORITY << UART0_PRI_POS)
NVIC_ISER_UART0_MASK			EQU UART0_IRQ_MASK
UART0_C2_T_RI					EQU	(UART0_C2_RIE_MASK :OR: UART0_C2_T_R)
UART0_C2_TI_RI					EQU	(UART0_C2_TIE_MASK :OR: UART0_C2_T_RI)
	
	
PIT_MCR_EN_FRZ					EQU	PIT_MCR_FRZ_MASK
PIT_TCTRL_CH_IE					EQU \
				(PIT_TCTRL_TIE_MASK :OR: PIT_TCTRL_TEN_MASK)
PIT_LDVAL_10ms	EQU	239999
PIT_IRQ_PRI		EQU	0
	
;---------------------------------------------------------------


;****************************************************************
;Program
;Linker requires Reset_Handler
            AREA    MyCode,CODE,READONLY
			ENTRY
;>>>>> begin subroutine code <<<<<
SetupUARTVariables	PROC	{R0,R14}, {}
					PUSH	{R0-R1,LR}	
					;setup communication queues
					LDR		R0,=RxBuffer
					LDR		R1,=RxRecord
					MOVS	R2,#QT_BUF_SZ
					BL		InitQueue
					LDR		R0,=TxBuffer
					LDR		R1,=TxRecord
					MOVS	R2,#QT_BUF_SZ
					BL		InitQueue
					
					POP		{R0-R1,PC}
			
		ENDP


Init_PIT_IRQ		PROC	{R0,R14}, {}
					PUSH	{R0-R3,LR}
					;enable module clock
					LDR		R0,=SIM_SCGC6
					LDR		R1,=SIM_SCGC6_PIT_MASK
					LDR		R2,[R0,#0]
					ORRS	R2,R2,R1
					STR		R2,[R0,#0]
					;Disable PIT clock
					;LDR		R0,=PIT_BASE
					;LDR		R1,=PIT_MCR_MDIS_MASK
					;STR		R1,[R0,#PIT_MCR_OFFSET]
					;Set Priority to 0
					LDR		R0,=PIT_IPR
					LDR		R1,=(PIT_IRQ_PRI << PIT_PRI_POS)
					LDR		R2,[R0,#0]
					ORRS	R2,R2,R1
					STR		R2,[R0,#0]; might have to be R1
					;Clear PIT Channel 0 interrupt
					LDR		R0,=PIT_CH0_BASE
					LDR		R1,=PIT_TFLG_TIF_MASK
					STR		R1,[R0,#PIT_TFLG_OFFSET]
					;Unmask PIT Interrupts
					LDR		R0,=NVIC_ISER
					LDR		R1,=PIT_IRQ_MASK
					STR		R1,[R0,#0]
					;Enable PIT clock
					LDR		R0,=PIT_BASE
					LDR		R1,=PIT_MCR_EN_FRZ
					STR		R1,[R0,#PIT_MCR_OFFSET]					
					;Set timer 0 period for .01s
					LDR		R0,=PIT_CH0_BASE
					LDR		R1,=PIT_LDVAL_10ms
					STR		R1,[R0,#PIT_LDVAL_OFFSET]
					;Enable PIT timer interrupts
					LDR		R0,=PIT_CH0_BASE
					MOVS	R1,#PIT_TCTRL_CH_IE
					STR		R1,[R0,#PIT_TCTRL_OFFSET]
					
					POP		{R0-R3,PC}
			
		ENDP
	LTORG
Init_UART0_IRQ		PROC	{R0,R14}, {}
					PUSH	{R0,R1,R2}
					LDR		R0,=UART0_IPR
					LDR		R1,=NVIC_IPR_UART0_PRI_3
					LDR		R3,[R0,#0]
					ORRS	R3,R3,R2
					STR		R3,[R0,#0]
					
					LDR		R0,=NVIC_ICPR
					LDR		R1,=NVIC_ICPR_UART0_MASK
					STR		R1,[R0,#0]
					
					LDR		R0,=NVIC_ISER
					LDR		R1,=NVIC_ISER_UART0_MASK
					STR		R1,[R0,#0]
					
					
					
					
					LDR		R0,=SIM_SOPT2
					LDR		R1,=SIM_SOPT2_UART0SRC_MASK
					LDR		R2,[R0,#0]
					BICS	R2,R2,R1
					LDR		R1,=SIM_SOPT2_UART0_MCGPLLCLK_DIV2
					ORRS	R2,R2,R1
					STR		R2,[R0,#0]
					;Enable external connection for UART0
					LDR		R0,=SIM_SOPT5
					LDR		R1,=SIM_SOPT5_UART0_EXTERN_MASK_CLEAR
					LDR		R2,[R0,#0]
					BICS	R2,R2,R1
					STR		R2,[R0,#0]
					
					LDR		R0,=SIM_SCGC4
					LDR		R1,=SIM_SCGC4_UART0_MASK
					LDR		R2,[R0,#0]
					ORRS	R2,R2,R1
					STR		R2,[R0,#0]
					
					LDR		R1,=SIM_SCGC5_PORTA_MASK
					LDR		R2,[R0,#0]
					ORRS	R2,R2,R1
					STR		R2,[R0,#0]
					
					
					LDR		R0,=PORTA_PCR1
					LDR		R1,=PORT_PCR_SET_PTA1_UART0_RX
					STR		R1,[R0,#0]
					
					LDR		R0,=PORTA_PCR2
					LDR		R1,=PORT_PCR_SET_PTA2_UART0_TX
					STR		R1,[R0,#0]
					POP		{R0,R1,R2}
					
					; start up listening
					
					LDR		R0,=UART0_BASE	
					MOVS	R1,#UART0_C2_T_R
					LDRB	R2,[R0,#UART0_C2_OFFSET]
					BICS	R2,R2,R1
					STRB	R2,[R0,#UART0_C2_OFFSET]
					
					MOVS	R1,#UART0_BDH_9600
					STRB	R1,[R0,#UART0_BDH_OFFSET]
					MOVS	R1,#UART0_BDL_9600
					STRB	R1,[R0,#UART0_BDL_OFFSET]
					MOVS	R1,#UART0_C1_8N1
					STRB	R1,[R0,#UART0_C1_OFFSET]
					MOVS	R1,#UART0_C3_NO_TXINV
					STRB	R1,[R0,#UART0_C3_OFFSET]
					MOVS	R1,#UART0_C4_NO_MATCH_OSR_16
					STRB	R1,[R0,#UART0_C4_OFFSET]
					MOVS	R1,#UART0_C5_NO_DMA_SSR_SYNC
					STRB	R1,[R0,#UART0_C5_OFFSET]
					MOVS	R1,#UART0_S1_CLEAR_FLAGS
					STRB	R1,[R0,#UART0_S1_OFFSET]
					MOVS	R1,#UART0_S2_NO_RXINV_BRK10_NO_LBKDETECT_CLEAR_FLAGS
					STRB	R1,[R0,#UART0_S2_OFFSET]
					;MOVS	R1,#UART0_C2_T_R
					;STRB	R1,[R0,#UART_C2_OFFSET]
					MOVS	R1,#UART0_C2_T_RI
					STRB	R1,[R0,#UART_C2_OFFSET]					
					BX		LR
			
		ENDP
	LTORG
   

PutChar				PROC	{R0,R14},{}
					PUSH	{R0-R1,LR}
PCLoop				NOP					
					CPSID	I
					LDR		R1,=TxRecord
					BL		Enqueue
					CPSIE	I	
					BCS		PCLoop
					LDR		R0,=UART0_BASE	
					MOVS	R1,#UART0_C2_TI_RI
					STRB	R1,[R0,#UART_C2_OFFSET]	
					
					POP		{R0-R1,PC}	
		ENDP

GetChar				PROC	{R1,R14},{}
					PUSH	{R1,R2,R3,LR}
GCLoop				CPSID	I
					LDR		R1,=RxRecord
					BL		Dequeue
					CPSIE	I	
					BCS		GCLoop
					POP		{R1,R2,R3,PC}
		ENDP

CheckChar			PROC	{R1,R14},{}
					PUSH	{R1,R2,R3,LR}
					CPSID	I
					LDR		R1,=RxRecord
					LDRB	R0,[R1,#NUM_ENQD]
					CPSIE	I	
					POP		{R1,R2,R3,PC}
		ENDP
			
;Sends string from pointer
;R0 pointer location ro read from
;R1 Buffer size
;uses PutChar
PutStringSB			PROC	{R0,R14},{}			
					PUSH	{R0,R1,R2,LR}
					SUBS	R1,R1,#1
					ADDS	R2,R0,R1					;get max buffer length and put in R2
					MOVS	R1,R0						;move pointer to R1	
PSLoop				CMP		R1,R2						;check if at end of buffer
					BEQ		PSEndLoop					;get out of loop if end of loop
					LDRB	R0,[R1,#0]					;load char from string
					ADDS	R1,R1,#1					;increment pointer
					CMP		R0,#0						;check if null
					BEQ		PSEndLoop					;if true get out of loop
					BL		PutChar						;display char
					B		PSLoop						;loop
PSEndLoop			POP		{R0,R1,R2,PC}
					
					
	ENDP
;
;
;
;
ClearTimerCount		PROC	{R0,R14},{}			
					PUSH	{R0-R2,LR}
					LDR		R0,=Count
					MOVS	R1,#0
					STR		R1,[R0,#0]					
					POP		{R0-R2,PC}		
	ENDP		
;
;
;
;
StartTimer			PROC	{R0,R14},{}			
					PUSH	{R0-R2,LR}
					LDR		R0,=RunStopWatch
					MOVS	R1,#1
					STRB	R1,[R0,#0]
					POP		{R0-R2,PC}		
	ENDP
;
;
;
;
StopTimer			PROC	{R0,R14},{}			
					PUSH	{R0-R2,LR}
					LDR		R0,=RunStopWatch
					MOVS	R1,#0
					STRB	R1,[R0,#0]				
					POP		{R0-R2,PC}		
	ENDP

		
;Gets string from input
;R0 pointer location to save to
;R1 Buffer size
;uses GetChar and PutChar
GetStringSB			PROC	{R0,R14},{}			
					PUSH	{R0,R1,R2,LR}
					SUBS	R1,R1,#1
					ADDS	R2,R0,R1					;get max buffer length and put in R2
					MOVS	R1,R0						;move pointer to R1
GSLoop				CMP		R1,R2						;check if it at the end
					BEQ		GSWaitingCR					;if at end go and wait for carriage return
					BL		GetChar						;get char from console
					CMP		R0,#13			
					BEQ		GSEndLoop					;get out of loop if carriage returrn
					CMP		R0,#31			
					BLS		GSLoop						;dont do anything if ascii is below or equal to 31
					CMP		R0,#127						
					BEQ		GSLoop						;dont do anything if backspace
					BL		PutChar						;echo the character to console
					STRB	R0,[R1,#0]					;store char in memory
					ADDS	R1,R1,#1					;increment the pointer
					B		GSLoop						;loop
GSWaitingCR			BL		GetChar						;get char
					CMP		R0,#13						;check if carriage return
					BNE		GSWaitingCR					;check if carriage return	
GSEndLoop			BL		PutChar						;echo last char to terminal
					MOVS	R0,#0						;put null in R0
					STRB	R0,[R1,#0]					;store in memory
					POP		{R0,R1,R2,PC}	
	ENDP
;converts binary into a decimal to print to screen
;R0 number to print to screen
;Uses DIVU,HexToASCII,PutChar
PutNumU				PROC	{R0,R14},{}		
					PUSH	{R0,R1,R2,LR}
					LDR		R2,=0x3B9ACA00				;load 1,000,000,000 into R2
					MOVS	R1,R0						;move number to R1
NUMULoop1			CMP		R2,#0						;check pos equals 0
					BEQ		DIV1						
					MOVS	R0,R2						;copy current pos into R0
					BL		DIVU						;divide current number by current pos
					PUSH	{R0,R1}
					MOVS	R1,R2						;move R2 into R1
					LSRS	R1,R1,#1					;shift R1 right by 1 aka divide by 2
					MOVS	R0,#5						;place 5 into R0
					BL		DIVU						;equation (R1/2)/5 = R1/10
					MOVS	R2,R0						;move result into R2
					POP		{R0,R1}
					CMP		R0,#0						;check if R0 is 0
					BEQ		NUMULoop1					;loop if it is still 0
					B		SKIP						;if not you have reached the first number so skip getting number
DIV1				MOVS	R0,R2						;move R2 into R0
					BL		DIVU						;divide R1 / current pos
					PUSH	{R0,R1}	
					MOVS	R1,R2						;move current pos into R1
					LSRS	R1,R1,#1					;shift R1 right by 1 aka divide by 2
					MOVS	R0,#5						;place 5 into R0					
					BL		DIVU						;equation (R1/2)/5 = R1/10
					MOVS	R2,R0						;move result back into R2
					POP		{R0,R1}
SKIP				PUSH	{R0}	
					BL		HexToASCII					;convert number to ASCII
					BL		PutChar						;print char number to screen
					POP		{R0}
					CMP		R2,#0						;check if it has printed full number
					BNE		DIV1						;loop if not
					POP		{R0,R1,R2,PC}
	ENDP
;converts number to ascii version
;R0 number
;output R0 ascii
HexToASCII			PROC	{R1,R14},{}
					PUSH	{LR}
					CMP		R0,#10						;check if below 10
					BHS		HTAHIGHER			
					ADDS	R0,R0,#'0'					;add the ascii '0'
					B 		HTAEND
HTAHIGHER			CMP		R0,#16						;check if below 16
					BHS		HTAERROR
					ADDS	R0,R0,#('A'-10)				;add the ascii 'A' subtract 10
					B		HTAEND
HTAERROR			MOVS	R0,#'?'						;else return my error character
HTAEND				POP		{PC}
	ENDP


;my new DIVU that uses long division with binary
;R0 divisor
;R1 number
;output R0 answer
;output R1 remainder
DIVU				PROC  {R2-R14}, {}	; R1/R0=R0 R R1
					PUSH	{R2,R3,R4,LR}
					CMP		R0, #0		;check if zero
					BEQ		DIVUBYZERO
					CMP		R0,R1
					BHI		LDIVISOR2
					MOVS	R4,R0
					MOVS	R3,#0		; total
					MOVS	R2,#1		; current pos
DIVULOOP1			CMP		R0,R1		; check if divisor is larger than number
					BHI		DIVUSEND
					LSLS	R2,R2,#1	;left shift pos over by one
					LSLS	R0,R0,#1	;left shift divisor over by one
					B		DIVULOOP1
DIVUSEND			LSRS	R2,R2,#1	;right shift pos over by one
					LSRS	R0,R0,#1    ;right shift divisor over by one
DIVULOOP2			SUBS	R1,R1,R0	;subtract divisor from number
					ADDS	R3,R3,R2	;add pos to result
					LSRS	R2,R2,#1	;right shift pos over by one
					LSRS	R0,R0,#1    ;right shift divisor over by one
					CMP		R4,R0		;end loop if divisor smaller than init divisor
					BHI		DIVUELOOP
					CMP		R1,R0		;check if number is larger than divisor 
					BHS		DIVULOOP2
DIVULOOP3			LSRS	R2,R2,#1	;right shift pos over by one
					LSRS	R0,R0,#1    ;right shift divisor over by one
					CMP		R4,R0		;end loop if divisor smaller than init divisor
					BHI		DIVUELOOP
					CMP		R1,R0		;check if number is larger than divisor 
					BHS		DIVULOOP2
					CMP		R1,R0		;else loop
					BLO		DIVULOOP3
DIVUELOOP			NOP			
					MOVS	R0,R3		;put result in R0
					B		CLEARDIVU
DIVUBYZERO			NOP
					PUSH	{R0}		;save registers in stack
					PUSH	{R1}
					MRS		R0, APSR
					MOVS	R1,	#0x20
					LSLS	R1,R1,#24
					ORRS	R0,R0,R1
					MSR		APSR,R0		;Set C bit
					POP		{R1}		;get numbers from stack
					POP		{R0}
					B		ENDDIVU
LDIVISOR2 			NOP
					MOVS	R0, #0	
CLEARDIVU			NOP
					PUSH	{R0,R1}		;save register in stack
					MRS		R0, APSR
					MOVS	R1,	#0x20
					LSLS	R1,R1,#24
					BICS	R0,R0,R1
					MSR		APSR,R0		;clear C bit
					POP		{R0,R1}		;retrieve registers from stack
ENDDIVU				POP		{R2,R3,R4,PC}
	ENDP
;		
;	***INITQUEUE***	
;	R0 - Queue buffer pointer
;	R1 - Queue record pointer
;		
		
InitQueue			PROC	{R0-R14},{}
					PUSH	{R0,R1,R2,LR}
					STR   	R0,[R1,#IN_PTR] 	;set in pointer
					STR   	R0,[R1,#OUT_PTR] 	;set out pointer
					STR   	R0,[R1,#BUF_STRT]	;set buffer start
					ADDS  	R0,R0,R2 			;add R0 with R1 for size
					STR   	R0,[R1,#BUF_PAST] 	;set buffer past
					STRB  	R2,[R1,#BUF_SIZE] 	;set buffer size
					MOVS  	R0,#0 				
					STRB  	R0,[R1,#NUM_ENQD] 	;set number enqueued to 0
					POP		{R0,R1,R2,PC}
	ENDP

;		
;	***DEQUEUE***
;	R1 - Queue record pointer
;	Returns - R0 dequeued value	
;		
Dequeue				PROC	{R1-R14},{}
					PUSH	{R1,R2,R3,R4,LR}					
					LDRB	R2,[R1,#NUM_ENQD]	;load number enqueued
					CMP		R2,#0				;check if empty
					BEQ		DQError
					SUBS	R2,R2,#1			;subtract one 
					STRB	R2,[R1,#NUM_ENQD]	;store back in queue
					LDR		R3,[R1,#IN_PTR]		;get in pointer
					LDRB	R0,[R3,#0]			;get value to dequeue
					ADDS	R3,R3,#1			;add one to the address
					LDR		R4,[R1,#BUF_PAST]	;get the end address
					CMP		R4,R3				;check if at end
					BNE		DQContinue
					LDR		R3,[R1,#BUF_STRT]	;get wrap
DQContinue			STR		R3,[R1,#IN_PTR]		;save updated pointer in, in pointer
					B		DQSuccess
DQError				NOP
					PUSH	{R0}		;save registers in stack
					PUSH	{R1}
					MRS		R0, APSR
					MOVS	R1,	#0x20
					LSLS	R1,R1,#24
					ORRS	R0,R0,R1
					MSR		APSR,R0		;Set C bit
					POP		{R1}		;get numbers from stack
					POP		{R0}
					B		DQEnd
DQSuccess			NOP
					PUSH	{R0,R1}		;save register in stack
					MRS		R0, APSR
					MOVS	R1,	#0x20
					LSLS	R1,R1,#24
					BICS	R0,R0,R1
					MSR		APSR,R0		;clear C bit
					POP		{R0,R1}		;retrieve registers from stack					
DQEnd				POP		{R1,R2,R3,R4,PC}	
	ENDP
	
;		
;	***ENQUEUE***
;	R0 - Value to enqueue
;	R1 - Queue record pointer	
;								
Enqueue				PROC	{R0-R14},{}
					PUSH	{R0,R1,R2,R3,LR}
					
					
					LDRB	R2,[R1,#NUM_ENQD]	;get number enqueued
					LDRB	R3,[R1,#BUF_SIZE]	;get buffer size
					CMP		R2,R3				;check if full
					BEQ		EQError				;error if full
					ADDS	R2,R2,#1			;add one to number enqueued
					STRB	R2,[R1,#NUM_ENQD]	;store updated value
					LDR		R2,[R1,#OUT_PTR]	;load out pointer
					STRB	R0,[R2,#0]			;store value to enqueue
					LDR		R0,[R1,#BUF_PAST]	;load buffer past address
					ADDS	R2,R2,#1			;add one to it
					CMP		R2,R0				;if equal wrap
					BNE		EQContinue				
					LDR		R2,[R1,#BUF_STRT]	;get buffer start to wrap
EQContinue			STR		R2,[R1,#OUT_PTR]	;store the out pointer update
					B		EQSuccess
EQError				NOP
					PUSH	{R0}		;save registers in stack
					PUSH	{R1}
					MRS		R0, APSR
					MOVS	R1,	#0x20
					LSLS	R1,R1,#24
					ORRS	R0,R0,R1
					MSR		APSR,R0		;Set C bit
					POP		{R1}		;get numbers from stack
					POP		{R0}	
					B		EQEnd
EQSuccess			NOP
					PUSH	{R0,R1}		;save register in stack
					MRS		R0, APSR
					MOVS	R1,	#0x20
					LSLS	R1,R1,#24
					BICS	R0,R0,R1
					MSR		APSR,R0		;clear C bit
					POP		{R0,R1}		;retrieve registers from stack					
EQEnd				POP		{R0,R1,R2,R3,PC}
	ENDP

;		
;	***PUTNUMHEX***
;	R0 - Hex number
;	Uses: HexToASCII, PutChar		
;							
PutNumHex			PROC	{R0-R14},{}
					PUSH	{R0,R1,R2,R3,LR}
					LDR		R3,=0xF0000000		;bit mask
					MOVS	R2,#8				;set counter to 8
PNHLoop				MOVS	R1,R0				;copy R0 into R1	
					ANDS	R0,R0,R3			;mask R0 with R3
					LSRS	R0,R0,#28			;shift result over to the first byte
					BL		HexToASCII			;convert to ASCII
					BL		PutChar				;print out
					LSLS	R1,R1,#4			;shift R1 over by a word
					MOVS	R0,R1				;copy R1 back to R0
					SUBS	R2,R2,#1			;subtract the counter
					CMP		R2,#0				;check if counter has hit 0
					BHI		PNHLoop				;exit if it has hit 0
					POP		{R0,R1,R2,R3,PC}
	ENDP

;		
;	***PUTNUMUB***	
;	R0 - Byte number
;	Uses: PutNumU
;						
PutNumUB			PROC	{R0-R14},{}
					PUSH	{R0,R1,R2,R3,LR}
					LDR		R1,=0xFF			;bit mask
					ANDS	R0,R0,R1			;and with bitmask
					BL		PutNumU				;print to terminal
					POP		{R0,R1,R2,R3,PC}
	ENDP
;		
;	***CONVERTTOUPPER***
;	R0 - char number
;	Return - R0 uppercase letter
;
ConvertToUpper		PROC	{R1,R14},{}
					PUSH	{R1}
					MOVS	R1,#'z'						;move lower case z into R1
					CMP		R0,R1						;check if R0 is less
					BHI		DONECU						;if not skip steps
					MOVS	R1,#'a'						;move lower case a into R1
					CMP		R0,R1						;check if R0 is more
					BLO		DONECU						;if not skip
					SUBS	R1,R1,#'A'					;subtract upper case A from a to get difference
					SUBS	R0,R0,R1					;subtract difference R0
DONECU				POP		{R1}
					BX		LR
		ENDP	
			

UART0_IRQHandler		
UART0_ISR			PROC	{R0-R14},{}
					PUSH	{LR}
					CPSID	I
					PUSH	{R4}
					LDR		R0,=UART0_BASE	
					MOVS	R1,#UART0_C2_TIE_MASK
					LDRB	R2,[R0,#UART0_C2_OFFSET]
					BICS	R1,R1,R2
					BNE		CheckRx
					MOVS	R1,#UART0_S1_TDRE_MASK
					LDRB	R2,[R0,#UART0_S1_OFFSET]
					BICS	R1,R1,R2
					BNE		CheckRx
					LDR		R1,=TxRecord
					MOVS	R2,R0
					PUSH	{R0}
					BL		Dequeue
					BCS		SendError
					STRB	R0,[R2,#UART0_D_OFFSET]
					POP		{R0}
					B		CheckRx
SendError			POP		{R0}
					MOVS	R1,#UART0_C2_T_RI
					STRB	R1,[R0,#UART_C2_OFFSET]	
CheckRx				MOVS	R1,#UART0_S1_RDRF_MASK
					LDRB	R2,[R0,#UART0_S1_OFFSET]
					BICS	R1,R1,R2
					BNE		UART0Exit
					MOVS	R2,R0
					LDRB	R0,[R0,#UART0_D_OFFSET] 
					LDR		R1,=RxRecord
					BL		Enqueue
UART0Exit			POP		{R4}
					POP		{PC}
					CPSIE	I
	ENDP
		
PIT_ISR				PROC	{R1,R14},{}
					PUSH	{R1-R4,LR}
					LDR		R0,=RunStopWatch
					LDRB	R0,[R0,#0]
					CMP		R0,#0
					BEQ		PIT_ISR_EXIT
					LDR		R0,=Count
					LDR		R1,[R0,#0]
					ADDS	R1,R1,#1
					STR		R1,[R0,#0]
PIT_ISR_EXIT		LDR		R0,=PIT_CH0_BASE
					LDR		R1,=PIT_TFLG_TIF_MASK
					STR		R1,[R0,#PIT_TFLG_OFFSET]			
					POP		{R1-R4,PC}
		ENDP	
					
					
;>>>>>   end subroutine code <<<<<
            ALIGN
;****************************************************************
;Vector Table Mapped to Address 0 at Reset
;Linker requires __Vectors to be exported
;****************************************************************
;Constants
            AREA    MyConst,DATA,READONLY
;>>>>> begin constants here <<<<<
ENTERTEXT	DCB		"Enter your name.\r\n>",0
ENTERDATE	DCB		"Enter the date.\r\n>",0
ENTERTA 	DCB		"Enter the last name of a 250 lab TA.\r\n>",0
GOODBYE     DCB     "Thank You. Goodbye!",0
RESULT		DCB		" x 0.01 s\r\n",0
NEWLINE		DCB		"\r\n",0
;>>>>>   end constants here <<<<<
            ALIGN
;****************************************************************
;Variables
            AREA    MyData,DATA,READWRITE
;>>>>> begin variables here <<<<<
;byte vars
RunStopWatch	SPACE	1
	
			ALIGN
;word vars

QBuffer 		SPACE  	Q_BUF_SZ
QRecord			SPACE	Q_REC_SZ
			ALIGN
RxBuffer 		SPACE  	QT_BUF_SZ
RxRecord		SPACE	Q_REC_SZ
			ALIGN
TxBuffer 		SPACE  	QT_BUF_SZ
TxRecord		SPACE	Q_REC_SZ
			ALIGN
Count			SPACE	WORD	
			ALIGN
Name			SPACE	STRING
;>>>>>   end variables here <<<<<
            ALIGN
            END