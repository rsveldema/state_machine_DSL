CASE state OF 
StateMachineStateNames.STATE_NONE:
	state := StateMachineStateNames.ENTER_STATE_bootup;
	events[EventType.EVENT_b1] := FALSE;
	events[EventType.EVENT_b2] := FALSE;
	events[EventType.EVENT_step] := FALSE;


StateMachineStateNames.ENTER_STATE_bootup: 
	state_statistics[StateMachineStateNames.ENTER_STATE_bootup] := state_statistics[StateMachineStateNames.ENTER_STATE_bootup] + 1;
	state := StateMachineStateNames.EXIT_STATE_bootup;
	next_state := StateMachineStateNames.ENTER_STATE_blink1;
	IF next_state = StateMachineStateNames.STATE_NONE THEN
		state := StateMachineStateNames.STATE_bootup;
	END_IF


StateMachineStateNames.STATE_bootup: 
	;


StateMachineStateNames.EXIT_STATE_bootup: 
	state := next_state;
	next_state := StateMachineStateNames.STATE_NONE;


StateMachineStateNames.ENTER_STATE_blink1: 
	state_statistics[StateMachineStateNames.ENTER_STATE_blink1] := state_statistics[StateMachineStateNames.ENTER_STATE_blink1] + 1;
	led1();
	events[EventType.EVENT_step] := TRUE;
	IF next_state = StateMachineStateNames.STATE_NONE THEN
		state := StateMachineStateNames.STATE_blink1;
	END_IF


StateMachineStateNames.STATE_blink1: 
	IF events[EventType.EVENT_step] THEN
		events[EventType.EVENT_step] := FALSE;
		led1();
		led1();
		events[EventType.EVENT_step] := TRUE;
	END_IF
	IF events[EventType.EVENT_b1] THEN
		events[EventType.EVENT_b1] := FALSE;
		logger(str := "ignoring button1 press, already in b1\n");
	END_IF
	IF events[EventType.EVENT_b2] THEN
		events[EventType.EVENT_b2] := FALSE;
		state := StateMachineStateNames.EXIT_STATE_blink1;
		next_state := StateMachineStateNames.ENTER_STATE_blink2;
	END_IF


StateMachineStateNames.EXIT_STATE_blink1: 
	led1();
	state := next_state;
	next_state := StateMachineStateNames.STATE_NONE;


StateMachineStateNames.ENTER_STATE_blink2: 
	state_statistics[StateMachineStateNames.ENTER_STATE_blink2] := state_statistics[StateMachineStateNames.ENTER_STATE_blink2] + 1;
	led2();
	events[EventType.EVENT_step] := TRUE;
	IF next_state = StateMachineStateNames.STATE_NONE THEN
		state := StateMachineStateNames.STATE_blink2;
	END_IF


StateMachineStateNames.STATE_blink2: 
	IF events[EventType.EVENT_step] THEN
		events[EventType.EVENT_step] := FALSE;
		led2();
		events[EventType.EVENT_step] := TRUE;
	END_IF
	IF events[EventType.EVENT_b1] THEN
		events[EventType.EVENT_b1] := FALSE;
		state := StateMachineStateNames.EXIT_STATE_blink2;
		next_state := StateMachineStateNames.ENTER_STATE_blink1;
	END_IF
	IF events[EventType.EVENT_b2] THEN
		events[EventType.EVENT_b2] := FALSE;
		logger(str := "ignoring button2 press, already in b2\n");
	END_IF


StateMachineStateNames.EXIT_STATE_blink2: 
	led2();
	state := next_state;
	next_state := StateMachineStateNames.STATE_NONE;


END_CASE
