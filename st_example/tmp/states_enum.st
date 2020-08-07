TYPE StateMachineStateNames :
(
	STATE_NONE := 0
	, ENTER_STATE_bootup := 1
	, STATE_bootup := 2
	, EXIT_STATE_bootup := 3
	, ENTER_STATE_blink1 := 4
	, STATE_blink1 := 5
	, EXIT_STATE_blink1 := 6
	, ENTER_STATE_blink2 := 7
	, STATE_blink2 := 8
	, EXIT_STATE_blink2 := 9
	, MAX_STATE := 10
);
END_TYPE
