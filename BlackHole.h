#ifndef BLACKHOLE_H
#define BLACKHOLE_H
#include <QTextStream>
class Galaxy;
class BlackHole
{
public:
    BlackHole(QTextStream &stream, unsigned id=0);
    unsigned id() const;
    unsigned star1Id() const;
    unsigned star2Id() const;
    unsigned turnsToClose() const;

private:
    unsigned _id;
    unsigned _star1Id, _star2Id;
    unsigned _turnsToClose;
};
void readBlackHoles(QTextStream &stream, Galaxy &galaxy);
#endif // BLACKHOLE_H
