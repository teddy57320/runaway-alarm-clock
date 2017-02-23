# runaway-alarm-clock

Implementation of an alarm clock that spontaneously runs around a room, forcing the sleeper to chase it. 
This helps people who have a routine of "snoozing" the alarm wake up on time, as chasing and turning off the alarm clock would already make them awake. 

Uses two Arduino UNO boards, one as a transmitter (client), one as a receiver (server).
The client side takes in user input for setting an alarm time. When the current time = set time,
the client will send a wireless signal to the server side. 

The server side is connected to the actual alarm clock robot. It composes of a robot chasis,
speaker, ultrasound proximity sensor, and motors for driving the wheels. 
