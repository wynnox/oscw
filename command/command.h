#ifndef COMMAND_H
#define COMMAND_H

class command {
public:
    virtual void execute() = 0;
    virtual ~command() = default;
};

#endif // COMMAND_H
