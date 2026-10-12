StartVert		.equ	3
NumVerts		.equ	4

.init_vf_all
.init_vi_all

.syntax new

.vu

--enter
--endenter

	iaddiu		iVert, vi00, 0				; Start vertex counter
	iaddiu		iVertPtr, vi00, StartVert	; Point to the first vert
	iaddiu		iNumVerts, vi00, NumVerts	; Load the loop end condition
	loi			2048.0						; Put 2048 into the I register

loop:										; For each vertex
--LoopCS 4,0
	lq.xy		fVert, 0(iVertPtr)			; Load the XY components of the vert
	add.xy		fVert, fVert, i				; Add 2048 to the XY components
	ftoi4.xy	fVert, fVert				; Convert to 12:4 fixed point
	sq.xy		fVert, 0(iVertPtr)			; Store the newly converted data
	iaddiu		iVert, iVert, 1				; Increment the vertex counter
	iaddiu		iVertPtr, iVertPtr, 3		; Increment the vertex pointer
	ibne		iVert, iNumVerts, loop		; Branch back to "loop" label if
											; iVert and iNumVerts are not equal
	xgkick		vi00						; Kick the data at VUMEM location 0
											; to the GS
--exit
--endexit

.end
