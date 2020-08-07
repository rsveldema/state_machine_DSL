<?xml version="1.0" encoding="utf-8"?>
<TcPlcObject Version="1.1.0.1" ProductVersion="3.1.4024.4">
  <POU Name="MAIN" Id="{8d527c68-d8f0-46b2-b9dd-07e518c18609}" SpecialFunc="None">
    <Declaration><![CDATA[PROGRAM MAIN

// 
// NOTE: generated
//

VAR
	ticker : UDINT := 0;
  state : StateMachineStateNames := StateMachineStateNames.STATE_NONE;
  next_state : StateMachineStateNames := StateMachineStateNames.STATE_NONE;
  events : ARRAY [0..EventType.MAX_EVENT] OF BOOL;
  state_statistics : ARRAY [0..StateMachineStateNames.MAX_STATE] OF UDINT;
  logger : LoggerFB ;
{{decls}}
END_VAR]]></Declaration>
    <Implementation>
      <ST><![CDATA[

ticker := ticker + 1;
events := EventDetectorFC(events);

{{switch_code}}]]></ST>
    </Implementation>
  </POU>
</TcPlcObject>


