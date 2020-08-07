next_state := STATE_NONE;
CASE state OF 
STATE_NONE:
	state := ENTER_STATE_bootup;
	event[EVENT_b1] := FALSE;
	event[EVENT_b2] := FALSE;
	event[EVENT_blip] := FALSE;
ENTER_STATE_bootup: 
	state := STATE_EXIT_bootup;
	next_state := STATE_ENTER_blink1;
	IF next_state = STATE_NONE THEN
		state := STATE_bootup;
	END_IF
STATE_bootup: 
EXIT_STATE_bootup: 
	state := next_state;
ENTER_STATE_blink1: 
	self->led1();
	event[EVENT_blip] = true;
	IF next_state = STATE_NONE THEN
		state := STATE_blink1;
	END_IF
STATE_blink1: 
	IF event[EVENT_blip] THEN
		event[EVENT_blip] := FALSE;
		self->led1();
		self->led1();
	END_IF
	IF event[EVENT_b1] THEN
		event[EVENT_b1] := FALSE;
		logger("ignoring button1 press, already in b1\n");
	END_IF
	IF event[EVENT_b2] THEN
		event[EVENT_b2] := FALSE;
		state := STATE_EXIT_blink1;
		next_state := STATE_ENTER_blink2;
	END_IF
EXIT_STATE_blink1: 
	self->led1();
	state := next_state;
ENTER_STATE_blink2: 
	self->led2();
	event[EVENT_blip] = true;
	IF next_state = STATE_NONE THEN
		state := STATE_blink2;
	END_IF
STATE_blink2: 
	IF event[EVENT_blip] THEN
		event[EVENT_blip] := FALSE;
		self->led2();
	END_IF
	IF event[EVENT_b1] THEN
		event[EVENT_b1] := FALSE;
		state := STATE_EXIT_blink2;
		next_state := STATE_ENTER_blink1;
	END_IF
	IF event[EVENT_b2] THEN
		event[EVENT_b2] := FALSE;
		logger("ignoring button2 press, already in b2\n");
	END_IF
EXIT_STATE_blink2: 
	self->led2();
	state := next_state;
END_CASE
