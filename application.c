#include"chaos.h"

union Payload p;
char result;

void LoadInitPayload() {
	p.content.destAddress = p.content.destAddress = 0xef;
	p.content.data_[0] = 0xff;
}

int main()
{
	LoadInitPayload();
	Chaos_masterInit(9600);
	while(Chaos_masterGetOperation()(&p)!=OUT);
	Chaos_slaveInit(9600);
	Choas_slaveSetOperation(SlaveNOP);
	while (1) {
		Choas_slaveSetOperation(SlaveWrite);
		Chaos_Write(&p);
		Choas_slaveSetOperation(SlaveRead);
		Chaos_Read(&p);
	}
}