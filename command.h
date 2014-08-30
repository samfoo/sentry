#ifndef COMMAND_H
#define COMMAND_H

#define  NO_DATA   -1
#define  THROTTLE  '='
#define  PAN       '-'
#define  TILT      '|'

typedef char Command;

Command readCommand();
void execute(Command c, Sentry & sentry);

#endif
